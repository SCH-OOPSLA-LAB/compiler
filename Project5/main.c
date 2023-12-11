
#define _CRT_SECURE_NO_WARNINGS
#include "globals.h"


#define NO_PARSE FALSE

#define NO_ANALYZE FALSE


#define NO_CODE FALSE

#include "util.h"
#if NO_PARSE
#include "scan.h"
#else
#include "parse.h"
#if !NO_ANALYZE
#include "analyze.h"
#if !NO_CODE
//#include "cgen.h"
#endif
#endif
#endif


int lineno = 0;
FILE* source;
FILE* listing;
FILE* code;


int EchoSource = FALSE;
int TraceScan = FALSE;
int TraceParse = TRUE;
int TraceAnalyze = TRUE;
int TraceCode = TRUE;

int Error = FALSE;

void genCode(TreeNode* t);
void genDec(TreeNode* t);
void genExp(TreeNode* t);
void genStmt(TreeNode* t);
char* typeName2(ExpType type);
main(int argc, char* argv[])
{
    TreeNode* syntaxTree;
    char pgm[120]; 

 
    if (argc != 2)
    {
        fprintf(stderr, "usage: %s <filename>\n", argv[0]);
        exit(1);
    }

  
    strcpy(pgm, argv[1]);
    if (strchr(pgm, '.') == NULL)
        strcat(pgm, ".cm");

    source = fopen(pgm, "r");
    if (source == NULL)
    {
        fprintf(stderr, "File %s not found\n", pgm);
        exit(1);
    }
    listing = stdout;
    fprintf(listing, "\nC-Minus COMPILATION: %s\n", pgm);


#if NO_PARSE
    while (getToken() != ENDFILE);
#else
    syntaxTree = Parse();
    if (TraceParse) {
        fprintf(listing, "\nSyntax tree:\n");
        printTree(syntaxTree);
    }
    
//#if !NO_ANALYZE
//    if (!Error)
//    {
//        if (TraceAnalyze) fprintf(listing, "\nBuilding Symbol Table...\n");
//        buildSymtab(syntaxTree);
//        if (TraceAnalyze) fprintf(listing, "\nChecking Types...\n");
//        typeCheck(syntaxTree);
//        if (TraceAnalyze) fprintf(listing, "\nType Checking Finished\n");
//    }
#if !NO_CODE
    if (!Error)
    {
        printf("#include stdio.h\n");
        genCode(syntaxTree);
        buildSymbolTable(syntaxTree);

        genCode(syntaxTree);
    }
#endif
#endif
//#endif
    fclose(source);
    return 0;
}



void genStmt(TreeNode* t) {
    TreeNode* p1 = NULL;
    TreeNode* p2 = NULL;
    TreeNode* p3 = NULL;

    switch (t->kind.stmt)
    {
    case SelectionK:
        p1 = t->child[0];
        p2 = t->child[1];
        p3 = t->child[2];
        printf("if(");
        genCode(p1);
        printf(")\n");
        printf("{\n");
        genCode(p2);
        printf("\n}\n");
        if (p3 != NULL) {
            printf("else");
            printf("\n{\n");
            genCode(p3);
            printf("\n}\n");
        }
        break;
    case IterationK:
        p1 = t->child[0];
        p2 = t->child[1];
        printf("while(\n");
        genCode(p2);
        printf("}\n");
        break;
    case CompoundK:
        printf("\n{\n");
        p1 = t->child[0];
        p2 = t->child[1];
        genCode(p1);
        printf("\n");
        while (p2 != NULL) {
            genCode(p2);
            p2 = p2->sibling;
        }
        printf("}\n");
        
        break;
    case ReturnK:
        p1 = t->child[0];
        printf("return ");
        genCode(p1);
       
        break;
    case CallK:
        p1 = t;
        printf("%s", p1->name);
        printf("(");
        genCode(p1->child[0]);
        printf(")");
        
    default:
        break;
    }
    
}
char* typeName2(ExpType type)
{
    static char i[] = "int";
    static char v[] = "void";
    static char invalid[] = "<<invalid type>>";

    switch (type)
    {
    case Integer: return i; break;
    case Void:    return v; break;
    default:      return invalid;
    }
}

void genDec(TreeNode* t) {
    TreeNode* p1 = NULL;
    TreeNode* p2 = NULL;
    
    switch (t->kind.dec) {
    case VarK:

        printf("%s %s ", typeName2(t->variableDataType), t->name);

        genCode(t->sibling);
      
        break;
    case ArrVarK:
        printf("%s %s[%d] ", typeName2(t->variableDataType), t->name, t->val);
        genCode(t->sibling);
        
        break;
    case FunK:

        p1 = t->child[0]; // 매개변수 리스트
        p2 = t->child[1]; // 함수 몸체



        printf("%s %s ", typeName2(t->functionReturnType), t->name);


        printf("(");
        genCode(p1); // 매개변수 리스트 처리
        printf(")");
        
        genCode(p2); // 함수 몸체 처리
        
        genCode(t->sibling);
        
        break;
    }
    
 
}

void genExp(TreeNode* t) {
    TreeNode* p1 = NULL;
    TreeNode* p2 = NULL;
    switch (t->kind.exp)
    {
    case ConstK:
        printf("%d", t->val);
        break;
    case IdK:
        printf("%s", t->name);
        break;
    case OpK:
        p1 = t->child[0];
        p2 = t->child[1];
        switch (t->op)
        {
        case PLUS:
            genCode(p1);
            printf(" + ");
            genCode(p2);
            break;
        case MINUS:
            genCode(p1);
            printf(" - ");
            genCode(p2);
            break;
        case TIMES:
            genCode(p1);
            printf(" * ");
            genCode(p2);
            break;
        case OVER:
            genCode(p1);
            printf(" / ");
            genCode(p2);
            break;
        case LE:
            genCode(p1);
            printf(" <= ");
            genCode(p2);
            break;
        case LT:
            genCode(p1);
            printf(" < ");
            genCode(p2);
            break;
        case GE:
            genCode(p1);
            printf(" >= ");
            genCode(p2);
            break;
        case GT:
            genCode(p1);
            printf(" > ");
            genCode(p2);
            break;
            
        case NE:
            genCode(p1);
            printf(" != ");
            genCode(p2);
            break;
          
        case EQ:
            genCode(p1);
            printf(" == ");
            genCode(p2);
            break;

        case ASSIGN:
            printf("%s=", p1->name);
            genCode(p1);
            break;
        case CallK:
            
            printf("%s(", t->name);
            while (p1 != NULL) {
                genCode(p1);
                if (p1->sibling != NULL)
                    printf(",");
                p1 = p1->sibling;
            }
            printf(");");
            break;
        }
        break;  // 이 부분이 빠져있었습니다.
    default:
        break;
    }
    
}
void genCode(TreeNode* t) {
    
    
    if (t != NULL) {
        
        switch (t->nodekind)
        {

        case StmtK:
            genStmt(t);
            break;
        case ExpK:
            genExp(t);
            break;
        case DecK:
            genDec(t);
            break;
        default:
            break;
        }
    }
}