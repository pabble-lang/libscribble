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
    st_expr *expr;
    st_role *role;
    st_role_group *roles;
    msg_cond_t *msg_cond;
}


%parse-param {st_tree *tree}
%locations
%error-verbose

    /* Keywords */
%token AND AS AT BY CHOICE CONTINUE DO FROM GLOBAL IMPORT INSTANTIATES INTERRUPTIBLE LOCAL MODULE OR PAR PROTOCOL REC ROLE SIG TO TYPE WITH
%token CATCHES THROWS

    /* Pabble keywords */
%token ALLREDUCE CONST FOREACH GROUP IF INF RANGE

    /* Symbols */
%token NUMRANGE SHL SHR LPAREN RPAREN LBRACE RBRACE LSQUARE RSQUARE LANGLE RANGLE COMMA COLON SEMICOLON EQUAL PLUS MINUS MULTIPLY DIVIDE MODULO DOT

    /* Variables */
%token <str> IDENTIFIER EXTIDENTIFIER COMMENT
%token <num> DIGITS

    /* 3.1 Primitive Names */
%type <str> payload_type_name protocol_name parameter_name role_name group_name annotation_name recursion_label_name scope_name

    /* 3.2.1 Package, Module and Module Member Names*/
%type <str> package_name module_name member_name simple_member_name full_member_name

    /* 3.5 Message Signatures */
%type <str> message_operator
%type <msgsig> message message_signature
%type <str> payload payload_element

%type <role> role_params_decl role_params role2_params

    /* Expressions */
%type <expr> arith_expr bind_expr range_expr

    /* Groups */
%type <roles> roles role_name_ role_name_param_

    /* Global statements */
%type <node> global_interaction global_message_transfer global_choice global_recursion global_continue global_parallel global_interruptible global_do global_foreach global_allreduce

    /* Global blocks */
%type <node> global_interaction_block and_global_interaction_blocks or_global_interaction_blocks

    /* Inside Global blocks */
%type <node_list> global_interaction_sequence

    /* Local statements */
%type <node> local_interaction local_send local_receive local_choice local_parallel local_recursion local_continue local_interruptible local_foreach local_allreduce

    /* Local blocks */
%type <node> local_interaction_block and_local_interaction_blocks or_local_interaction_blocks

    /* Inside Local blocks */
%type <node_list> local_interaction_sequence

%left PLUS MINUS
%left MULTIPLY DIVIDE MODULO SHL SHR

%start module

%%

/** 3.1 Primitive Names **/

payload_type_name : IDENTIFIER { $$ = strdup($1); }
                  ;

protocol_name : IDENTIFIER { $$ = strdup($1); }
              ;

parameter_name : IDENTIFIER { $$ = strdup($1); }
               ;

role_name : IDENTIFIER { $$ = strdup($1); }
          ;

group_name : IDENTIFIER { $$ = strdup($1); } /* [Pabble] */
           ;

annotation_name : IDENTIFIER { $$ = strdup($1); }
                ;

recursion_label_name : IDENTIFIER { $$ = strdup($1); }
                     ;

scope_name : IDENTIFIER { $$ = strdup($1); }
           ;


/** 3.2.1 Package, Module and Module Member Names **/

package_name :                  IDENTIFIER { $$ = strdup($1); }
             | package_name DOT IDENTIFIER { $$ = join_str(join_str($1, "."), $3);}
             ;

module_name :                  IDENTIFIER { $$ = strdup($1); }
            | package_name DOT IDENTIFIER { $$ = join_str(join_str($1, "."), $3); }
            ;

member_name : simple_member_name { $$ = strdup($1); }
            | full_member_name   { $$ = strdup($1); }
            ;

simple_member_name : payload_type_name { $$ = strdup($1); }
                   | protocol_name     { $$ = strdup($1); } /* TODO: reduce-reduce */
                   ;

full_member_name : module_name DOT simple_member_name { $$ = join_str(join_str($1, "."), $3); }
                 ;


/** 3.2.2 Top-Level Module Structure **/
/** [Pabble] + const_decls_ */

module : module_decl import_decls_ payload_type_decls_ const_decls_ protocol_decls_
       ;

