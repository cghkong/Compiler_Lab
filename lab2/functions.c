#include "libs.h"

pTable table;

// type functions
pType newType(Kind kind,...){
    pType p = (pType)malloc(sizeof(Type));
    assert(p!=NULL);
    p->kind = kind;
    va_list vaList;
    assert(kind == BASIC || kind == ARRAY || kind == STRUCTURE || kind==FUNCTION);
    switch (kind)
    {
    case BASIC:
        va_start(vaList ,1);
        p->u.basic = va_arg(vaList,BasicType);
        break;
    case ARRAY:
        va_start(vaList,2);
        p->u.array.elem = va_arg(vaList,pType);
        p->u.array.size = va_arg(vaList,int);
        break;
    case STRUCTURE:
        va_start(vaList,2);
        p->u.structure.structName = va_arg(vaList,char*);
        p->u.structure.field = va_arg(vaList,pFieldList);
        break;
    case FUNCTION:
        va_start(vaList,3);
        p->u.function.argc = va_arg(vaList,int);
        p->u.function.argv = va_arg(vaList,pFieldList);
        p->u.function.returnType = va_arg(vaList,pType);
        break;
    }
    va_end(vaList);
    return p;
}

// copy src to p
pType copyType(pType src)
{
    if (src == NULL) return NULL;
    pType p = (pType)malloc(sizeof(Type));
    assert(p!=NULL);
    p->kind = src->kind;
    assert(p->kind == BASIC || p->kind ==ARRAY || p->kind ==STRUCTURE || p->kind ==FUNCTION);
    switch (p->kind)
    {
    case BASIC:
        p->u.basic = src->u.basic;
        break;
    case ARRAY:
        p->u.array.elem = copyType(src->u.array.elem);
        p->u.array.size = src->u.array.size;
        break;
    case STRUCTURE:
        p->u.structure.structName = String(src->u.structure.structName);
        p->u.structure.field = copyFieldList(src->u.structure.field);
        break;
    case FUNCTION:
        p->u.function.argc = src->u.function.argc;
        p->u.function.argv = copyFieldList(src->u.function.argv);
        p->u.function.returnType = copyType(src->u.function.returnType);
  
    }
    return p;
}

void deleteType(pType type)
{
    assert(type !=NULL);
    assert(type->kind == BASIC || type->kind == ARRAY || type->kind == STRUCTURE || type->kind == FUNCTION);
    pFieldList pFList = NULL;
    switch (type->kind)
    {
    case BASIC:
        break;
    case ARRAY:
        deleteType(type->u.array.elem);
        type->u.array.elem = NULL;
        break;
    case STRUCTURE:
        if(type->u.structure.structName)
            free(type->u.structure.structName);
        type->u.structure.structName = NULL;

        pFList = type->u.structure.field;
        while (pFList)
        {
            pFieldList delete_field = pFList;
            pFList = pFList->tail;
            deleteFieldList(delete_field);
        }
        type->u.structure.field = NULL;
        break;
    case FUNCTION:
        deleteType(type->u.function.returnType);
        type->u.function.returnType = NULL;
        pFList = type->u.function.argv;
        while(pFList)
        {
            pFieldList delete_field = pFList;
            pFList = pFList->tail;
            deleteFieldList(delete_field);
        }
        type->u.function.argv=NULL;
        break;
    }
    free(type);
}

// check the type1,type2
bool checkType(pType type1,pType type2)
{
    if (type1==NULL || type2==NULL) return TRUE;
    if (type1->kind == FUNCTION || type2->kind == FUNCTION) return FALSE;
    if (type1->kind != type2->kind)
        return FALSE;
    else {
        assert(type1->kind == BASIC || type1->kind == ARRAY ||
               type1->kind == STRUCTURE);
        switch (type1->kind) {
            case BASIC:
                return type1->u.basic == type2->u.basic;
            case ARRAY:
                return checkType(type1->u.array.elem, type2->u.array.elem);
            case STRUCTURE:
                return !strcmp(type1->u.structure.structName,
                               type2->u.structure.structName);
        }
    }
}

