%{
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<../src/tree.h>
#include<../src/strtab.h>
#include<../src/nodetype.h>

extern int yylineno;

tree *ast;  /* pointer to AST root */

char* id;
%}

%union
{
    int value;
    struct treenode *node;
    char *strval;
}

/* Add token declarations below. The type <value> indicates that the associated token will be of a value type such as integer, float etc., and <strval> indicates that the associated token will be of string type. */

%token <strval> ID
%token <value> INTCONST
%token <strval> CHARCONST
%token <strval> STRCONST

%token <value> KWD_IF KWD_ELSE KWD_WHILE KWD_INT KWD_STRING KWD_CHAR KWD_VOID KWD_RETURN
%token <value> OPER_ADD OPER_SUB OPER_MUL OPER_DIV OPER_MOD OPER_LT OPER_GT OPER_LTE OPER_GTE OPER_EQ OPER_NEQ OPER_ASGN OPER_AT OPER_INC OPER_DEC
%token <value> LSQ_BRKT RSQ_BRKT LCRLY_BRKT RCRLY_BRKT LPAREN RPAREN 
%token <value> COMMA SEMICLN
%token <value> OPER_AND OPER_OR OPER_NOT

%token ERROR

%left OPER_ADD OPER_SUB
%left OPER_MUL OPER_DIV 
%right OPER_ASGN

/* For removing shift-reduce conflict for IF-ELSE */

%nonassoc IFX
%nonassoc KWD_ELSE

/* Declate NTs as of type node. */

%type <node> program declList decl varDecl funDecl typeSpecifier funcTypeName formalDeclList funBody formalDecl
localDeclList statementList statement compoundStmt assignStmt condStmt loopStmt returnStmt unaryStmt var expression addExpr
relop addop term mulop factor funCallExpr argList conditionExpr logicalop unaryop 

%start program


%%

program         : declList
			{
				
				tree *progNode = maketree(PROGRAM);
				addChild(progNode, $1);
                		ast = progNode;
			}
                ;

declList       	: decl
			{
				tree *declListNode = maketree(DECLLIST);
                		addChild(declListNode, $1);
                		$$ = declListNode;
			}
                | decl declList 
                	{
				tree *declListNode = maketree(DECLLIST);
                		addChild(declListNode, $1);
                		addChild(declListNode, $2);
                		$$ = declListNode;
			}
                ;

decl            : varDecl 
			{
	// ----- Symbol Table Entry for global variable ----- //
	// 'varDecl' node contains both data_type & identifier value.
	// Here indicates the scope of the variable.

				tree *varNode = $1;

				tree *typeSpecNode = varNode->children[0];
				char *data_type = typeSpecNode->strVal;

				tree *idNode = varNode->children[1];
				char *id = idNode->strVal;

				if(varNode->children[2] != NULL){
					int hash = ST_insert(id, "global", data_type, ARRAY);
				}
				else{
					int hash = ST_insert(id, "global", data_type, SCALAR);
				}

	// ----- Symbol Table Entry for global variable ----- //

				tree *declNode = maketree(DECL);
                		addChild(declNode, $1);
                		$$ = declNode;
			}
                | funDecl
                {
					tree *declNode = maketree(DECL);
                	addChild(declNode, $1);
                	$$ = declNode;
				}
                ;

varDecl        	: typeSpecifier ID LSQ_BRKT INTCONST RSQ_BRKT SEMICLN
				{
					tree *varDeclNode = maketree(VARDECL);
                	addChild(varDeclNode, $1);
                	addChild(varDeclNode, maketreeWithStrVal(IDENTIFIER, id));
                	addChild(varDeclNode, maketreeWithIntVal(INTEGER, yylval.value));
                	addChild(varDeclNode, maketree(ARRAYDECL));
                	$$ = varDeclNode;
				}
				| typeSpecifier ID SEMICLN 
				{
					tree *varDeclNode = maketree(VARDECL);
                	addChild(varDeclNode, $1);
                	addChild(varDeclNode, maketreeWithStrVal(IDENTIFIER, yylval.strval));
                	$$ = varDeclNode;
				}
                ;

