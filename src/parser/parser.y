%{
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sesstype/msg.h>
#include <sesstype/node.h>
#include <sesstype/role.h>

#include <sesstype/parameterised/expr.h>
#include <sesstype/parameterised/expr/var.h>
#include <sesstype/parameterised/expr/val.h>
#include <sesstype/parameterised/expr/add.h>
#include <sesstype/parameterised/expr/sub.h>
#include <sesstype/parameterised/expr/mul.h>
#include <sesstype/parameterised/expr/div.h>
#include <sesstype/parameterised/expr/mod.h>
#include <sesstype/parameterised/expr/shl.h>
#include <sesstype/parameterised/expr/shr.h>
#include <sesstype/parameterised/expr/seq.h>
#include <sesstype/parameterised/expr/rng.h>
#include <sesstype/parameterised/msg.h>
#include <sesstype/parameterised/node.h>
#include <sesstype/parameterised/role.h>
#include <sesstype/parameterised/role_grp.h>

#include "parser/parser_helper.h"

#ifdef __DEBUG__
#define YYDEBUG 1
#endif

extern int yylex();
extern FILE *yyin;
extern int yylineno;

void yyerror(st_module *module, const char *s)
{
    fprintf(stderr, "Error: %s on line %d\n", s, yylineno);
}
%}

%code requires {
#include "parser/parser_helper.h"
}

%union {
    unsigned long        num;
    char                 *str;
    pbl_stmt             *stmt;
    pbl_stmt_list        *stmt_list;
    pbl_msg              *msg;
    pbl_msg_list         *msg_list;
    pbl_msg_payload      *payload;
    pbl_msg_payload_list *payload_list;
    pbl_expr             *expr;
    pbl_role             *role;
    pbl_role_decls       *role_decls;
    pbl_role_list        *role_list;
    pbl_role_grp         *role_grp;
    pbl_role_grp_list    *role_grp_list;
    pbl_role             *cond;
    pbl_protocol         *protocol;
    pbl_protocol_list    *protocol_list;
    pbl_module           *module;
    pbl_exttype          *type;
    pbl_exttype_list     *type_list;
    pbl_const            *constant;
    pbl_const_list       *const_list;
    pbl_import           *import;
    pbl_import_list      *import_list;
    pbl_rng              *rng;
    pbl_rng_list         *rng_list;
    pbl_interrupt        *interrupt;
    pbl_interrupt_list   *interrupt_list;
}


%parse-param {st_module *module}
%locations
%error-verbose

    /* Keywords */
%token ALLREDUCE AND AS AT BETWEEN BY CATCHES CHOICE CONST CONTINUE DO EXCEPT FOR FROM GLOBAL GROUP IF IMPORT IN INF INSTANTIATES INTERRUPTIBLE IS LOCAL MODULE ONEOF OR PAR PROTOCOL RANGE REC REPEAT ROLE SIG THROWS TO TYPE WITH

    /* Symbols */
%token NUMRANGE SHL SHR LPAREN RPAREN LBRACE RBRACE LSQUARE RSQUARE LANGLE RANGLE COMMA COLON SEMICOLON EQUAL PLUS MINUS MULTIPLY DIVIDE MODULO DOT

    /* Variables */
%token <str> IDENTIFIER COMMENT
%token <num> DIGITS

%type <str> simple_name

    /* 3.1 Primitive Names */
%type <str> annotation_name parameter_name recursion_var_name role_name group_name scope_name

    /* 3.2.1 Package, Module and Module Member Names */
%type <str> simple_module_name simple_payload_type_name simple_message_signature_name simple_protocol_name simple_member_name
%type <str> qualified_name module_name member_name protocol_name payload_type_name

    /* 3.2.2 Top-Level Module Structure */

    /* 3.2.3 Module Declarations */
%type <module> module_decl

    /* 3.3 Import Declarations */
%type <import>      import_decl import_module import_member
%type <import_list> import_decls

    /* 3.4 Payload Type Declarations */
%type <type>      datatype_decl payload_type_decl message_signature_decl
%type <type_list> datatype_decls

    /* [Pabble] Constant Declarations */
%type <constant>      const_decl
%type <const_list> const_decls

    /* [Pabble] Range Declarations */
%type <rng>      range_decl
%type <rng_list> range_decls

    /* [Pabble] Expressions */
