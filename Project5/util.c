#define _CRT_SECURE_NO_WARNINGS
#include "globals.h"
#include "util.h"

/* Procedure printToken prints a token
 * and its lexeme to the listing file
 * 
 */
static TreeNode* allocNewNode(void);
void printToken(TokenType token, const char* tokenString)
{
    switch (token)
    {
    case ELSE:
       
    case IF:
       
    case INT:
 
    case RETURN:
   
    case VOID:
    
    case WHILE:
        fprintf(listing, "reserved word: %s\n", tokenString);
        break;
    case PLUS:     fprintf(listing, "+");  break;
    case MINUS:    fprintf(listing, "-");  break;
    case TIMES:    fprintf(listing, "*");  break;
    case OVER:     fprintf(listing, "/");  break;
    case LT:       fprintf(listing, "<");  break;
    case LE:       fprintf(listing, "<="); break;
    case GT:       fprintf(listing, ">");  break;
    case GE:       fprintf(listing, ">="); break;
    case EQ:       fprintf(listing, "=="); break;
    case NE:       fprintf(listing, "!="); break;
    case ASSIGN:   fprintf(listing, "=");  break;
    case SEMI:     fprintf(listing, ";\n");  break;
    case COMMA:    fprintf(listing, ",\n");  break;
    case LPAREN:   fprintf(listing, "(\n");  break;
    case RPAREN:   fprintf(listing, ")\n");  break;
    case LBRACKET: fprintf(listing, "[\n");  break;
    case RBRACKET: fprintf(listing, "]\n");  break;
    case LBLOCK:   fprintf(listing, "{\n");  break;
    case RBLOCK:   fprintf(listing, "}\n");  break;
    case ENDFILE:  fprintf(listing, "%s\n", "ENDFILE"); break;
    case NUM: fprintf(listing, "NUM, val = %s\n", tokenString); break;
    case ID: fprintf(listing, "ID, name = %s\n", tokenString); break;
    case ERROR: fprintf(listing, "ERROR: %s\n", tokenString); break;
    default: /* should never happen */
        fprintf(listing, "Unknown token: %d \n", token);
    }
}
char* typeName(ExpType type)
{
    static char i[] = "integer";
    static char v[] = "void";
    static char invalid[] = "<<invalid type>>";

    switch (type)
    {
    case Integer: return i; break;
    case Void:    return v; break;
    default:      return invalid;
    }
}

static int indentno = 0;

#define INDENT   indentno += 4
#define UNINDENT indentno -= 4

static void printSpaces(void) {
    int i;
    for (i = 0;i < indentno;i++)
        fprintf(listing, " ");
}
void printTree(TreeNode* tree)
{
    int i;

    INDENT;

    while (tree != NULL)  /* try not to visit null tree children */
    {
        printSpaces();

        /* Examine node type, and base output on that. */
        if (tree->nodekind == DecK)
        {
            switch (tree->kind.dec)
            {
            case VarK:
                fprintf(listing, "[Var declaration \"%s\" of type \"%s\"]\n"
                    , tree->name, typeName(tree->variableDataType));
                break;
            case ArrVarK:
                fprintf(listing, "[Array declaration \"%s\" of size %d"
                    " and type \"%s\"]\n",
                    tree->name, tree->val, typeName(tree->variableDataType));
                break;
            case FunK:
                fprintf(listing, "[Function declaration \"%s()\""
                    " of return type \"%s\"]\n",
                    tree->name, typeName(tree->functionReturnType));
                break;
            default:
                fprintf(listing, "<<<unknown declaration type>>>\n");
                break;
            }
        }
        else if (tree->nodekind == ExpK)
        {
            switch (tree->kind.exp)
            {
            case OpK:
                fprintf(listing, "[Op \"");
                printToken(tree->op, "");
                fprintf(listing, "\"]\n");
                break;
            case IdK:

                fprintf(listing, "[Var \"%s", tree->name);
                if (tree->val != 0) /* array indexing */
                    fprintf(listing, "[%d]", tree->val);
                fprintf(listing, "\"]\n");
                break;
            case ConstK:
                fprintf(listing, "[NUM \"%d\"]\n", tree->val);
                break;
            case AssignK:
                fprintf(listing, "[Expression]\n");
                break;
            default:
                fprintf(listing, "<<<unknown exp type>>>\n");
                break;
            }
        }
        else if (tree->nodekind == StmtK)
        {
            switch (tree->kind.stmt)
            {
            case CompoundK:
                fprintf(listing, "[Compound statement]\n");
                break;
            case SelectionK:
                fprintf(listing, "[IF statement]\n");
                break;
            case IterationK:
                fprintf(listing, "[WHILE statement]\n");
                break;
            case ReturnK:
                fprintf(listing, "[RETURN statement]\n");
                break;
            case CallK:
                fprintf(listing, "[Call to function \"%s()\"]\n",
                    tree->name);
                break;
          
            default:
                fprintf(listing, "<<<unknown statement type>>>\n");
                break;
            }
        }
        else
            fprintf(listing, "<<<unknown node kind>>>\n");

        for (i = 0; i < MAXCHILDREN; ++i)
            printTree(tree->child[i]);

        tree = tree->sibling;
    }

    UNINDENT;
}


/* Function newStmtNode creates a new statement
 * node for syntax tree construction
 */
TreeNode* newStmtNode(StmtKind kind) {
    TreeNode* t;

    t = allocNewNode();
    if (t)
    {
        t->nodekind = StmtK;
        t->kind.stmt = kind;
    }

    return t;
}

/* Function newExpNode creates a new expression
 * node for syntax tree construction
 */
TreeNode* newExpNode(ExpKind kind) {
    TreeNode* t;

    t = allocNewNode();
    if (t)
    {
        t->nodekind = ExpK;
        t->kind.exp = kind;
    }

    return t;
}

/* Function newDeclNode creates a new declaration
 * node for syntax tree construction
 */
TreeNode* newDecNode(DecKind kind) {
    TreeNode* t;

    t = allocNewNode();
    if (t)
    {
        t->nodekind = DecK;
        t->kind.dec = kind;
    }

    return t;
}
static TreeNode* allocNewNode(void)
{
    TreeNode* t;
    int      i;


    t = (TreeNode*)malloc(sizeof(TreeNode));
    if (!t)
    {
        fprintf(listing, "*** Out of memory at line %d.\n", lineno);
    }
    else
    {
        for (i = 0; i < MAXCHILDREN; ++i) t->child[i] = NULL;
        t->sibling = NULL;
        t->lineno = lineno;
        t->declaration = NULL;    /* if an identifier, ptr to dec. node */
        t->isParameter = FALSE;   /* is declaration a formal parameter? */
        t->isGlobal = FALSE;      /* is the variable a global? */
        t->assemblyAreaSize = 0;  /* attributes used for emitting dcode. */
        t->localSize = 0;
        t->offset = 0;            /* if local/parm definition, offset */
    }

    return t;
}

char* copyString(char* source)
{
    int  sLength;
    char* newString;


    if (!source)
        return NULL;

    sLength = strlen(source) + 1;
    newString = (char*)malloc(sLength);

    if (!newString)
        fprintf(listing, "*** Out of memory on line %d.\n", lineno);
    else
        strcpy(newString, source);

    return newString;
}