typeSpecifier   : KWD_INT 
				{
					$$ = maketreeWithStrVal(TYPESPEC, INT_TYPE);
				}
				| KWD_CHAR
				{
					$$ = maketreeWithStrVal(TYPESPEC, CHAR_TYPE);
				}
                | KWD_STRING 
                {
					$$ = maketreeWithStrVal(TYPESPEC, STRING_TYPE);
				}
                | KWD_VOID
                {
					$$ = maketreeWithStrVal(TYPESPEC, VOID_TYPE);
				}
                ;

funDecl 		: funcTypeName LPAREN formalDeclList RPAREN funBody
				{
					tree *funDeclNode = maketree(FUNDECL);
                			addChild(funDeclNode, $1);
                			addChild(funDeclNode, $3);
                			addChild(funDeclNode, $5);
                			$$ = funDeclNode;
					//scope = "";
				}
				| funcTypeName LPAREN RPAREN funBody
				{
					tree *funDeclNode = maketree(FUNDECL);
                			addChild(funDeclNode, $1);
                			addChild(funDeclNode, $4);
                			$$ = funDeclNode;
					//scope = "";
				}
				;

funcTypeName 	: typeSpecifier ID
				{
					// ----- Symbol Table Entry for function ----- //
					tree *typeSpecNode = $1;
					char *data_type = typeSpecNode->strVal;
					int hash = ST_insert(yylval.strval, "global", data_type, FUNCTION);

					// ----- Symbol Table Entry for function ----- //

					tree *funcTypeNameNode = maketree(FUNCTYPENAME);
                	addChild(funcTypeNameNode, $1);
                	addChild(funcTypeNameNode, maketreeWithStrVal(IDENTIFIER, yylval.strval));
                	$$ = funcTypeNameNode;
				}
				;

formalDeclList  : formalDecl
				{
					tree *formalDeclListNode = maketree(FORMALDECLLIST);
                	addChild(formalDeclListNode, $1);
                	$$ = formalDeclListNode;
				}
				| formalDecl COMMA formalDeclList 
				{
					tree *formalDeclListNode = maketree(FORMALDECLLIST);
                	addChild(formalDeclListNode, $1);
                	addChild(formalDeclListNode, $3);
                	$$ = formalDeclListNode;
				} 
				;

formalDecl 		: typeSpecifier ID
				{
					// ----- Symbol Table Entry for function arg ----- //

					tree *typeSpecNode = $1;
					char *data_type = typeSpecNode->strVal;
					int hash = ST_insert(yylval.strval, "local", data_type, ARG);

					// ----- Symbol Table Entry for function arg ----- //

					tree *formalDeclNode = maketree(FORMALDECL);
                	addChild(formalDeclNode, $1);
                	addChild(formalDeclNode, maketreeWithStrVal(IDENTIFIER, yylval.strval));
                	$$ = formalDeclNode;
				}
				| typeSpecifier ID LSQ_BRKT RSQ_BRKT
				{
					// ----- Symbol Table Entry for function arg ----- //

					tree *typeSpecNode = $1;
					char *data_type = typeSpecNode->strVal;
					int hash = ST_insert(yylval.strval, "local", data_type, ARG);

					// ----- Symbol Table Entry for function arg ----- //

					tree *formalDeclNode = maketree(FORMALDECL);
                	addChild(formalDeclNode, $1);
                	addChild(formalDeclNode, maketreeWithStrVal(IDENTIFIER, yylval.strval));
                	addChild(formalDeclNode, maketree(ARRAYDECL));
                	$$ = formalDeclNode;
				}
				;

funBody 		: LCRLY_BRKT localDeclList statementList RCRLY_BRKT 
				{
					tree *funBodyNode = maketree(FUNBODY);
                	addChild(funBodyNode, $2);
                	addChild(funBodyNode, $3);
                	$$ = funBodyNode;
				}
				;

