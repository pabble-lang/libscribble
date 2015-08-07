#ifndef SCRIBBLE__PARSER_HELPER__H__
#define SCRIBBLE__PARSER_HELPER__H__

#include <assert.h>
#include <string.h>

#include <scribble/parameterised/msg.h>
#include <scribble/parameterised/ast.h>
#include <scribble/parameterised/ast/block_stmt.h>
#include <scribble/parameterised/ast/choice_stmt.h>
#include <scribble/parameterised/ast/sendrecv_stmt.h>
#include <scribble/parameterised/ast/send_stmt.h>
#include <scribble/parameterised/ast/recv_stmt.h>
#include <scribble/parameterised/ast/recur_stmt.h>
#include <scribble/parameterised/ast/continue_stmt.h>
#include <scribble/parameterised/ast/interruptible_stmt.h>
#include <scribble/parameterised/ast/par_stmt.h>
#include <scribble/parameterised/ast/do_stmt.h>
#include <scribble/parameterised/ast/for_stmt.h>
#include <scribble/parameterised/ast/allreduce_stmt.h>
#include <scribble/parameterised/ast/oneof_stmt.h>
#include <scribble/parameterised/ast/ifblk_stmt.h>
#include <scribble/parameterised/ast/expr.h>
#include <scribble/parameterised/ast/var_expr.h>
#include <scribble/parameterised/ast/val_expr.h>
#include <scribble/parameterised/ast/add_expr.h>
#include <scribble/parameterised/ast/sub_expr.h>
#include <scribble/parameterised/ast/mul_expr.h>
#include <scribble/parameterised/ast/div_expr.h>
#include <scribble/parameterised/ast/mod_expr.h>
#include <scribble/parameterised/ast/shl_expr.h>
#include <scribble/parameterised/ast/shr_expr.h>
#include <scribble/parameterised/ast/seq_expr.h>
#include <scribble/parameterised/ast/rng_expr.h>
#include <scribble/parameterised/module.h>
#include <scribble/parameterised/import.h>
#include <scribble/parameterised/protocol.h>
#include <scribble/parameterised/role.h>
#include <scribble/parameterised/role_grp.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  char *name;
  st_expr *expr;
} symbol_t;

typedef struct {
  unsigned int count;
  symbol_t **symbols;
} symbol_table_t;

typedef struct {
  unsigned int count;
  pbl_msg_payload **payloads;
} pbl_msg_payload_list;

typedef struct {
    unsigned int count;
    pbl_protocol **protocols;
} pbl_protocol_list;

typedef struct {
    unsigned int count;
    pbl_import **imports;
} pbl_import_list;

typedef struct {
    unsigned int count;
    pbl_msg **msgs;
} pbl_msg_list;

typedef struct {
    unsigned int count;
    pbl_stmt **stmts;
} pbl_stmt_list;

typedef struct {
    unsigned int count;
    pbl_role **roles;
} pbl_role_list;

typedef struct {
    unsigned int count;
    pbl_role_grp **role_grps;
} pbl_role_grp_list;

typedef struct {
    union {
        pbl_role *role;
        pbl_role_list *role_list;
    };
    pbl_msg_list *msg_list;
} pbl_interrupt;

typedef struct {
    unsigned int count;
    pbl_interrupt **interrupts;
} pbl_interrupt_list;

typedef struct {
    pbl_role_list *role_list;
    pbl_role_grp_list *role_grp_list;
} pbl_role_decls;

typedef struct {
    unsigned int count;
    pbl_rng **ranges;
} pbl_rng_list;

typedef struct {
    unsigned int count;
    pbl_exttype **types;
} pbl_exttype_list;

typedef struct {
    unsigned int count;
    pbl_const **consts;
} pbl_const_list;

static symbol_table_t symbol_table;

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-function"

static char *join_str(const char *s0, const char *s1)
{
    int str_len = strlen(s0) + strlen(s1);
    char *s = (char *)calloc(str_len+1, sizeof(char));
    sprintf(s, "%s%s", s0, s1);
    return s;
}

static pbl_module *_module(pbl_module *module, pbl_import_list *imports, pbl_exttype_list *types, pbl_const_list *consts, pbl_rng_list *rngs, pbl_protocol_list *protocols)
{
    for (int i=0; i<imports->count; i++) {
        pbl_module_add_import(module, imports->imports[i]);
    }
    for (int i=0; i<types->count; i++) {
        pbl_module_add_exttype(module, types->types[i]);
    }
    for (int i=0; i<consts->count; i++) {
        pbl_module_add_const(module, consts->consts[i]);
    }
    for (int i=0; i<rngs->count; i++) {
        pbl_module_add_range(module, rngs->ranges[i]);
    }
    for (int i=0; i<protocols->count; i++) {
        pbl_module_add_protocol(module, protocols->protocols[i]);
    }
    return module;
}