%type <expr> arith_expr bind_expr range_expr

    /* 3.5 Message signatures */
%type <str> message_operator
%type <msg> message_signature
%type <payload>      payload_element
%type <payload_list> payloads

    /* 3.6 Protocol Declarations */
%type <protocol>      protocol_decl
%type <protocol_list> protocol_decls

    /* 3.7 Global Protocol Declarations */
%type <protocol> global_protocol_header
%type <protocol> global_protocol_decl

    /** [Pabble] Role/Role Group Declaration **/
%type <role_decls> role_decl_list
%type <role>      role_decl role_params_decl
%type <role_list> role_decls

    /* TODO param_decl should go here*/

    /* [Pabble] Role Parameters */
%type <role> role role_binding role_relative role_bindable
%type <role> role_params role_params_bindable role_params_relative role_params_binding

    /* [Pabble] Group Declarations */
%type <role_grp>      group_decl
%type <role_grp_list> group_decls
%type <role_list> roles roles_relative

    /* 3.7.1 Global Protocol Definitions */
%type <stmt> global_protocol_definition
%type <role>      role_instantiation
%type <role_list> role_instantiations role_instantiation_list
%type <msg>      argument
%type <msg_list> arguments argument_list

    /* 3.7.3 Global Interaction Sequences and Blocks */
%type <stmt> global_interaction_block global_interaction
%type <stmt_list> global_interaction_sequence

    /* 3.7.4 Global Message Transfer */
%type <stmt> global_message_transfer
%type <msg> message
%type <msg_list> messages

    /* 3.7.5 Global Choice */
%type <stmt>      global_choice
%type <stmt_list> or_global_interaction_blocks

    /* 3.7.6 Global Recursion */
%type <stmt> global_recursion global_continue

    /* 3.7.7 Global Parallel */
%type <stmt>      global_parallel
%type <stmt_list> and_global_interaction_blocks

    /* 3.7.8 Global Interruptible */
%type <stmt> global_interruptible
%type <interrupt>      global_interrupt
%type <interrupt_list> global_interrupts

    /* 3.7.9 Global Do */
%type <stmt> global_do

%type <stmt> global_for global_allreduce global_oneof

    /* 3.8 Local Protocol Declarations */
%type <protocol> local_protocol_decl
%type <protocol> local_protocol_header

    /* 3.8.1Local Protocol Definitions */
%type <stmt> local_protocol_definition

    /* 3.8.3 Local Interaction Blocks and Sequences */
%type <stmt> local_interaction_block local_interaction
%type <stmt_list> local_interaction_sequence

    /* 3.8.4 Local Send and Receive */
%type <stmt> local_send local_receive

    /* 3.8.5 Local Choice */
%type <stmt> local_choice
%type <stmt_list> or_local_interaction_blocks

    /* 3.8.6 Local Recursion */
%type <stmt> local_recursion local_continue

    /* 3.8.7 Local Parallel */
%type <stmt> local_parallel
%type <stmt_list> and_local_interaction_blocks

    /* 3.8.8 Local Interruptible */
%type <stmt> local_interruptible
%type <interrupt> local_throw local_catch
%type <interrupt_list> local_catches

    /* 3.8.9 Local Do */
%type <stmt> local_do

%type <stmt> local_for local_allreduce local_ifblock local_oneof


%left PLUS MINUS
%left MULTIPLY DIVIDE MODULO SHL SHR

%start module

%%

simple_name : IDENTIFIER { $$ = $1; }
           ;

/** 3.1 Primitive Names **/

annotation_name    : simple_name ;
parameter_name     : simple_name ;
recursion_var_name : simple_name ;
role_name          : simple_name ;
group_name         : simple_name ; /* Pabble */
scope_name         : simple_name ;

/** 3.2.1 Package, Module and Module Member Names **/

simple_module_name            : simple_name ;
simple_payload_type_name      : simple_name ;
simple_message_signature_name : simple_name ;
simple_protocol_name          : simple_name ;
simple_member_name            : simple_name ;
qualified_name                :                    IDENTIFIER { $$ = $1; }
                              | qualified_name DOT IDENTIFIER { $$ = join_str(join_str($1, "."), $3); }
                              ;

module_name  : qualified_name ;
member_name  : qualified_name ;

protocol_name         : member_name ;
payload_type_name     : member_name ;

