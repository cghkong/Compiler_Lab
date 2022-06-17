%{
#include<stdio.h>
#include"Node.h"
#include"lex.yy.c"
extern unsigned synError;
TreeNode* root;
#define YYERROR_VERBOSE 1

%}

// node
%union{ 
        TreeNode* Node; 
}

// tokens stream
%token <Node> ID TYPE
%token <Node> INT FLOAT
%token <Node> RELOP
%token <Node> DOT SEMI COMMA ASSIGNOP
%token <Node> PLUS MINUS STAR DIV
%token <Node> AND OR NOT 
%token <Node> LP RP LB RB LC RC
%token <Node> IF ELSE WHILE
%token <Node> STRUCT
%token <Node> RETURN

// non_terminal based on guildline
//  High-level Definitions
%type <Node> Program ExtDefList ExtDef ExtDecList
//  Specifiers
%type <Node> Specifier StructSpecifier OptTag Tag
//  Declarators
%type <Node> VarDec FunDec VarList ParamDec
//  Statements
%type <Node> CompSt StmtList Stmt
//  Local Definitions
%type <Node> DefList Def Dec DecList
//  Expressions
%type <Node> Exp Args

//precedence associativity(low to height)
%right ASSIGNOP
%left OR
%left AND
%left RELOP
%left PLUS MINUS
%left STAR DIV
%right NOT
%left DOT
%left LB RB
%left LP RP
%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

%%
// high-level define
Program :    ExtDefList                 {$$=mallocNode(@$.first_line,NOT_A_TOKEN,"Program",1,$1); root=$$; }
        ;
ExtDefList : ExtDef ExtDefList          {$$=mallocNode(@$.first_line,NOT_A_TOKEN,"ExtDefList",2,$1,$2); }
        |                               {$$=NULL; }
        ;
ExtDef :     Specifier ExtDecList SEMI  {$$=mallocNode(@$.first_line,NOT_A_TOKEN,"ExtDef",3,$1,$2,$3); }
        |    Specifier SEMI             {$$=mallocNode(@$.first_line,NOT_A_TOKEN,"ExtDef",2,$1,$2); }
        |    Specifier FunDec CompSt    {$$=mallocNode(@$.first_line, NOT_A_TOKEN, "ExtDef",3,$1,$2,$3); }
        |    error SEMI                 {synError = TRUE; }
        ;
ExtDecList : VarDec                     {$$=mallocNode(@$.first_line, NOT_A_TOKEN, "ExtDecList", 1,$1); }
        |    VarDec COMMA ExtDecList    {$$=mallocNode(@$.first_line, NOT_A_TOKEN, "ExtDecList", 3,$1,$2,$3); }
        ;

//Specifier
Specifier :          TYPE                           {$$=mallocNode(@$.first_line, NOT_A_TOKEN, "Specifier", 1, $1); }
        |            StructSpecifier                {$$=mallocNode(@$.first_line, NOT_A_TOKEN, "Specifier", 1, $1); }
        ;
StructSpecifier :    STRUCT OptTag LC DefList RC    {$$=mallocNode(@$.first_line, NOT_A_TOKEN, "StructSpecifier", 5, $1, $2, $3, $4, $5); }
        |            STRUCT Tag                     {$$=mallocNode(@$.first_line, NOT_A_TOKEN, "StructSpecifier", 2, $1, $2); }
        ; 
OptTag :             ID                             {$$=mallocNode(@$.first_line, NOT_A_TOKEN, "OptTag", 1, $1); }
        |                                           {$$=NULL; }
        ; 
Tag:                 ID                             {$$=mallocNode(@$.first_line, NOT_A_TOKEN, "Tag", 1, $1); }
        ;
// Declarators
VarDec :            ID                              {$$=mallocNode(@$.first_line, NOT_A_TOKEN, "VarDec", 1, $1); }
        |           VarDec LB INT RB                {$$=mallocNode(@$.first_line, NOT_A_TOKEN, "VarDec", 4, $1, $2, $3, $4); }
        |           error RB                        {synError = TRUE; }
        ; 
FunDec :            ID LP VarList RP                {$$=mallocNode(@$.first_line, NOT_A_TOKEN, "FunDec", 4, $1, $2, $3, $4); }
        |           ID LP RP                        {$$=mallocNode(@$.first_line, NOT_A_TOKEN, "FunDec", 3, $1, $2, $3); }
        |           error RP                        {synError = TRUE; }
        ; 
VarList :           ParamDec COMMA VarList          {$$=mallocNode(@$.first_line, NOT_A_TOKEN, "VarList", 3, $1, $2, $3); }
        |           ParamDec                        {$$=mallocNode(@$.first_line, NOT_A_TOKEN, "VarList", 1, $1); }
        ;
ParamDec :          Specifier VarDec                {$$=mallocNode(@$.first_line, NOT_A_TOKEN, "ParamDec", 2, $1, $2); }
        ;
// Statements
CompSt:             LC DefList StmtList RC                  {$$ = mallocNode(@$.first_line, NOT_A_TOKEN, "CompSt", 4, $1, $2, $3, $4); }
    |               error RC                                {synError = TRUE; }
    ; 
StmtList:           Stmt StmtList                           {$$ = mallocNode(@$.first_line, NOT_A_TOKEN, "StmtList", 2, $1, $2); }
    |                                                       {$$ = NULL; }
    ; 
