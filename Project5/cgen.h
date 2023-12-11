#pragma once

#ifndef CGEN_H
#define CGEN_H

#define STACKMARKSIZE  8


#include "globals.h"


#define WORDSIZE 4




extern int TraceCode;




void codeGen(TreeNode* syntaxTree, char* fileName, char* moduleName);

#endif