localDeclList  	: 
				{
                	$$ = NULL;
				}
				| varDecl localDeclList
				{
					// ----- Symbol Table Entry for local variable ----- //
					// 'varDecl' node contains both data_type & identifier value.
					// Here indicates the scope of the variable.

					tree *varNode = $1;

					tree *typeSpecNode = varNode->children[0];
					char *data_type = typeSpecNode->strVal;

					tree *idNode = varNode->children[1];
					char *id = idNode->strVal;

					if(varNode->children[2] != NULL){
						int hash = ST_insert(id, "local", data_type, ARRAY);
					}
					else{
						int hash = ST_insert(id, "local", data_type, SCALAR);
					}

					// ----- Symbol Table Entry for local variable ----- //

					tree *localDeclListNode = maketree(LOCALDECLLIST);
                	addChild(localDeclListNode, $1);
                	addChild(localDeclListNode, $2);
                	$$ = localDeclListNode;
				}
                ;

statementList  	: 
				{
                	$$ = NULL;
				}
				| statement statementList
				{
					tree *statementListNode = maketree(STATEMENTLIST);
                	addChild(statementListNode, $1);
                	addChild(statementListNode, $2);
                	$$ = statementListNode;
				}
                ;

statement       : compoundStmt
				{
					tree *statementNode = maketree(STATEMENT);
                	addChild(statementNode, $1);
                	$$ = statementNode;
				}
				| assignStmt
				{
					tree *statementNode = maketree(STATEMENT);
                	addChild(statementNode, $1);
                	$$ = statementNode;
				} 
                | condStmt
                {
					tree *statementNode = maketree(STATEMENT);
                	addChild(statementNode, $1);
                	$$ = statementNode;
				}
                | loopStmt
                {
					tree *statementNode = maketree(STATEMENT);
                	addChild(statementNode, $1);
                	$$ = statementNode;
				}
                | returnStmt
                {
					tree *statementNode = maketree(STATEMENT);
                	addChild(statementNode, $1);
                	$$ = statementNode;
				}
				| unaryStmt
				{
					tree *statementNode = maketree(STATEMENT);
                	addChild(statementNode, $1);
                	$$ = statementNode;
				}
                ;

compoundStmt 	: LCRLY_BRKT statementList RCRLY_BRKT
				{
					tree *compoundStmtNode = maketree(COMPOUNDSTMT);
                	addChild(compoundStmtNode, $2);
                	$$ = compoundStmtNode;
				}
				;

assignStmt      : var OPER_ASGN expression SEMICLN 
				{
					tree *assignStmtNode = maketree(ASSIGNSTMT);
                	addChild(assignStmtNode, $1);
                	addChild(assignStmtNode, maketree(ASSIGNOP));
                	addChild(assignStmtNode, $3);
                	$$ = assignStmtNode;
				}
				| expression SEMICLN
				{
					tree *assignStmtNode = maketree(ASSIGNSTMT);
                	addChild(assignStmtNode, $1);
                	$$ = assignStmtNode;
				}
				;

condStmt 		: KWD_IF LPAREN conditionExpr RPAREN statement %prec IFX
				{
					tree *condStmtNode = maketree(CONDSTMT);
					addChild(condStmtNode, maketree(IF));
					addChild(condStmtNode, $3);
                	addChild(condStmtNode, $5);	
                	$$ = condStmtNode;
				}
				| KWD_IF LPAREN conditionExpr RPAREN statement KWD_ELSE statement
				{
					tree *condStmtNode = maketree(CONDSTMT);
					addChild(condStmtNode, maketree(IF));
					addChild(condStmtNode, $3);
                	addChild(condStmtNode, $5);	
                	addChild(condStmtNode, maketree(ELSE));
                	addChild(condStmtNode, $7);
                	$$ = condStmtNode;
				}
				;

loopStmt 		: KWD_WHILE LPAREN expression RPAREN statement
				{
					tree *loopStmtNode = maketree(LOOPSTMT);
                	addChild(loopStmtNode, maketree(WHILE));
                	addChild(loopStmtNode, $3);
                	addChild(loopStmtNode, $5);
                	$$ = loopStmtNode;
				}
				;

