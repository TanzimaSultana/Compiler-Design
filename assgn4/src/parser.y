%{
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<../src/tree.h>
#include<../src/strtab.h>

extern int yylineno;

/* Update p_ast & p_symtab value for yyerror. If error occurs, ast tree & symbol table is not printed */
extern int p_ast;
extern int p_symtab;

enum nodeTypes {PROGRAM, DECLLIST, DECL, VARDECL, TYPESPEC, FUNDECL,
                FORMALDECLLIST, FORMALDECL, FUNBODY, LOCALDECLLIST,
                STATEMENTLIST, STATEMENT, COMPOUNDSTMT, ASSIGNSTMT,
                CONDSTMT, LOOPSTMT, RETURNSTMT, UNARYSTMT, EXPRESSION, RELOP,
                ADDEXPR, ADDOP, TERM, MULOP, FACTOR, FUNCCALLEXPR,
                ARGLIST, INTEGER, IDENTIFIER, VAR, ARRAYDECL, CHAR, STRING, 
                FUNCTYPENAME, LOGICALOP, UNARYOP, IF, ELSE, WHILE, RETURN, ASSIGNOP};

enum opType {ADD, SUB, MUL, DIV, MOD, LT, LTE, EQ, GTE, GT, NEQ, AND, OR, NOT, INC, DEC, ASSIGN};

tree *ast;  /* pointer to AST root */

char* id;
char *funName;

%}

%union
{
    int value;
    struct treenode *node;
    char *strval;
}

/* Add token declarations below. The type <value> indicates that the associated token will be of a value type such as integer, float etc., and <strval> indicates that the associated token will be of string type. */

%token <strval>ID
%token <value>INTCONST
%token <strval>CHARCONST
%token <strval>STRCONST

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
					tree *idNode = varNode->children[1];
					idNode->scope = "global";

					// Last value of ST_insert is for FUNCTION parameter

					int hash = ST_insert(idNode->str_val, idNode->scope, idNode->data_type, idNode->sym_type, NULL);
					idNode->val = hash;

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
					// ----- IDENTIFIER ----- //

					tree *typeSpecNode = $1;
					int data_type = typeSpecNode->val;

					tree *idNode = maketreeWithStrVal(IDENTIFIER, id);
					idNode->scope = "";
					idNode->data_type = data_type;
					idNode->sym_type = ARRAY;

					// ----- IDENTIFIER ----- //

					// ----- Check error for Array ----- //

					int array_size = yylval.value;
					if(array_size == 0){
						yyerror("Array variable declared with size of zero.");
					}
					else if(array_size == -1){
						yyerror("Statically sized array indexed with constant, out-of-bounds expression.");
					}

					// ----- Check error for Array ----- //

					tree *varDeclNode = maketree(VARDECL);
                	addChild(varDeclNode, $1);
                	addChild(varDeclNode, idNode);
                	addChild(varDeclNode, maketreeWithIntVal(INTEGER, yylval.value));
                	addChild(varDeclNode, maketree(ARRAYDECL));
                	$$ = varDeclNode;
				}
				| typeSpecifier ID SEMICLN
				{
					// ----- IDENTIFIER ----- //

					tree *typeSpecNode = $1;
					int data_type = typeSpecNode->val;

					tree *idNode = maketreeWithStrVal(IDENTIFIER, id);
					idNode->scope = "";
					idNode->data_type = data_type;
					idNode->sym_type = SCALAR;

					// ----- IDENTIFIER ----- //

					tree *varDeclNode = maketree(VARDECL);
                	addChild(varDeclNode, $1);
                	addChild(varDeclNode, idNode);
                	$$ = varDeclNode;
				}
                ;

typeSpecifier   : KWD_INT 
				{
					$$ = maketreeWithIntVal(TYPESPEC, INT_TYPE);
				}
				| KWD_CHAR
				{
					$$ = maketreeWithIntVal(TYPESPEC, CHAR_TYPE);
				}
                | KWD_STRING 
                {
					$$ = maketreeWithIntVal(TYPESPEC, STRING_TYPE);
				}
                | KWD_VOID
                {
					$$ = maketreeWithIntVal(TYPESPEC, VOID_TYPE);
				}
                ;

