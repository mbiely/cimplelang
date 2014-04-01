%{
#include<stdlib.h>
#include "parser.tab.h"

void track_location();

#define YY_USER_ACTION track_location();

%}

%option noyywrap
%option yylineno

OP_OR    "||"
OP_AND   "&&"
OP_EQL   "=="
OP_ASS   "="
OP_ADD   "+"
OP_MORE  [-+*<!]
KW_LET   "let"
KW_IN    "in"
KW_END   "end"
DIGIT    [0-9]
IDENT    [_a-zA-Z][_a-zA-Z0-9]*

%%

{KW_LET}    {
    return LET;
}

{KW_END}    {
    return END;
}

{KW_IN}     {
    return IN;
}

{DIGIT}+    {
    yylval.integer = atoi(yytext);
    return INTEGER;
}

{IDENT}     {
    yylval.string = yytext;
    return IDENTIFIER;
}

{OP_ADD}    {
    yylval.character = *yytext;
    return OPERATOR;
}

{OP_AND}    {
    printf("An AND\n");
}

{OP_EQL}    {
    printf("An EQL?\n");
}

{OP_ASS}    {
    return ASSIGNMENT;
}

{OP_MORE}   {
    printf("Some operator: %s\n", yytext);
}

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