static pbl_exttype_list *pbl_mk_exttype_list_init()
{
    pbl_exttype_list *list = (pbl_exttype_list *)malloc(sizeof(pbl_exttype_list));
    list->count = 0;
    return list;
}

static pbl_exttype_list *pbl_exttype_list_add_type(pbl_exttype_list *list, pbl_exttype *type)
{
    list->count++;
    list->types = (pbl_exttype **)realloc(list->types, list->count * sizeof(pbl_exttype *));
    list->types[list->count-1] = type;
    return list;
}

static pbl_const_list *pbl_mk_const_list_init()
{
    pbl_const_list *list = (pbl_const_list *)malloc(sizeof(pbl_const_list));
    list->count = 0;
    return list;
}

static pbl_const_list *pbl_const_list_add_const(pbl_const_list *list, pbl_const *con)
{
    list->count++;
    list->consts = (pbl_const **)realloc(list->consts, list->count * sizeof(pbl_const *));
    list->consts[list->count-1] = con;
    return list;
}

static pbl_import_list *pbl_mk_import_list_init()
{
    pbl_import_list *list = (pbl_import_list *)malloc(sizeof(pbl_import_list));
    list->count = 0;
    return list;
}

static pbl_import_list *pbl_import_list_add_import(pbl_import_list *list, pbl_import *import)
{
    list->count++;
    list->imports = (pbl_import **)realloc(list->imports, sizeof(pbl_import *) * list->count);
    list->imports[list->count-1] = import;
    return list;
}

static pbl_protocol *pbl_protocol_add_role_list(pbl_protocol *protocol, pbl_role_list *role_list)
{
    for (int i=0; i<role_list->count; i++) {
        pbl_protocol_add_role(protocol, role_list->roles[i]);
    }
    return protocol;
}

static pbl_protocol *pbl_protocol_add_role_grp_list(pbl_protocol *protocol, pbl_role_grp_list *role_grp_list)
{
    for (int i=0; i<role_grp_list->count; i++) {
        pbl_protocol_add_role_grp(protocol, role_grp_list->role_grps[i]);
    }
    return protocol;
}

static pbl_protocol_list *pbl_mk_protocol_list_init()
{
    pbl_protocol_list *list = (pbl_protocol_list *)malloc(sizeof(pbl_protocol_list));
    list->count = 0;
    return list;
}

static pbl_protocol_list *pbl_protocol_list_add_protocol(pbl_protocol_list *list, pbl_protocol *protocol)
{
    list->count++;
    list->protocols = (pbl_protocol **)realloc(list->protocols, list->count * sizeof(pbl_protocol *));
    list->protocols[list->count-1] = protocol;
    return list;
}

static pbl_msg_list *pbl_mk_msg_list(pbl_msg *msg)
{
    pbl_msg_list *list = (pbl_msg_list *)malloc(sizeof(pbl_msg_list));
    list->count = 1;
    list->msgs = (pbl_msg **)calloc(list->count, sizeof(pbl_msg *));
    list->msgs[0] = msg;
    return list;
}

static pbl_msg_list *pbl_msg_list_add_msg(pbl_msg_list *list, pbl_msg *msg)
{
    list->count++;
    list->msgs = (pbl_msg **)realloc(list->msgs, list->count * sizeof(pbl_msg *));
    list->msgs[list->count-1] = msg;
    return list;
}

static pbl_role_list *pbl_mk_role_list(pbl_role *role)
{
    pbl_role_list *list = (pbl_role_list *)malloc(sizeof(pbl_role_list));
    list->count = 1;
    list->roles = (pbl_role **)calloc(list->count, sizeof(pbl_role *));
    list->roles[0] = role;
    return list;
}

static pbl_role_list *pbl_role_list_add_role(pbl_role_list *list, pbl_role *role)
{
    list->count++;
    list->roles = (pbl_role **)realloc(list->roles, list->count * sizeof(pbl_role *));
    list->roles[list->count-1] = role;
    return list;
}

static pbl_role_grp_list *pbl_mk_role_grp_list_init()
{
    pbl_role_grp_list *list = (pbl_role_grp_list *)malloc(sizeof(pbl_role_grp_list));
    list->count = 0;
    return list;
}

