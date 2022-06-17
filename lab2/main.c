#include "libs.h"
#include "syntax.tab.h"


//flex inner variables;
extern int yylineno;  
extern int yyparse();  
extern void yyrestart(FILE*);  

//syntax tree root;
extern TreeNode* root;

//error flag
unsigned iserror;
unsigned lexError = FALSE;
unsigned synError = FALSE;

int main(int argc, char** argv)
{
    if(argc<=1)
    {
        printf("The input command is error, please input ./paser test.cmm\n");
        return 1;
    }
    //file instance
    FILE* file = fopen(argv[1],"r");
    //file failed to read;
    if(!file)
    {
        printf("failed to open the test file\n");
        return 1;
    }
    //parse the test file
    yyrestart(file);
    yyparse();

    iserror = !lexError && !synError;
    if(iserror)
    {
        table = initTable();
        traverseTree(root);
        deleteTable(table);
    }
    deleteNode(&root);
    return 0;
}

