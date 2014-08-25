%{
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sesstype/st_node.h>
#include "scribble/parser_helper.h"

#ifdef __DEBUG__
#define YYDEBUG 1
#endif

#include "scribble/parser.h"

extern int yylex();
extern FILE *yyin;
extern int yylineno;

void yyerror(st_tree *tree, const char *s)
{
    fprintf(stderr, "Error: %s on line %d\n", s, yylineno);
}
%}

%code requires {
#include <string.h>

#include <sesstype/st_node.h>
#include "scribble/parser_helper.h"
}

%union {
    unsigned long num;
    char *str;
    st_node *node;
    st_nodes *node_list;
    st_node_msgsig_t msgsig;
    st_node_msgsig_payload_t payload;
    st_node_msgsig_payloads *payloads;
    st_expr *expr;
    st_role *role;
    st_role_group *roles;
    msg_cond_t *msg_cond;
}


%parse-param {st_tree *tree}
%locations
%error-verbose

    /* Keywords */
%token ALLREDUCE AND AS AT BETWEEN BY CATCHES CHOICE CONST CONTINUE DO EXCEPT FOREACH FROM GLOBAL GROUP IF IMPORT IN INF INSTANTIATES INTERRUPTIBLE IS LOCAL MODULE ONEOF OR PAR PROTOCOL RANGE REC REPEAT ROLE SIG THROWS TO TYPE WITH

    /* Symbols */
%token NUMRANGE SHL SHR LPAREN RPAREN LBRACE RBRACE LSQUARE RSQUARE LANGLE RANGLE COMMA COLON SEMICOLON EQUAL PLUS MINUS MULTIPLY DIVIDE MODULO DOT

    /* Variables */
%token <str> IDENTIFIER COMMENT
%token <num> DIGITS

    /* 3.1 Primitive Names */
%type <str> payload_type_name protocol_name parameter_name role_name group_name annotation_name recursion_label_name scope_name

    /* 3.2.1 Package, Module and Module Member Names */
%type <str> package_name module_name member_name simple_member_name full_member_name

    /* 3.5 Message signatures */
%type <str> message_operator
%type <msgsig> message_signature
%type <payloads> payloads
%type <payload> payload_element

    /* [Pabble] Expressions */
%type <expr> arith_expr bind_expr range_expr

    /* [Pabble] Role Parameters */
%type <role> role_params role_params_ role_param_decls

    /* [Pabble] Group Declarations */
%type <roles> roles

    /* 3.7.3 Global Interaction Sequences and Blocks */
%type <node> global_interaction_block global_interaction
%type <node_list> global_interaction_sequence

    /* 3.7.4 Global Message Transfer */
%type <node> global_message_transfer
%type <msgsig> message

    /* 3.7.5 Global Choice */
%type <node> global_choice or_global_interaction_blocks

    /* 3.7.6 Global Recursion */
%type <node> global_recursion global_continue

    /* 3.7.7 Global Parallel */
%type <node> global_parallel and_global_interaction_blocks

    /* 3.7.8 Global Interruptible */
    /* %type <node> global_interruptible */

    /* 3.7.9 Global Do */
    /* %type <node> global_do */

%type <node> global_foreach global_allreduce global_oneof

    /* 3.8.3 Local Interaction Blocks and Sequences */
%type <node> local_interaction_block local_interaction
%type <node_list> local_interaction_sequence

    /* 3.8.4 Local Send and Receive */
%type <node> local_send local_receive

    /* 3.8.5 Local Choice */
%type <node> local_choice or_local_interaction_blocks

    /* 3.8.6 Local Recursion */
%type <node> local_recursion local_continue

    /* 3.8.7 Local Parallel */
%type <node> local_parallel and_local_interaction_blocks

    /* 3.8.8 Local Interruptible */
    /* %type <node> local_interruptible */

    /* 3.8.9 Local Do */
    /* %type <node> local_do */

%type <node> local_foreach local_allreduce local_ifblock local_oneof