Stmt:               Exp SEMI                                {$$ = mallocNode(@$.first_line, NOT_A_TOKEN, "Stmt", 2, $1, $2); }
    |               CompSt                                  {$$ = mallocNode(@$.first_line, NOT_A_TOKEN, "Stmt", 1, $1); }
    |               RETURN Exp SEMI                         {$$ = mallocNode(@$.first_line, NOT_A_TOKEN, "Stmt", 3, $1, $2, $3); }    
    |               IF LP Exp RP Stmt %prec LOWER_THAN_ELSE {$$ = mallocNode(@$.first_line, NOT_A_TOKEN, "Stmt", 5, $1, $2, $3, $4, $5); }
    |               IF LP Exp RP Stmt ELSE Stmt             {$$ = mallocNode(@$.first_line, NOT_A_TOKEN, "Stmt", 7, $1, $2, $3, $4, $5, $6, $7); }
    |               WHILE LP Exp RP Stmt                    {$$ = mallocNode(@$.first_line, NOT_A_TOKEN, "Stmt", 5, $1, $2, $3, $4, $5); }
    |               error SEMI                              {synError = TRUE; }
    ;
// Local Definitions
DefList:            Def DefList                             {$$ = mallocNode(@$.first_line, NOT_A_TOKEN, "DefList", 2, $1, $2); }
    |                                                       {$$ = NULL; }
    ;     
Def:                Specifier DecList SEMI                  {$$ = mallocNode(@$.first_line, NOT_A_TOKEN, "Def", 3, $1, $2, $3); }
    ; 
DecList:            Dec                                     {$$ = mallocNode(@$.first_line, NOT_A_TOKEN, "DecList", 1, $1); }
    |               Dec COMMA DecList                       {$$ = mallocNode(@$.first_line, NOT_A_TOKEN, "DecList", 3, $1, $2, $3); }
    ; 
Dec:                VarDec                                  {$$ = mallocNode(@$.first_line, NOT_A_TOKEN, "Dec", 1, $1); }
    |               VarDec ASSIGNOP Exp                     {$$ = mallocNode(@$.first_line, NOT_A_TOKEN, "Dec", 3, $1, $2, $3); }
    ;
//7.1.7 Expressions
Exp:                Exp ASSIGNOP Exp                        {$$ = mallocNode(@$.first_line, NOT_A_TOKEN, "Exp", 3, $1, $2, $3); }
    |               Exp AND Exp                             {$$ = mallocNode(@$.first_line, NOT_A_TOKEN, "Exp", 3, $1, $2, $3); }
    |               Exp OR Exp                              {$$ = mallocNode(@$.first_line, NOT_A_TOKEN, "Exp", 3, $1, $2, $3); }
    |               Exp RELOP Exp                           {$$ = mallocNode(@$.first_line, NOT_A_TOKEN, "Exp", 3, $1, $2, $3); }
    |               Exp PLUS Exp                            {$$ = mallocNode(@$.first_line, NOT_A_TOKEN, "Exp", 3, $1, $2, $3); }
    |               Exp MINUS Exp                           {$$ = mallocNode(@$.first_line, NOT_A_TOKEN, "Exp", 3, $1, $2, $3); }
    |               Exp STAR Exp                            {$$ = mallocNode(@$.first_line, NOT_A_TOKEN, "Exp", 3, $1, $2, $3); }
    |               Exp DIV Exp                             {$$ = mallocNode(@$.first_line, NOT_A_TOKEN, "Exp", 3, $1, $2, $3); }
    |               LP Exp RP                               {$$ = mallocNode(@$.first_line, NOT_A_TOKEN, "Exp", 3, $1, $2, $3); }
    |               MINUS Exp                               {$$ = mallocNode(@$.first_line, NOT_A_TOKEN, "Exp", 2, $1, $2); }
    |               NOT Exp                                 {$$ = mallocNode(@$.first_line, NOT_A_TOKEN, "Exp", 2, $1, $2); }
    |               ID LP Args RP                           {$$ = mallocNode(@$.first_line, NOT_A_TOKEN, "Exp", 4, $1, $2, $3, $4); }
    |               ID LP RP                                {$$ = mallocNode(@$.first_line, NOT_A_TOKEN, "Exp", 3, $1, $2, $3); }
    |               Exp LB Exp RB                           {$$ = mallocNode(@$.first_line, NOT_A_TOKEN, "Exp", 4, $1, $2, $3, $4); }
    |               Exp DOT ID                              {$$ = mallocNode(@$.first_line, NOT_A_TOKEN, "Exp", 3, $1, $2, $3); }
    |               ID                                      {$$ = mallocNode(@$.first_line, NOT_A_TOKEN, "Exp", 1, $1); }
    |               INT                                     {$$ = mallocNode(@$.first_line, NOT_A_TOKEN, "Exp", 1, $1); }
    |               FLOAT                                   {$$ = mallocNode(@$.first_line, NOT_A_TOKEN, "Exp", 1, $1); }
    ;
Args :              Exp COMMA Args                          {$$ = mallocNode(@$.first_line, NOT_A_TOKEN, "Args", 3, $1, $2, $3); }
    |               Exp                                     {$$ = mallocNode(@$.first_line, NOT_A_TOKEN, "Args", 1, $1); }
    ;

%%

yyerror(char* message){
    fprintf(stderr, "Error type B at line %d: %s.\n", yylineno, message);
}

