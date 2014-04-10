#include "ast.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct env_node {
    struct env_node* parent;
    char* name;
    struct ast_node* value;
};

struct env_node* env_empty() {
    return NULL;
};

struct env_node* env_add(struct env_node* parent, char* name, struct ast_node* value) {
    struct env_node* n = malloc(sizeof(struct env_node));
    struct ast_node* v = malloc(sizeof(struct ast_node));

    memcpy(v, value, sizeof(struct ast_node));

    n->name = name;
    n->value = v;
    n->parent = parent;

    return n;
}

struct env_node* env_free_head(struct env_node* env) {
    struct env_node* parent = env->parent;
    free(env->value);
    free(env);
    return parent;
}

struct env_node* env_find_node(struct env_node* env, char* name) {
    struct env_node* e = env;
    while (NULL != e) {
        if (0 == strcmp(name, e->name)) {
            return e;
        } else {
            e = e->parent;
        }
    }
    return NULL;
}

struct ast_node* env_lookup(struct env_node* env, char* name) {
    struct env_node* e = env_find_node(env, name);

    if (NULL != e) {
        return e->value;
    } else {
        return NULL;
    }
}

void env_print(struct env_node* env) {
    struct env_node* e = env;
    while (NULL!=e) {
        printf("%s = ", e->name);
        if (INT == e->value->type) {
            printf("%ld, ", e->value->integer);
        } else {
            printf("??, ");
        }
        e = e->parent;
    }
}

#define INTER_ERROR(args...) do{ fprintf(stderr, "Error: "); fprintf(stderr, args); fprintf(stderr,"\n"); exit(1); }while(0)

void inter_assumes_type(struct ast_node* node, enum ast_node_type type, char* err) {
    if (node->type != type) {
        INTER_ERROR("Wrong ast_node_type: expected %d but got %d: %s", type, node->type, err);
    }
}

void inter_assumes_not_null(void* ptr, char* err) {
    if (NULL == ptr) {
        fprintf(stderr, "Unexpected null: %s", err);
        exit(1);
    }
}


struct inter_context {
    struct ast_node* node;
    struct env_node* env;
};

struct inter_context* inter(struct inter_context* ctx) {
    static struct ast_node ret = { .type = INT, .integer = 0 };
    static struct inter_context cret;
    static struct env_node* recur_env = NULL;
    static struct ast_node* current_loop = NULL;

    if (NULL == ctx) {
        return NULL;
    }

    struct env_node* env = ctx->env;
    struct ast_node* node = ctx->node;
    cret.env = env;
    cret.node = &ret;

    if (NULL == node) {
        return &cret;
    }