static pbl_role_grp_list *pbl_role_grp_list_add_role_grp(pbl_role_grp_list *list, pbl_role_grp *role_grp)
{
    list->count++;
    list->role_grps = (pbl_role_grp **)realloc(list->role_grps, list->count * sizeof(pbl_role_grp *));
    list->role_grps[list->count-1] = role_grp;
    return list;
}

static pbl_rng_list *pbl_mk_rng_list_init()
{
    pbl_rng_list *list = (pbl_rng_list *)malloc(sizeof(pbl_rng_list));
    list->count = 0;
    return list;
}

static pbl_rng_list *pbl_rng_list_add_rng(pbl_rng_list *list, pbl_rng *rng)
{
    list->count++;
    list->ranges = (pbl_rng **)realloc(list->ranges, list->count * sizeof(pbl_rng *));
    list->ranges[list->count-1] = rng;
    return list;
}

static pbl_stmt_list *pbl_mk_stmt_list_init()
{
    pbl_stmt_list *list = (pbl_stmt_list *)malloc(sizeof(pbl_stmt_list));
    list->count = 0;
    return list;
}

static pbl_stmt_list *pbl_stmt_list_add_stmt(pbl_stmt_list *list, pbl_stmt *stmt)
{
    list->count++;
    list->stmts = (pbl_stmt **)realloc(list->stmts, list->count * sizeof(pbl_stmt *));
    list->stmts[list->count-1] = stmt;
    return list;
}

static pbl_msg_payload_list *pbl_mk_msg_payload_list(pbl_msg_payload *payload)
{
    pbl_msg_payload_list *list = (pbl_msg_payload_list *)malloc(sizeof(pbl_msg_payload_list));
    list->count = 1;
    list->payloads = (pbl_msg_payload **)calloc(list->count, sizeof(pbl_msg_payload *));
    list->payloads[list->count-1] = payload;
    return list;
}

static pbl_msg_payload_list *pbl_msg_payload_list_add_msg_payload(pbl_msg_payload_list *list, pbl_msg_payload *payload)
{
    list->count++;
    list->payloads = (pbl_msg_payload **)realloc(list->payloads, list->count * sizeof(pbl_msg_payload *));
    list->payloads[list->count-1] = payload;
    return list;
}

static pbl_stmt *_sendrecv(pbl_role *from, pbl_role_list *tos, pbl_msg *msg)
{
    pbl_stmt *stmt = pbl_mk_sendrecvstmt(msg);
    pbl_recvstmt_set_from(stmt, from);
    for (int i=0; i<tos->count; i++) {
        pbl_sendstmt_add_to(stmt, tos->roles[i]);
    }
    return stmt;
}

static pbl_stmt *_blk(pbl_stmt_list *body)
{
    pbl_stmt *stmt = pbl_mk_blkstmt_init();
    for (int i=0; i<body->count; i++) {
        pbl_blkstmt_append_child(stmt, body->stmts[i]);
    }
    return stmt;
}

static pbl_stmt *_choice(pbl_role *at, pbl_stmt *head, pbl_stmt_list *tail)
{
    pbl_stmt *stmt = pbl_mk_choicestmt(at);
    pbl_blkstmt_append_child(stmt, head);
    for (int i=0; i<tail->count; i++) {
        pbl_blkstmt_append_child(stmt, tail->stmts[i]);
    }
    return stmt;
}

static pbl_stmt *_send(pbl_role_list *tos, pbl_msg *msg)
{
    pbl_stmt *stmt = pbl_mk_sendstmt(msg);
    for (int i=0; i<tos->count; i++) {
        pbl_sendstmt_add_to(stmt, tos->roles[i]);
    }
    return stmt;
}

static pbl_stmt *_send_cond(pbl_role_list *tos, pbl_msg *msg, pbl_cond *cond)
{
    pbl_stmt *stmt = _send(tos, msg);
    pbl_sendstmt_set_cond(stmt, cond);
    return stmt;
}

static pbl_stmt *_recv(pbl_role *from, pbl_msg *msg)
{
    pbl_stmt *stmt = pbl_mk_recvstmt(msg);
    pbl_recvstmt_set_from(stmt, from);
    return stmt;
}

static pbl_stmt *_recv_cond(pbl_role *from, pbl_msg *msg, pbl_cond *cond)
{
    pbl_stmt *stmt = _recv(from, msg);
    pbl_recvstmt_set_cond(stmt, cond);
    return stmt;
}