%left PLUS MINUS
%left MULTIPLY DIVIDE MODULO SHL SHR

%start module

%%

/** 3.1 Primitive Names **/

payload_type_name    : IDENTIFIER { $$ = strdup($1); } ;
protocol_name        : IDENTIFIER { $$ = strdup($1); } ;
parameter_name       : IDENTIFIER { $$ = strdup($1); } ;
role_name            : IDENTIFIER { $$ = strdup($1); } ;
group_name           : IDENTIFIER { $$ = strdup($1); } /* Pabble */ ;
annotation_name      : IDENTIFIER { $$ = strdup($1); } ;
recursion_label_name : IDENTIFIER { $$ = strdup($1); } ;
scope_name           : IDENTIFIER { $$ = strdup($1); } ;


/** 3.2.1 Package, Module and Module Member Names **/

package_name :                  IDENTIFIER { $$ = strdup($1); }
             | package_name DOT IDENTIFIER { $$ = join_str(join_str($1, "."), $3); }
             ;

module_name :                  IDENTIFIER { $$ = strdup($1); }
            | package_name DOT IDENTIFIER { $$ = join_str(join_str($1, "."), $3); }
            ;

member_name : simple_member_name { $$ = strdup($1); }
            | full_member_name   { $$ = strdup($1); }
            ;

simple_member_name : protocol_name     { $$ = strdup($1); }
                /* | payload_type_name { $$ = strdup($1); } */
                   ;

full_member_name : module_name DOT simple_member_name { $$ = join_str(join_str($1, "."), $3); }
                 ;

/** 3.2.2 Top-Level Module Structure **/

module : module_decl import_decls payload_type_decls const_decls protocol_decls
       ;

/** 3.2.3 Module Declarations **/

module_decl : MODULE module_name SEMICOLON { st_tree_set_module(tree, $2); }
            ;

/** 3.3 Import Delcarations **/

import_decls : /* Empty */
             | import_decls import_decl
             ;

import_decl : import_module
            | import_member
            ;

import_module : IMPORT module_name               SEMICOLON { st_tree_add_import(tree, (st_tree_import_t){ .name=strdup($2)                 }); }
              | IMPORT module_name AS IDENTIFIER SEMICOLON { st_tree_add_import(tree, (st_tree_import_t){ .name=strdup($2), .as=strdup($4) }); }
              ;

import_member : FROM module_name IMPORT simple_member_name                       { st_tree_add_import(tree, (st_tree_import_t){ .from=strdup($2), .name=strdup($4)                 }); }
              | FROM module_name IMPORT simple_member_name AS simple_member_name { st_tree_add_import(tree, (st_tree_import_t){ .from=strdup($2), .name=strdup($4), .as=strdup($6) }); }
              ;


/** 3.4 Payload Type Delcarations **/

payload_type_decls : /* Empty */
                   | payload_type_decls payload_type_decl
                   ;

payload_type_decl : TYPE LANGLE IDENTIFIER RANGLE IDENTIFIER FROM IDENTIFIER AS payload_type_name SEMICOLON
                  ;


/** 3.5 Payload Type Declarations **/

message_operator : /* Empty */ { $$ = ""; }
                 | IDENTIFIER  { $$ = $1;   }
                 ;

message_signature : message_operator LPAREN          RPAREN { $$ = (st_node_msgsig_t){ .op=$1, .npayload=0,         .payloads=NULL }; }
                  | message_operator LPAREN payloads RPAREN { $$ = (st_node_msgsig_t){ .op=$1, .npayload=$3->count, .payloads=$3->payloads }; }
                  ;

payloads :                payload_element { $$ = payloads_add(NULL, $1); }
         | payloads COMMA payload_element { $$ = payloads_add($1,   $3); }
         ;

payload_element :                       parameter_name { $$ = payload(NULL, $1, NULL); }
                | annotation_name COLON parameter_name { $$ = payload($1,   $3, NULL); }
                |                       parameter_name LSQUARE arith_expr RSQUARE { $$ = payload(NULL, $1, $3); }
                | annotation_name COLON parameter_name LSQUARE arith_expr RSQUARE { $$ = payload($1,   $3, $5); }
             /* |                       payload_type_name { $$ = $1; } */
             /* | annotation_name COLON payload_type_name { $$ = $3; } */
                ;