/** 3.2.2 Top-Level Module Structure **/

module : module_decl import_decls datatype_decls const_decls range_decls protocol_decls { module = _module($1, $2, $3, $4, $5, $6); }
       ;

/** 3.2.3 Module Declarations **/

module_decl : MODULE module_name SEMICOLON { $$ = pbl_mk_module($2); } ;

/** 3.3 Import Delcarations **/

import_decl : import_module { $$ = $1; }
            | import_member { $$ = $1; }
            ;

import_decls : /* Empty */              { $$ = pbl_mk_import_list_init(); }
             | import_decls import_decl { $$ = pbl_import_list_add_import($1, $2); }
             ;

import_module : IMPORT module_name                       SEMICOLON { $$ = pbl_mk_import($2); }
              | IMPORT module_name AS simple_module_name SEMICOLON { $$ = pbl_mk_import_full($2, "", $4); }
              ;

import_member : FROM module_name IMPORT simple_member_name                       { $$ = pbl_mk_import_full($4, $2, ""); }
              | FROM module_name IMPORT simple_member_name AS simple_member_name { $$ = pbl_mk_import_full($4, $2, $6); }
              ;


/** 3.4 Payload Type Delcarations **/

payload_type_decl      : TYPE LANGLE IDENTIFIER RANGLE IDENTIFIER FROM IDENTIFIER AS simple_payload_type_name      SEMICOLON { $$ = pbl_mk_exttype_sig($9, $7, $5, $3); } ;
message_signature_decl : SIG  LANGLE IDENTIFIER RANGLE IDENTIFIER FROM IDENTIFIER AS simple_message_signature_name SEMICOLON { $$ = pbl_mk_exttype_type($9, $7, $5, $3); } ;

datatype_decl : payload_type_decl      { $$ = $1; }
              | message_signature_decl { $$ = $1; }
              ;

datatype_decls : /* Empty */                  { $$ = pbl_mk_exttype_list_init(); }
               | datatype_decls datatype_decl { $$ = pbl_exttype_list_add_type($1, $2); }
               ;

/** 3.5 Message Signatures **/

message_operator : IDENTIFIER { $$ = $1; } ;

message_signature : message_operator LPAREN          RPAREN { $$ = pbl_mk_msg($1); }
                  | message_operator LPAREN payloads RPAREN { $$ = pbl_mk_msg($1); pbl_msg_add_payload_list($$, $3); }
                  ;

payload_element :                       payload_type_name { $$ = pbl_mk_msg_payload($1); }
                | annotation_name COLON payload_type_name { $$ = pbl_mk_msg_payload_annotated($3, $1); }
                |                       parameter_name LSQUARE arith_expr RSQUARE { $$ = pbl_mk_msg_payload($1); pbl_msg_payload_add_arg($$, $3); }
                | annotation_name COLON parameter_name LSQUARE arith_expr RSQUARE { $$ = pbl_mk_msg_payload_annotated($3, $1); pbl_msg_payload_add_arg($$, $5); }
                ;

payloads :                payload_element { $$ = pbl_mk_msg_payload_list($1); }
         | payloads COMMA payload_element { $$ = pbl_msg_payload_list_add_msg_payload($1, $3); }
         ;


/** [Pabble] Constant Declarations **/

const_decl : CONST IDENTIFIER EQUAL DIGITS                 SEMICOLON { $$ = pbl_mk_val_const($2, $4); }
           | CONST IDENTIFIER EQUAL DIGITS NUMRANGE DIGITS SEMICOLON { $$ = pbl_mk_rng_const($2, $4, $6); }
           | CONST IDENTIFIER EQUAL DIGITS NUMRANGE INF    SEMICOLON { $$ = pbl_mk_inf_const($2, $4); }
           ;

const_decls : /* Empty */            { $$ = pbl_mk_const_list_init(); }
            | const_decls const_decl { $$ = pbl_const_list_add_const($1, $2); }
            ;


/** [Pabble] Range Declarations **/

range_decl : RANGE IDENTIFIER EQUAL DIGITS NUMRANGE DIGITS SEMICOLON { $$ = pbl_mk_rng($2, $4, $6); } ;

range_decls : /* Empty */            { $$ = pbl_mk_rng_list_init(); }
            | range_decls range_decl { $$ = pbl_rng_list_add_rng($1, $2); }
            ;