    switch (node->type) {
    case INT:
        cret.node = node;
        return &cret;
        break;

    case UNARY:
        {
            cret.node = node->unary.child;
            struct inter_context* childctx = inter(&cret);
            inter_assumes_not_null(childctx->node, "child of unary");
            inter_assumes_type(childctx->node, INT, "child of unary");
            ret.type = INT;
            switch (node->unary.op) {
            case '!':
                ret.integer = !childctx->node->integer;
                break;
            case '-':
                ret.integer = -childctx->node->integer;
                break;
            default:
                INTER_ERROR("unknown op: %c", node->unary.op);
            }
            return &cret;
        }
        break;
    case BINARY:
        {
            cret.node = node->binary.left;
            struct inter_context* childctx = inter(&cret);
            inter_assumes_not_null(childctx->node, "left child of binary");
            inter_assumes_type(childctx->node, INT, "left child of binary");
            long left = childctx->node->integer;

            if (((0==left) && ('&'==node->binary.op))
                || ((0!=left) && ('|'==node->binary.op))) {
                ret.type = INT;
                ret.integer = left;
                return &cret;
            }
            cret.node = node->binary.right;
            childctx = inter(&cret);
            inter_assumes_not_null(childctx->node, "right child of binary");
            inter_assumes_type(childctx->node, INT, "right child of binary");
            long right = childctx->node->integer;

            ret.type = INT;
            switch(node->binary.op) {
            case '+':
                ret.integer = left + right;
                break;
            case '*':
                ret.integer = left * right;
                break;
            case '|':
                ret.integer = left || right;
                break;
            case '&':
                ret.integer = left && right;
                break;
            case '<':
                ret.integer = left < right;
                break;
            case '=':
                ret.integer = left == right;
                break;
            default:
                INTER_ERROR("unknown binary op: %c", node->binary.op);
                break;
            }
            cret.node = &ret;
        }
        return &cret;
        break;

    case FUNDEF:
        cret.env = env_add(env, list_first(node->binding.name_and_args), node);
        cret.node = NULL;

        return &cret;
        break;

    case VALDEF:
        {
            cret.node = node->binding.value;
            struct inter_context* childctx = inter(&cret);
            inter_assumes_not_null(childctx->node, "value in binding");
            inter_assumes_type(childctx->node, INT, "value in binding");

            cret.env = env_add(env, node->binding.name, childctx->node);
            cret.node = NULL;
            return &cret;
        }
        break;

    case VAR:
        cret.env = env;
        cret.node = env_lookup(env, node->string);
        inter_assumes_type(cret.node, INT, "value of var");
        ret.type = INT;
        ret.integer = cret.node->integer;

        cret.node = &ret;
        return &cret;
        break;

    case FUNCALL:
        {
            struct env_node* old_env = env;
            struct env_node* e = env_find_node(env, node->call.name);
            e = e->parent;
            struct ast_node* a = env_lookup(env, node->call.name);
            if (NULL == a) {
                INTER_ERROR("undefined function called: %s", node->call.name);
            }
            if (FUNDEF != a->type) {
                INTER_ERROR("called non function object: %s", node->call.name);
            }


            struct list_node* names = list_rest(a->binding.name_and_args);
            struct list_node* args = node->call.args;
            while (NULL != names) {
                if (NULL == args) {
                    break;
                }
                cret.env = old_env;
                cret.node = list_first(args);
                struct inter_context* childctx = inter(&cret);

                inter_assumes_not_null(childctx->node, "some argument of call");
                inter_assumes_type(childctx->node, INT, "some argument of call");

                e = env_add(e, list_first(names), childctx->node);

                names = list_rest(names);
                args = list_rest(args);
            }

            if ((NULL == names) && (NULL == args)) {
                cret.env = e;
                cret.node = a->binding.value;

                struct inter_context* childctx = inter(&cret);

                ret.type = INT;
                ret.integer = childctx->node->integer;

                cret.node = &ret;
            } else {
                INTER_ERROR("wrong number of arguments in call to %s", node->call.name);
            }

            while (NULL != e) {
                e = env_free_head(e);
            }
            cret.env = old_env;
        }
        return &cret;
        break;

    case LET:
        {
            struct inter_context  c = { .node = NULL, .env = env };
            struct inter_context* p = &c;
            struct env_node* old_env = env;

            for (struct list_node* l = node->let.bindings; NULL != l; l = l->next) {
                c.env = p->env;
                c.node = l->elem;
                p = inter(&c);
            }

            cret.env = p->env;
            cret.node = node->let.in;
            p = inter(&cret);

            env = p->env;
            while (env != old_env) {
                env = env_free_head(env);
            }

            cret.env = old_env;
            cret.node = p->node;
        }
        return &cret;
        break;
    case IF:
        {
            cret.node = node->when.cond;
            struct inter_context* cond_ctx = inter(&cret);
            inter_assumes_not_null(cond_ctx->node, "condition of if");
            inter_assumes_type(cond_ctx->node, INT, "condition of if");

            struct ast_node* branch;
            if(cond_ctx->node->integer) {
                branch = node->when.then;
            } else {
                branch = node->when.els;
            }

            cret.env = env;
            cret.node = branch;
            struct inter_context* c = inter(&cret);

            cret.env = env;
            cret.node = c->node;
        }

        return &cret;
        break;

    case LOOP:
        {
            struct ast_node* previous_loop = current_loop;
            current_loop = node;
            struct env_node* old_env = env;
            struct env_node* first = NULL;
            struct inter_context  c = { .node = NULL, .env = env };
            struct inter_context* p = &c;

            for (struct list_node* l = node->let.bindings; NULL != l; l = l->next) {
                c.env = p->env;
                c.node = l->elem;
                p = inter(&c);
                if (NULL == first) {
                    first = p->env;
                }
            }

            if (first->parent != old_env) {
                INTER_ERROR("first->parent is not old environment");
            }
            first->parent = NULL;

            recur_env = p->env;

            inter_assumes_not_null(recur_env, "loop environment");

            do {
                env = recur_env;
                while (env->parent != NULL) {
                    env = env->parent;
                }

                env->parent = old_env;

                cret.env = recur_env;
                cret.node = node->let.in;
                recur_env = NULL;
                p = inter(&cret);

                if (NULL != p->node) {
                    ret.type = INT;
                    ret.integer = p->node->integer;
                }

                env = p->env;
                while (env != old_env) {
                    env = env_free_head(env);
                }

                cret.node = &ret;
                cret.env = old_env;
            } while (NULL != recur_env);

            current_loop = previous_loop;
        }

        recur_env = NULL;
        return &cret;
        break;

    case RECUR:
        {
            struct list_node* loop_args = current_loop->let.bindings;
            struct list_node* args = node->call.args;
            recur_env = env_empty();

            while (NULL != args) {
                if (NULL == loop_args) {
                    break;
                }

                cret.env = env;
                cret.node = list_first(args);
                struct inter_context* childctx = inter(&cret);

                inter_assumes_not_null(childctx->node, "some argument of recur");
                inter_assumes_type(childctx->node, INT, "some argument of recur");

                struct ast_node* loop_binding = loop_args->elem;
                inter_assumes_not_null(loop_binding, "some name in loop");
                inter_assumes_type(loop_binding, VALDEF, "some val in loop");

                recur_env = env_add(recur_env, loop_binding->binding.name, childctx->node);

                args = list_rest(args);
                loop_args = list_rest(loop_args);
            }
        }
        cret.env = NULL;
        cret.node = NULL;
        return &cret;

    default:
        INTER_ERROR("Unhandled node type.");
    }
}

void interpret(struct list_node* top_level, int argc, long* argv) {
    struct env_node* env = env_empty();
    struct inter_context ctx = { .node = NULL, .env = env };
    struct inter_context* p = &ctx;

    for (struct list_node* l = top_level; NULL != l; l = l->next) {
        ctx.env = p->env;
        ctx.node = l->elem;
        p = inter(&ctx);
    }

    if (NULL != p->node && INT == p->node->type) {
        printf("%ld\n", p->node->integer);
    } else {
        struct ast_node* anodes = calloc(argc, sizeof(struct ast_node));
        struct list_node* lnodes = calloc(argc, sizeof(struct list_node));
        struct ast_node call;
        int i = 0;
        for (; i<argc; i++) {
            anodes[i].type = INT;
            anodes[i].integer = argv[i];

            lnodes[i].elem = &anodes[i];
            lnodes[i].next = &lnodes[i+1];
        }
        lnodes[argc-1].next = NULL;

        call.type = FUNCALL;
        call.call.name = "main";
        call.call.args = lnodes;
        ctx.node = &call;
        ctx.env = p->env;

        p = inter(&ctx);
        printf("%ld\n", p->node->integer);
    }
}
