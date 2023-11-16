/****************************************************/
/* File: globals.h                                  */
/* Global types and vars for TINY compiler          */
/* must come before other include files             */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#ifndef YYPARSER
#include "symtab.h"

#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

/* MAXRESERVED = the number of reserved words */
#define MAXRESERVED 6

typedef enum

{
    ENDFILE, ERROR,

    IF, THEN, ELSE, END, INT, VOID, RETURN, WHILE,

    ID, NUM,LE,RE,GE,NE,GT,

    ASSIGN, EQ, LT, RT, PLUS, MINUS, TIMES, OVER, LPAREN, RPAREN, SEMI, LBLOCK, RBLOCK,COMMA, LBRACKET, RBRACKET
} TokenType;

extern FILE* source; /* source code text file */
extern FILE* listing; /* listing output text file */
extern FILE* code; /* code text file for TM simulator */

extern int lineno; /* source line number for listing */

/**************************************************/
/***********   Syntax tree for parsing ************/
/**************************************************/

typedef enum { StmtK, ExpK, DeclK } NodeKind;
typedef enum { IfK, WhileK, AssignK, CompoundK, ReturnK } StmtKind;
typedef enum { OpK, ConstK, IdK, TypeK, ArrIdK, CallK, CalcK } ExpKind;
typedef enum { VarK, FunK, ArrVarK, ArrParamK, ParamK } DeclKind;

/* ExpType is used for type checking */
typedef enum { Void, Integer, IntegerArray } ExpType;

#define MAXCHILDREN 3

struct ScopeListRec;

typedef struct ArrayAttribute {
  TokenType type;
  char * name;
  int size;
} ArrAttr;

typedef struct treeNode {
  struct treeNode * child[MAXCHILDREN];
  struct treeNode * sibling;
  int lineno;
  NodeKind nodekind;

  union {
   StmtKind stmt;
   ExpKind exp;
   DeclKind decl;
  } kind;

  union {
   TokenType op;
   TokenType type;
   int val;
   char * name;
   ArrAttr arr;
   struct ScopeListRec * scope;
  } attr;

  ExpType type;
} TreeNode;


extern int EchoSource;


extern int TraceScan;

extern int TraceParse;


extern int TraceAnalyze;



extern int TraceCode;

extern int Error;
#endif
