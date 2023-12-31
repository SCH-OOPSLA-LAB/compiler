/****************************************************/
/* File: symtab.c                                   */
/* Symbol table implementation for the TINY compiler*/
/* (allows only one symbol table)                   */
/* Symbol table is implemented as a chained         */
/* hash table                                       */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symtab.h"

/* SHIFT is the power of two used as multiplier
   in hash function  */
#define SHIFT 4

/* the hash function */
static int hash(char * key) {
  int temp = 0;
  int i = 0;
  while (key[i] != '\0') {
    temp = ((temp << SHIFT) + key[i]) % SIZE;
    ++i;
  }
  return temp;
}

/*
 * Create new Scope
 */
Scope newScope(char * scopeName) {
  Scope newScope = (Scope) malloc(sizeof(struct ScopeListRec));
  newScope->name = scopeName;
  return newScope;
}

void popScope(void) {
  scopeStack[topScope--] = NULL;
}

void pushScope(Scope scope) {
  for (int i=0; i<sizeOfList; i++) {
    if (strcmp(scopeList[i]->name, scope->name) == 0) {
      scope->nestedLevel++;
    }
  }
  scopeStack[topScope++] = scope;
  insertScopeToList(scope);
}

void insertScopeToList(Scope scope) {
  scopeList[sizeOfList++] = scope;
}

Scope currScope() {
  return scopeStack[topScope-1];
}

/* Procedure st_insert inserts line numbers and
 * memory locations into the symbol table
 * loc = memory location is inserted only the
 * first time, otherwise ignored
 */

void st_insert( char * scopeName,
                char * name,
                ExpType type,
                TreeNode * treeNode,
                int loc ) {

  int h = hash(name);
  Scope scope = currScope();

  BucketList l = scope->hashTable[h];
  BucketList lb = scopeList[sizeOfList-1]->hashTable[h];
  // fprintf(listing, "%s %s st_insert\n", scope->name, name);
  /** try to find bucket */
  while ((l != NULL) && (strcmp(name, l->name) != 0)) l = l->next;

  /* variable not yet in BucketList */
  if (l == NULL) {
    l = (BucketList) malloc(sizeof(struct BucketListRec));
    l->name = name;
    l->treeNode = treeNode;
    l->lines = (LineList) malloc(sizeof(struct LineListRec));
    l->lines->lineno = treeNode->lineno;
    l->type = type;
    l->memloc = loc;
    l->lines->next = NULL;
    l->next = scope->hashTable[h];
    scope->hashTable[h] = l;

    // TODO Improve
    lb = (BucketList) malloc(sizeof(struct BucketListRec));
    lb->name = name;
    lb->treeNode = treeNode;
    lb->lines = (LineList) malloc(sizeof(struct LineListRec));
    lb->lines->lineno = treeNode->lineno;
    lb->type = type;
    lb->memloc = loc;
    lb->lines->next = NULL;
    lb->next = scopeList[sizeOfList-1]->hashTable[h];

  } else {
    /* already exist in the BucketList */
    LineList t = l->lines;
    while (t->next != NULL) t = t->next;
    t->next = (LineList) malloc(sizeof(struct LineListRec));
    t->next->lineno = lineno;
    t->next->next = NULL;
  }
} /* st_insert */

/* Function st_lookup returns Bucket
 * location of a variable or NULL if not found
 */
BucketList st_lookup(char * name) {

  Scope scope = currScope();
  int h = hash(name);
  BucketList bucket = scope->hashTable[h];

  while ((bucket != NULL) && (strcmp(name, bucket->name) != 0)) bucket = bucket->next;
  return bucket;
}

/* st_lookup_all_scope(char *name)*/
BucketList st_lookup_all_scope(char * name) {
  Scope scope = currScope();
  int h = hash(name);
  BucketList bucket;

  while (scope != NULL ) {
    BucketList bucket = scope->hashTable[h];
    while ((bucket != NULL) && (strcmp(name, bucket->name) != 0)) bucket = bucket->next;
    if (bucket != NULL) return bucket;
    scope = scope->parent;
  }
  return NULL;
}
/* Function st_lookup returns Bucket
 * location of a variable or NULL if not found
 * To check redefinition of function
 */
Scope st_lookup_scope(char * scopeName) {
  Scope scope = NULL;
  for (int i=0; i<sizeOfList; i++) {
    if (strcmp(scopeList[i]->name, scopeName) == 0) {
      scope = scopeList[i];
      break;
    }
  }
  return scope;
}

/* insert line */
void insertLines(char* name, int lineno) {
  Scope scope = currScope();
  int h = hash(name);
  BucketList l = scope->hashTable[h];

  while (scope != NULL) {

    if (l != NULL) {
      LineList lines = l->lines;

      while (lines->next != NULL) {
        lines = lines->next;
      }

      lines->next =  (LineList) malloc(sizeof(struct LineListRec));
      lines->next->lineno = lineno;
      lines->next->next = NULL;
      return;
    }
    scope = scope->parent;
  }
}

void printSymTabRows(Scope scope) {

  BucketList * hashTable = scope->hashTable;

  for (int i=0; i<SIZE; ++i) {
    if (hashTable[i] != NULL) {
      BucketList l = hashTable[i];
      TreeNode *node = l->treeNode;
      while (l != NULL) {

        LineList lines = l->lines;
        fprintf(listing,"%-10s", l->name);

        switch (node->nodekind) {
          case DecK:
          switch (node->kind.dec) {
            case FunK:
            fprintf(listing, "Function         ");
            break;
            case VarK:
            fprintf(listing, "Variable         ");
            break;
            case ArrVarK:
            fprintf(listing, "Array Variable   ");
            break;
           
            break;
            default:
            break;
          }
          break;
          default:
          break;
        }

        switch (l->type) {
          case Void:
          fprintf(listing, "Void          ");
          break;
          case Integer:
          fprintf(listing, "Integer       ");
          break;
          case IntegerArray:
          fprintf(listing, "Integer Array ");
          break;
          default:
          break;
        }

        // print memory location
        fprintf(listing, "%d          ", l->memloc);

        // print line numbers
        while (lines->next != NULL) {
          fprintf(listing, "%d, ", lines->lineno);
          lines = lines->next;
        }
        fprintf(listing, "%d\n", lines->lineno);
        l = l->next;
      }
    }
  }
}
/* Procedure printSymTab prints a formatted
 * listing of the symbol table contents
 * to the listing file
 */
void printSymTab(FILE * listing) {

  fprintf(listing, "\n------------------\n");
  fprintf(listing, "|  Symbol table  |");
  fprintf(listing, "\n------------------\n\n");

  for (int i = 0; i<sizeOfList; ++i) {
    Scope scope = scopeList[i];
    fprintf(listing, "Scope Name : %s, Nested Level: %d\n", scope->name, scope->nestedLevel);
    fprintf(listing, "---------------------------------------------------------------------------\n");
    fprintf(listing, "Name       Type             Data Type     Location   Line Numbers \n");
    fprintf(listing, "---------  ---------------  ------------  ---------  ----------------------\n");
    printSymTabRows(scope);
    fprintf(listing, "---------------------------------------------------------------------------\n");
    fputc('\n', listing);
  }
} /* printSymTab */