/** [Pabble] Constant Declarations **/

const_decls  : /* Empty */
             | const_decls const_decl
             ;

const_decl : CONST IDENTIFIER EQUAL DIGITS                 SEMICOLON { register_constant(tree, $2, $4); }
           | CONST IDENTIFIER EQUAL DIGITS NUMRANGE DIGITS SEMICOLON { register_constant_bounds(tree, $2, $4, $6); }
           | CONST IDENTIFIER EQUAL DIGITS NUMRANGE INF    SEMICOLON { register_constant_bounds_inf(tree, $2, $4); }
           | RANGE IDENTIFIER EQUAL DIGITS NUMRANGE DIGITS SEMICOLON { register_range($2, $4, $6); /* w/o tree */ }
           ;


/** [Pabble] Expressions **/

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
           | arith_expr /* Shorthand*/      { $$ = st_expr_range($1, $1); }
           ;


/** 3.6 Protocol Declarations **/

protocol_decls : /* Empty */
               | protocol_decls protocol_decl
               ;

protocol_decl : global_protocol_decl
              | local_protocol_decl
              ;


/** 3.7 Global Protocol Declarations **/

global_protocol_decl : global_protocol_header global_protocol_definition
                     | global_protocol_header global_protocol_instance
                     ;

global_protocol_header : GLOBAL PROTOCOL protocol_name                     role_decl_list { st_tree_set_name(tree, $3); }
                       | GLOBAL PROTOCOL protocol_name parameter_decl_list role_decl_list { st_tree_set_name(tree, $3); }
                       ;

role_decl_list : LPAREN role_decls group_decls RPAREN /* Pabble */
               ;

role_decls :                  role_decl
           | role_decls COMMA role_decl
           ;

role_decl : ROLE role_name role_param_decls                   { st_tree_add_role(tree, st_role_set_name($3, $2)); }
          | ROLE role_name role_param_decls AS parameter_name { st_tree_add_role(tree, st_role_set_name($3, $2)); /* Ignoring parameter_name */ }
          ;

parameter_decl_list : LANGLE parameter_decls RANGLE
                    ;

parameter_decls :                       parameter_decl
                | parameter_decls COMMA parameter_decl
                ;

parameter_decl : TYPE parameter_name
               | TYPE parameter_name AS parameter_name
               | SIG  parameter_name
               | SIG  parameter_name AS parameter_name
               ;


/** [Pabble] Role Parameters **/

role_param_decls : /* Empty */                                 { $$ = role_empty(); }
                 | role_param_decls LSQUARE range_expr RSQUARE { $$ = st_role_add_param($1, $3); }
                 ;

role_params : /* Empty */                            { $$ = role_empty(); }
            | role_params LSQUARE bind_expr  RSQUARE { $$ = st_role_add_param($1, $3); }
            | role_params LSQUARE arith_expr RSQUARE { $$ = st_role_add_param($1, $3); }
            | role_params LSQUARE range_expr RSQUARE { $$ = st_role_add_param($1, $3); }
            ;

role_params_ : /* Empty */                             { $$ = role_empty(); }
             | role_params_ LSQUARE arith_expr RSQUARE { $$ = st_role_add_param($1, $3); }
             | role_params_ LSQUARE range_expr RSQUARE { $$ = st_role_add_param($1, $3); }
             ;


/** [Pabble] Group Declarations **/

group_decls :
            | group_decl group_decls 
            ;

group_decl : COMMA GROUP group_name EQUAL LBRACE roles RBRACE { st_tree_add_role_group(tree, st_role_group_set_name($6, $3)); }
           ;

