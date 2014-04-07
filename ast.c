#include "ast.h"

#include <stdio.h>
#include <stdlib.h>

struct list_node {
    void * elem;
    struct list_node* prev;
    struct list_node* next;
};

void iter_list(struct list_node* head, void (*f)(void *)) {
    struct list_node* h;
    for(h = head; h!=NULL; h=h->next) {
        f(h->elem);
    }
}

struct list_node* list_prepend(struct list_node* list, void* elem) {
    struct list_node* head = malloc(sizeof(struct list_node));

    head->elem = elem;
    head->next = list;

    if (list!=NULL) {
        list->prev = head;
    }

    return head;
}

void print_tree_list(struct list_node* list) {
    struct list_node *l = list;

    while(l != NULL) {
        print_tree((struct ast_node*)l->elem);
        l = l->next;
    }
}

void print_string_list(struct list_node* list) {
    struct list_node *l = list;

    while(l != NULL) {
        printf("%s, ", (char*)l->elem);
        l = l->next;
    }
}


void print_tree(struct ast_node* value) {
    if (NULL == value) {
        printf(" $nix ");
        return;
    }
    switch(value->type) {
    case INT:
        printf("%d", value->integer);
        break;
    case UNARY:
        printf("%c(", value->unary.op);
        print_tree(value->unary.child);
        printf(")");
        break;
    case BINARY:
        printf("(");
        print_tree(value->binary.left);
        printf(")%c(", value->binary.op);
        print_tree(value->binary.right);
        printf(")");
        break;
    case FUNDEF:
        printf("(fun ");
	print_string_list(value->binding.name_and_args);
        print_tree(value->binding.value);
        printf(" nuf)");
	break;
    case FUNCALL:
        printf("call %s(", value->call.name);
        print_tree_list(value->call.args);
        printf(")llac ");
	break;
    case VAR:
	printf("%s", value->string);
	break;
    default:
        printf("??");
        break;
    }
}

struct ast_node* node_def_val(char* name, struct ast_node* value) {
    struct ast_node* node = malloc(sizeof(struct ast_node));

    node->type = VALDEF;
    node->binding.value = value;
    node->binding.name = name;

    return node;
}


struct ast_node* node_def_fun(struct list_node* name_and_args, struct ast_node* value) {
    struct ast_node* node = malloc(sizeof(struct ast_node));

    node->type = FUNDEF;
    node->binding.value = value;
    node->binding.name_and_args = name_and_args;

    return node;
}

struct ast_node* node_int(int i) {
    struct ast_node* node = malloc(sizeof(struct ast_node));

    node->type = INT;
    node->integer = i;

    return node;
}

struct ast_node* node_unary(char op, struct ast_node* child) {
    struct ast_node* node = malloc(sizeof(struct ast_node));

    node->type = UNARY;
    node->unary.op = op;
    node->unary.child = child;

    return node;
}

struct ast_node* node_binary(char op, struct ast_node* left, struct ast_node* right) {
    struct ast_node* node = malloc(sizeof(struct ast_node));

    node->type = BINARY;

    node->binary.op = op;
    node->binary.left = left;
    node->binary.right = right;

    return node;
}

struct ast_node* node_let(struct list_node* bindings, struct ast_node* in) {
    struct ast_node* node = malloc(sizeof(struct ast_node));

    node->type = LET;
    node->let.bindings = bindings;
    node->let.in = in;

    return node;
}
struct ast_node* node_funcall(char* name, struct list_node* args) {
    struct ast_node* node = malloc(sizeof(struct ast_node));

    node->type = FUNCALL;
    node->call.name = name;
    node->call.args = args;

    return node;
}
struct ast_node* node_use_var(char* name) {
    struct ast_node* node = malloc(sizeof(struct ast_node));

    node->type = VAR;
    node->string = name;

    return node;
}

struct ast_node* node_loop(struct list_node* bindings, struct ast_node* in) {
    struct ast_node* node = malloc(sizeof(struct ast_node));

    node->type = LOOP;
    node->let.bindings = bindings;
    node->let.in = in;

    return node;
}


struct ast_node* node_recur(struct list_node* bindings) {
    struct ast_node* node = malloc(sizeof(struct ast_node));

    node->type = RECUR;
    node->call.args = bindings;
    node->call.name = NULL;

    return node;
}

struct ast_node* node_if(struct ast_node* cond, struct ast_node* then, struct ast_node* els) {
    struct ast_node* node = malloc(sizeof(struct ast_node));

    node->type = IF;
    node->when.cond = cond;
    node->when.then = then;
    node->when.els = els;

    return node;
}