funDecl 		: funcTypeName LPAREN formalDeclList RPAREN funBody
				{
					// ----- Symbol Table Entry for function ----- //

					tree *funTypeNameNode = $1;
					tree *idNode = funTypeNameNode->children[1];

					tree *funParamNode = $3;

					// Last value of ST_insert is for FUNCTION parameter

					int hash = ST_insert(idNode->str_val, idNode->scope, idNode->data_type, idNode->sym_type, funParamNode);
					idNode->val = hash;

					// ----- Symbol Table Entry for function ----- //


					tree *funDeclNode = maketree(FUNDECL);
                	addChild(funDeclNode, $1);
                	addChild(funDeclNode, $3);
                	addChild(funDeclNode, $5);
                	$$ = funDeclNode;
				}
				| funcTypeName LPAREN RPAREN funBody
				{
					// ----- Symbol Table Entry for function ----- //

					tree *funTypeNameNode = $1;
					tree *idNode = funTypeNameNode->children[1];

					// Last value of ST_insert is for FUNCTION parameter

					int hash = ST_insert(idNode->str_val, idNode->scope, idNode->data_type, idNode->sym_type, NULL);
					idNode->val = hash;

					// ----- Symbol Table Entry for function ----- //

					tree *funDeclNode = maketree(FUNDECL);
                	addChild(funDeclNode, $1);
                	addChild(funDeclNode, $4);
                	$$ = funDeclNode;
				}
				;

funcTypeName 	: typeSpecifier ID
				{
					tree *typeSpecNode = $1;
					int data_type = typeSpecNode->val;

					tree *idNode = maketreeWithStrVal(IDENTIFIER, id);
					idNode->scope = "global";
					idNode->data_type = data_type;
					idNode->sym_type = FUNCTION;

					tree *funcTypeNameNode = maketree(FUNCTYPENAME);
                	addChild(funcTypeNameNode, $1);
                	addChild(funcTypeNameNode, idNode);
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
					int data_type = typeSpecNode->val;

					tree *idNode = maketreeWithStrVal(IDENTIFIER, id);
					idNode->scope = "local";
					idNode->data_type = data_type;
					idNode->sym_type = ARG_SCALAR;

					// Last value of ST_insert is for FUNCTION parameter

					int hash = ST_insert(idNode->str_val, idNode->scope, idNode->data_type, idNode->sym_type, NULL);
					idNode->val = hash;

					// ----- Symbol Table Entry for function arg ----- //

					tree *formalDeclNode = maketree(FORMALDECL);
                	addChild(formalDeclNode, $1);
                	addChild(formalDeclNode, idNode);
                	$$ = formalDeclNode;
				}
				| typeSpecifier ID LSQ_BRKT RSQ_BRKT
				{
					// ----- Symbol Table Entry for function arg ----- //

					tree *typeSpecNode = $1;
					int data_type = typeSpecNode->val;

					tree *idNode = maketreeWithStrVal(IDENTIFIER, id);
					idNode->scope = "local";
					idNode->data_type = data_type;
					idNode->sym_type = ARG_ARRAY;

					// Last value of ST_insert is for FUNCTION parameter

					int hash = ST_insert(idNode->str_val, idNode->scope, idNode->data_type, idNode->sym_type, NULL);
					idNode->val = hash;

					// ----- Symbol Table Entry for function arg ----- //

					tree *formalDeclNode = maketree(FORMALDECL);
                	addChild(formalDeclNode, $1);
                	addChild(formalDeclNode, idNode);
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
					tree *idNode = varNode->children[1];
					idNode->scope = "local";

					// Last value of ST_insert is for FUNCTION parameter

					int hash = ST_insert(idNode->str_val, idNode->scope, idNode->data_type, idNode->sym_type, NULL);
					idNode->val = hash;

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
					// ----- TYPE MIS-MATCH ----- //

					tree *leftNode = $1;
					tree *rightNode = $3;

					int isMatch = isTypeMatch(leftNode, rightNode);
					if(isMatch == -1){
						yyerror("Type mismatch in assignment.");
					}


					// ----- TYPE MIS-MATCH ----- //

					tree *assignStmtNode = maketree(ASSIGNSTMT);
                	addChild(assignStmtNode, $1);
                	addChild(assignStmtNode, maketreeWithIntVal(ASSIGNOP, ASSIGN));
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
					$$ = maketreeWithIntVal(UNARYOP, INC);
				}
				| OPER_DEC
				{
					$$ = maketreeWithIntVal(UNARYOP, DEC);
				}
				;