roles :       role_name role_param_decls { $$ = st_role_group_add_role((st_role_group *)malloc(sizeof(st_role_group)), st_role_set_name($2, $1)); }
      | roles role_name role_param_decls { $$ = st_role_group_add_role($$, st_role_set_name($3, $2)); }
      ;


/** 3.7.1 Global Protocol Definitions **/

global_protocol_definition : global_interaction_block { tree->root = $1; }
                           ;

/** 3.7.2 Global Protocol Instantiation **/

global_protocol_instance : INSTANTIATES member_name               role_instantiation_list
                         | INSTANTIATES member_name argument_list role_instantiation_list
                         ;

role_instantiation_list : LPAREN role_instantiations RPAREN
                        ;

role_instantiations :                           role_instantiation
                    | role_instantiations COMMA role_instantiation
                    ;

role_instantiation : role_name
                   | role_name AS role_name
                   ;

argument_list : LANGLE arguments RANGLE
              ;

arguments :                 argument
          | arguments COMMA argument
          ;

argument : message_signature
         | message_signature AS parameter_name
         | payload_type_name
         | payload_type_name AS parameter_name
         | parameter_name
         | parameter_name AS parameter_name
         ;


/** 3.7.3 Global Interaction Sequences and Blocks **/

global_interaction_block : LBRACE global_interaction_sequence RBRACE { $$ = interaction_block($2); }
                         ;

global_interaction_sequence : /* Empty */                                    { $$ = interaction_sequence(NULL, NULL); }
                            | global_interaction global_interaction_sequence { $$ = interaction_sequence($2, $1);}
                            ;

/** [Pabble] For-each **/
/** [Pabble] All-reduce **/
/** [Pabble] One-of **/

global_interaction : global_message_transfer { $$ = $1; }
                   | global_choice           { $$ = $1; }
                   | global_recursion        { $$ = $1; }
                   | global_continue         { $$ = $1; }
                   | global_parallel         { $$ = $1; }
                   | global_interruptible    { $$ = NULL; /* TODO */ }
                   | global_do               { $$ = NULL; /* TODO */ }
                   | global_foreach          { $$ = $1; /* Pabble */ }
                   | global_allreduce        { $$ = $1; /* Pabble */ }
                   | global_oneof            { $$ = $1; /* Pabble */ }
                   ;


/** 3.7.4 Global Message Transfer **/
/** [Pabble] No multple receivers **/

global_message_transfer : message FROM role_name role_params TO role_name role_params_ SEMICOLON { $$ = message_node($1, st_role_set_name($4, $3), st_role_set_name($7, $6)); }
                        ;

messages :                message
         | messages COMMA message
         ;

message : message_signature { $$ = $1; }
        | parameter_name    { $$ = (st_node_msgsig_t){ .op="", .npayload=0, .payloads=NULL }; /* What is this supposed to do? */ assert(0); }
        ;


/** 3.7.5 Global Choice **/

global_choice : CHOICE AT role_name role_params global_interaction_block or_global_interaction_blocks { $$ = choice_node(st_role_set_name($4, $3), $5, $6); }
       ;

or_global_interaction_blocks : /* Empty */                                              { $$ = st_node_init((st_node *)malloc(sizeof(st_node)), ST_NODE_ROOT); }
                             | OR global_interaction_block or_global_interaction_blocks { $$ = st_node_append($3, $2); }
                             ;


/** 3.7.6 Global Recursion **/

global_recursion : REC recursion_label_name global_interaction_block { $$ = rec_node($2, $3); }
                 ;

global_continue : CONTINUE recursion_label_name SEMICOLON { ($$ = st_node_init((st_node *)malloc(sizeof(st_node)), ST_NODE_CONTINUE))->cont->label = strdup($2); }
                ;


/** 3.7.7 Global Parallel **/

global_parallel : PAR global_interaction_block and_global_interaction_blocks { $$ = par_node($2, $3); }
                ;

and_global_interaction_blocks : /* Empty */                                                { $$ = st_node_init((st_node *)malloc(sizeof(st_node)), ST_NODE_ROOT); }
                              | AND global_interaction_block and_global_interaction_blocks { $$ = st_node_append($3, $2); }
                              ;