static pbl_stmt *_recur(char *label, pbl_stmt *body)
{
    pbl_stmt *stmt = pbl_mk_recurstmt(label);
    for (int i=0; i<pbl_blkstmt_num_children(body); i++) {
        pbl_blkstmt_append_child(stmt, pbl_blkstmt_get_child(body, i));
    }
    pbl_blkstmt_free(body);
    return stmt;
}

static pbl_stmt *_continue(char *label)
{
    pbl_stmt *stmt = pbl_mk_continuestmt(label);
    return stmt;
}

static pbl_stmt *_par(pbl_stmt *head, pbl_stmt_list *tail)
{
    pbl_stmt *stmt = pbl_mk_parstmt_init();
    pbl_blkstmt_append_child(stmt, head);
    for (int i=0; i<tail->count; i++) {
        pbl_blkstmt_append_child(stmt, tail->stmts[i]);
    }
    return stmt;
}

static pbl_stmt *_interruptible(pbl_stmt *body, pbl_interrupt_list *list)
{
    pbl_stmt *stmt = pbl_mk_interruptiblestmt_init();
    for (int i=0; i<list->count; i++) {
        for (int j=0; j<list->interrupts[i]->msg_list->count; j++) {
            pbl_interruptiblestmt_add_interrupt(stmt, list->interrupts[i]->role, list->interrupts[i]->msg_list->msgs[j]);
        }
    }
    return stmt;
}

static pbl_stmt *_interruptible_name(pbl_stmt *body, pbl_interrupt_list *list, char *scope_name)
{
    pbl_stmt *stmt = _interruptible(body, list);
    pbl_interruptiblestmt_set_scope(stmt, scope_name);
    return stmt;
}


static pbl_stmt *_interruptible_throw_catch_name(pbl_stmt *body, pbl_interrupt *thr, pbl_interrupt_list *cat, char *scope_name)
{
    pbl_stmt *stmt = pbl_mk_interruptiblestmt_init();
    if (thr!=NULL) {
        for (int j=0; j<thr->role_list->count; j++) {
            for (int i=0; i<thr->msg_list->count; i++) {
                pbl_interruptiblestmt_add_throw(stmt, thr->role_list->roles[j], thr->msg_list->msgs[i]);
            }
        }
    }
    if (cat!=NULL) {
        for (int i=0; i<cat->count; i++) {
            for (int j=0; j<cat->interrupts[i]->msg_list->count; j++) {
                pbl_interruptiblestmt_add_throw(stmt, cat->interrupts[i]->role, cat->interrupts[i]->msg_list->msgs[j]);
            }
        }
    }
    pbl_interruptiblestmt_set_scope(stmt, scope_name);
    return stmt;
}

static pbl_interrupt *pbl_mk_interrupt(pbl_role *role, pbl_msg_list *msg_list)
{
    pbl_interrupt *interrupt = (pbl_interrupt *)malloc(sizeof(pbl_interrupt));
    interrupt->role = role;
    interrupt->msg_list = msg_list;
    return interrupt;
}

static pbl_interrupt *pbl_mk_interrupt_roles(pbl_role_list *role_list, pbl_msg_list *msg_list)
{
    pbl_interrupt *interrupt = (pbl_interrupt *)malloc(sizeof(pbl_interrupt));
    interrupt->role_list = role_list;
    interrupt->msg_list = msg_list;
    return interrupt;
}

static pbl_interrupt_list *pbl_mk_interrupt_list_init()
{
    pbl_interrupt_list *list = (pbl_interrupt_list *)malloc(sizeof(pbl_interrupt_list));
    list->count = 0;
    return list;
}

static pbl_interrupt_list *pbl_interrupt_list_add_interrupt(pbl_interrupt_list *list, pbl_interrupt *interrupt)
{
    list->count++;
    list->interrupts = (pbl_interrupt **)realloc(list->interrupts, list->count * sizeof(pbl_interrupt *));
    list->interrupts[list->count-1] = interrupt;
    return list;
}

static pbl_stmt *_do(char *protocol_name, pbl_role_list *role_list)
{
    pbl_stmt *stmt = pbl_mk_dostmt(protocol_name);
    for (int i=0; i<role_list->count; i++) {
        pbl_dostmt_add_rolearg(stmt, role_list->roles[i]);
    }
    return stmt;
}

static pbl_stmt *_do_arg(char *protocol_name, pbl_role_list *roleargs, pbl_msg_list *args)
{
    pbl_stmt *stmt = pbl_mk_dostmt(protocol_name);
    for (int i=0; i<roleargs->count; i++) {
        pbl_dostmt_add_rolearg(stmt, roleargs->roles[i]);
    }
    for (int i=0; i<args->count; i++) {
        pbl_dostmt_add_arg(stmt, args->msgs[i]);
    }
    return stmt;
}

