#ifndef ENUM_H
#define ENUM_H

// define node type
typedef enum _nodeType {
    TOKEN_INT,
    TOKEN_FLOAT,
    TOKEN_ID,
    TOKEN_TYPE,
    TOKEN_OTHER,
    NOT_A_TOKEN

} NodeType;

typedef enum ukind { BASIC, ARRAY, STRUCTURE, FUNCTION } Kind;
typedef enum ubasicType { INT_TYPE, FLOAT_TYPE } BasicType;
typedef enum uerrorType {
    UNDEF_VAR = 1,         
    UNDEF_FUNC,            
    REDEF_VAR,             
    REDEF_FUNC,            
    TYPE_MISMATCH_ASSIGN,  // Type mismatchedfor assignment.
    LEFT_VAR_ASSIGN,  // The left-hand side of an assignment must be a variable.
    TYPE_MISMATCH_OP,      // Type mismatched for operands.
    TYPE_MISMATCH_RETURN,  // Type mismatched for return.
    FUNC_AGRC_MISMATCH,    // Function is not applicable for arguments
    NOT_A_ARRAY,           
    NOT_A_FUNC,            
    NOT_A_INT,             
    ILLEGAL_USE_DOT,       
    NONEXISTFIELD,         
    REDEF_FEILD,           
    DUPLICATED_NAME,       
    UNDEF_STRUCT           
} ErrorType;

#endif