returnStmt 		: KWD_RETURN SEMICLN
				{
					tree *returnStmtNode = maketree(RETURNSTMT);
                	addChild(returnStmtNode, maketree(RETURN));
                	$$ = returnStmtNode;
				}
				| KWD_RETURN expression SEMICLN
				{
					tree *returnStmtNode = maketree(RETURNSTMT);
                	addChild(returnStmtNode, maketree(RETURN));
                	addChild(returnStmtNode, $2);
                	$$ = returnStmtNode;
				}
				;

unaryStmt 		: unaryop var SEMICLN
				{
					tree *unaryStmtNode = maketree(UNARYSTMT);
                	addChild(unaryStmtNode, $1);
                	addChild(unaryStmtNode, $2);
                	$$ = unaryStmtNode;
				}
				| var unaryop SEMICLN
				{
					tree *unaryStmtNode = maketree(UNARYSTMT);
                	addChild(unaryStmtNode, $1);
                	addChild(unaryStmtNode, $2);
                	$$ = unaryStmtNode;
				}
				;

unaryop 		: OPER_INC
				{
					$$ = maketreeWithStrVal(UNARYOP, "++");
				}
				| OPER_DEC
				{
					$$ = maketreeWithStrVal(UNARYOP, "--");
				}
				;

var             : ID
				{
					// ----- Symbol table entry look up ----- //
					// Checking whether the var is declared in global or local scope
					// If not declared, then warning message is given

					int isGlobal = ST_lookup(yylval.strval, "global");
					int isLocal = ST_lookup(yylval.strval, "local");

					if(isGlobal == UNDECLARED_VAR && isLocal == UNDECLARED_VAR){
						yywarning("Use of undeclared symbol", yylval.strval);
					}

					// ----- Symbol table entry look up ----- //

					tree *varNode = maketree(VAR);
                	addChild(varNode, maketreeWithStrVal(IDENTIFIER, yylval.strval));
                	$$ = varNode;
				}
				| ID LSQ_BRKT addExpr RSQ_BRKT
				{
					// ----- Symbol table entry look up ----- //

					int isGlobal = ST_lookup(yylval.strval, "global");
					int isLocal = ST_lookup(yylval.strval, "local");

					if(isGlobal == UNDECLARED_VAR && isLocal == UNDECLARED_VAR){
						yywarning("Use of undeclared symbol", yylval.strval);
					}

					// ----- Symbol table entry look up ----- //

					tree *varNode = maketree(VAR);
                	addChild(varNode, maketreeWithStrVal(IDENTIFIER, yylval.strval));
                	addChild(varNode, $3);
                	$$ = varNode;
				}
				;			

expression      : addExpr
				{
					tree *expressionNode = maketree(EXPRESSION);
                	addChild(expressionNode, $1);
                	$$ = expressionNode;
				}
				| expression relop addExpr 
				{
					tree *expressionNode = maketree(EXPRESSION);
                	addChild(expressionNode, $1);
                	addChild(expressionNode, $2);
                	addChild(expressionNode, $3);
                	$$ = expressionNode;
				}
                ;          

relop 	        : OPER_LT 
				{
					$$ = maketreeWithStrVal(RELOP, "<");
				}
				| OPER_GT 
				{
					$$ = maketreeWithStrVal(RELOP, ">");
				}
				| OPER_LTE 
				{
					$$ = maketreeWithStrVal(RELOP, "<=");
				}
				| OPER_GTE
				{
					$$ = maketreeWithStrVal(RELOP, ">=");
				}
				| OPER_EQ 
				{
					$$ = maketreeWithStrVal(RELOP, "==");
				}
				| OPER_NEQ
				{
					$$ = maketreeWithStrVal(RELOP, "!=");
				}
				;

addExpr 		: term
				{
					tree *addExprNode = maketree(ADDEXPR);
                	addChild(addExprNode, $1);
                	$$ = addExprNode;
				}
				| addExpr addop term
				{
					tree *addExprNode = maketree(ADDEXPR);
                	addChild(addExprNode, $1);
                	addChild(addExprNode, $2);
                	addChild(addExprNode, $3);
                	$$ = addExprNode;
				}
				;

