#include <stdio.h>
#include <stdlib.h>

#include "ast.h"

extern struct list_node* yyparse();
extern FILE* yyin;

struct list_node* top_level_defs;


int main( int argc, char **argv )
{
    yyin = fopen("fac.sl", "r");

    if (0 != yyparse()) {
        exit(1);
    }

    printf("top: %p\n",  top_level_defs);

    print_tree_list(top_level_defs);
}