// print the type information
void printType(pType type) {
    if (type == NULL) {
        printf("type = NULL.\n");
    } else {
        printf("type kind is: %d\n", type->kind);
        switch (type->kind) {
            case BASIC:
                printf("type basic: %d\n", type->u.basic);
                break;
            case ARRAY:
                printf("array size: %d\n", type->u.array.size);
                printType(type->u.array.elem);
                break;
            case STRUCTURE:
                if (!type->u.structure.structName)
                    printf("struct name is NULL\n");
                else {
                    printf("struct name is %s\n", type->u.structure.structName);
                }
                printFieldList(type->u.structure.field);
                break;
            case FUNCTION:
                printf("function argc is %d\n", type->u.function.argc);
                printf("function args:\n");
                printFieldList(type->u.function.argv);
                printf("function return type:\n");
                printType(type->u.function.returnType);
                break;
        }
    }
}


// FieldList functions
pFieldList newFieldList(char* mName,pType mType){
    pFieldList p =(pFieldList)malloc(sizeof(FieldList));
    assert(p!= NULL);
    p->name = String(mName);
    p->tail = NULL;
    p->type = mType;
    return p;
}

// copy Field List
pFieldList copyFieldList(pFieldList src) {
    assert(src != NULL);
    pFieldList head = NULL, cur = NULL;
    pFieldList temp = src;

    while (temp) {
        if (!head) {
            head = newFieldList(temp->name, copyType(temp->type));
            cur = head;
            temp = temp->tail;
        } else {
            cur->tail = newFieldList(temp->name, copyType(temp->type));
            cur = cur->tail;
            temp = temp->tail;
        }
    }
    return head;
}

// delete field List
void deleteFieldList(pFieldList fieldList) {
    assert(fieldList != NULL);
    if (fieldList->name) {
        free(fieldList->name);
        fieldList->name = NULL;
    }
    if (fieldList->type) deleteType(fieldList->type);
    fieldList->type = NULL;
    free(fieldList);
}

//set the name of FieldList
void setFieldListName(pFieldList p, char* newName) {
    assert(p != NULL && newName != NULL);
    if (p->name != NULL) {
        free(p->name);
    }
    p->name = String(newName);
}

// print the FieldList
void printFieldList(pFieldList fieldList) {
    if (fieldList == NULL)
        printf("fieldList is NULL\n");
    else {
        printf("fieldList name is: %s\n", fieldList->name);
        printf("FieldList Type:\n");
        printType(fieldList->type);
        printFieldList(fieldList->tail);
    }
}

// Item functions
pItem newItem(int symbolDepth, pFieldList pfield) {
    pItem p = (pItem)malloc(sizeof(TableItem));
    assert(p != NULL);
    p->symbolDepth = symbolDepth;
    p->field = pfield;
    p->nextHash = NULL;
    p->nextSymbol = NULL;
    return p;
}

//delete Item
void deleteItem(pItem item) {
    assert(item != NULL);
    if (item->field != NULL) deleteFieldList(item->field);
    free(item);
}

// new Hash
pHash newHash() {
    pHash p = (pHash)malloc(sizeof(HashTable));
    assert(p != NULL);
    p->hashArray = (pItem*)malloc(sizeof(pItem) * HASH_TABLE_SIZE);
    assert(p->hashArray != NULL);
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        p->hashArray[i] = NULL;
    }
    return p;
}

//delete hash
void deleteHash(pHash hash) {
    assert(hash != NULL);
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        pItem temp = hash->hashArray[i];
        while (temp) {
            pItem tdelete = temp;
            temp = temp->nextHash;
            deleteItem(tdelete);
        }
        hash->hashArray[i] = NULL;
    }
    free(hash->hashArray);
    hash->hashArray = NULL;
    free(hash);
}

// get HashHead
pItem getHashHead(pHash hash, int index) {
    assert(hash != NULL);
    return hash->hashArray[index];
}

//set Hash Head
void setHashHead(pHash hash, int index, pItem newVal) {
    assert(hash != NULL);
    hash->hashArray[index] = newVal;
}

