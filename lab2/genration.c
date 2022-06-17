#include "libs.h"

// Global function
void traverseTree(TreeNode* node) {
    if (node == NULL) return;

    if (!strcmp(node->name, "ExtDef")) ExtDef(node);

    traverseTree(node->child);
    traverseTree(node->next);
}

// Generate symbol table functions
void ExtDef(TreeNode* node) {
    assert(node != NULL);
    // ExtDef -> Specifier ExtDecList SEMI
    //         | Specifier SEMI
    //         | Specifier FunDec CompSt
    pType specifierType = Specifier(node->child);
    char* secondName = node->child->next->name;

    
    // ExtDef -> Specifier ExtDecList SEMI
    if (!strcmp(secondName, "ExtDecList")) {
        // TODO: process first situation
        ExtDecList(node->child->next, specifierType);
    }
    // ExtDef -> Specifier FunDec CompSt
    else if (!strcmp(secondName, "FunDec")) {
        // TODO: process third situation
        FunDec(node->child->next, specifierType);
        CompSt(node->child->next->next, specifierType);
    }
    //Specifier SEMI
    if (specifierType) deleteType(specifierType);

}

void ExtDecList(TreeNode* node, pType specifier) {
    assert(node != NULL);
    // ExtDecList -> VarDec
    //             | VarDec COMMA ExtDecList
    TreeNode* temp = node;
    while (temp) {
        pItem item = VarDec(temp->child, specifier);
        if (checkTableItemConflict(table, item)) {
            char msg[100] = {0};
            sprintf(msg, "Redefined variable \"%s\".", item->field->name);
            pError(REDEF_VAR, temp->lineNumber, msg);
            deleteItem(item);
        } else {
            addTableItem(table, item);
        }
        if (temp->child->next) {
            temp = temp->next->next->child;
        } else {
            break;
        }
    }
}

pType Specifier(TreeNode* node) {
    assert(node != NULL);
    // Specifier -> TYPE
    //            | StructSpecifier

    TreeNode* t = node->child;
    // Specifier -> TYPE
    if (!strcmp(t->name, "TYPE")) {
        if (!strcmp(t->val, "float")) {
            return newType(BASIC, FLOAT_TYPE);
        } else {
            return newType(BASIC, INT_TYPE);
        }
    }
    // Specifier -> StructSpecifier
    else {
        return StructSpecifier(t);
    }
}

pType StructSpecifier(TreeNode* node) {
    assert(node != NULL);
    // StructSpecifier -> STRUCT OptTag LC DefList RC
    //                  | STRUCT Tag

    // OptTag -> ID | e
    // Tag -> ID
    pType returnType = NULL;
    TreeNode* t = node->child->next;
    // StructSpecifier->STRUCT OptTag LC DefList RC
    // printTreeInfo(t, 0);
    if (strcmp(t->name, "Tag")) {
        // addStructLayer(table);
        pItem structItem =
            newItem(table->stack->curdepth,
                    newFieldList("", newType(STRUCTURE, NULL, NULL)));
        if (!strcmp(t->name, "OptTag")) {
            setFieldListName(structItem->field, t->child->val);
            t = t->next;
        }
        // unnamed struct
        else {
            table->unNameStructNum++;
            char structName[20] = {0};
            sprintf(structName, "%d", table->unNameStructNum);
            // printf("unNamed struct's name is %s.\n", structName);
            setFieldListName(structItem->field, structName);
        }

        if (!strcmp(t->next->name, "DefList")) {
            DefList(t->next, structItem);
        }

        if (checkTableItemConflict(table, structItem)) {
            char msg[100] = {0};
            sprintf(msg, "Duplicated name \"%s\".", structItem->field->name);
            pError(DUPLICATED_NAME, node->lineNumber, msg);
            deleteItem(structItem);
        } else {
            returnType = newType(
                STRUCTURE, String(structItem->field->name),
                copyFieldList(structItem->field->type->u.structure.field));


            if (!strcmp(node->child->next->name, "OptTag")) {
                addTableItem(table, structItem);
            }
            // OptTag -> e
            else {
                deleteItem(structItem);
            }
        }

    }

    // StructSpecifier->STRUCT Tag
    else {
        pItem structItem = searchTableItem(table, t->child->val);
        if (structItem == NULL || !isStructDef(structItem)) {
            char msg[100] = {0};
            sprintf(msg, "Undefined structure \"%s\".", t->child->val);
            pError(UNDEF_STRUCT, node->lineNumber, msg);
        } else
            returnType = newType(
                STRUCTURE, String(structItem->field->name),
                copyFieldList(structItem->field->type->u.structure.field));
    }
    // printType(returnType);
    return returnType;
}