/** [Pabble] Expressions **/

arith_expr : IDENTIFIER                     { $$ = pbl_mk_var_expr($1); }
           | DIGITS                         { $$ = pbl_mk_val_expr($1); }
           | arith_expr PLUS     arith_expr { $$ = pbl_mk_add_expr($1, $3); }
           | arith_expr MINUS    arith_expr { $$ = pbl_mk_sub_expr($1, $3); }
           | arith_expr MULTIPLY arith_expr { $$ = pbl_mk_mul_expr($1, $3); }
           | arith_expr DIVIDE   arith_expr { $$ = pbl_mk_div_expr($1, $3); }
           | arith_expr MODULO   arith_expr { $$ = pbl_mk_mod_expr($1, $3); }
           | arith_expr SHL      arith_expr { $$ = pbl_mk_shl_expr($1, $3); }
           | arith_expr SHR      arith_expr { $$ = pbl_mk_shr_expr($1, $3); }
           | LPAREN arith_expr RPAREN       { $$ = $2; }
           ;

bind_expr : IDENTIFIER COLON range_expr { $$ = pbl_rng_expr_set_bindvar($3, $1); } ;

range_expr : arith_expr NUMRANGE arith_expr { $$ = pbl_mk_rng_expr(NULL, $1, $3); } ;


/** 3.6 Protocol Declarations **/

protocol_decl : global_protocol_decl { $$ = $1; }
              | local_protocol_decl  { $$ = $1; }
              ;

protocol_decls : /* Empty */                  { $$ = pbl_mk_protocol_list_init(); }
               | protocol_decls protocol_decl { $$ = pbl_protocol_list_add_protocol($1, $2); }
               ;


/** 3.7 Global Protocol Declarations **/

global_protocol_decl : global_protocol_header global_protocol_definition { pbl_protocol_set_body($1, $2); } ;

global_protocol_header : GLOBAL PROTOCOL simple_protocol_name                     role_decl_list { $$ = pbl_mk_protocol($3); pbl_protocol_add_role_list($$, $4->role_list); pbl_protocol_add_role_grp_list($$, $4->role_grp_list); }
                       | GLOBAL PROTOCOL simple_protocol_name parameter_decl_list role_decl_list { $$ = pbl_mk_protocol($3); pbl_protocol_add_role_list($$, $5->role_list); pbl_protocol_add_role_grp_list($$, $5->role_grp_list); }
                       ;


/** [Pabble] Role/Role Group Declaration **/

role_decl_list : LPAREN role_decls group_decls RPAREN { $$ = pbl_mk_role_decls($2, $3); } ;

role_decl : ROLE role_name role_params_decl { $$ = pbl_role_set_name($3, $2); } ;

role_params_decl : /* Empty */                                 { $$ = pbl_mk_role(""); }
                 | role_params_decl LSQUARE range_expr RSQUARE { $$ = pbl_role_add_param($1, $3); }
                 ;

role_decls :                  role_decl { $$ = pbl_mk_role_list($1); }
           | role_decls COMMA role_decl { $$ = pbl_role_list_add_role($1, $3); }
           ;


/** [Pabble] Role Parameters **/

role          : role_name role_params          { $$ = pbl_role_set_name($2, $1); } ;
role_binding  : role_name role_params_binding  { $$ = pbl_role_set_name($2, $1); } ;
role_relative : role_name role_params_relative { $$ = pbl_role_set_name($2, $1); } ;
role_bindable : role_name role_params_bindable { $$ = pbl_role_set_name($2, $1); } ;

role_params : /* Empty */                            { $$ = pbl_mk_role(""); }
            | role_params LSQUARE arith_expr RSQUARE { $$ = pbl_role_add_param($1, $3); }
            | role_params LSQUARE range_expr RSQUARE { $$ = pbl_role_add_param($1, $3); }
            ;

role_params_binding : /* Empty */                                    { $$ = pbl_mk_role(""); }
                    | role_params_bindable LSQUARE bind_expr RSQUARE { $$ = pbl_role_add_param($1, $3); }
                    ;

role_params_relative : /* Empty */                                     { $$ = pbl_mk_role(""); }
                     | role_params_relative LSQUARE arith_expr RSQUARE { $$ = pbl_role_add_param($1, $3); }
                     ;

