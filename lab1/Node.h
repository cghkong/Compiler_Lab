#ifndef NODE_H
#define NODE_H

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdarg.h>
#include <string.h>

#define TRUE 1
#define FALSE 0

//node type enum
typedef enum Node_Type {
    TOKEN_INT,
    TOKEN_FLOAT,
    TOKEN_ID,
    TOKEN_TYPE,
    TOKEN_OTHER,
    NOT_A_TOKEN
} nodetype;

//define Node
typedef struct Node{
    int lineNumber;  //the line Number
    nodetype type;  
    char* name;   // name
    char* val;   // value
    struct Node* child;  //childnode
    struct Node* next;  //nextnode
    char* info;  // information
}TreeNode;

//basic functions for building a syntax tree
//create node for lexical
static inline TreeNode* mallocNode(int lineNumber, nodetype type, char* name, int argc,...)
{
    TreeNode* newnode=NULL;
    int name_len = strlen(name)+1;
    newnode = (TreeNode*)malloc(sizeof(TreeNode));
    //malloc success 
    assert(newnode!=NULL);
    newnode->name = (char*)malloc(name_len*sizeof(char));
    newnode->lineNumber=lineNumber;
    newnode->type = type;
    
    assert(newnode->name!=NULL);
    strncpy(newnode->name,name,name_len);
    
    va_list vaList;
    va_start(vaList,argc);
    TreeNode* childNode = va_arg(vaList,TreeNode*);
    newnode->child = childNode;
    //multi-node
    int i=1;
    while(i<argc)
    {
        childNode->next = va_arg(vaList,TreeNode*);
        if(childNode->next!=NULL)
        {
            childNode = childNode->next;
        }
        i++;
    }
    va_end(vaList);
    return newnode;
}
//create tokenNode for syntax
static inline TreeNode* mallocTokenNode(int linenumber, nodetype type, char* tokenname,char* tokentext)
{
    TreeNode* tokennode = (TreeNode*)malloc(sizeof(TreeNode));
    assert(tokennode!=NULL);
    int name_len = strlen(tokenname)+1;
    int text_len = strlen(tokentext)+1;
    
    tokennode->lineNumber=linenumber;
    tokennode->type=type;
    tokennode->name=(char*)malloc(name_len*sizeof(char*));
    tokennode->val = (char*)malloc(text_len*sizeof(char*));

    assert(tokennode->name!=NULL);
    assert(tokennode->val!=NULL);
    strncpy(tokennode->name,tokenname,name_len);
    strncpy(tokennode->val,tokentext,text_len);
    tokennode->child=NULL;
    tokennode->next=NULL;

    return tokennode;
}

static inline void deleteNode(TreeNode* node)
{
    if(node==NULL){
        return ;
    }
    while(node->child!=NULL)
    {
        TreeNode* pnode = node->child;
        node->child = node->child->next;
        deleteNode(pnode);
    }
    free(node->name);
    free(node->val);
    free(node);
    node->name=NULL;
    node->val=NULL;
    node=NULL;
}

static inline void printinfoTree(TreeNode* node,int h,unsigned error)
{
    if(!error)
    {
        return;
    }
    if(node==NULL) return;
    for(int i=0;i<h;++i)
    {
        printf("  ");
    }
    printf("%s",node->name);
    //node type is not token
    if(node->type == NOT_A_TOKEN)
    {
        printf(" (%d)",node->lineNumber);
    }
    else if(node->type == TOKEN_TYPE || node->type == TOKEN_ID || 
            node->type == TOKEN_INT)
            {
                printf(": %s",node->val);
            }
    else if(node->type == TOKEN_FLOAT)
    {
        printf(": %lf",atof(node->val));
    }
    printf("\n");
    printinfoTree(node->child,h+1,error);
    printinfoTree(node->next,h,error);

}

#endif