import_decls_ : /* Empty */
              | import_decls_ import_decl
              ;

payload_type_decls_ : /* Empty */
                    | payload_type_decls_ payload_type_decl
                    ;

/** [Pabble] **/
const_decls_ : /* Empty */
             | const_decls_ const_decl
             ;

protocol_decls_ : /* Empty */
                | protocol_decls_ protocol_decl
                ;


/** 3.2.3 Module Declarations **/

module_decl : MODULE module_name SEMICOLON { st_tree_set_module(tree, $2); }
            ;


/** 3.3 Import Declarations **/

import_decl : import_module
            | import_member
            ;

import_module : IMPORT module_name               SEMICOLON { st_tree_add_import(tree, (st_tree_import_t){ .from=NULL, .name = strdup($2), .as=NULL }); }
              | IMPORT module_name AS IDENTIFIER SEMICOLON { st_tree_add_import(tree, (st_tree_import_t){ .from=NULL, .name = strdup($2), .as=strdup($4) }); }
              ;

import_member : FROM module_name IMPORT simple_member_name                       SEMICOLON { st_tree_add_import(tree, (st_tree_import_t){ .from=strdup($2), .name=strdup($4), .as=NULL }); }
              | FROM module_name IMPORT simple_member_name AS simple_member_name SEMICOLON { st_tree_add_import(tree, (st_tree_import_t){ .from=strdup($2), .name=strdup($4), .as=strdup($6) }); }
              ;


/** 3.4 Payload Type Delcarations **/

payload_type_decl : TYPE LANGLE IDENTIFIER RANGLE EXTIDENTIFIER FROM EXTIDENTIFIER AS payload_type_name SEMICOLON
                  ;


/** 3.5 Message Signatures **/

message_operator : /* Empty */ { $$ = NULL; }
                 | IDENTIFIER  { $$ = $1;   }
                 ;

message_signature : message_operator LPAREN         RPAREN { $$ = (st_node_msgsig_t){ .op=$1==NULL?"":strdup($1), .payload=NULL }; }
                  | message_operator LPAREN payload RPAREN { $$ = (st_node_msgsig_t){ .op=$1==NULL?"":strdup($1), .payload=strdup($3) }; }
                  ;

payload :               payload_element { $$ = $1; }
        | payload COMMA payload_element { $$ = $3; /* TODO: prepending payloads are ignored */ }
        ;

payload_element :                       payload_type_name { $$ = $1; }
                | annotation_name COLON payload_type_name { $$ = join_str(join_str($1, ":"), $3); }
                /* TODO: reduce-reduce */
                |                       parameter_name    { $$ = $1; }
                | annotation_name COLON parameter_name    { $$ = join_str(join_str($1, ":"), $3); }
                ;


/** [Pabble] Constant Declarations **/

const_decl : CONST IDENTIFIER EQUAL DIGITS                           { register_constant(tree, $2, $4); }
           | CONST IDENTIFIER EQUAL DIGITS                 SEMICOLON { register_constant(tree, $2, $4); }
           | CONST IDENTIFIER EQUAL DIGITS NUMRANGE DIGITS           { register_constant_bounds(tree, $2, $4, $6); }
           | CONST IDENTIFIER EQUAL DIGITS NUMRANGE DIGITS SEMICOLON { register_constant_bounds(tree, $2, $4, $6); }
           | CONST IDENTIFIER EQUAL DIGITS NUMRANGE INF              { register_constant_bounds_inf(tree, $2, $4); }
           | CONST IDENTIFIER EQUAL DIGITS NUMRANGE INF    SEMICOLON { register_constant_bounds_inf(tree, $2, $4); }
           | RANGE IDENTIFIER EQUAL DIGITS NUMRANGE DIGITS           { register_range($2, $4, $6); /* w/o tree */ }
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

protocol_decl : global_protocol_decl { }
              | local_protocol_decl  { }
              ;


/** 3.7 Global Protocols **/

global_protocol_decl : global_protocol_header global_protocol_definition { }
                     | global_protocol_header global_protocol_instance   { }
                     ;

global_protocol_header : GLOBAL PROTOCOL protocol_name                     role_decl_list { st_tree_set_name(tree, $3); }
                       | GLOBAL PROTOCOL protocol_name parameter_decl_list role_decl_list { st_tree_set_name(tree, $3); }
                       ;

