%{
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "st_node.h"
#include "parser_helper.h"

#ifdef __DEBUG__
#define YYDEBUG 1
#endif

extern int yylex();
extern FILE *yyin;

void yyerror(st_tree *tree, const char *s)
{
    fprintf(stderr, "Error: %s\n", s);
}

%}

    /* Keywords */
%token ALLREDUCE AND AS AT BETWEEN BY CATCH CHOICE CONST CONTINUE CREATE DO ENTER FOREACH FROM GLOBAL GROUP IF IMPORT INSTANTIATES INTERRUPTIBLE IS LOCAL OR PACKAGE PAR PROTOCOL RANGE REC ROLE SIG SPAWNS THROW TO TYPE WITH

    /* Symbols */
%token NUMRANGE SHL SHR LPAREN RPAREN LBRACE RBRACE LSQUARE RSQUARE LANGLE RANGLE COMMA COLON SEMICOLON EQUAL PLUS MINUS MULTIPLY DIVIDE MODULO DOT

    /* Variables */
%token <str> IDENTIFIER COMMENT
%token <num> DIGITS

    /* Intermediate representations */
%type <str> package_name role_name group_name message_operator import_decl_as
%type <str> role_names payload_types parameter_list

    /* Roles */
%type <role> role_params role_params_ role_param_decls

    /* Groups */
%type <roles> roles

    /* Expressions */
%type <expr> arith_expr bind_expr range_expr

    /* Message signature */
%type <msgsig> message_signature
%type <msgsig> message_signatures argument_list

    /* Global statements */
%type <node> global_interaction message choice parallel recursion continue interruptible do spawn foreach allreduce

    /* Global blocks */
%type <node> global_interaction_block and_global_interaction_blocks or_global_interaction_blocks

    /* Inside Global blocks */
%type <node_list> global_interaction_sequence

    /* Local statements */
%type <node> local_interaction send receive local_choice local_parallel local_recursion local_continue local_interruptible local_foreach local_allreduce

    /* Local blocks */
%type <node> local_interaction_block and_local_interaction_blocks or_local_interaction_blocks

    /* Inside Local blocks */
%type <node_list> local_interaction_sequence

%left PLUS MINUS
%left MULTIPLY DIVIDE MODULO SHL SHR

%code requires {
#include <string.h>

#include "st_node.h"
#include "parser_helper.h"
}

%parse-param {st_tree *tree}

%union {
    unsigned long num;
    char *str;
    st_node *node;
    st_nodes *node_list;
    st_node_msgsig_t msgsig;
    st_expr *expr;
    st_role *role;
    st_role_group *roles;
    msg_cond_t *msg_cond;
}

%start module

%%

/** 3.1.1 Packages **/

package_name : IDENTIFIER                  { $$ = strdup($1); }
             | package_name DOT IDENTIFIER { $$ = join_str(join_str($1, "."), $3); }
             ;


/** 3.1.2 Modules **/

module : package_decl import_decls payload_type_decls const_decls protocol_decls
       ;

package_decl : PACKAGE package_name { /* Package name is $2 */ }
             ;


/** 3.1.3 Import Delcarations **/

import_decls : /* Empty */
             | import_decls import_decl
             ;

import_decl : IMPORT package_name                                SEMICOLON { st_tree_add_import(tree, (st_tree_import_t){ .name = strdup($2) }); }
            | FROM package_name IMPORT IDENTIFIER import_decl_as SEMICOLON { st_tree_add_import(tree, (st_tree_import_t){ .from=strdup($2), .name=strdup($4), .as=strdup($5) }); }
            ;

import_decl_as : /* Empty */   { $$ = NULL; }
               | AS IDENTIFIER { $$ = $2;   }
               ;


/** 3.2 Payload Type Delcarations **/

payload_type_decls : /* Empty */
                   | payload_type_decls payload_type_decl
                   ;

payload_type_decl : TYPE LANGLE IDENTIFIER RANGLE IDENTIFIER FROM IDENTIFIER AS IDENTIFIER SEMICOLON
                  ;


/** 3.3 Message Signatures **/

message_operator : /* Empty */ { $$ = NULL; }
                 | IDENTIFIER  { $$ = $1; }
                 ;

message_signature : message_operator LPAREN payload_types RPAREN { $$ = (st_node_msgsig_t){ .op=$1==NULL?"":strdup($1), .payload=strdup($3) }; }
                  ;

payload_types : /* Empty */                                     { $$ = ""; }
              |                                      IDENTIFIER { $$ = $1; }
              |                     IDENTIFIER COLON IDENTIFIER { $$ = $3; }
              | payload_types COMMA                  IDENTIFIER { $$ = $3; }
              | payload_types COMMA IDENTIFIER COLON IDENTIFIER { $$ = $5; }
              ;