role_params_bindable : /* Empty */                                     { $$ = pbl_mk_role(""); }
                     | role_params_bindable LSQUARE bind_expr  RSQUARE { $$ = pbl_role_add_param($1, $3); }
                     | role_params_bindable LSQUARE arith_expr RSQUARE { $$ = pbl_role_add_param($1, $3); }
                     | role_params_bindable LSQUARE range_expr RSQUARE { $$ = pbl_role_add_param($1, $3); }
                     ;


/** [Pabble] Group Declarations **/

group_decl : COMMA GROUP group_name EQUAL LBRACE roles RBRACE { $$ = pbl_mk_role_grp_from_role_list($3, $6); }
           ;

group_decls :                        { $$ = pbl_mk_role_grp_list_init(); }
            | group_decl group_decls { $$ = pbl_role_grp_list_add_role_grp($2, $1); }
            ;


roles :             role { $$ = pbl_mk_role_list($1); }
      | roles COMMA role { $$ = pbl_role_list_add_role($1, $3); }
      ;

roles_relative :                      role_relative { $$ = pbl_mk_role_list($1); }
               | roles_relative COMMA role_relative { $$ = pbl_role_list_add_role($1, $3); }
               ;

parameter_decl_list : LANGLE parameter_decls RANGLE ;

parameter_decl : TYPE parameter_name
               | SIG  parameter_name
               ;

parameter_decls :                       parameter_decl
                | parameter_decls COMMA parameter_decl
                ;


/** 3.7.1 Global Protocol Definitions **/

global_protocol_definition : global_interaction_block { $$ = $1; } ;

role_instantiation_list : LPAREN role_instantiations RPAREN { $$ = $2; } ;

role_instantiation : role { $$ = $1; } ;

role_instantiations :                           role_instantiation { $$ = pbl_mk_role_list($1); }
                    | role_instantiations COMMA role_instantiation { $$ = pbl_role_list_add_role($1, $3); }
                    ;


argument_list : LANGLE arguments RANGLE { $$ = $2; } ;

argument : message_signature { $$ = $1; }
         | parameter_name    { $$ = pbl_mk_msg("__PARAMETER__");  }
         ;

arguments :                 argument { $$ = pbl_mk_msg_list($1); }
          | arguments COMMA argument { $$ = pbl_msg_list_add_msg($1, $3); }
          ;



/** 3.7.3 Global Interaction Sequences and Blocks **/

global_interaction_block : LBRACE global_interaction_sequence RBRACE { $$ = _blk($2); } ;

global_interaction_sequence : /* Empty */                                    { $$ = pbl_mk_stmt_list_init(); }
                            | global_interaction global_interaction_sequence { $$ = pbl_stmt_list_add_stmt($2, $1);}
                            ;

/** [Pabble] For-each **/
/** [Pabble] All-reduce **/
/** [Pabble] One-of **/

global_interaction : global_message_transfer { $$ = $1; }
                   | global_choice           { $$ = $1; }
                   | global_recursion        { $$ = $1; }
                   | global_continue         { $$ = $1; }
                   | global_parallel         { $$ = $1; }
                   | global_interruptible    { $$ = $1; }
                   | global_do               { $$ = $1; }
                   | global_for              { $$ = $1; /*Pabble*/ }
                   | global_allreduce        { $$ = $1; /*Pabble*/ }
                   | global_oneof            { $$ = $1; /*Pabble*/ }
                   ;


/** 3.7.4 Global Message Transfer **/
/** [Pabble] No multple receivers **/

global_message_transfer : message FROM role_binding TO roles_relative SEMICOLON { $$ = _sendrecv($3, $5, $1); } ;

message : message_signature { $$ = $1; }
        | parameter_name    { $$ = pbl_mk_msg(/*label*/$1); }
        ;

messages :                message { $$ = pbl_mk_msg_list($1); }
         | messages COMMA message { $$ = pbl_msg_list_add_msg($1, $3); }
         ;


/** 3.7.5 Global Choice **/

global_choice : CHOICE AT role global_interaction_block or_global_interaction_blocks { $$ = _choice($3, $4, $5); }
              ;

or_global_interaction_blocks : /* Empty */                                              { $$ = pbl_mk_stmt_list_init(); }
                             | OR global_interaction_block or_global_interaction_blocks { $$ = pbl_stmt_list_add_stmt($3, $2); }
                             ;