pItem VarDec(TreeNode* node, pType specifier) {
    assert(node != NULL);
    // VarDec -> ID
    //         | VarDec LB INT RB
    TreeNode* id = node;
    // get ID
    while (id->child) id = id->child;
    pItem p = newItem(table->stack->curdepth, newFieldList(id->val, NULL));

    // VarDec -> ID
    // printTreeInfo(node, 0);
    if (!strcmp(node->child->name, "ID")) {
        // printf("copy type tp %s.\n", node->child->val);
        p->field->type = copyType(specifier);
    }
    // VarDec -> VarDec LB INT RB
    else {
        TreeNode* varDec = node->child;
        pType temp = specifier;
        // printf("VarDec -> VarDec LB INT RB.\n");
        while (varDec->next) {
            p->field->type =
                newType(ARRAY, copyType(temp), atoi(varDec->next->next->val));
            //  p->field->type->u.array.elem->kind,
            //  p->field->type->u.array.size);
            temp = p->field->type;
            varDec = varDec->child;
        }
    }
    return p;
}



void FunDec(TreeNode* node, pType returnType) {
    assert(node != NULL);
    // FunDec -> ID LP VarList RP
    //         | ID LP RP
    pItem p =
        newItem(table->stack->curdepth,
                newFieldList(node->child->val,
                             newType(FUNCTION, 0, NULL, copyType(returnType))));

    // FunDec -> ID LP VarList RP
    if (!strcmp(node->child->next->next->name, "VarList")) {
        VarList(node->child->next->next, p);
    }

    // FunDec -> ID LP RP don't need process

    // check redefine
    if (checkTableItemConflict(table, p)) {
        char msg[100] = {0};
        sprintf(msg, "Redefined function \"%s\".", p->field->name);
        pError(REDEF_FUNC, node->lineNumber, msg);
        deleteItem(p);
        p = NULL;
    } else {
        addTableItem(table, p);
    }
}

void VarList(TreeNode* node, pItem func) {
    assert(node != NULL);
    // VarList -> ParamDec COMMA VarList
    //          | ParamDec
    addStackDepth(table->stack);
    int argc = 0;
    TreeNode* temp = node->child;
    pFieldList cur = NULL;

    // VarList -> ParamDec
    pFieldList paramDec = ParamDec(temp);
    func->field->type->u.function.argv = copyFieldList(paramDec);
    cur = func->field->type->u.function.argv;
    argc++;

    // VarList -> ParamDec COMMA VarList
    while (temp->next) {
        temp = temp->next->next->child;
        paramDec = ParamDec(temp);
        if (paramDec) {
            cur->tail = copyFieldList(paramDec);
            cur = cur->tail;
            argc++;
        }
    }

    func->field->type->u.function.argc = argc;

    minusStackDepth(table->stack);
}

pFieldList ParamDec(TreeNode* node) {
    assert(node != NULL);
    // ParamDec -> Specifier VarDec
    pType specifierType = Specifier(node->child);
    pItem p = VarDec(node->child->next, specifierType);
    if (specifierType) deleteType(specifierType);
    if (checkTableItemConflict(table, p)) {
        char msg[100] = {0};
        sprintf(msg, "Redefined variable \"%s\".", p->field->name);
        pError(REDEF_VAR, node->lineNumber, msg);
        deleteItem(p);
        return NULL;
    } else {
        addTableItem(table, p);
        return p->field;
    }
}

void CompSt(TreeNode* node, pType returnType) {
    assert(node != NULL);
    // CompSt -> LC DefList StmtList RC
    // printTreeInfo(node, 0);
    addStackDepth(table->stack);
    TreeNode* temp = node->child->next;
    if (!strcmp(temp->name, "DefList")) {
        DefList(temp, NULL);
        temp = temp->next;
    }
    if (!strcmp(temp->name, "StmtList")) {
        StmtList(temp, returnType);
    }

    clearCurDepthStackList(table);
}

void StmtList(TreeNode* node, pType returnType) {
    // assert(node != NULL);
    // StmtList -> Stmt StmtList
    //           | e
    // printTreeInfo(node, 0);
    while (node) {
        Stmt(node->child, returnType);
        node = node->child->next;
    }
}