/** [Pabble] + Constant Declarations **/

const_decls  : /* Empty */
             | const_decls const_decl
             ;

const_decl : CONST IDENTIFIER EQUAL DIGITS                           { register_constant(tree, $2, $4); }
           | CONST IDENTIFIER EQUAL DIGITS                 SEMICOLON { register_constant(tree, $2, $4); }
           | CONST IDENTIFIER IS BETWEEN DIGITS AND DIGITS           { register_constant_bounds(tree, $2, $5, $7); }
           | CONST IDENTIFIER IS BETWEEN DIGITS AND DIGITS SEMICOLON { register_constant_bounds(tree, $2, $5, $7); }
           | RANGE IDENTIFIER EQUAL DIGITS NUMRANGE DIGITS           { register_range($2, $4, $6); /* w/o tree */ }
           | RANGE IDENTIFIER EQUAL DIGITS NUMRANGE DIGITS SEMICOLON { register_range($2, $4, $6); /* w/o tree */ }
           ;


/** [Pabble] + Expressions **/

arith_expr : IDENTIFIER                     { $$ = st_expr_variable($1); }
           | DIGITS                         { $$ = st_expr_constant($1); }
           | arith_expr PLUS     arith_expr { $$ = st_expr_binary($1, ST_EXPR_TYPE_ADD, $3); }
           | arith_expr MINUS    arith_expr { $$ = st_expr_binary($1, ST_EXPR_TYPE_SUB, $3); }
           | arith_expr MULTIPLY arith_expr { $$ = st_expr_binary($1, ST_EXPR_TYPE_MUL, $3); }
           | arith_expr DIVIDE   arith_expr { $$ = st_expr_binary($1, ST_EXPR_TYPE_DIV, $3); }
           | arith_expr MODULO   arith_expr { $$ = st_expr_binary($1, ST_EXPR_TYPE_MOD, $3); }
           | arith_expr SHL      arith_expr { $$ = st_expr_binary($1, ST_EXPR_TYPE_SHL, $3); }
           | arith_expr SHR      arith_expr { $$ = st_expr_binary($1, ST_EXPR_TYPE_SHR, $3); }
           | LPAREN arith_expr RPAREN       { $$ = $2; }
           ;

bind_expr : IDENTIFIER COLON range_expr { assert($3->type == ST_EXPR_TYPE_RNG); ($$ = $3)->rng->bindvar = strdup($1); }
          ;

range_expr : arith_expr NUMRANGE arith_expr { $$ = st_expr_range($1, $3); }
           ;


/** 3.4 Protocol Declarations **/

protocol_decls : /* Empty */                  { }
               | protocol_decls protocol_decl { }
               ;

protocol_decl : global_protocol_decl { }
              | local_protocol_decl  { }
              ;


/** 3.5 Global Protocols **/

global_protocol_decl : GLOBAL PROTOCOL IDENTIFIER global_protocol_definition { st_tree_set_name(tree, $3); }
                     ;


global_protocol_definition :                              LPAREN role_list            RPAREN global_protocol_body                                          { }
                           |                              LPAREN role_list group_list RPAREN global_protocol_body                                          { }
                           | LANGLE parameter_list RANGLE LPAREN role_list            RPAREN global_protocol_body                                          { }
                           |                              LPAREN role_list            RPAREN INSTANTIATES IDENTIFIER                             SEMICOLON { }
                           |                              LPAREN role_list            RPAREN INSTANTIATES IDENTIFIER LANGLE argument_list RANGLE SEMICOLON { }
                           ;

role_list :                 ROLE role_name role_param_decls { st_tree_add_role(tree, role_set_name($3, $2)); }
          | role_list COMMA ROLE role_name role_param_decls { st_tree_add_role(tree, role_set_name($5, $4)); }
          ;

role_name : IDENTIFIER { $$ = $1; }
          ;


/** [Pabble] + Role Parameters **/

role_param_decls : /* Empty */                                 { $$ = role_empty(); }
                 | role_param_decls LSQUARE range_expr RSQUARE { $$ = role_add_param($1, $3); }
                 ;

role_params : /* Empty */                            { $$ = role_empty(); }
            | role_params LSQUARE bind_expr  RSQUARE { $$ = role_add_param($1, $3); }
            | role_params LSQUARE arith_expr RSQUARE { $$ = role_add_param($1, $3); }
            ;

role_params_ : /* Empty */                             { $$ = role_empty(); }
             | role_params_ LSQUARE arith_expr RSQUARE { $$ = role_add_param($1, $3); }
             ;


/** [Pabble] + Group Declarations **/

group_list :            COMMA GROUP group_name EQUAL LBRACE roles RBRACE { st_tree_add_role_group(tree, role_group_set_name($6, $3)); }
           | group_list COMMA GROUP group_name EQUAL LBRACE roles RBRACE { st_tree_add_role_group(tree, role_group_set_name($7, $4)); }
           ;

