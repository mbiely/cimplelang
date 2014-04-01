%{
    #include <stdio.h>
    int yylex(void);
    void yyerror(char const *);
%}

%locations


%union {
    int integer; /* integer value */
    char * string; /* symbol table index */
    char character;
    // nodeType *nPtr; /* node pointer */
};


%token LET
%token IN
%token END
%token INTEGER
%token IDENTIFIER
%token ASSIGNMENT
%token OPERATOR

%%

simplelang: let_clause;

let_clause: LET assignments IN value END;

assignments: assignment assignments
           | /* NOTHING */
           ;

assignment: IDENTIFIER ASSIGNMENT value;

value: INTEGER
     | IDENTIFIER
     | value OPERATOR value
     | let_clause
     ;


%%


void yyerror (char const *s) {
    fprintf (stderr, "%s between %d,%d and %d,%d\n", s, yylloc.first_line, yylloc.first_column, yylloc.last_line, yylloc.last_column);
}

 int main( int argc, char **argv )
 {
    yyparse();
 }
