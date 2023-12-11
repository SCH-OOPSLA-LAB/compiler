

#include "globals.h"
#include "util.h"
#include "scan.h"
#include "parse.h"

static TokenType token; 
static TreeNode* declaration_list(void);
static TreeNode* declaration(void);
static TreeNode* var_declaration(void);
static TreeNode* param_list(void);
static TreeNode* param(void);
static TreeNode* local_declarations(void);

static TreeNode* statement_list(void);
static TreeNode* statement(void);
static TreeNode* selection_stmt(void);
static TreeNode* iteration_stmt(void);
static TreeNode* expression_stmt(void);
static TreeNode* compound_stmt(void);
static TreeNode* return_stmt(void);

static TreeNode* exp(void);
static TreeNode* simple_exp(TreeNode* passdown);
static TreeNode* additive_expression(TreeNode* passdown);

static TreeNode* term(TreeNode* passdown);
static TreeNode* factor(TreeNode* passdown);
static TreeNode* args(void);
static TreeNode* arg_list(void);
static TreeNode* ident_statement(void);

static void syntaxError(char* message)
{
    fprintf(listing, "\n>>> ");
    fprintf(listing, "Syntax error at line %d: %s", lineno, message);
    Error = TRUE;
}

static void match(TokenType expected)
{
    if (token == expected)
        token = getToken();
    else
    {
        syntaxError("unexpected token ");
        printToken(token, tokenString);
        fprintf(listing, "\n");
    }
}
static ExpType matchType()
{
    ExpType t_type = Void;


    switch (token)
    {
    case INT:  t_type = Integer; token = getToken(); break;
    case VOID: t_type = Void; token = getToken(); break;
    default: 
        syntaxError("expected a type identifier but got a ");
        printToken(token, tokenString);
        fprintf(listing, "\n");
        break;
        
    }

    return t_type;
}

static int isAType(TokenType tok)
{
    if ((tok == INT) || (tok == VOID))
        return TRUE;
    else
        return FALSE;
}

static TreeNode* declaration_list(void)
{
    TreeNode* tree;
    TreeNode* ptr;


    

    tree = declaration();
    ptr = tree;

    while (token != ENDFILE)
    {
        TreeNode* q;  /* temp node */

        q = declaration();
        if ((q != NULL) && (ptr != NULL))
        {
            ptr->sibling = q;
            ptr = q;
        }
    }

    

        return tree;
}


static TreeNode* declaration(void)
{
    TreeNode* tree = NULL;
    ExpType   decType;
    char* identifier;


    

    decType = matchType();   /* get type of declaration */

    identifier = copyString(tokenString);
    match(ID);
    
    switch (token)
    {
    case SEMI:     /* variable declaration */

        tree = newDecNode(VarK);

        if (tree != NULL)
        {
            tree->variableDataType = decType;
            tree->name = identifier;
        }

        match(SEMI);
        break;

    case LBRACKET:  /* array declaration */
        tree = newDecNode(ArrVarK);
        
        if (tree != NULL)
        {
            tree->variableDataType = decType;
            tree->name = identifier;
        }

        match(LBRACKET);
        
        if (tree != NULL) tree->val = atoi(tokenString);

        match(NUM);
        match(RBRACKET);
        match(SEMI);
        break;

    case LPAREN:   /* function declaration */
        tree = newDecNode(FunK);

        if (tree != NULL)
        {
            tree->functionReturnType = decType;
            tree->name = identifier;
        }

        match(LPAREN);
        if (tree != NULL) tree->child[0] = param_list();
        match(RPAREN);
        if (tree != NULL) tree->child[1] = compound_stmt();
        break;

    default:
        syntaxError("unexpected token ");
        printToken(token, tokenString);
        fprintf(listing, "\n");
        token = getToken();
        break;
    }

   
        return tree;
}


static TreeNode* var_declaration(void)
{
    TreeNode* tree = NULL;
    ExpType   decType;
    char* identifier;

        decType = matchType();

    identifier = copyString(tokenString);
    match(ID);

    switch (token)
    {
    case SEMI:     /* variable declaration */

        tree = newDecNode(VarK);

        if (tree != NULL)
        {
            tree->variableDataType = decType;
            tree->name = identifier;
        }

        match(SEMI);
        break;

    case LBRACKET:  /* array declaration */
        tree = newDecNode(ArrVarK);
        
        if (tree != NULL)
        {
            tree->variableDataType = decType;
            tree->name = identifier;
        }

        match(LBRACKET);

        if (tree != NULL) tree->val = atoi(tokenString);

        match(NUM);
        match(RBRACKET);
        match(SEMI);
        break;

    default:
        syntaxError("unexpected token ");
        printToken(token, tokenString);
        fprintf(listing, "\n");
        token = getToken();
        break;
    }

    return tree;
}