void Stmt(TreeNode* node, pType returnType) {
    assert(node != NULL);
    // Stmt -> Exp SEMI
    //       | CompSt
    //       | RETURN Exp SEMI
    //       | IF LP Exp RP Stmt
    //       | IF LP Exp RP Stmt ELSE Stmt
    //       | WHILE LP Exp RP Stmt
    // printTreeInfo(node, 0);

    pType expType = NULL;
    // Stmt -> Exp SEMI
    if (!strcmp(node->child->name, "Exp")) expType = Exp(node->child);

    // Stmt -> CompSt
    else if (!strcmp(node->child->name, "CompSt"))
        CompSt(node->child, returnType);

    // Stmt -> RETURN Exp SEMI
    else if (!strcmp(node->child->name, "RETURN")) {
        expType = Exp(node->child->next);

        // check return type
        if (!checkType(returnType, expType))
            pError(TYPE_MISMATCH_RETURN, node->lineNumber,
                   "Type mismatched for return.");
    }

    // Stmt -> IF LP Exp RP Stmt
    else if (!strcmp(node->child->name, "IF")) {
        TreeNode* stmt = node->child->next->next->next->next;
        expType = Exp(node->child->next->next);
        Stmt(stmt, returnType);
        // Stmt -> IF LP Exp RP Stmt ELSE Stmt
        if (stmt->next != NULL) Stmt(stmt->next->next, returnType);
    }

    // Stmt -> WHILE LP Exp RP Stmt
    else if (!strcmp(node->child->name, "WHILE")) {
        expType = Exp(node->child->next->next);
        Stmt(node->child->next->next->next->next, returnType);
    }

    if (expType) deleteType(expType);
}

void DefList(TreeNode* node, pItem structInfo) {
    // assert(node != NULL);
    // DefList -> Def DefList
    //          | e
    while (node) {
        Def(node->child, structInfo);
        node = node->child->next;
    }
}

void Def(TreeNode* node, pItem structInfo) {
    assert(node != NULL);
    // Def -> Specifier DecList SEMI
    pType dectype = Specifier(node->child);

    DecList(node->child->next, dectype, structInfo);
    if (dectype) deleteType(dectype);
}

void DecList(TreeNode* node, pType specifier, pItem structInfo) {
    assert(node != NULL);
    // DecList -> Dec
    //          | Dec COMMA DecList
    TreeNode* temp = node;
    while (temp) {
        Dec(temp->child, specifier, structInfo);
        if (temp->child->next)
            temp = temp->child->next->next;
        else
            break;
    }
}

void Dec(TreeNode* node, pType specifier, pItem structInfo) {
    assert(node != NULL);
    // Dec -> VarDec
    //      | VarDec ASSIGNOP Exp

    // Dec -> VarDec
    if (node->child->next == NULL) {
        if (structInfo != NULL) {
            
            pItem decitem = VarDec(node->child, specifier);
            pFieldList payload = decitem->field;
            pFieldList structField = structInfo->field->type->u.structure.field;
            pFieldList last = NULL;
            while (structField != NULL) {
                // then we have to check
                if (!strcmp(payload->name, structField->name)) {

                    char msg[100] = {0};
                    sprintf(msg, "Redefined field \"%s\".",
                            decitem->field->name);
                    pError(REDEF_FEILD, node->lineNumber, msg);
                    deleteItem(decitem);
                    return;
                } else {
                    last = structField;
                    structField = structField->tail;
                }
            }
           
            if (last == NULL) {
                // that is good
                structInfo->field->type->u.structure.field =
                    copyFieldList(decitem->field);
            } else {
                last->tail = copyFieldList(decitem->field);
            }
            deleteItem(decitem);
        } else {
            
            pItem decitem = VarDec(node->child, specifier);
            if (checkTableItemConflict(table, decitem)) {
                
                char msg[100] = {0};
                sprintf(msg, "Redefined variable \"%s\".",
                        decitem->field->name);
                pError(REDEF_VAR, node->lineNumber, msg);
                deleteItem(decitem);
            } else {
                addTableItem(table, decitem);
            }
        }
    }
    // Dec -> VarDec ASSIGNOP Exp
    else {
        if (structInfo != NULL) {
            
            pError(REDEF_FEILD, node->lineNumber,
                   "Illegal initialize variable in struct.");
        } else {
            
            pItem decitem = VarDec(node->child, specifier);
            pType exptype = Exp(node->child->next->next);
            if (checkTableItemConflict(table, decitem)) {
                //confliction，error
                char msg[100] = {0};
                sprintf(msg, "Redefined variable \"%s\".",
                        decitem->field->name);
                pError(REDEF_VAR, node->lineNumber, msg);
                deleteItem(decitem);
            }
            if (!checkType(decitem->field->type, exptype)) {
                
                //error
                pError(TYPE_MISMATCH_ASSIGN, node->lineNumber,
                       "Type mismatchedfor assignment.");
                deleteItem(decitem);
            }
            if (decitem->field->type && decitem->field->type->kind == ARRAY) {
                
                pError(TYPE_MISMATCH_ASSIGN, node->lineNumber,
                       "Illegal initialize variable.");
                deleteItem(decitem);
            } else {
                addTableItem(table, decitem);
            }
            if (exptype) deleteType(exptype);
        }
    }
}

