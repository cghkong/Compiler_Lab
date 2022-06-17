#ifndef definition_h
#define definition_h

#define HASH_TABLE_SIZE 0x3fff
#define STACK_DEEP

#include "Node.h"

typedef struct type* pType;
typedef struct fieldList* pFieldList;
typedef struct tableItem* pItem;
typedef struct hashTable* pHash;
typedef struct stack* pStack;
typedef struct table* pTable;

typedef enum ukind { BASIC, ARRAY, STRUCTURE, FUNCTION } Kind;
typedef enum ubasicType { INT_TYPE, FLOAT_TYPE } BasicType;
typedef enum uerrorType {
    UNDEF_VAR = 1,         
    UNDEF_FUNC,            
    REDEF_VAR,             
    REDEF_FUNC,            
    TYPE_MISMATCH_ASSIGN,  // Type mismatchedfor assignment 
    LEFT_VAR_ASSIGN,  // The left-hand side of an assignment must be a variable 
    TYPE_MISMATCH_OP,      // Type mismatched for operands 
    TYPE_MISMATCH_RETURN,  // Type mismatched for return 
    FUNC_AGRC_MISMATCH,    // Function is not applicable for arguments
    NOT_A_ARRAY,          
    NOT_A_FUNC,            
    NOT_A_INT,             
    ILLEGAL_USE_DOT,       // Illegal use of "."
    NONEXISTFIELD,         // Non-existentfield
    REDEF_FEILD,           // Redefined field
    DUPLICATED_NAME,       // Duplicated name
    UNDEF_STRUCT          
} ErrorType;


typedef struct type{
    Kind kind;
    union {
        BasicType basic;
        struct {
            pType elem;
            int size;
        } array;

        struct{
            char* structName;
            pFieldList field;
        }structure;

        struct{
            int argc;
            pFieldList argv;
            pType returnType;
        }function;
    }u;
}Type;

typedef struct fieldList{
    char* name;
    pType type;
    pFieldList tail;
}FieldList;

typedef struct tableItem{
    int symbolDepth;
    pFieldList field;
    pItem nextSymbol;
    pItem nextHash;
}TableItem;

typedef struct hashTable{
    pItem* hashArray;
}HashTable;

typedef struct stack{
    pItem* stackArray;
    int curdepth;
}Stack;

typedef struct table{
    pHash hash;
    pStack stack;
    int unNameStructNum;
}Table;

extern pTable table;

// Type functions
pType newType(Kind kind, ...);
pType copyType(pType src);
void deleteType(pType type);
bool checkType(pType type1, pType type2);
void printType(pType type);

// fieldList functions
pFieldList newFieldList(char* newName,pType newType);
pFieldList copyFieldList(pFieldList src);
void deleteFieldList(pFieldList fieldList);
void setFieldListName(pFieldList p,char* newName);
void printFieldList(pFieldList fieldList);

// item functions
pItem newItem(int symbolDepth,pFieldList pfield);
void deleteItem(pItem item);
bool isStructDef(pItem src);

// Hash functions
pHash newHash();
void deleteHash(pHash hash);
pItem getHashHead(pHash hash,int index);
void setHashHead(pHash hash,int index,pItem newVal);

//stack functions
pStack newStack();
void deleteStack(pStack stack);
void addStackDepth(pStack stack);
void minusStackDepth(pStack stack);
pItem getCurDepthStackHead(pStack stack);
void setCurDepthStackHead(pStack stack,pItem newVal);

//Table functions
pTable initTable();
void deleteTable(pTable table);
pItem searchTableItem(pTable table,char* name);
bool checktTableItemConflict(pType table,pItem item);
void addTableItem(pTable table,pItem item);
void deleteTableItem(pTable table,pItem item);
void clearCurDepthStackList(pTable table);

// syntax tree
void traverseTree(TreeNode* node);

void ExtDef(TreeNode* node);
void ExtDecList(TreeNode* node, pType specifier);
pType Specifier(TreeNode* node);
pType StructSpecifier(TreeNode* node);
pItem VarDec(TreeNode* node, pType specifier);
void FunDec(TreeNode* node, pType returnType);
void VarList(TreeNode* node, pItem func);
pFieldList ParamDec(TreeNode* node);
void CompSt(TreeNode* node, pType returnType);
void StmtList(TreeNode* node, pType returnType);
void Stmt(TreeNode* node, pType returnType);
void DefList(TreeNode* node, pItem structInfo);
void Def(TreeNode* node, pItem structInfo);
void DecList(TreeNode* node, pType specifier, pItem structInfo);
void Dec(TreeNode* node, pType specifier, pItem structInfo);
pType Exp(TreeNode* node);
void Args(TreeNode* node, pItem funcInfo);


static inline unsigned int getHashCode(char* name){
    unsigned int val=0,i;
    for(;*name;++name)
    {
        val = (val<<2) + *name;
        if(i==val & ~HASH_TABLE_SIZE)
        {
            val = (val ^ (i>>12)) & HASH_TABLE_SIZE;
        }
    }
    return val;
}

static inline void pError(ErrorType type , int line , char* msg){
    printf("Error type %d at line %d: %s\n",type,line,msg);
}

#endif