role_decl_list : LPAREN role_decl_             RPAREN
               | LPAREN role_decl_ group_decl_ RPAREN /* [Pabble] */
               ;

role_decl_ :                  role_decl
           | role_decl_ COMMA role_decl
           ;

role_decl : ROLE role_name                               { st_tree_add_role(tree, role_set_name(role_empty(), $2)); }
          | ROLE role_name                  AS role_name { st_tree_add_role(tree, role_set_name(role_empty(), $2)); }
          | ROLE role_name role_params_decl              { st_tree_add_role(tree, role_set_name($3, $2)); } /* [Pabble] */
          | ROLE role_name role_params_decl AS role_name { st_tree_add_role(tree, role_set_name($3, $2)); } /* [Pabble] */
          ;

parameter_decl_list : LANGLE parameter_decl_ RANGLE
                    ;

parameter_decl_ :                       parameter_decl
                | parameter_decl_ COMMA parameter_decl
                ;

parameter_decl : TYPE parameter_name
               | TYPE parameter_name AS parameter_name
               | SIG  parameter_name
               | SIG  parameter_name AS parameter_name
               ;


/** [Pabble] Role Parameters **/
role_params_decl :                  LSQUARE range_expr RSQUARE { $$ = role_add_param(role_empty(), $2); }
                 | role_params_decl LSQUARE range_expr RSQUARE { $$ = role_add_param($1, $3); }
                 ;

/** [Pabble] Group Declarations and Definitions **/
group_decl_ :             COMMA group_decl
            | group_decl_ COMMA group_decl
            ;

group_decl : GROUP group_name EQUAL LBRACE roles RBRACE { st_tree_add_role_group(tree, role_group_set_name($5, $2)); }
           ;


/** 3.7.1 Global Protocol Definitions **/

global_protocol_definition : global_interaction_block { tree->root = $1; }
                           ;


/** 3.7.2 Global Protocol Instantiation **/

global_protocol_instance : INSTANTIATES member_name               role_instantiation_list SEMICOLON
                         | INSTANTIATES member_name argument_list role_instantiation_list SEMICOLON
                         ;

role_instantiation_list : LPAREN role_instantiation_ RPAREN
                        ;

role_instantiation_ :                           role_instantiation
                    | role_instantiation_ COMMA role_instantiation
                    ;

role_instantiation : role_name AS role_name
                   | role_name
                   ;

argument_list : LANGLE argument_ RANGLE
              ;

argument_ :                 argument
          | argument_ COMMA argument
          ;

argument : message_signature
         | message_signature AS parameter_name
         | payload_type_name
         | payload_type_name AS parameter_name
         /* TODO: reduce-reduce */
         | parameter_name
         | parameter_name    AS parameter_name
         ;


/** 3.7.3 Global Interaction Sequences and Blocks **/

global_interaction_block : LBRACE global_interaction_sequence RBRACE { $$ = interaction_block($2); }
                         ;

global_interaction_sequence : /* Empty */                                    { $$ = interaction_sequence(NULL, NULL); }
                            | global_interaction global_interaction_sequence { $$ = interaction_sequence($2, $1);}
                            ;

/** [Pabble] For-each **/
/** [Pabble] All-reduce **/
global_interaction : global_message_transfer { $$ = $1; }
                   | global_choice           { $$ = $1; }
                   | global_recursion        { $$ = $1; }
                   | global_continue         { $$ = $1; }
                   | global_parallel         { $$ = $1; }
                   | global_interruptible    { $$ = NULL; /* TODO */ }
                   | global_do               { $$ = NULL; /* TODO */ }
                   | global_foreach          { $$ = $1; } /* [Pabble] */
                   | global_allreduce        { $$ = $1; } /* [Pabble] */
                   ;


/** 3.7.4 Point-to-Point Message Transfer **/

global_message_transfer : message FROM role_name             TO role_name_             SEMICOLON { $$ = message_node($1, role_set_name(role_empty(), $3), $5->membs[0]); /* TODO: free $5 */ }
                        | message FROM role_name role_params TO role_name role2_params SEMICOLON { $$ = message_node($1, role_set_name($4, $3), role_set_name($7, $6)); } /* [Pabble] */
                        ;