pType Exp(TreeNode* node) {
    assert(node != NULL);
    // Exp -> Exp ASSIGNOP Exp
    //      | Exp AND Exp
    //      | Exp OR Exp
    //      | Exp RELOP Exp
    //      | Exp PLUS Exp
    //      | Exp MINUS Exp
    //      | Exp STAR Exp
    //      | Exp DIV Exp
    //      | LP Exp RP
    //      | MINUS Exp
    //      | NOT Exp
    //      | ID LP Args RP
    //      | ID LP RP
    //      | Exp LB Exp RB
    //      | Exp DOT ID
    //      | ID
    //      | INT
    //      | FLOAT
    TreeNode* t = node->child;
    // exp will only check if the cal is right
    //  printTable(table);
    if (!strcmp(t->name, "Exp")) {
        if (strcmp(t->next->name, "LB") && strcmp(t->next->name, "DOT")) {
            pType p1 = Exp(t);
            pType p2 = Exp(t->next->next);
            pType returnType = NULL;

            // Exp -> Exp ASSIGNOP Exp
            if (!strcmp(t->next->name, "ASSIGNOP")) {
                //check left-hand
                TreeNode* tchild = t->child;

                if (!strcmp(tchild->name, "FLOAT") ||
                    !strcmp(tchild->name, "INT")) {
                   
                    pError(LEFT_VAR_ASSIGN, t->lineNumber,
                           "The left-hand side of an assignment must be "
                           "avariable.");

                } else if (!strcmp(tchild->name, "ID") ||
                           !strcmp(tchild->next->name, "LB") ||
                           !strcmp(tchild->next->name, "DOT")) {
                    if (!checkType(p1, p2)) {
                       
                        pError(TYPE_MISMATCH_ASSIGN, t->lineNumber,
                               "Type mismatched for assignment.");
                    } else
                        returnType = copyType(p1);
                } else {
                   
                    pError(LEFT_VAR_ASSIGN, t->lineNumber,
                           "The left-hand side of an assignment must be "
                           "avariable.");
                }

            }
            // Exp -> Exp AND Exp
            //      | Exp OR Exp
            //      | Exp RELOP Exp
            //      | Exp PLUS Exp
            //      | Exp MINUS Exp
            //      | Exp STAR Exp
            //      | Exp DIV Exp
            else {
                if (p1 && p2 && (p1->kind == ARRAY || p2->kind == ARRAY)) {
                   
                    pError(TYPE_MISMATCH_OP, t->lineNumber,
                           "Type mismatched for operands.");
                } else if (!checkType(p1, p2)) {
                   
                    pError(TYPE_MISMATCH_OP, t->lineNumber,
                           "Type mismatched for operands.");
                } else {
                    if (p1 && p2) {
                        returnType = copyType(p1);
                    }
                }
            }

            if (p1) deleteType(p1);
            if (p2) deleteType(p2);
            return returnType;
        }
        
        else {
            // Exp -> Exp LB Exp RB
            if (!strcmp(t->next->name, "LB")) {
                
                pType p1 = Exp(t);
                pType p2 = Exp(t->next->next);
                pType returnType = NULL;

                if (!p1) {
                   
                } else if (p1 && p1->kind != ARRAY) {
                   
                    char msg[100] = {0};
                    sprintf(msg, "\"%s\" is not an array.", t->child->val);
                    pError(NOT_A_ARRAY, t->lineNumber, msg);
                } else if (!p2 || p2->kind != BASIC ||
                           p2->u.basic != INT_TYPE) {
                    
                    char msg[100] = {0};
                    sprintf(msg, "\"%s\" is not an integer.",
                            t->next->next->child->val);
                    pError(NOT_A_INT, t->lineNumber, msg);
                } else {
                    returnType = copyType(p1->u.array.elem);
                }
                if (p1) deleteType(p1);
                if (p2) deleteType(p2);
                return returnType;
            }
            // Exp -> Exp DOT ID
            else {
                pType p1 = Exp(t);
                pType returnType = NULL;
                if (!p1 || p1->kind != STRUCTURE ||
                    !p1->u.structure.structName) {
                    pError(ILLEGAL_USE_DOT, t->lineNumber, "Illegal use of \".\".");
                    if (p1) deleteType(p1);
                } else {
                    TreeNode* ref_id = t->next->next;
                    pFieldList structfield = p1->u.structure.field;
                    while (structfield != NULL) {
                        if (!strcmp(structfield->name, ref_id->val)) {
                            break;
                        }
                        structfield = structfield->tail;
                    }
                    if (structfield == NULL) {
                        
                        printf("Error type %d at Line %d: %s.\n", 14, t->lineNumber,
                               "not exist field");
                        ;
                    } else {
                        returnType = copyType(structfield->type);
                    }
                }
                if (p1) deleteType(p1);
                return returnType;
            }
        }
    }
    // Exp -> MINUS Exp
    //      | NOT Exp
    else if (!strcmp(t->name, "MINUS") || !strcmp(t->name, "NOT")) {
        pType p1 = Exp(t->next);
        pType returnType = NULL;
        if (!p1 || p1->kind != BASIC) {
            printf("Error type %d at Line %d: %s.\n", 7, t->lineNumber,
                   "TYPE_MISMATCH_OP");
        } else {
            returnType = copyType(p1);
        }
        if (p1) deleteType(p1);
        return returnType;
    } else if (!strcmp(t->name, "LP")) {
        return Exp(t->next);
    }
    // Exp -> ID LP Args RP
    //		| ID LP RP
    else if (!strcmp(t->name, "ID") && t->next) {
        pItem funcInfo = searchTableItem(table, t->val);

        // function not find
        if (funcInfo == NULL) {
            char msg[100] = {0};
            sprintf(msg, "Undefined function \"%s\".", t->val);
            pError(UNDEF_FUNC, node->lineNumber, msg);
            return NULL;
        } else if (funcInfo->field->type->kind != FUNCTION) {
            char msg[100] = {0};
            sprintf(msg, "\"i\" is not a function.", t->val);
            pError(NOT_A_FUNC, node->lineNumber, msg);
            return NULL;
        }
        // Exp -> ID LP Args RP
        else if (!strcmp(t->next->next->name, "Args")) {
            Args(t->next->next, funcInfo);
            return copyType(funcInfo->field->type->u.function.returnType);
        }
        // Exp -> ID LP RP
        else {
            if (funcInfo->field->type->u.function.argc != 0) {
                char msg[100] = {0};
                sprintf(msg,
                        "too few arguments to function \"%s\", except %d args.",
                        funcInfo->field->name,
                        funcInfo->field->type->u.function.argc);
                pError(FUNC_AGRC_MISMATCH, node->lineNumber, msg);
            }
            return copyType(funcInfo->field->type->u.function.returnType);
        }
    }
    // Exp -> ID
    else if (!strcmp(t->name, "ID")) {
        pItem tp = searchTableItem(table, t->val);
        if (tp == NULL || isStructDef(tp)) {
            char msg[100] = {0};
            sprintf(msg, "Undefined variable \"%s\".", t->val);
            pError(UNDEF_VAR, t->lineNumber, msg);
            return NULL;
        } else {
            // good
            return copyType(tp->field->type);
        }
    } else {
        // Exp -> FLOAT
        if (!strcmp(t->name, "FLOAT")) {
            return newType(BASIC, FLOAT_TYPE);
        }
        // Exp -> INT
        else {
            return newType(BASIC, INT_TYPE);
        }
    }
}