group_name : IDENTIFIER { $$ = $1; }
           ;

roles :       role_name role_param_decls { $$ = role_group_add_role((st_role_group *)malloc(sizeof(st_role_group)), role_set_name($2, $1)); }
      | roles role_name role_param_decls { $$ = role_group_add_role($$, role_set_name($3, $2)); }
      ;

parameter_list :                      SIG IDENTIFIER { $$ = NULL; /* TODO */ }
               | parameter_list COMMA SIG IDENTIFIER { $$ = NULL; /* TODO */ }
               ;

argument_list :                     message_signature { $$ = $1; /* TODO */ }
              | argument_list COMMA message_signature { $$ = $3; /* TODO */ }
              ;


/** 3.5.1 Global Interaction Blocks and Sequences **/

global_protocol_body : global_interaction_block { tree->root = $1; }
                     ;

global_interaction_block : LBRACE global_interaction_sequence RBRACE { $$ = interaction_block($2); }
                         ;

global_interaction_sequence : /* Empty */                                    { $$ = interaction_sequence(NULL, NULL); }
                            | global_interaction global_interaction_sequence { $$ = interaction_sequence($2, $1);}
                            ;

/** [Pabble] + For-each **/
/** [Pabble] + All-reduce **/

global_interaction : message       { $$ = $1; }
                   | choice        { $$ = $1; }
                   | parallel      { $$ = $1; }
                   | recursion     { $$ = $1; }
                   | continue      { $$ = $1; }
                   | interruptible { $$ = NULL; /* TODO */ }
                   | do            { $$ = NULL; /* TODO */ }
                   | spawn         { $$ = NULL; /* TODO */ }
                   | foreach       { $$ = $1; }
                   | allreduce     { $$ = $1; }
                   ;


/** 3.5.2 Point-to-Point Message Transfer **/
/** [Pabble] - Multple receivers **/

message : message_signature FROM role_name role_params TO role_name role_params_ SEMICOLON { $$ = message_node($1, role_set_name($4, $3), role_set_name($7, $6)); }
        ;


/** 3.5.3 Choice **/

choice : CHOICE AT role_name role_params global_interaction_block or_global_interaction_blocks { $$ = choice_node(role_set_name($4, $3), $5, $6); }
       ;

or_global_interaction_blocks : /* Empty */                                              { $$ = st_node_init((st_node *)malloc(sizeof(st_node)), ST_NODE_ROOT); }
                             | OR global_interaction_block or_global_interaction_blocks { $$ = st_node_append($3, $2); }
                             ;


/** 3.5.4 Parallel **/

parallel : PAR global_interaction_block and_global_interaction_blocks { $$ = par_node($2, $3); }
         ;

and_global_interaction_blocks : /* Empty */                                                { $$ = st_node_init((st_node *)malloc(sizeof(st_node)), ST_NODE_ROOT); }
                              | AND global_interaction_block and_global_interaction_blocks { $$ = st_node_append($3, $2); }
                              ;


/** 3.5.5 Recursion **/

recursion : REC IDENTIFIER global_interaction_block { $$ = rec_node($2, $3); }
          ;

continue : CONTINUE IDENTIFIER SEMICOLON { ($$ = st_node_init((st_node *)malloc(sizeof(st_node)), ST_NODE_CONTINUE))->cont->label = strdup($2); }
         ;


/** 3.5.6 Interruptible **/

interruptible : INTERRUPTIBLE global_interaction_block WITH LBRACE interrupts RBRACE { $$ = NULL; }
              ;

interrupts : message_signatures BY role_names SEMICOLON
           ;

message_signatures :                          message_signature { $$ = $1; /* TODO */ }
                   | message_signatures COMMA message_signature { $$ = $3; /* TODO */ }
                   ;

role_names :                  role_name { $$ = $1; /* TODO */ }
           | role_names COMMA role_name { $$ = $3; /* TODO */ }
           ;

/** 3.5.7 Do **/

do : DO IDENTIFIER                             LPAREN role_instantiation_list RPAREN SEMICOLON { $$ = NULL; }
   | DO IDENTIFIER LANGLE argument_list RANGLE LPAREN role_instantiation_list RPAREN SEMICOLON { $$ = NULL; }
   ;

role_instantiation_list :                               role_name AS role_name { }
                        | role_instantiation_list COMMA role_name AS role_name { }
                        ;

/** 3.5.8 Spawn **/

spawn : role_name SPAWNS IDENTIFIER LANGLE argument_list RANGLE LPAREN role_instantiation_list RPAREN SEMICOLON { $$ = NULL; }
      ;


/** [Pabble] + For-each **/

