%{

#include <stdio.h>
#include "ast.h"

int yylex(void);
void yyerror(char const *);

#define YYSTYPE struct ast_node

%}

%locations


%token TLET
%token IN
%token END
%token TLOOP
%token AND
%token TRECUR
%token TIF
%token THEN
%token ELSE
%token <integer> INTEGER
%token <string> IDENTIFIER
%token ASSIGNMENT
// integer -> integer
%token <character> ADD
%token <character> MULT
// integer -> boolean (equality-operator)
%token <character> EOPERATOR
// boolean operator
%token <character> BOPERATOR
// unary operators
%token <character> NEG
%token <character> NOT
// ()
%token P_OPEN;
%token P_CLOSE;

%type <list> bindings and_bindings identifiers par_values top_let_clauses simplelang;
%type <tree> binding value par_value let_clause loop_clause recur_clause if_clause;
%type <tree> top_let_clause;

%left BOPERATOR
%left NOT
%left EOPERATOR
%left ADD
%left NEG
%left P_OPEN


%%

simplelang: top_let_clauses                     {  $$ = $1; }
;

top_let_clauses: top_let_clause top_let_clauses { $$ = list_prepend($2, $1); }
               | top_let_clause                 { $$ = list_prepend(NULL, $1); }
               ;

top_let_clause: TLET binding END        { $$ = $2; }
              | let_clause                { $$ = $1; }
              ;

let_clause: TLET bindings IN value END  { $$ = node_let($2, $4); }
          ;

bindings: binding bindings       { $$ = list_prepend($2, $1); }
           | binding                   { $$ = list_prepend(NULL, $1); }
           ;

binding: IDENTIFIER ASSIGNMENT value   { $$ = node_def_val($1, $3); }
       | identifiers ASSIGNMENT value  { $$ = node_def_fun($1, $3); }
          ;

identifiers: IDENTIFIER identifiers       { $$ = list_prepend($2, $1); }
           | IDENTIFIER                   { $$ = list_prepend(NULL, $1); }
           ;

value: INTEGER                  { $$ = node_int($1); }
     | IDENTIFIER par_values    { $$ = node_funcall($1, $2); }
     | IDENTIFIER               { $$ = node_use_var($1); }
     | NEG value                { $$ = node_unary('!', $2); }
     | loop_clause              { $$ = $1; }
     | recur_clause             { $$ = $1; }
     | if_clause                { $$ = $1; }
     | value ADD value          { $$ = node_binary($2, $1, $3); }
     | value MULT value         { $$ = node_binary($2, $1, $3); }
     | value EOPERATOR value    { $$ = node_binary($2, $1, $3); }
     | value BOPERATOR value    { $$ = node_binary($2, $1, $3); }
     | NOT value                { $$ = node_unary('!', $2); }
     | let_clause               { $$ = $1; }
     ;

loop_clause: TLOOP and_bindings IN value END  { $$ = node_loop($2, $4); }
           ;

and_bindings: binding AND and_bindings    { $$ = list_prepend($3, $1); }
            | binding                     { $$ = list_prepend(NULL, $1); }
               ;

recur_clause: TRECUR par_values                  { $$ = node_recur($2); }
;

par_values: par_value par_values                { $$ = list_prepend($2, $1); }
          | par_value                           { $$ = list_prepend(NULL, $1)}
          ;

par_value: P_OPEN value P_CLOSE                 { $$ = $2; }
         ;

if_clause: TIF value THEN value ELSE value END   { $$ = node_if($2, $4, $6); }
         | TIF value THEN value END              { $$ = node_if($2, $4, NULL); }
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
