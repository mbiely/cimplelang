#include <stdio.h>
#include <stdlib.h>

#include "ast.h"
#include "inter.h"

extern struct list_node* yyparse();
extern FILE* yyin;

struct list_node* top_level_defs;


int main(int argc, char **argv)
{
    long* intargs = calloc(argc-2, sizeof(long));

    if (1 == argc) {
        yyin = fopen("fac.sl", "r");
    } else {
        yyin = fopen(argv[1], "r");
    }

    for(int i=2; i<argc; i++) {
        intargs[i-1] = atol(argv[i]);
    }

    if (0 != yyparse()) {
        exit(1);
    }

    //print_tree_list(top_level_defs);
    //printf("\n");
    interpret(top_level_defs, argc-2, intargs);
}