foreach : FOREACH LPAREN bind_expr RPAREN global_interaction_block { $$ = foreach_node($3, $5); }
        ;


/** [Pabble] + All-reduce **/

allreduce : ALLREDUCE message_signature { $$ = allreduce_node($2); }
          ;


/** 3.6 Local Protocols **/

local_protocol_decl : LOCAL PROTOCOL IDENTIFIER AT role_name role_param_decls LPAREN role_list            RPAREN local_protocol_body { st_tree_set_local_name(tree, $3, role_set_name($6, $5)); }
                    | LOCAL PROTOCOL IDENTIFIER AT role_name role_param_decls LPAREN role_list group_list RPAREN local_protocol_body { st_tree_set_local_name(tree, $3, role_set_name($6, $5)); }
                    ;


/** 3.6.1 Local Interaction Blocks and Sequences **/

local_protocol_body : local_interaction_block { tree->root = $1; }
                    ;

local_interaction_block : LBRACE local_interaction_sequence RBRACE { $$ = interaction_block($2); }
                        ;

local_interaction_sequence : /* Empty */                                  { $$ = interaction_sequence(NULL, NULL); }
                           | local_interaction local_interaction_sequence { $$ = interaction_sequence($2, $1); }
                           ;

/** [Pabble] + For-each **/
/** [Pabble] + All-reduce **/

local_interaction : send                { $$ = $1; }
                  | receive             { $$ = $1; }
                  | local_choice        { $$ = $1; }
                  | local_parallel      { $$ = $1; }
                  | local_recursion     { $$ = $1; }
                  | local_continue      { $$ = $1; }
                  | local_interruptible { $$ = NULL; /* TODO */}
                  | local_foreach       { $$ = $1; }
                  | local_allreduce     { $$ = $1; }
                  ;


/** 3.6.2 Point-to-Point Message Transfer **/

send :                          message_signature TO role_name role_params_ SEMICOLON { $$ = send_node(role_set_name($4, $3), $1, NULL); }
     | IF role_name role_params message_signature TO role_name role_params_ SEMICOLON { $$ = send_node(role_set_name($7, $6), $4, role_set_name($3, $2)); }
     ;

receive :                          message_signature FROM role_name role_params_ SEMICOLON { $$ = recv_node(role_set_name($4, $3), $1, NULL); }
        | IF role_name role_params message_signature FROM role_name role_params_ SEMICOLON { $$ = recv_node(role_set_name($7, $6), $4, role_set_name($3, $2)); }
        ;


/** 3.6.3 Choice **/

local_choice : CHOICE AT role_name role_params local_interaction_block or_local_interaction_blocks { $$ = choice_node(role_set_name($4, $3), $5, $6); }
             ;

or_local_interaction_blocks : /* Empty */                                            { $$ = st_node_init((st_node *)malloc(sizeof(st_node)), ST_NODE_ROOT); }
                            | OR local_interaction_block or_local_interaction_blocks { $$ = st_node_append($3, $2); }
                            ;


/** 3.6.4 Parallel **/

local_parallel : PAR local_interaction_block and_local_interaction_blocks { $$ = par_node($2, $3); }
               ;

and_local_interaction_blocks : /* Empty */                                              { $$ = st_node_init((st_node *)malloc(sizeof(st_node)), ST_NODE_ROOT); }
                             | AND local_interaction_block and_local_interaction_blocks { $$ = st_node_append($3, $2); }
                             ;


/** 3.6.5 Recursion **/

local_recursion : REC IDENTIFIER local_interaction_block { $$ = rec_node($2, $3) ;}
                ;

local_continue : CONTINUE IDENTIFIER SEMICOLON { ($$ = st_node_init((st_node *)malloc(sizeof(st_node)), ST_NODE_CONTINUE))->cont->label = strdup($2); }
               ;


/** 3.6.6 Interruptible **/

local_interruptible : INTERRUPTIBLE local_interaction_block throw catch SEMICOLON { $$ = NULL; }
                    ;

throw : /* Empty */
      | THROW message_signatures
      ;

catch : /* Empty */
      | CATCH message_signatures
      ;


/** 3.6.7 Protocol Creation and Entrance **/

create : CREATE IDENTIFIER LANGLE parameter_list RANGLE LPAREN                         RPAREN SEMICOLON
       | CREATE IDENTIFIER LANGLE parameter_list RANGLE LPAREN role_instantiation_list RPAREN SEMICOLON
       ;

enter : ENTER IDENTIFIER AS role_name
      ;


/** [Pabble] + For-each **/

local_foreach : FOREACH LPAREN bind_expr RPAREN local_interaction_block { $$ = foreach_node($3, $5); }
              ;


/** [Pabble] + All-reduce **/

local_allreduce : ALLREDUCE message_signature { $$ = allreduce_node($2); }
                ;
%%