addop 			: OPER_ADD
				{
					$$ = maketreeWithStrVal(ADDOP, "+");
				} 
				| OPER_SUB
				{
					$$ = maketreeWithStrVal(ADDOP, "-");
				} 
				;

term 			: factor
				{
					tree *termNode = maketree(TERM);
                	addChild(termNode, $1);
                	$$ = termNode;
				}
				| term mulop factor
				{
					tree *termNode = maketree(TERM);
                	addChild(termNode, $1);
                	addChild(termNode, $2);
                	addChild(termNode, $3);
                	$$ = termNode;
				}
				;

mulop 			: OPER_MUL 
				{
					$$ = maketreeWithStrVal(MULOP, "*");
				} 
				| OPER_DIV 
				{
					$$ = maketreeWithStrVal(MULOP, "/");
				} 
				| OPER_MOD
				{
					$$ = maketreeWithStrVal(MULOP, "%");
				} 
				;

factor 			: LPAREN expression RPAREN
				{
					tree *factorNode = maketree(FACTOR);
                	addChild(factorNode, $2);
                	$$ = factorNode;
				} 
				| var
				{
					tree *factorNode = maketree(FACTOR);
                	addChild(factorNode, $1);
                	$$ = factorNode;
				}
				| funCallExpr
				{
					tree *factorNode = maketree(FACTOR);
                	addChild(factorNode, $1);
                	$$ = factorNode;
				}
				| INTCONST
				{
					tree *factorNode = maketree(FACTOR);
                	addChild(factorNode, maketreeWithIntVal(INTEGER, yylval.value));
                	$$ = factorNode;
				}
				| CHARCONST
				{
					tree *factorNode = maketree(FACTOR);
                	addChild(factorNode, maketreeWithIntVal(CHAR, yylval.value));
                	$$ = factorNode;
				}
				| STRCONST
				{
					tree *factorNode = maketree(FACTOR);
                	addChild(factorNode, maketreeWithStrVal(STRING, yylval.strval));
                	$$ = factorNode;
				}
				;

funCallExpr 	: ID LPAREN argList RPAREN
				{	
					tree *funCallExprNode = maketree(FUNCCALLEXPR);
                	addChild(funCallExprNode, maketreeWithStrVal(IDENTIFIER, id));
                	addChild(funCallExprNode, $3);
                	$$ = funCallExprNode;
					
				}
				| ID LPAREN RPAREN
				{
					tree *funCallExprNode = maketree(FUNCCALLEXPR);
                	addChild(funCallExprNode, maketreeWithStrVal(IDENTIFIER, yylval.strval));
                	$$ = funCallExprNode;
				}
				;

argList 		: expression
				{
					tree *argListNode = maketree(ARGLIST);
                	addChild(argListNode, $1);
                	$$ = argListNode;
				}
				| argList COMMA expression
				{
					tree *argListNode = maketree(ARGLIST);
                	addChild(argListNode, $1);
                	addChild(argListNode, $3);
                	$$ = argListNode;
				}
				;

conditionExpr 	: expression
				{
					tree *expressionNode = maketree(EXPRESSION);
                	addChild(expressionNode, $1);
                	$$ = expressionNode;
				}
				| expression logicalop expression
				{
					tree *expressionNode = maketree(EXPRESSION);
                	addChild(expressionNode, $1);
                	addChild(expressionNode, $2);
                	addChild(expressionNode, $3);
                	$$ = expressionNode;
				}
				| OPER_NOT expression
				{
					tree *expressionNode = maketree(EXPRESSION);
                	addChild(expressionNode, maketreeWithStrVal(LOGICALOP, "!"));
                	addChild(expressionNode, $2);
                	$$ = expressionNode;
				}
				;

logicalop 		: OPER_AND
				{
					$$ = maketreeWithStrVal(LOGICALOP, "&&");
				}
				| OPER_OR
				{
					$$ = maketreeWithStrVal(LOGICALOP, "||");
				}
				;

%%

int yywarning(char * msg, char *id){
    printf("warning: line %d: %s %s\n", yylineno, msg, id);
    return 0;
}

int yyerror(char * msg){
    printf("error: line %d: %s\n", yylineno, msg);
    return 0;
}