static TreeNode* param(void)
{
    TreeNode* tree;
    ExpType  parmType;
    char* identifier;


    

        parmType = matchType();  /* get type of formal parameter */

    identifier = copyString(tokenString);
    match(ID);

    /* array-type formal parameter */
    if (token == LBRACKET)
    {
        match(LBRACKET);
        
        match(RBRACKET);
        tree = newDecNode(ArrVarK);
        
    }
    else
        tree = newDecNode(VarK);

    if (tree != NULL)
    {
        tree->name = identifier;
        tree->val = 0;
        tree->variableDataType = parmType;
        tree->isParameter = TRUE;
    }

    return tree;
}


static TreeNode* param_list(void)
{
    TreeNode* tree;
    TreeNode* ptr;
    TreeNode* newNode;


 

        if (token == VOID)
        {
            match(VOID);
            return NULL;
        }

    tree = param();
    ptr = tree;

    while ((tree != NULL) && (token == COMMA))
    {
        match(COMMA);
        newNode = param();
        if (newNode != NULL)
        {
            ptr->sibling = newNode;
            ptr = newNode;
        }
    }

    return tree;
}


static TreeNode* compound_stmt(void)
{
    TreeNode* tree = NULL;


        match(LBLOCK);

    if ((token != RBLOCK) && (tree = newStmtNode(CompoundK)))
    {
        if (isAType(token))
            tree->child[0] = local_declarations();
        if (token != RBLOCK)
            tree->child[1] = statement_list();
    }

    match(RBLOCK);



        return tree;
}


static TreeNode* local_declarations(void)
{
    TreeNode* tree = NULL;
    TreeNode* ptr = NULL;
    TreeNode* newNode = NULL;

    /* find first variable declaration, if it exists */
    if (isAType(token))
        tree = var_declaration();

    /* subsequent variable declarations */
    if (tree != NULL)
    {
        ptr = tree;

        while (isAType(token))
        {
            newNode = var_declaration();
            if (newNode != NULL)
            {
                ptr->sibling = newNode;
                ptr = newNode;
            }
        }
    }

    return tree;
}



TreeNode* statement_list(void)
{
    TreeNode* tree = NULL;
    TreeNode* ptr;
    TreeNode* newNode;


        if (token != RBLOCK)
        {
            tree = statement();
            ptr = tree;

            while (token != RBLOCK)
            {
                newNode = statement();
                if ((ptr != NULL) && (newNode != NULL))
                {
                    ptr->sibling = newNode;
                    ptr = newNode;
                }
            }
        }

        return tree;
}

TreeNode* statement(void)
{
    TreeNode* tree = NULL;


        switch (token)
        {
        case IF:
           
            tree = selection_stmt();
            break;
        case WHILE:
            
            tree = iteration_stmt();
            break;
        case RETURN:
            
            tree = return_stmt();
            break;
        case LBLOCK:
            
            tree = compound_stmt();
            break;
        case ID:
        case SEMI:
        case LPAREN:
        case NUM:
            tree = expression_stmt();
            break;
        default:
            syntaxError("unexpected token %s",token);
            printToken(token, tokenString);
            fprintf(listing, "\n");
            token = getToken();
            break;
        }

   

        return tree;
}

TreeNode* selection_stmt(void)
{
    TreeNode* tree;
    TreeNode* expr;
    TreeNode* ifStmt;
    TreeNode* elseStmt = NULL;



        match(IF);
    match(LPAREN);
    expr = exp();
    match(RPAREN);
    ifStmt = statement();

    if (token == ELSE)
    {
        match(ELSE);
        elseStmt = statement();
    }

    tree = newStmtNode(SelectionK);
    if (tree != NULL)
    {
        tree->child[0] = expr;
        tree->child[1] = ifStmt;
        tree->child[2] = elseStmt;
    }

    return tree;
}

TreeNode* iteration_stmt(void)
{
    TreeNode* tree;
    TreeNode* expr;
    TreeNode* stmt;

    match(WHILE);
    match(LPAREN);
    expr = exp();
    match(RPAREN);
    stmt = statement();

    tree = newStmtNode(IterationK);
    if (tree != NULL)
    {
        tree->child[0] = expr;
        tree->child[1] = stmt;
    }

    return tree;
}

TreeNode* expression_stmt(void)
{
    TreeNode* tree = NULL;

   
        if (token == SEMI)
            match(SEMI);
        else if (token != RBLOCK)
        {
            tree = exp();
            match(SEMI);
        }

        return tree;
}
TreeNode* return_stmt(void) {
    TreeNode* tree;
    TreeNode* expr = NULL;


   
        match(RETURN);

    tree = newStmtNode(ReturnK);
    if (token != SEMI)
        expr = exp();

    if (tree != NULL)
        tree->child[0] = expr;

    match(SEMI);

    return tree;

}