static pbl_stmt *_do_scope(char *protocol_name, pbl_role_list *roleargs, char *scope_name)
{
    pbl_stmt *stmt = pbl_mk_dostmt(protocol_name);
    for (int i=0; i<roleargs->count; i++) {
        pbl_dostmt_add_rolearg(stmt, roleargs->roles[i]);
    }
    pbl_dostmt_set_scope(stmt, scope_name);
    return stmt;
}

static pbl_stmt *_do_arg_scope(char *protocol_name, pbl_role_list *roleargs, pbl_msg_list *args, char *scope_name)
{
    pbl_stmt *stmt = pbl_mk_dostmt(protocol_name);
    for (int i=0; i<roleargs->count; i++) {
        pbl_dostmt_add_rolearg(stmt, roleargs->roles[i]);
    }
    for (int i=0; i<args->count; i++) {
        pbl_dostmt_add_arg(stmt, args->msgs[i]);
    }
    pbl_dostmt_set_scope(stmt, scope_name);
    return stmt;
}

static pbl_stmt *_for(pbl_expr *expr, pbl_stmt *body)
{
    pbl_stmt *stmt = pbl_mk_forstmt(expr, body);
    for (int i=0; i<pbl_blkstmt_num_children(body); i++) {
        pbl_blkstmt_append_child(stmt, pbl_blkstmt_get_child(body, i));
    }
    return stmt;
}


static pbl_stmt *_for_except(pbl_expr *expr, pbl_stmt *body, pbl_expr *except)
{
    pbl_stmt *stmt = pbl_mk_forstmt_except(expr, body, except);
    return stmt;
}

static pbl_stmt *_allreduce(pbl_msg *msg)
{
    pbl_stmt *stmt = pbl_mk_allreducestmt(msg);
    return stmt;
}

static pbl_stmt *_oneof(char *rolename, char *var, pbl_expr *range, pbl_stmt *body)
{
    pbl_stmt *stmt = pbl_mk_oneofstmt(pbl_mk_role(rolename), 0);
    pbl_oneofstmt_set_var(stmt, var);
    pbl_oneofstmt_set_range(stmt, range);
    for (int i=0; i<pbl_blkstmt_num_children(body); i++) {
        pbl_blkstmt_append_child(stmt, pbl_blkstmt_get_child(body, i));
    }
    return stmt;
}

static pbl_stmt *_oneof_repeat(char *rolename, char *var, pbl_expr *range, pbl_stmt *body)
{
    pbl_stmt *stmt = pbl_mk_oneofstmt(pbl_mk_role(rolename), 0);
    pbl_oneofstmt_set_var(stmt, var);
    pbl_oneofstmt_set_range(stmt, range);
    pbl_oneofstmt_set_repeat(stmt, true);
    for (int i=0; i<pbl_blkstmt_num_children(body); i++) {
        pbl_blkstmt_append_child(stmt, pbl_blkstmt_get_child(body, i));
    }
    return stmt;
}

static pbl_stmt *_if(pbl_role *role, pbl_stmt *body)
{
    pbl_stmt *stmt = pbl_mk_ifblkstmt(role);
    for (int i=0; i<pbl_blkstmt_num_children(body); i++) {
        pbl_blkstmt_append_child(stmt, pbl_blkstmt_get_child(body, i));
    }
    return stmt;
}

static pbl_msg *pbl_msg_add_payload_list(pbl_msg *msg, pbl_msg_payload_list *list)
{
    for (int i=0; i<list->count; i++) {
        pbl_msg_add_payload(msg, list->payloads[i]);
    }
    return msg;
}

static pbl_role_decls *pbl_mk_role_decls(pbl_role_list *role_list, pbl_role_grp_list *role_grp_list)
{
    pbl_role_decls *list = (pbl_role_decls *)malloc(sizeof(pbl_role_decls));
    list->role_list = role_list;
    list->role_grp_list = role_grp_list;
    return list;
}

static pbl_role_grp *pbl_mk_role_grp_from_role_list(char *name, pbl_role_list *list)
{
    pbl_role_grp *grp = pbl_mk_role_grp(name);
    for (int i=0; i<list->count; i++) {
        pbl_role_grp_add_role(grp, list->roles[i]);
    }
    return grp;
}

#pragma clang diagnostic pop

#ifdef __cplusplus
} // extern "C"
#endif

#endif // SCRIBBLE__PARSER_HELPER__H__
