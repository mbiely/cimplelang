#ifndef CIMPLELANG_AST_H
#define CIMPLELANG_AST_H


enum ast_node_type { INT, FUNDEF, VALDEF, UNARY, BINARY, LET, FUNCALL, VAR, IF, RECUR, LOOP };

struct ast_node {
    enum ast_node_type type;
    union {
        int integer; /* integer value */
        char* string; /* symbol table index */
        char character;
        struct list_node* list;
        struct ast_node* tree;
        struct {
            union {
                struct list_node* name_and_args;
                char* name;
            };
            struct ast_node* value;
        } binding;
        struct {
            char op;
            struct ast_node* child;
        } unary;
        struct {
            char op;
            struct ast_node* left;
            struct ast_node* right;
        } binary;
        struct {
            struct list_node* bindings;
            struct ast_node*  in;
        } let;
        struct {
            char* name;
            struct list_node* args;
        } call;
        struct {
            struct ast_node* cond;
            struct ast_node* then;
            struct ast_node* els;
        } when;
    };
};

void print_string_list(struct list_node* list);
void print_tree_list(struct list_node* list);
void print_tree(struct ast_node* tree);
void iter_list(struct list_node* head, void (*f)(void *));

struct list_node;
struct list_node* list_prepend(struct list_node* list, void* elem);

struct ast_node* node_int(int i);
struct ast_node* node_unary(char op, struct ast_node* child);
struct ast_node* node_binary(char op, struct ast_node* left, struct ast_node* right);
struct ast_node* node_def_val(char* name, struct ast_node* foo);
struct ast_node* node_def_fun(struct list_node* name_and_args, struct ast_node* foo);
struct ast_node* node_let(struct list_node* assignments, struct ast_node* in);
struct ast_node* node_funcall(char* name, struct list_node* args);
struct ast_node* node_use_var(char* var);
struct ast_node* node_loop(struct list_node* assignments, struct ast_node* in);
struct ast_node* node_recur(struct list_node* assignments);
struct ast_node* node_if(struct ast_node* cond, struct ast_node* then, struct ast_node* els);

#endif
