%{
#include"Node.h"
#include"syntax.tab.h"

extern unsigned lexError;
int yycolumn=1;   //column
int lines=0;   // total lines 

#define YY_USER_ACTION \
    yylloc.first_line=yylloc.last_line=yylineno; \
    yylloc.first_column=yycolumn; \
    yylloc.last_column=yycolumn+yyleng-1; \
    yycolumn+=yyleng;
%}

%option yylineno


letter [_a-zA-Z]
digit [0-9]
ld [0-9_a-zA-Z]
ws [ \t]
nonletter [^_a-zA-Z]
type int|float
relop >|<|>=|<=|==|!=
plus "+"
minus "-"
star "*"
div "/"
and "&&"
or "||"
xor "^"
not "!"
dot "."
semi ";"
comma ","
assignop "="
lp "("
rp ")"
lb "["
rb "]"
lc "{"
rc "}"
STRUCT struct
RETURN return
WHILE while
id {letter}{ld}*
eight_digit 0[0-7]*
sixteen_digit 0x[0-9a-fA-F]+
INT [1-9]+[0-9]*|{eight_digit}|{sixteen_digit}|0
FLOAT {digit}+"."{digit}+|{digit}*"."{digit}+[eE][+-]?{digit}+|{digit}+"."{digit}*[eE][+-]?{digit}+


id_error {digit}+{id}
float_error "."{digit}+|{digit}+"."|{digit}*"."{digit}+[eE]|{digit}+"."{digit}*[eE]|{digit}+[eE][+-]?{digit}*|"."[eE][+-]?{digit}+
character_error .

%%

{ws}+ {;}
\n|\r { yycolumn=1;}
while { yylval.Node = mallocTokenNode(yylineno,TOKEN_OTHER,"WHILE",yytext); return(WHILE); }
if { yylval.Node = mallocTokenNode(yylineno,TOKEN_OTHER,"IF",yytext); return(IF); }
else { yylval.Node = mallocTokenNode(yylineno,TOKEN_OTHER,"ELSE",yytext); return(ELSE); }
{type} {yylval.Node = mallocTokenNode(yylineno,TOKEN_TYPE,"TYPE",yytext); return TYPE;}
{STRUCT} {yylval.Node = mallocTokenNode(yylineno,TOKEN_OTHER,"STRUCT",yytext); return STRUCT; }
{RETURN} {yylval.Node = mallocTokenNode(yylineno,TOKEN_OTHER,"RETURN",yytext); return RETURN; }
{relop} { yylval.Node = mallocTokenNode(yylineno,TOKEN_OTHER,"RELOP",yytext); return RELOP; }

{plus} {yylval.Node = mallocTokenNode(yylineno,TOKEN_OTHER,"PLUS",yytext); return PLUS; }
{minus} {yylval.Node = mallocTokenNode(yylineno,TOKEN_OTHER,"MINUS",yytext); return MINUS; }
{star} {yylval.Node = mallocTokenNode(yylineno,TOKEN_OTHER,"STAR",yytext); return STAR; }
{div} {yylval.Node = mallocTokenNode(yylineno,TOKEN_OTHER,"DIV",yytext); return DIV; }
{and} {yylval.Node = mallocTokenNode(yylineno,TOKEN_OTHER,"AND",yytext); return AND; }
{or} {yylval.Node = mallocTokenNode(yylineno,TOKEN_OTHER,"OR",yytext); return OR; }
{not} {yylval.Node = mallocTokenNode(yylineno,TOKEN_OTHER,"NOT",yytext); return NOT; }

{dot} {yylval.Node = mallocTokenNode(yylineno,TOKEN_OTHER,"DOT",yytext); return DOT; }
{semi} {yylval.Node = mallocTokenNode(yylineno,TOKEN_OTHER,"SEMI",yytext); return SEMI; }
{comma} {yylval.Node = mallocTokenNode(yylineno,TOKEN_OTHER,"COMMA",yytext); return COMMA; }
{assignop} {yylval.Node = mallocTokenNode(yylineno,TOKEN_OTHER,"ASSIGNOP",yytext); return ASSIGNOP; }
{lp} {yylval.Node = mallocTokenNode(yylineno,TOKEN_OTHER,"LP",yytext); return LP; }
{rp} {yylval.Node = mallocTokenNode(yylineno,TOKEN_OTHER,"RP",yytext); return RP; }
{lb} {yylval.Node = mallocTokenNode(yylineno,TOKEN_OTHER,"LB",yytext); return LB; }
{rb} {yylval.Node = mallocTokenNode(yylineno,TOKEN_OTHER,"RB",yytext); return RB; }
{lc} {yylval.Node = mallocTokenNode(yylineno,TOKEN_OTHER,"LC",yytext); return LC; }
{rc} {yylval.Node = mallocTokenNode(yylineno,TOKEN_OTHER,"RC",yytext); return RC; }

{id} {yylval.Node = mallocTokenNode(yylineno,TOKEN_OTHER,"ID",yytext); return ID; }
{INT} { yylval.Node = mallocTokenNode(yylineno, TOKEN_INT, "INT", yytext); return INT;}
{FLOAT} { yylval.Node = mallocTokenNode(yylineno, TOKEN_FLOAT, "FLOAT", yytext); return FLOAT;}

{id_error} {lexError = TRUE; printf("Error type A at Line %d: Illegal ID \"%s\".\n", yylineno, yytext); }
{float_error} { lexError = TRUE; printf("Error type A at Line %d: Illegal floating number \"%s\".\n", yylineno, yytext); }
{character_error} { lexError = TRUE; printf("Error type A at Line %d: Mysterious character \'%s\'.\n", yylineno, yytext); }

%%

/*
int main(int argc, char** argv) {
    if (argc > 1) {
        if (!(yyin = fopen(argv[1], "r"))) {
        perror(argv[1]);
        return 1;
        }
    }
    yylex();
    printf("%d\n",lines);
    return 0;
}

*/