/** 3.7.6 Global Recursion **/

global_recursion : REC recursion_var_name global_interaction_block { $$ = _recur($2, $3); } ;

global_continue : CONTINUE recursion_var_name SEMICOLON { $$ = _continue($2); } ;


/** 3.7.7 Global Parallel **/

global_parallel : PAR global_interaction_block and_global_interaction_blocks { $$ = _par($2, $3); }
                ;

and_global_interaction_blocks : /* Empty */                                                { $$ = pbl_mk_stmt_list_init(); }
                              | AND global_interaction_block and_global_interaction_blocks { $$ = pbl_stmt_list_add_stmt($3, $2); }
                              ;


/** 3.7.8 Global Interruptible **/

global_interruptible : INTERRUPTIBLE                  global_interaction_block WITH LBRACE global_interrupts RBRACE { $$ = _interruptible($2, $5); }
                     | INTERRUPTIBLE scope_name COLON global_interaction_block WITH LBRACE global_interrupts RBRACE { $$ = _interruptible_name($4, $7, $2); }
                     ;

global_interrupt : messages BY role SEMICOLON { $$ = pbl_mk_interrupt($3, $1); } ;

global_interrupts : /* Empty */                        { $$ = pbl_mk_interrupt_list_init(); }
                  | global_interrupts global_interrupt { $$ = pbl_interrupt_list_add_interrupt($1, $2); }
                  ;


/** 3.7.9 Global Do **/

global_do : DO protocol_name               role_instantiation_list SEMICOLON { $$ = _do($2, $3); }
          | DO protocol_name argument_list role_instantiation_list SEMICOLON { $$ = _do_arg($2, $4, $3); }
          ;


/** [Pabble] For-each **/

global_for : FOR LPAREN bind_expr                   RPAREN global_interaction_block { $$ = _for($3, $5); }
           | FOR LPAREN bind_expr EXCEPT arith_expr RPAREN global_interaction_block { $$ = _for_except($3, $7, $5); }
           ;


/** [Pabble] All-reduce **/

global_allreduce : ALLREDUCE message_signature { $$ = _allreduce($2); }
          ;


/** [Pabble] One-of **/

global_oneof :        ONEOF LPAREN IDENTIFIER LSQUARE IDENTIFIER IN range_expr RSQUARE RPAREN global_interaction_block { $$ = _oneof(       /*role*/$3, /*exist_var*/$5, $7, $10); }
             | REPEAT ONEOF LPAREN IDENTIFIER LSQUARE IDENTIFIER IN range_expr RSQUARE RPAREN global_interaction_block { $$ = _oneof_repeat(/*role*/$4, /*exist_var*/$6, $8, $11); }

             ;


/** 3.8 Local Protocol Declarations **/

local_protocol_decl : local_protocol_header local_protocol_definition { pbl_protocol_set_body($1, $2); } ;

local_protocol_header : LOCAL PROTOCOL protocol_name AT role                     role_decl_list { $$ = pbl_mk_protocol($3); pbl_protocol_set_endpoint($$, $5); }
                      | LOCAL PROTOCOL protocol_name AT role parameter_decl_list role_decl_list { $$ = pbl_mk_protocol($3); pbl_protocol_set_endpoint($$, $5); }
                      ;


/** 3.8.1 Local Protocol Definitions **/

local_protocol_definition : local_interaction_block { $$ = $1; } ;


/** 3.8.3 Local Interaction Blocks and Sequences **/

local_interaction_block : LBRACE local_interaction_sequence RBRACE { $$ = _blk($2); } ;

local_interaction_sequence : /* Empty */                                  { $$ = pbl_mk_stmt_list_init(); }
                           | local_interaction local_interaction_sequence { $$ = pbl_stmt_list_add_stmt($2, $1); }
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
                  | local_interruptible { $$ = $1; }
                  | local_do            { $$ = $1; }
                  | local_for           { $$ = $1; /*Pabble*/ }
                  | local_allreduce     { $$ = $1; /*Pabble*/ }
                  | local_ifblock       { $$ = $1; /*Pabble*/ }
                  | local_oneof         { $$ = $1; /*Pabble*/ }
                  ;


/** 3.8.4 Local Send and Receive **/

