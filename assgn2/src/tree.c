#include<tree.h>
#include<strtab.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<nodetype.h>

void printNodeName(tree *node);

// ----- STACK ----- //

typedef struct stacknode stack;

struct stacknode {
      tree *tree_node;
      struct stacknode *next;
};

stack *head;

void pushStack(tree *tree_node){
	stack *node = (stack*)malloc(sizeof(stack));
	node->tree_node = tree_node;
	node->next = NULL;

	if(head != NULL){
		node->next = head;
	}
	head = node;
}

tree *popStack(){
	stack *node = head;
	if(head != NULL){
		head = head->next;
		tree *tree_node = node->tree_node;
		free(node);
		return tree_node;
	}
	else{
		return NULL;
	}
}

// ----- TREE ----- //

tree *createTreeNode(int kind){
	tree *node = (tree*)malloc(sizeof(tree));
	node->nodeKind = kind;
	node->numChildren = 0;
	node->val = 0;
	node->strVal = " ";
	node->parent = NULL;
	node->children[0] = NULL;
	node->indent = 0;

	return node;
}

tree *maketree(int kind){
	tree *node = createTreeNode(kind);
	return node;
}

tree *maketreeWithIntVal(int kind, int val){
	tree *node = maketree(kind);
	node->val = val;
	return node;
}

tree *maketreeWithStrVal(int kind, char *val){
	tree *node = maketree(kind);
	node->strVal = val;
	return node;
}

void addChild(tree *parent, tree *child){

	if(parent != NULL && child != NULL){
		parent->children[parent->numChildren] = child;
		parent->numChildren = parent->numChildren + 1;
	}
}

void printAst(tree *root, int nestLevel){

	// AST tree is traversed in in-order way

	printf("\nAST TREE:\n\n");

    tree *curr = root; 

    head = NULL;

    while (curr != NULL || head != NULL) { 

    	printNodeName(curr);

    	for(int i = curr->numChildren - 1; i >= 0; i--){
    		curr->children[i]->indent = curr->indent + 1;
    		pushStack(curr->children[i]);
    	}
        curr = popStack(); 

        if(curr == NULL)
        	break;
    }
}

// Print the tree node names.

void printNodeName(tree *node){

	for(int i = 0;i < node->indent; i++)
		printf(" ");

	int kind = node->nodeKind;
	switch(kind){
		case PROGRAM:
		printf("program");
		break;

		case DECLLIST:
		printf("declList");
		break;

		case DECL:
		printf("decl");
		break;

		case VARDECL:
		printf("varDecl");
		break;

		case FUNDECL:
		printf("funDecl");
		break;

		case FUNCTYPENAME:
		printf("funcTypeName");
		break;

		case FORMALDECLLIST:
		printf("formalDeclList");
		break;

		case FORMALDECL:
		printf("formalDecl");
		break;

		case ARRAYDECL:
		printf("arrayDecl");
		break;

		case FUNBODY:
		printf("funBody");
		break;

		case LOCALDECLLIST:
		printf("localDeclList");
		break;

		case STATEMENTLIST:
		printf("statementList");
		break;

		case STATEMENT:
		printf("statement");
		break;

		case COMPOUNDSTMT:
		printf("compoundStmt");
		break;

		case ASSIGNSTMT:
		printf("assignStmt");
		break;

		case CONDSTMT:
		printf("condStmt");
		break;

		case LOOPSTMT:
		printf("loopStmt");
		break;

		case RETURNSTMT:
		printf("returnStmt");
		break;

		case UNARYSTMT:
		printf("unaryStmt");
		break;

		case UNARYOP:
		printf("unaryop , %s", node->strVal);
		break;

		case TYPESPEC:
		printf("typeSpecifier , %s", node->strVal);
		break;

		case IDENTIFIER:
		printf("identifier , %s", node->strVal);
		break;

		case INTEGER:
		printf("integer , %d", node->val);
		break;

		case CHAR:
		printf("char const , %d", node->val);
		break;

		case STRING:
		printf("string const , %s", node->strVal);
		break;

		case VAR:
		printf("var");
		break;

		case ASSIGNOP:
		printf(" = ");
		break;

		case EXPRESSION:
		printf("expression");
		break;

		case ADDEXPR:
		printf("addExpr");
		break;

		case TERM:
		printf("term");
		break;

		case ADDOP:
		printf("addop , %s", node->strVal);
		break;

		case FACTOR:
		printf("factor");
		break;

		case FUNCCALLEXPR:
		printf("funcCallExpr");
		break;

		case ARGLIST:
		printf("argList");
		break;

		case MULOP:
		printf("mulop , %s", node->strVal);
		break;

		case RELOP:
		printf("relop , %s", node->strVal);
		break;

		case IF:
		printf("if");
		break;

		case ELSE:
		printf("else");
		break;

		case RETURN:
		printf("return");
		break;

		case WHILE:
		printf("while");
		break;

		case LOGICALOP:
		printf("logicalop , %s", node->strVal);
		break;

		default:
		printf("ERROR");
		break;
	}
	printf("\n");
}