// Table functions
pTable initTable() {
    pTable table = (pTable)malloc(sizeof(Table));
    assert(table != NULL);
    table->hash = newHash();
    table->stack = newStack();
    table->unNameStructNum = 0;
    return table;
};

// delete table
void deleteTable(pTable table) {
    deleteHash(table->hash);
    table->hash = NULL;
    deleteStack(table->stack);
    table->stack = NULL;
    free(table);
};

// search item
pItem searchTableItem(pTable table, char* name) {
    unsigned hashCode = getHashCode(name);
    pItem temp = getHashHead(table->hash, hashCode);
    if (temp == NULL) return NULL;
    while (temp) {
        if (!strcmp(temp->field->name, name)) return temp;
        temp = temp->nextHash;
    }
    return NULL;
}

// check whether it is redefined
bool checkTableItemConflict(pTable table, pItem item) {
    pItem temp = searchTableItem(table, item->field->name);
    if (temp == NULL) return FALSE;
    while (temp) {
        if (!strcmp(temp->field->name, item->field->name)) {
            if (temp->field->type->kind == STRUCTURE ||
                item->field->type->kind == STRUCTURE)
                return TRUE;
            if (temp->symbolDepth == table->stack->curdepth) return TRUE;
        }
        temp = temp->nextHash;
    }
    return FALSE;
}

// add table item
void addTableItem(pTable table, pItem item) {
    assert(table != NULL && item != NULL);
    unsigned hashCode = getHashCode(item->field->name);
    pHash hash = table->hash;
    pStack stack = table->stack;
    item->nextSymbol = getCurDepthStackHead(stack);
    setCurDepthStackHead(stack, item);

    item->nextHash = getHashHead(hash, hashCode);
    setHashHead(hash, hashCode, item);
}

// delete item
void deleteTableItem(pTable table, pItem item) {
    assert(table != NULL && item != NULL);
    unsigned hashCode = getHashCode(item->field->name);
    if (item == getHashHead(table->hash, hashCode))
        setHashHead(table->hash, hashCode, item->nextHash);
    else {
        pItem cur = getHashHead(table->hash, hashCode);
        pItem last = cur;
        while (cur != item) {
            last = cur;
            cur = cur->nextHash;
        }
        last->nextHash = cur->nextHash;
    }
    deleteItem(item);
}

bool isStructDef(pItem src) {
    if (src == NULL) return FALSE;
    if (src->field->type->kind != STRUCTURE) return FALSE;
    if (src->field->type->u.structure.structName) return FALSE;
    return TRUE;
}

void clearCurDepthStackList(pTable table) {
    assert(table != NULL);
    pStack stack = table->stack;
    pItem temp = getCurDepthStackHead(stack);
    while (temp) {
        pItem tDelete = temp;
        temp = temp->nextSymbol;
        deleteTableItem(table, tDelete);
    }
    setCurDepthStackHead(stack, NULL);
    minusStackDepth(stack);
}

// Stack functions
pStack newStack() {
    pStack p = (pStack)malloc(sizeof(Stack));
    assert(p != NULL);
    p->stackArray = (pItem*)malloc(sizeof(pItem) * HASH_TABLE_SIZE);
    assert(p->stackArray != NULL);
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        p->stackArray[i] = NULL;
    }
    p->curdepth = 0;
    return p;
}

void deleteStack(pStack stack) {
    assert(stack != NULL);
    free(stack->stackArray);
    stack->stackArray = NULL;
    stack->curdepth = 0;
    free(stack);
}

void addStackDepth(pStack stack) {
    assert(stack != NULL);
    stack->curdepth++;
}

void minusStackDepth(pStack stack) {
    assert(stack != NULL);
    stack->curdepth--;
}

pItem getCurDepthStackHead(pStack stack) {
    assert(stack != NULL);
    return stack->stackArray[stack->curdepth];
    // return p == NULL ? NULL : p->stackArray[p->curStackDepth];
}

void setCurDepthStackHead(pStack stack, pItem newVal) {
    assert(stack != NULL);
    stack->stackArray[stack->curdepth] = newVal;
}
