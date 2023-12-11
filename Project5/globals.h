
#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#ifndef YYPARSER
//#include "symtab.h"

#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif


#define MAXRESERVED 6

typedef enum

{
    ENDFILE, ERROR,

    IF, THEN, ELSE, END, INT, VOID, RETURN, WHILE,

    ID, NUM, LE, RE, GE, GT, NE, ASSIGN

   , EQ, LT, RT, PLUS, MINUS, TIMES, OVER, LPAREN, RPAREN, SEMI
    , LBRACKET, RBRACKET, COMMA, LBLOCK, RBLOCK,
} TokenType;

extern FILE* source;
extern FILE* listing;
extern FILE* code;

extern int lineno;





typedef enum { StmtK, ExpK, DecK } NodeKind;
typedef enum { SelectionK, IterationK, ReturnK, CallK, CompoundK } StmtKind;
typedef enum { OpK, IdK, ConstK, AssignK } ExpKind;
typedef enum { VarK, ArrVarK, FunK} DecKind;



typedef enum { Void, Integer, IntegerArray, Function} ExpType;

#define MAXCHILDREN 3
typedef struct arrayAttr {
    TokenType type;
    char* name;
    int size;
} ArrayAttr;
typedef struct treeNode {
    struct treeNode* child[MAXCHILDREN];
    struct treeNode* sibling;
    int lineno;
    NodeKind nodekind;

    union {
        StmtKind stmt;
        ExpKind exp;
        DecKind dec;
        
    } kind;
    union {
        TokenType op;
        TokenType type;
        int val;
        char* name;
        ArrayAttr arr;
        struct ScopeRec* scope;
    } attr;
    TokenType  op;
    int        val;
    char* name;
    ExpType    functionReturnType;
    ExpType    variableDataType;
    ExpType         expressionType;
    int isParameter;
    int isGlobal;
    struct treeNode* declaration;
    int assemblyAreaSize;
    int localSize;
    int offset;
} TreeNode;


extern int EchoSource;

extern int TraceScan;


extern int TraceParse;


extern int TraceAnalyze;

extern int TraceCode;


extern int Error;
#endif