local_send :                  message TO roles_relative SEMICOLON { $$ = _send($3, $1); }
           | IF role_bindable message TO roles_relative SEMICOLON { $$ = _send_cond($5, $3, $2); }

local_receive :                  message FROM role_relative SEMICOLON { $$ = _recv($3, $1); }
              | IF role_bindable message FROM role_relative SEMICOLON { $$ = _recv_cond($5, $3, $2); }
              ;


/** 3.8.5 Local Choice **/

local_choice : CHOICE AT role local_interaction_block or_local_interaction_blocks { $$ = _choice($3, $4, $5); }
             ;

or_local_interaction_blocks : /* Empty */                                            { $$ = pbl_mk_stmt_list_init(); }
                            | OR local_interaction_block or_local_interaction_blocks { $$ = pbl_stmt_list_add_stmt($3, $2); }
                            ;


/** 3.8.6 Local Recursion **/

local_recursion : REC recursion_var_name local_interaction_block { $$ = _recur($2, $3) ;} ;

local_continue : CONTINUE recursion_var_name SEMICOLON { $$ = _continue($2); } ;


/** 3.8.7 Local Parallel **/

local_parallel : PAR local_interaction_block and_local_interaction_blocks { $$ = _par($2, $3); } ;

and_local_interaction_blocks : /* Empty */                                              { $$ = pbl_mk_stmt_list_init(); }
                             | AND local_interaction_block and_local_interaction_blocks { $$ = pbl_stmt_list_add_stmt($3, $2); }
                             ;


/** 3.8.8 Local Interruptible **/

local_interruptible : INTERRUPTIBLE scope_name COLON local_interaction_block WITH LBRACE                           RBRACE { $$ = _interruptible_throw_catch_name($4, NULL, NULL, $2); }
                    | INTERRUPTIBLE scope_name COLON local_interaction_block WITH LBRACE             local_catches RBRACE { $$ = _interruptible_throw_catch_name($4, NULL, $7, $2); }
                    | INTERRUPTIBLE scope_name COLON local_interaction_block WITH LBRACE local_throw               RBRACE { $$ = _interruptible_throw_catch_name($4, $7, NULL, $2); }
                    | INTERRUPTIBLE scope_name COLON local_interaction_block WITH LBRACE local_throw local_catches RBRACE { $$ = _interruptible_throw_catch_name($4, $7, $8, $2); }
                    ;

local_throw : THROWS messages TO roles SEMICOLON  { $$ = pbl_mk_interrupt_roles($4, $2); };

local_catch : CATCHES messages FROM role SEMICOLON { $$ = pbl_mk_interrupt($4, $2); } ;

local_catches :                     local_catch { $$ = pbl_mk_interrupt_list_init(); pbl_interrupt_list_add_interrupt($$, $1); }
              | local_catches COMMA local_catch { $$ = pbl_interrupt_list_add_interrupt($1, $3); }
              ;


/** 3.8.9 Local Do **/

local_do : DO            COLON protocol_name               role_instantiation_list SEMICOLON { $$ = _do($3, $4); }
         | DO            COLON protocol_name argument_list role_instantiation_list SEMICOLON { $$ = _do_arg($3, $5, $4); }
         | DO scope_name COLON protocol_name               role_instantiation_list SEMICOLON { $$ = _do_scope($4, $5, $2); }
         | DO scope_name COLON protocol_name argument_list role_instantiation_list SEMICOLON { $$ = _do_arg_scope($4, $6, $5, $2); }
         ;


/** [Pabble] For-each **/

local_for : FOR LPAREN bind_expr                   RPAREN local_interaction_block { $$ = pbl_mk_forstmt($3, $5); }
          | FOR LPAREN bind_expr EXCEPT arith_expr RPAREN local_interaction_block { $$ = pbl_mk_forstmt_except($3, $7, $5); }
          ;


/** [Pabble] All-reduce **/

local_allreduce : ALLREDUCE message_signature { $$ = _allreduce($2); }
                ;


/** [Pabble] If-block **/

local_ifblock : IF role local_interaction_block { $$ = _if($2, $3); }
              ;


/** [Pabble] One-of **/

local_oneof : ONEOF LPAREN IDENTIFIER LSQUARE IDENTIFIER IN range_expr RSQUARE RPAREN local_interaction_block { $$ = _oneof($3, $5, $7, $10); }
            ;

%%