TreeNode* exp(void)
{
    TreeNode* tree = NULL;
    TreeNode* lvalue = NULL;
    TreeNode* rvalue = NULL;
    int gotLvalue = FALSE;  /* boolean */

        if (token == ID)
        {
          
            lvalue = ident_statement();
            lvalue->val = 0;
           
          
            gotLvalue = TRUE;
        }
        
    /* Assignment? */
    if ((gotLvalue == TRUE) && (token == ASSIGN))
    {
        if ((lvalue != NULL) && (lvalue->nodekind == ExpK) &&
            (lvalue->kind.exp == IdK))
        {
           
            match(ASSIGN);
            rvalue = exp();

            tree = newExpNode(AssignK);
            if (tree != NULL)
            {
                tree->child[0] = lvalue;
                
                tree->child[1] = rvalue;
            }
        }
        else
        {
            syntaxError("attempt to assign to something not an lvalue\n");
            token = getToken();
        }
    }
    else
        tree = simple_exp(lvalue);

    return tree;
}

TreeNode* simple_exp(TreeNode* passdown)
{
    TreeNode* tree;
    TreeNode* lExpr = NULL;
    TreeNode* rExpr = NULL;
    TokenType operator;



        lExpr = additive_expression(passdown);

    if ((token == LE) || (token == GE) || (token == GT)
        || (token == LT) || (token == EQ) || (token == NE))
    {
        operator = token;
        match(token);
        rExpr = additive_expression(NULL);

        tree = newExpNode(OpK);
        if (tree != NULL)
        {
            tree->child[0] = lExpr;
            tree->child[1] = rExpr;
            tree->op = operator;
        }
    }
    else
        tree = lExpr;

    return tree;
}
static TreeNode* additive_expression(TreeNode* passdown)
{
    TreeNode* tree;
    TreeNode* newNode;


    tree = term(passdown);

    while ((token == PLUS) || (token == MINUS))
    {
        newNode = newExpNode(OpK);
        if (newNode != NULL)
        {
            newNode->child[0] = tree;
            newNode->op = token;
            tree = newNode;
            match(token);
            tree->child[1] = term(NULL);
        }
    }

    return tree;
}
TreeNode* term(TreeNode* passdown)
{
    TreeNode* tree;
    TreeNode* newNode;

    

        tree = factor(passdown);

    while ((token == TIMES) || (token == OVER))
    {
        newNode = newExpNode(OpK);

        if (newNode != NULL)
        {
            newNode->child[0] = tree;
            newNode->op = token;
            tree = newNode;
            match(token);
            newNode->child[1] = factor(NULL);
        }
    }

    return tree;
}

TreeNode* factor(TreeNode* passdown)
{
    TreeNode* tree = NULL;
    if (passdown != NULL)
    {
       
            return passdown;
    }

    if (token == ID)
    {
        tree = ident_statement();
    }
    else if (token == LPAREN)
    {
        match(LPAREN);
        tree = exp();
        match(RPAREN);
    }
    else if (token == NUM)
    {
        tree = newExpNode(ConstK);
        if (tree != NULL)
        {
            tree->val = atoi(tokenString);
            tree->variableDataType = Integer;
        }
        match(NUM);
    }
    else
    {
        syntaxError("unexpected token ");
        printToken(token, tokenString);
        fprintf(listing, "\n");
        token = getToken();
    }

    return tree;
}

static TreeNode* ident_statement(void)
{
    TreeNode* tree;
    TreeNode* expr = NULL;
    TreeNode* arguments = NULL;
    char* identifier=NULL;
    if (token == ID)
        identifier = copyString(tokenString);
    match(ID);

    if (token == LPAREN)
    {
        match(LPAREN);
        arguments = args();
        match(RPAREN);

        tree = newStmtNode(CallK);
        if (tree != NULL)
        {
            tree->child[0] = arguments;
            tree->name = identifier;
        }
    }
    else
    {
        if (token == LBRACKET)
        {
            
            match(LBRACKET);
            expr = exp();
            match(RBRACKET);
        }

        tree = newExpNode(IdK);
        if (tree != NULL)
        {
            tree->child[0] = expr;
            tree->name = identifier;
        }
    }

    return tree;
}


static TreeNode* args(void)
{
    TreeNode* tree = NULL;



        if (token != RPAREN)
            tree = arg_list();

    return tree;
}


static TreeNode* arg_list(void)
{
    TreeNode* tree;
    TreeNode* ptr;
    TreeNode* newNode;


        tree = exp();
    ptr = tree;

    while (token == COMMA)
    {
        match(COMMA);
        newNode = exp();

        if ((ptr != NULL) && (tree != NULL))
        {
            ptr->sibling = newNode;
            ptr = newNode;
        }
    }

    return tree;
}


TreeNode* Parse(void)
{
    TreeNode* t;

   
        token = getToken();
    t = declaration_list();
    if (token != ENDFILE)
        syntaxError("Unexpected symbol at end of file\n");

    /* t points to the fully-constructed syntax tree */
    return t;
}