/** 3.7.8 Global Interruptible **/

global_interruptible : INTERRUPTIBLE                  global_interaction_block WITH LBRACE global_interrupts RBRACE
                     | INTERRUPTIBLE scope_name COLON global_interaction_block WITH LBRACE global_interrupts RBRACE
                     ;

global_interrupts : messages BY role_name SEMICOLON
                  ;


/** 3.7.9 Global Do **/

global_do : DO                  member_name               role_instantiation_list SEMICOLON
          | DO scope_name COLON member_name               role_instantiation_list SEMICOLON
          | DO                  member_name argument_list role_instantiation_list SEMICOLON
          | DO scope_name COLON member_name argument_list role_instantiation_list SEMICOLON
          ;


/** [Pabble] For-each **/

global_foreach : FOREACH LPAREN bind_expr                   RPAREN global_interaction_block { $$ = foreach_node($3, $5); }
               | FOREACH LPAREN bind_expr EXCEPT IDENTIFIER RPAREN global_interaction_block { $$ = foreach_except_node($3, $5, $7); }
               ;


/** [Pabble] All-reduce **/

global_allreduce : ALLREDUCE message_signature { $$ = allreduce_node($2); }
          ;


/** [Pabble] One-of **/

global_oneof :        ONEOF LPAREN IDENTIFIER LSQUARE IDENTIFIER IN range_expr RSQUARE RPAREN global_interaction_block { $$ = oneof_node($3, $5, $7, $10); }
             | REPEAT ONEOF LPAREN IDENTIFIER LSQUARE IDENTIFIER IN range_expr RSQUARE RPAREN global_interaction_block { $$ = repeat_oneof_node($4, $6, $8, $11); }

             ;


/** 3.8 Local Protocol Declarations **/

local_protocol_decl : local_protocol_header local_protocol_definition
                    | local_protocol_header local_protocol_instance
                    ;

local_protocol_header : LOCAL PROTOCOL protocol_name AT role_name role_param_decls                     role_decl_list { st_tree_set_local_name(tree, $3, st_role_set_name($6, $5)); }
                      | LOCAL PROTOCOL protocol_name AT role_name role_param_decls parameter_decl_list role_decl_list { st_tree_set_local_name(tree, $3, st_role_set_name($6, $5)); }
                      ;


/** 3.8.1 Local Protocol Definitions **/

local_protocol_definition : local_interaction_block { tree->root = $1; }
                          ;


/** 3.8.2 Local Protocol Instantiation **/

local_protocol_instance : INSTANTIATES member_name               role_instantiation_list SEMICOLON
                        | INSTANTIATES member_name argument_list role_instantiation_list SEMICOLON
                        ;


/** 3.8.3 Local Interaction Blocks and Sequences **/

local_interaction_block : LBRACE local_interaction_sequence RBRACE { $$ = interaction_block($2); }
                        ;

local_interaction_sequence : /* Empty */                                  { $$ = interaction_sequence(NULL, NULL); }
                           | local_interaction local_interaction_sequence { $$ = interaction_sequence($2, $1); }
                           ;


/** [Pabble] For-each **/
/** [Pabble] All-reduce **/
/** [Pabble] If-block **/
/** [Pabble] One-of **/

local_interaction : local_send          { $$ = $1; }
                  | local_receive       { $$ = $1; }
                  | local_choice        { $$ = $1; }
                  | local_recursion     { $$ = $1; }
                  | local_continue      { $$ = $1; }
                  | local_parallel      { $$ = $1; }
                  | local_interruptible { $$ = NULL; /* TODO */ }
                  | local_do            { $$ = NULL; /* TODO */ }
                  | local_foreach       { $$ = $1; /* Pabble */ }
                  | local_allreduce     { $$ = $1; /* Pabble */ }
                  | local_ifblock       { $$ = $1; /* Pabble */ }
                  | local_oneof         { $$ = $1; /* Pabble */ }
                  ;


/** 3.8.4 Local Send and Receive **/