role_name_ :                  role_name { $$ = role_group_add_role((st_role_group *)malloc(sizeof(st_role_group)), role_set_name(role_empty(), $1)); }
           | role_name_ COMMA role_name { $$ = role_group_add_role($$, role_set_name(role_empty(), $3)); }
           ;

role_name_param_ :                        role_name role_params { $$ = role_group_add_role((st_role_group *)malloc(sizeof(st_role_group)), role_set_name($2, $1)); }
                 | role_name_param_ COMMA role_name role_params { $$ = role_group_add_role($$, role_set_name($4, $3)); }
                 ;

message : message_signature { $$ = $1; }
        | IDENTIFIER /* =parameter_name */ { $$ = (st_node_msgsig_t){ .op="__parameter__", .payload=strdup($1) }; }
        ;


/** 3.7.5 Global Choice **/

global_choice : CHOICE AT role_name             global_interaction_block or_global_interaction_blocks { $$ = choice_node(role_set_name(role_empty(), $3), $4, $5); }
              | CHOICE AT role_name role_params global_interaction_block or_global_interaction_blocks { $$ = choice_node(role_set_name($4, $3), $5, $6); } /* [Pabble] */
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

global_interruptible : INTERRUPTIBLE                  global_interaction_block WITH LBRACE global_interrupts_ RBRACE { $$ = NULL; }
                     | INTERRUPTIBLE scope_name COLON global_interaction_block WITH LBRACE global_interrupts_ RBRACE { $$ = NULL; }
                     ;

global_interrupts_ : /* Empty */
                   | global_interrupts_ global_interrupt
                   ;

global_interrupt : message_ BY role_name SEMICOLON
                 ;

message_ :                message
         | message_ COMMA message
         ;


/** 3.7.9 Global Do **/

global_do : DO                  member_name               role_instantiation_list SEMICOLON { $$ = NULL; }
          | DO scope_name COLON member_name               role_instantiation_list SEMICOLON { $$ = NULL; }
          | DO                  member_name argument_list role_instantiation_list SEMICOLON { $$ = NULL; }
          | DO scope_name COLON member_name argument_list role_instantiation_list SEMICOLON { $$ = NULL; }
          ;


/** [Pabble] + For-each **/

global_foreach : FOREACH LPAREN bind_expr RPAREN global_interaction_block { $$ = foreach_node($3, $5); }
               ;


/** [Pabble] + All-reduce **/

global_allreduce : ALLREDUCE message_signature { $$ = allreduce_node($2); }
                 ;


/** 3.8 Local Protocol Declarations **/

local_protocol_decl : local_protocol_header local_protocol_definition
                    | local_protocol_header local_protocol_instance
                    ;

local_protocol_header : LOCAL PROTOCOL protocol_name AT role_name role_params                     role_decl_list { st_tree_set_local_name(tree, $3, role_set_name($6, $5)); }
                      | LOCAL PROTOCOL protocol_name AT role_name role_params parameter_decl_list role_decl_list { st_tree_set_local_name(tree, $3, role_set_name($6, $5)); }
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


local_interaction : local_send          { $$ = $1; }
                  | local_receive       { $$ = $1; }
                  | local_choice        { $$ = $1; }
                  | local_recursion     { $$ = $1; }
                  | local_continue      { $$ = $1; }
                  | local_parallel      { $$ = $1; }
                  | local_interruptible { $$ = NULL; /* TODO */ }
                  | local_do            { $$ = NULL; /* TODO */ }
                  | local_foreach       { $$ = $1; } /* Pabble */
                  | local_allreduce     { $$ = $1; } /* Pabble */
                  ;


/** 3.8.4 Local Send and Receive **/

local_send :                          message TO role_name_       SEMICOLON { $$ = send_node($3->membs[0], $1, NULL); }
           |                          message TO role_name_param_ SEMICOLON { $$ = send_node($3->membs[0], $1, NULL); }
           | IF role_name role_params message TO role_name_       SEMICOLON { $$ = send_node($6->membs[0], $4, role_set_name($3, $2)); }
           | IF role_name role_params message TO role_name_param_ SEMICOLON { $$ = send_node($6->membs[0], $4, role_set_name($3, $2)); }
           ;

