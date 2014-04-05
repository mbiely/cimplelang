%{
#include<stdlib.h>
#include "ast.h"

#define YYSTYPE struct ast_node

#include "parser.tab.h"


void track_location();

#define YY_USER_ACTION track_location();

%}

%option noyywrap
%option yylineno

OP_OR    "||"
OP_AND   "&&"
OP_NOT   "!"
OP_EQL   "=="
OP_LT    "<"
OP_ADD   "+"
OP_MULT  "*"
OP_NEG   "-"
SYN_ASS  "="
KW_LET   "let"
KW_IN    "in"
KW_END   "end"
KW_LOOP  "loop"
KW_AND   "and"
KW_RECUR "recur"
KW_IF    "if"
KW_THEN  "then"
KW_ELSE  "else"
DIGIT    [0-9]
IDENT    [_a-zA-Z][_a-zA-Z0-9]*

%%

{OP_OR}     {
    yylval.character = *yytext;
    return BOPERATOR;
}

{OP_AND}    {
    yylval.character = *yytext;
    return BOPERATOR;
}

{OP_NOT}    { return NOT; }

{OP_EQL}    {
    yylval.character = *yytext;
    return EOPERATOR;
}

{OP_LT}     {
    yylval.character = *yytext;
    return EOPERATOR;
}

{OP_ADD}    {
    yylval.character = *yytext;
    return ADD;
}

{OP_MULT}    {
    yylval.character = *yytext;
    return MULT;
}

{OP_NEG}    { return NEG; }
{SYN_ASS}   { return ASSIGNMENT; }
{KW_LET}    { return TLET; }
{KW_IN}     { return IN; }
{KW_END}    { return END; }
{KW_LOOP}   { return TLOOP; }
{KW_AND}    { return AND; }
{KW_RECUR}  { return TRECUR; }
{KW_IF}     { return TIF; }
{KW_THEN}   { return THEN; }
{KW_ELSE}   { return ELSE; }

{DIGIT}+    {
    yylval.integer = atoi(yytext);
    return INTEGER;
}

{IDENT}     {
    yylval.string = strdup(yytext);
    return IDENTIFIER;
}

"("         { return P_OPEN; }
")"         { return P_CLOSE; }

[ \t\n]+ // whitespace

. { fprintf(stderr, "parse error: unexpected >%s<", yytext); }

%%

void track_location() {
    static int yycolumn = 0;
    if (yylloc.first_line != yylineno) {
        yycolumn = 0;
    }
    yylloc.first_line = yylloc.last_line = yylineno;
    yylloc.first_column = yycolumn;
    yycolumn += yyleng;
    yylloc.last_column = yycolumn - 1;
}
