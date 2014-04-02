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
%token LOOP
%token AND
%token RECUR
%token IF
%token THEN
%token ELSE
%token INTEGER
%token IDENTIFIER
%token ASSIGNMENT
// integer -> integer
%token OPERATOR
// integer -> boolean (equality-operator)
%token EOPERATOR
// boolean operator
%token BOPERATOR
// unary operators
%token NEG
%token NOT
// ()
%token P_OPEN;
%token P_CLOSE;

%%

simplelang: top_let_clauses;
/* TODO: inconsistency in definition. top-level let-clauses may not have in? */

top_let_clauses: top_let_clause top_let_clauses
               | top_let_clause;

top_let_clause: LET assignments END
              | let_clause;

let_clause: LET assignments IN value END
          ;

assignments: assignment assignments
           | /* NOTHING */
           ;

assignment: IDENTIFIER ASSIGNMENT value
          | identifiers ASSIGNMENT value
          ;

identifiers: IDENTIFIER identifiers
           | IDENTIFIER
           ;

value: INTEGER
     | IDENTIFIER par_values
     | IDENTIFIER
     | NEG value
     | loop_clause
     | recur_clause
     | if_clause
     | value OPERATOR value
     | let_clause
     ;

loop_clause: LOOP and_assignments IN value END
           ;

and_assignments: assignment AND and_assignments
               | assignment
               ;

recur_clause: RECUR par_values;

par_values: par_value par_values
          | /* NOTHING */
          ;

par_value: P_OPEN value P_CLOSE
         ;

if_clause: IF condition THEN value ELSE value END    { printf("read if\n"); }
         | IF condition THEN value END
         ;

condition: value EOPERATOR value
         | condition BOPERATOR condition
         | NOT condition
         ;
%%

extern char* yytext;

void yyerror (char const *s) {
    fprintf (stderr, "%s between %d,%d and %d,%d (near >%s<)\n", s, yylloc.first_line, yylloc.first_column, yylloc.last_line, yylloc.last_column, yytext);
}

 int main( int argc, char **argv )
 {
    yyparse();
 }