local_send :                          message TO role_name role_params_ SEMICOLON { $$ = send_node(st_role_set_name($4, $3), $1, NULL); }
           | IF role_name role_params message TO role_name role_params_ SEMICOLON { $$ = send_node(st_role_set_name($7, $6), $4, st_role_set_name($3, $2)); }
           ;

local_receive :                          message FROM role_name role_params_ SEMICOLON { $$ = recv_node(st_role_set_name($4, $3), $1, NULL); }
              | IF role_name role_params message FROM role_name role_params_ SEMICOLON { $$ = recv_node(st_role_set_name($7, $6), $4, st_role_set_name($3, $2)); }
              ;


/** 3.8.5 Local Choice **/

local_choice : CHOICE AT role_name role_params local_interaction_block or_local_interaction_blocks { $$ = choice_node(st_role_set_name($4, $3), $5, $6); }
             ;

or_local_interaction_blocks : /* Empty */                                            { $$ = st_node_init((st_node *)malloc(sizeof(st_node)), ST_NODE_ROOT); }
                            | OR local_interaction_block or_local_interaction_blocks { $$ = st_node_append($3, $2); }
                            ;


/** 3.8.6 Local Recursion **/

local_recursion : REC recursion_label_name local_interaction_block { $$ = rec_node($2, $3) ;}
                ;

local_continue : CONTINUE recursion_label_name SEMICOLON { ($$ = st_node_init((st_node *)malloc(sizeof(st_node)), ST_NODE_CONTINUE))->cont->label = strdup($2); }
               ;


/** 3.8.7 Local Parallel **/

local_parallel : PAR local_interaction_block and_local_interaction_blocks { $$ = par_node($2, $3); }
               ;

and_local_interaction_blocks : /* Empty */                                              { $$ = st_node_init((st_node *)malloc(sizeof(st_node)), ST_NODE_ROOT); }
                             | AND local_interaction_block and_local_interaction_blocks { $$ = st_node_append($3, $2); }
                             ;


/** 3.8.8 Local Interruptible **/

local_interruptible : INTERRUPTIBLE scope_name COLON local_interaction_block WITH LBRACE                           RBRACE
                    | INTERRUPTIBLE scope_name COLON local_interaction_block WITH LBRACE local_throw               RBRACE
                    | INTERRUPTIBLE scope_name COLON local_interaction_block WITH LBRACE             local_catches RBRACE
                    | INTERRUPTIBLE scope_name COLON local_interaction_block WITH LBRACE local_throw local_catches RBRACE
                    ;

local_throw : THROWS messages TO role_names SEMICOLON
            ;

role_names :                  role_name
           | role_names COMMA role_name
           ;

local_catches :                     CATCHES messages FROM role_name SEMICOLON
              | local_catches COMMA CATCHES messages FROM role_name SEMICOLON
              ;


/** 3.8.9 Local Do **/

local_do : DO scope_name COLON member_name               role_instantiation_list SEMICOLON
         | DO scope_name COLON member_name argument_list role_instantiation_list SEMICOLON
         ;


/** [Pabble] For-each **/

local_foreach : FOREACH LPAREN bind_expr                   RPAREN local_interaction_block { $$ = foreach_node($3, $5); }
              | FOREACH LPAREN bind_expr EXCEPT IDENTIFIER RPAREN local_interaction_block { $$ = foreach_except_node($3, $5, $7); }
              ;


/** [Pabble] All-reduce **/

local_allreduce : ALLREDUCE message_signature { $$ = allreduce_node($2); }
                ;


/** [Pabble] If-block **/

local_ifblock : IF role_name role_params local_interaction_block { $$ = ifblk_node(st_role_set_name($3, $2), $4); }
              ;


/** [Pabble] One-of **/

local_oneof : ONEOF LPAREN IDENTIFIER LSQUARE IDENTIFIER IN range_expr RSQUARE RPAREN local_interaction_block { $$ = oneof_node($3, $5, $7, $10); }
            ;

%%