var             : ID
				{
					// ----- Symbol table entry look up ----- //
					// Checking whether the var is declared in global or local scope
					// If not declared, then warning message is given

					int isGlobal = ST_lookup(yylval.strval, "global");
					int isLocal = ST_lookup(yylval.strval, "local");
					int hash;

					struct strEntry *idEntry = NULL;
					tree *idNode = NULL;
					if(isGlobal != UNDECLARED_VAR){
						hash = isGlobal;
						idEntry = strTable[isGlobal];
					}
					else if(isLocal != UNDECLARED_VAR){
						hash = isLocal;
						idEntry = strTable[isLocal];
					}

					if(idEntry != NULL){
						idNode = maketreeWithStrVal(IDENTIFIER, idEntry->id);
						idNode->val = hash;
						idNode->scope = idEntry->scope;
						idNode->data_type = idEntry->type->data_type;
						idNode->sym_type = idEntry->type->symbol_type;
					}
					else{
						yyerror("Use of undeclared varibale");
					}

					// ----- Symbol table entry look up ----- //

					tree *varNode = maketree(VAR);
                	addChild(varNode, idNode);
                	$$ = varNode;
				}
				| ID LSQ_BRKT addExpr RSQ_BRKT
				{
					// ----- Symbol table entry look up ----- //

					int isGlobal = ST_lookup(id, "global");
					int isLocal = ST_lookup(id, "local");
					int hash;

					struct strEntry *idEntry = NULL;
					tree *idNode = NULL;
					if(isGlobal != UNDECLARED_VAR){
						hash = isGlobal;
						idEntry = strTable[isGlobal];
					}
					else if(isLocal != UNDECLARED_VAR){
						hash = isLocal;
						idEntry = strTable[isLocal];
					}

					if(idEntry != NULL){
						idNode = maketreeWithStrVal(IDENTIFIER, idEntry->id);
						idNode->val = hash;
						idNode->scope = idEntry->scope;
						idNode->data_type = idEntry->type->data_type;
						idNode->sym_type = idEntry->type->symbol_type;

						// ----- Error checking for Array ----- //

						int arrayIndex = isValidArrayIndex($3);

						if(arrayIndex == ARRAY_INDEX_ZERO){
							yyerror("Array variable declared with size of zero.");
						}
						else if(arrayIndex == ARRAY_INDEX_OUT_OF_BOUND){
							yyerror("Statically sized array indexed with constant, out-of-bounds expression.");
						}
						else if(arrayIndex == ARRAY_INDEX_NON_INT){
							yyerror("Array indexed using non-integer expression.");
						}
						else if(idNode->sym_type != ARRAY && idNode->sym_type != ARG_ARRAY){
							yyerror("Non-array identifier used as an array.");
						}

						// ----- Error checking for Array ----- //
					}
					else{
						yyerror("Use of undeclared varibale");
					}

					// ----- Symbol table entry look up ----- //

					tree *varNode = maketree(VAR);
                	addChild(varNode, maketreeWithStrVal(IDENTIFIER, id));
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
					$$ = maketreeWithIntVal(RELOP, LT);
				}
				| OPER_GT 
				{
					$$ = maketreeWithIntVal(RELOP, GT);
				}
				| OPER_LTE 
				{
					$$ = maketreeWithIntVal(RELOP, LTE);
				}
				| OPER_GTE
				{
					$$ = maketreeWithIntVal(RELOP, GTE);
				}
				| OPER_EQ 
				{
					$$ = maketreeWithIntVal(RELOP, EQ);
				}
				| OPER_NEQ
				{
					$$ = maketreeWithIntVal(RELOP, NEQ);
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
					$$ = maketreeWithIntVal(ADDOP, ADD);
				} 
				| OPER_SUB
				{
					$$ = maketreeWithIntVal(ADDOP, SUB);
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
					$$ = maketreeWithIntVal(MULOP, MUL);
				} 
				| OPER_DIV 
				{
					$$ = maketreeWithIntVal(MULOP, DIV);
				} 
				| OPER_MOD
				{
					$$ = maketreeWithIntVal(MULOP, MOD);
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
					// ----- Symbol table entry look up ----- //

					id = funName;

					int isGlobal = ST_lookup(id, "global");
					int hash;

					struct strEntry *idEntry = NULL;
					tree *idNode = NULL;
					if(isGlobal != UNDECLARED_VAR){
						hash = isGlobal;
						idEntry = strTable[isGlobal];
					}

					if(idEntry != NULL){

						// ----- Function call mis-match ----- //

						struct typeEntry *idType1 = idEntry->type;
						struct typeEntry *idType2 = (struct typeEntry*)malloc(sizeof(struct typeEntry));
						updateFunParam(idType2, $3);

						// idType1 - Function declaration parameter
						// idType2 - Function call parameter

						matchFunParam(idType1, idType2);


						// ----- Function call mis-match ----- //

						idNode = maketreeWithStrVal(IDENTIFIER, idEntry->id);
						idNode->val = hash;
						idNode->scope = idEntry->scope;
						idNode->data_type = idEntry->type->data_type;
						idNode->sym_type = idEntry->type->symbol_type;
					}
					else{
						yyerror("Use of undeclared function");
					}

					// ----- Symbol table entry look up ----- //

					tree *funCallExprNode = maketree(FUNCCALLEXPR);
                	addChild(funCallExprNode, idNode);
                	addChild(funCallExprNode, $3);
                	$$ = funCallExprNode;
					
				}
				| ID LPAREN RPAREN
				{
					// ----- Symbol table entry look up ----- //

					int isGlobal = ST_lookup(id, "global");
					int hash;

					struct strEntry *idEntry = NULL;
					tree *idNode = NULL;
					if(isGlobal != UNDECLARED_VAR){
						hash = isGlobal;
						idEntry = strTable[isGlobal];
					}

					if(idEntry != NULL){

						// ----- Function call mis-match ----- //

						struct typeEntry *idType1 = idEntry->type;
						struct typeEntry *idType2 = (struct typeEntry*)malloc(sizeof(struct typeEntry));

						// idType1 - Function declaration parameter
						// idType2 - Function call parameter

						matchFunParam(idType1, idType2);


						// ----- Function call mis-match ----- //

						idNode = maketreeWithStrVal(IDENTIFIER, idEntry->id);
						idNode->val = hash;
						idNode->scope = idEntry->scope;
						idNode->data_type = idEntry->type->data_type;
						idNode->sym_type = idEntry->type->symbol_type;
					}
					else{
						yyerror("Use of undeclared function");
					}

					// ----- Symbol table entry look up ----- //

					tree *funCallExprNode = maketree(FUNCCALLEXPR);
                	addChild(funCallExprNode, idNode);
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
                	addChild(expressionNode, maketreeWithIntVal(LOGICALOP, NOT));
                	addChild(expressionNode, $2);
                	$$ = expressionNode;
				}
				;

logicalop 		: OPER_AND
				{
					$$ = maketreeWithIntVal(LOGICALOP, AND);
				}
				| OPER_OR
				{
					$$ = maketreeWithIntVal(LOGICALOP, OR);
				}
				;

%%

int yywarning(char * msg, char *id){
    printf("warning: line %d: %s %s\n", yylineno, msg, id);
    return 0;
}

int yyerror(char * msg){
    printf("error: line %d: %s\n", yylineno, msg);
    p_ast = 0;
    p_symtab = 0;
    return 0;
}