void Args(TreeNode* node, pItem funcInfo) {
    assert(node != NULL);
    TreeNode* temp = node;
    pFieldList arg = funcInfo->field->type->u.function.argv;
    
    while (temp) {
        if (arg == NULL) {
            char msg[100] = {0};
            sprintf(
                msg, "too many arguments to function \"%s\", except %d args.",
                funcInfo->field->name, funcInfo->field->type->u.function.argc);
            pError(FUNC_AGRC_MISMATCH, node->lineNumber, msg);
            break;
        }
        pType realType = Exp(temp->child);
        
        if (!checkType(realType, arg->type)) {
            char msg[100] = {0};
            sprintf(msg, "Function \"%s\" is not applicable for arguments.",
                    funcInfo->field->name);
            pError(FUNC_AGRC_MISMATCH, node->lineNumber, msg);
            if (realType) deleteType(realType);
            return;
        }
        if (realType) deleteType(realType);

        arg = arg->tail;
        if (temp->child->next) {
            temp = temp->child->next->next;
        } else {
            break;
        }
    }
    if (arg != NULL) {
        char msg[100] = {0};
        sprintf(msg, "too few arguments to function \"%s\", except %d args.",
                funcInfo->field->name, funcInfo->field->type->u.function.argc);
        pError(FUNC_AGRC_MISMATCH, node->lineNumber, msg);
    }
}