local_receive :                          message FROM role_name role_params SEMICOLON { $$ = recv_node(role_set_name($4, $3), $1, NULL); }
              | IF role_name role_params message FROM role_name role_params SEMICOLON { $$ = recv_node(role_set_name($7, $6), $4, role_set_name($3, $2)); }
              ;


/** 3.8.5 Local Choice **/

local_choice : CHOICE AT role_name             local_interaction_block or_local_interaction_blocks { $$ = choice_node(role_set_name(role_empty(), $3), $4, $5); }
             | CHOICE AT role_name role_params local_interaction_block or_local_interaction_blocks { $$ = choice_node(role_set_name($4, $3), $5, $6); } /* [Pabble] */
             ;

or_local_interaction_blocks : /* Empty */                                            { $$ = st_node_init((st_node *)malloc(sizeof(st_node)), ST_NODE_ROOT); }
                            | OR local_interaction_block or_local_interaction_blocks { $$ = st_node_append($3, $2); }
                            ;


/** 3.8.6 Local Recursion **/

local_recursion : REC IDENTIFIER local_interaction_block { $$ = rec_node($2, $3) ;}
                ;

local_continue : CONTINUE IDENTIFIER SEMICOLON { ($$ = st_node_init((st_node *)malloc(sizeof(st_node)), ST_NODE_CONTINUE))->cont->label = strdup($2); }
               ;


/** 3.8.7 Parallel **/

local_parallel : PAR local_interaction_block and_local_interaction_blocks { $$ = par_node($2, $3); }
               ;

and_local_interaction_blocks : /* Empty */                                              { $$ = st_node_init((st_node *)malloc(sizeof(st_node)), ST_NODE_ROOT); }
                             | AND local_interaction_block and_local_interaction_blocks { $$ = st_node_append($3, $2); }
                             ;


/** 3.8.8 Local Interruptible **/

local_interruptible : INTERRUPTIBLE scope_name COLON local_interaction_block                          { $$ = NULL; }
                    | INTERRUPTIBLE scope_name COLON local_interaction_block local_throw              { $$ = NULL; }
                    | INTERRUPTIBLE scope_name COLON local_interaction_block             local_catch_ { $$ = NULL; }
                    | INTERRUPTIBLE scope_name COLON local_interaction_block local_throw local_catch_ { $$ = NULL; }
                    ;

local_throw : THROWS message_ TO role_name_ SEMICOLON
            ;

local_catch_ :                    local_catch
             | local_catch_ COMMA local_catch
             ;

local_catch : CATCHES message_ FROM role_name SEMICOLON
            ;

/** 3.8.9 Local Do **/

local_do : DO scope_name COLON member_name               role_instantiation_list SEMICOLON
         | DO scope_name COLON member_name argument_list role_instantiation_list SEMICOLON
         ;


/** [Pabble] + For-each **/

local_foreach : FOREACH LPAREN bind_expr RPAREN local_interaction_block { $$ = foreach_node($3, $5); }
              ;


/** [Pabble] + All-reduce **/

local_allreduce : ALLREDUCE message { $$ = allreduce_node($2); }
                ;

/** [Pabble] + Role Parameters **/

role_params :             LSQUARE bind_expr  RSQUARE { $$ = role_add_param(role_empty(), $2); }
            | role_params LSQUARE bind_expr  RSQUARE { $$ = role_add_param($1, $3); }
            |             LSQUARE arith_expr RSQUARE { $$ = role_add_param(role_empty(), $2); }
            | role_params LSQUARE arith_expr RSQUARE { $$ = role_add_param($1, $3); }
            ;

role2_params : /* Empty */                             { $$ = role_empty(); }
             | role2_params LSQUARE arith_expr RSQUARE { $$ = role_add_param($1, $3); }
             ;

roles :       role_name role_params_decl { $$ = role_group_add_role((st_role_group *)malloc(sizeof(st_role_group)), role_set_name($2, $1)); }
      | roles role_name role_params_decl { $$ = role_group_add_role($$, role_set_name($3, $2)); }
      ;
%%
