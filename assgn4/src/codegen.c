#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "codegen.h"

enum nodeTypes {PROGRAM, DECLLIST, DECL, VARDECL, TYPESPEC, FUNDECL,
                FORMALDECLLIST, FORMALDECL, FUNBODY, LOCALDECLLIST,
                STATEMENTLIST, STATEMENT, COMPOUNDSTMT, ASSIGNSTMT,
                CONDSTMT, LOOPSTMT, RETURNSTMT, UNARYSTMT, EXPRESSION, RELOP,
                ADDEXPR, ADDOP, TERM, MULOP, FACTOR, FUNCCALLEXPR,
                ARGLIST, INTEGER, IDENTIFIER, VAR, ARRAYDECL, CHAR, STRING, 
                FUNCTYPENAME, LOGICALOP, UNARYOP, IF, ELSE, WHILE, RETURN, ASSIGNOP};

enum opType {ADD, SUB, MUL, DIV, MOD, LT, LTE, EQ, GTE, GT, NEQ, AND, OR, NOT, INC, DEC, ASSIGN};

/* string values for ast node types, makes tree output more readable */
char *NodeNames[42] = {"program", "declList", "decl", "varDecl", "typeSpecifier",
                       "funDecl", "formalDeclList", "formalDecl", "funBody",
                       "localDeclList", "statementList", "statement", "compoundStmt",
                       "assignStmt", "condStmt", "loopStmt", "returnStmt", "unaryStmt", "expression",
                       "relop", "addExpr", "addop", "term", "mulop", "factor",
                       "funcCallExpr", "argList", "integer", "identifier", "var",
                       "arrayDecl", "char", "string", "funcTypeName", "logicalop", "unaryop", "if", "else", "while", "return", "assignOp"};

char *TypeNames[4] = {"int", "char", "string", "void"};
char *Ops[17] = {"+", "-", "*", "/", "%", "<", "<=", "==", ">=", ">", "!=", "&&", "||", "!", "++", "--", "="};

enum op {Add, Sub, Mul, Div, LOAD, LOADI, STORE, MOVE, SET_ON_LESS_THAN, BEQ, BNE};

char *SymbolTypes[4] = {"scalar", "array", "function", "arg"};
char *DataTypes[4] = {"int", "char", "string", "void"};

char *mipsOp[11] = {"add", "sub", "mul", "div", "lw", "li", "sw", "move", "slt", "beq", "bne"};

char *MIPS_OP[10] = {"Add_it+ion-", "Sub_trac-tion+", "Mult_ipli*cati*on", 
					"Div--isi--on", "Lo`ad_Nor??mal", "Imm{ead]iate_Load", "Store", "Move", "SET_OweUHFUWEN_LESS_THAN", "Bracnh_Equal"};

int funScope;
int Label;
void genMainFunctionCode(tree *node);

// ----------- INSTRUCTION ----- //

instruction *makeRegInstruction(int operation, int dest_reg, int reg1, int reg2) {
      instruction *this = (instruction *) malloc(sizeof(struct instruction_node));
      this->operation = operation;
      this->dest_reg = dest_reg;
      this->reg1 = reg1;
      this->reg2 = reg2;
      this->next = NULL;
      return this;
}

// ----------- Open File ----- //

void openFile(){
	FILE *file;
    file = freopen("./src/output.asm", "w", stdout);

    if(file == NULL){
        printf("Unable to create file.\n");
        exit(1);
    }
}

// ----------- Registers ----- //

unsigned long genHash(char *str)
{
	unsigned long hash = 5381;
	int c;

	while (c = *str++)
		hash = ((hash << 5) + hash) + c; /*hash *33 + c */

	return hash % TEMP_VAR;
}

void initTempVarReg(){
	tempVar = 0;
	for(int i = 0;i <= TEMP_VAR;i++){
		RegHash[i] = NULL;
	}
}

void printHashReg(){
	printf("\n");
	for(int i = 0;i <= TEMP_VAR;i++){
		instruction *ins = RegHash[i];

		while(ins != NULL){
			printf("i : %d, op : %s, dest_reg : %d, reg1 : %d, reg2 : %d\n", 
				i, mipsOp[ins->operation], ins->dest_reg, ins->reg1, ins->reg2);

			ins = ins->next;
		}
	}
	printf("\n");
}

int nextReg(){
	return tempVar++;
}

char *stringLabel(char *strLabel, int label){
	char *strValue = (char*) malloc(sizeof(char));
	sprintf(strValue, "%s%d", strLabel, label);
	//printf("strValue %s\n", strValue);
	return strValue;
}

char *stringReg(int reg){

	char *strReg = (char*) malloc(sizeof(char) * 3);

	//printf("reg %d\n", reg);

	if(reg < -1){
		reg = reg + 2;
		sprintf(strReg, "$a%d", reg);
	}
	else if(funScope == MAIN_FUNCTION){
		sprintf(strReg, "$t%d", reg);
	}
	else if(funScope == SUB_FUNCTION){
		sprintf(strReg, "$s%d", reg);
	}
	//printf("strReg %s\n", strReg);
	return strReg;
}

char *stringValue(int value){
	char *strValue = (char*) malloc(sizeof(char));
	sprintf(strValue, "%d", value);
	//printf("strValue %s\n", strValue);
	return strValue;
}

char *stringIdentifier(char *id){

	/*
	if(isMain){
		return id;
	}
	else{
		int sp, fp;
		sp = idx * 4;

		char *ptr = (char*) malloc(sizeof(char) * 3);
		strcpy(ptr, "sp");

		for(int i = idx - 1;i >=0;i--){

			if(strcmp(activationRecord[i], "fp") == 0){
				fp = i;
				//printf("fp : %d\n", fp);
				strcpy(ptr, "fp");
			}

			if(strcmp(activationRecord[i], id) == 0){
				
				if(strcmp(ptr, "fp") == 0){
					sprintf(strId, "%d($%s)", (fp - i) * 4, ptr);
				}
				else{
					sprintf(strId, "%d($%s)", sp - (i * 4), ptr);
				}
			}
		}
		
	}
	*/

	//printf("strId %s\n", strId);
	/*
	int size = strlen(id) + 10;
	char *strId = (char*) malloc(sizeof(char) * size);
	strcpy(strId, id);
	*/

	return id;
}

int isRegInHashMap(int op, int r1, int r2){

	//printHashReg();

	//printf("isRegInHashMap - ");

	char *str1 = (char*) malloc(sizeof(char) * strlen(MIPS_OP[op]));
	strcpy(str1, MIPS_OP[op]);
	
	//printf("str1 %s\n", str1);

	if(r2 == -1)
		r2 = 0;

	char *str2 = (char*) malloc(sizeof(char) * 50);
	sprintf(str2, "%s%d%d", str1, r1, r2);

	//printf("str2 %s - ", str2);

	int key = genHash(str2);

	//printf("key %d\n", key);

	instruction *curr = RegHash[key];
	instruction *new = NULL;
	while(curr != NULL){
		if(curr->operation == op && curr->reg1 == r1 && curr->reg2 == r2){
			new = curr;
			break;
		}
		curr = curr->next;
	}

	if(new != NULL){
		return new->dest_reg;
	}
	else{
		return -1;
	}
}

// r1 & r2 -> source
// dest_reg -> dest

void saveRegInHashMap(int op, int r1, int r2, int dest_reg){

	//printHashReg();

	//printf("saveRegInHashMap - ");

	char *str1 = (char*) malloc(sizeof(char) * strlen(MIPS_OP[op]) );
	strcpy(str1, MIPS_OP[op]);
	
	//printf("str1 %s\n", str1);

	if(r2 == -1)
		r2 = 0;

	char *str2 = (char*) malloc(sizeof(char) * 50);
	sprintf(str2, "%s%d%d", str1, r1, r2);

	//printf("str2 %s - ", str2);

	int key = genHash(str2);

	//printf("key %d\n", key);

	// ---- First reg store
	if(RegHash[key] == NULL){

		RegHash[key] = makeRegInstruction(op, dest_reg, r1, r2);

	}
	// Store reg in linked list way
	else{

		instruction *curr = RegHash[key];
		instruction *new = makeRegInstruction(op, dest_reg, r1, r2);

		new->next = curr;
		RegHash[key] = new;

	}

	//printf("Key : %d, Saved reg %d\n", key, r3);
}

int getRegValueFromHashMap(int reg){

	int val = -1;

	for(int i = 0; i < TEMP_VAR; i++){

		instruction *ins = RegHash[i];

		if(ins != NULL){

			if(ins->operation == LOADI && ins->dest_reg == reg){
				val = ins->reg1;
				break;
			}
		}
	}

	return val;
}

// ----------- Tree ----------- //

int getKind(tree *node){

	char* nodeName = NodeNames[node->nodeKind];

	if(strcmp(nodeName,"identifier") == 0){
		return node->nodeKind;
  	}
  	else if(strcmp(nodeName,"integer") == 0){
  		return node->nodeKind;
  	}
	else if(strcmp(nodeName,"relop") == 0 || strcmp(nodeName,"mulop") == 0 || strcmp(nodeName,"addop") == 0){
        return node->val;
    }
}

tree *leftChild(tree *node){
	tree *parent = node->parent;
	tree *left = getChild(parent, 0);
	//printf("left : %s\n", NodeNames[left->nodeKind]);
	return left;
}

tree *rightChild(tree *node){
	tree *parent = node->parent;
	tree *right = getChild(parent, 2);
	//printf("right : %s\n", NodeNames[right->nodeKind]);
	return right;
}

// ----- Symbol Table ----- //

int isFunctionId(char *id, int hash_val){

	struct strEntry *entry = strTable[hash_val];

	 while(entry != NULL){

	 	struct typeEntry *type = entry->type;

    	if(strcmp(entry->id, id) == 0 && type->symbol_type == FUNCTION){
    		return 1;
    	}

    	entry = entry->next;
    }

    return 0;
}

int isArgId(char *id, int hash_val){

	struct strEntry *entry = strTable[hash_val];

	while(entry != NULL){

    	struct typeEntry *type = entry->type;

    	if(strcmp(entry->id, id) == 0 && type->symbol_type == ARG_SCALAR){

    		return 1;
    	}

    	entry = entry->next;
    }

    return 0;
}

// ------ Generate Instruction ----- //

void genInstructionComment(char *op, char *dest, char *r1, char *r2){
	if(strcmp(op,"li") == 0){
		printf("\t\t# Load immediate value %s into %s", r1, dest);
	}
	else if(strcmp(op,"lw") == 0){
		printf("\t\t# Load value from %s into %s", r1, dest);
	}
	else if(strcmp(op,"sw") == 0){
		printf("\t\t# Store value from %s into memory %s", dest, r1);
	}
	else if(strcmp(op,"add") == 0){
		printf("\t# Addition \t\t%s = %s + %s", dest, r1, r2);
	}
	else if(strcmp(op,"sub") == 0){
		printf("\t# Subtraction \t%s = %s - %s", dest, r1, r2);
	}
	else if(strcmp(op,"mul") == 0){
		printf("\t# Multiplication \t%s = %s * %s", dest, r1, r2);
	}
	else if(strcmp(op,"div") == 0){
		printf("\t# Division \t\t%s = %s / %s", dest, r1, r2);
	}

	printf("\n");
}

// op -> operation, dest -> dest register, r1 & r2 -> operand register
void emit(char *op, char *dest, char *r1, char *r2){

	printf("\t");
	if(strcmp(r2,"-1") == 0){
		printf("%s %s, %s", op, dest, r1);
	}
	else{
		printf("%s %s, %s, %s", op, dest, r1, r2);
	}

	//printf("\n");
	genInstructionComment(op, dest, r1, r2);
}

// ------ 1. Expression ----- //

int genExpression(tree *node){

	char* nodeName = NodeNames[node->nodeKind];
	//printf("Node name %s\n", nodeName);

	printf("");

	// ----- result -> result register
	// ----- t1 & t2 -> operand register
	int result, t1, t2;

	switch(getKind(node)){

		case ADD:
		case SUB:
		case MUL:
		case DIV:
		{
			t1 = genExpression(leftChild(node));
			t2 = genExpression(rightChild(node));

			//printf("op : %s, t1 : %d, t2 : %d\n", Ops[node->val], t1, t2);

			int checkReg = isRegInHashMap(node->val, t1, t2);
			if(checkReg != -1){
				result = checkReg;
			}
			else{
				result = nextReg();
				saveRegInHashMap(node->val, t1, t2, result);

				// ----- result ->dest reg, t1 & t2 -> source reg
				emit(mipsOp[node->val], stringReg(result), stringReg(t1), stringReg(t2));
			}

			//printf("result : %d\n", result);

			// ----- Save register valu in parent node 
			tree *parent = node->parent;
			parent->reg = result;
				
		}
		break;

		case IDENTIFIER:
		{
			int isFun = isFunctionId(node->str_val, node->val);
			int isArg = isArgId(node->str_val, node->val);

			if(isFun == 0 && isArg == 0){
				// ----- For ID, node->val = Hash value from Symbol table
				//printf("ID %s - ", node->str_val);

				int checkReg = isRegInHashMap(LOAD, node->val, -1);
				if(checkReg != -1){
					result = checkReg;
				}
				else{
					result = nextReg();
					saveRegInHashMap(LOAD, node->val, -1, result);

					// ----- result ->dest reg, id -> source
					emit(mipsOp[LOAD], stringReg(result), stringIdentifier(node->str_val), stringValue(-1));
				}

				//printf("checkReg %d\n", checkReg);

				// ----- Save register valu in parent node 
				node->reg = result;
			}
			else if(isFun == 0 && isArg == 1){

				for(int i = 0;i<ACTIVATION_RECORD;i++){
					if(strcmp(node->str_val, activationRecord[i]) == 0){
						node->reg = i - 2;
						break;
					}
				}
			}
		}
		break;

		case INTEGER:
		{
			// ----- For integer, node->val is its value
			int checkReg = isRegInHashMap(LOADI, node->val, -1);
			if(checkReg != -1){
				result = checkReg;
			}
			else{
				result = nextReg();
				saveRegInHashMap(LOADI, node->val, -1, result);
				emit(mipsOp[LOADI], stringReg(result), stringValue(node->val), stringValue(-1));
			}

			// ----- Save register valu in current node 
			node->reg = result;
		}
		break;

		default:
		{
			for (int i = 0; i < node->numChildren; i++) {

				result = genExpression(getChild(node, i));

				// ----- If the current node does not have the value, pull from its child node
				// ----- Every node in tree now can be represented by a register 
				if(node->reg == 0){
					tree *child = getChild(node, 0);
					node->reg = child->reg;
				}

				result = node->reg;

				//printf("\n%s, reg : %d\n", NodeNames[node->nodeKind], node->reg);
			}
    	}	
		break;
	}

	//printf("Result : %d\n", result);
	return result;
}

// ------ 2. Assignment Statement ----- //

// assignStmt      : var OPER_ASGN expression SEMICLN 
// var             : ID

int genAssignmentStmt(tree *node){

	char* nodeName = NodeNames[node->nodeKind];

	//printf("Node name %s\n", nodeName);

	// reuslt -> Left side of assign stmt, Id hash value
	// right -> right side of assign stmt, reg value

	int result, t;

	if(strcmp(nodeName,"assignOp") == 0){

		// assignStmt      : var OPER_ASGN expression SEMICLN 
		// var             : ID

		// ----- Left
		tree *varNode = leftChild(node);
		tree *idNode = varNode->children[0];
		result = idNode->val;

		// ----- right
		t = genExpression(rightChild(node));

		// --- Save value in symbol table

		int val = getRegValueFromHashMap(t);
		saveValueInSymbolTable(idNode->str_val, val);
		
		saveRegInHashMap(STORE, result, -1, t);
		emit(mipsOp[STORE], stringReg(t), stringIdentifier(idNode->str_val), stringValue(-1));



		// ----- Save register valu in parent node 
		tree *parent = node->parent;
		parent->reg = result;
	}

	for (int i = 0; i < node->numChildren; i++){

    	result = genAssignmentStmt(getChild(node, i));

    	// ----- If the current node does not have the value, pull from its child node
		// ----- Every node in tree now can be represented by a register 
		if(node->reg == 0){
			tree *child = getChild(node, 0);
			node->reg = child->reg;
		}

		result = node->reg;

		//printf("\n%s, reg : %d\n", NodeNames[node->nodeKind], node->reg);
    }

    //printf("Result : %d\n", result);
	return result;

}

// ------ 2. Condition Statement ----- //

// condStmt 		: KWD_IF LPAREN conditionExpr RPAREN statement %prec IFX
//					| KWD_IF LPAREN conditionExpr RPAREN statement KWD_ELSE statement

int genConditionExpr(tree *node, int isElse){

	char* nodeName = NodeNames[node->nodeKind];

	int cond, comp;
	int t1, t2;

	switch(getKind(node)){

		case LT:
		{
			// if(x < y)

			t1 = genExpression(leftChild(node));
			t2 = genExpression(rightChild(node));

			printf("\n");

			// ----- Condition
			// r1 -> x, r2 -> y
			// slt r3, r1, r2 -> r1 < r2

			cond = nextReg();
			emit(mipsOp[SET_ON_LESS_THAN], stringReg(cond), stringReg(t1), stringReg(t2));

			// ----- Compare
			comp = nextReg();
			emit(mipsOp[LOADI], stringReg(comp), stringValue(1), stringValue(-1));
	     		
			// ----- jump to 'label_if' if condition is true
			emit(mipsOp[BEQ], stringReg(cond), stringReg(comp), stringLabel("label_if", Label));

			// ----- jump to 'lable_else' if condition is false
			if(isElse){
				emit(mipsOp[BNE], stringReg(cond), stringReg(comp), stringLabel("label_else", Label));
			}

			// ----- jump to end if condition is false & no else stmt
			else{
				emit(mipsOp[BNE], stringReg(cond), stringReg(comp), stringLabel("label", Label));
			}
		}
		break;

		case GT:
		{
			// if(x > y)

			t1 = genExpression(leftChild(node));
			t2 = genExpression(rightChild(node));

			printf("\n");

			// ----- Condition
			// r1 -> x, r2 -> y
			// slt r3, r2, r1 -> r2 < r1

			cond = nextReg();
			emit(mipsOp[SET_ON_LESS_THAN], stringReg(cond), stringReg(t2), stringReg(t1));

			// ----- Compare
			comp = nextReg();
			emit(mipsOp[LOADI], stringReg(comp), stringValue(1), stringValue(-1));
	     		
			// ----- jump to 'label_if' if condition is true
			emit(mipsOp[BEQ], stringReg(cond), stringReg(comp), stringLabel("label_if", Label));

			// ----- jump to 'lable_else' if condition is false
			if(isElse){
				emit(mipsOp[BNE], stringReg(cond), stringReg(comp), stringLabel("label_else", Label));
			}

			// ----- jump to end if condition is false & no else stmt
			else{
				emit(mipsOp[BNE], stringReg(cond), stringReg(comp), stringLabel("label", Label));
			}
		}
		break;

		case EQ:
		{
			// if(x == y)

			t1 = genExpression(leftChild(node));
			t2 = genExpression(rightChild(node));

			printf("\n");

			// ----- Condition
			// r1 -> x, r2 -> y
			// slt r3, r1, r2 -> r2 < r1 is false. so r3 = 0

			cond = nextReg();
			emit(mipsOp[SET_ON_LESS_THAN], stringReg(cond), stringReg(t1), stringReg(t2));

			// ----- Compare
			comp = nextReg();
			emit(mipsOp[SET_ON_LESS_THAN], stringReg(comp), stringReg(t2), stringReg(t1));

			// ----- jump to 'label_if' if condition is true
			emit(mipsOp[BEQ], stringReg(cond), stringReg(comp), stringLabel("label_if", Label));

			// ----- jump to 'lable_else' if condition is false
			if(isElse){
				emit(mipsOp[BNE], stringReg(cond), stringReg(comp), stringLabel("label_else", Label));
			}

			// ----- jump to end if condition is false & no else stmt
			else{
				emit(mipsOp[BNE], stringReg(cond), stringReg(comp), stringLabel("label", Label));
			}
		}
		break;

		default:
		{
			for (int i = 0; i < node->numChildren; i++) {

				cond = genConditionExpr(getChild(node, i), isElse);

				// ----- If the current node does not have the value, pull from its child node
				// ----- Every node in tree now can be represented by a register 
				if(node->reg == 0){
					tree *child = getChild(node, 0);
					node->reg = child->reg;
				}

				cond = node->reg;

				//printf("\n%s, reg : %d\n", NodeNames[node->nodeKind], node->reg);
			}
		}
		break;
	}
}

int genCondStmt(tree *node){

	tree *condExprNode = NULL;
	tree *ifStmtNode = NULL;
	tree *elseStmtNode = NULL;

	for(int i = 0;i<node->numChildren;i++){

		tree *child = getChild(node, i);
		char *nodeName = NodeNames[child->nodeKind];

		if(strcmp(nodeName, "if") == 0){
			condExprNode = getChild(node, i + 1);
			ifStmtNode = getChild(node, i + 2);
		}
		if(strcmp(nodeName, "else") == 0){
			elseStmtNode = getChild(node, i + 1);
		}
	}

	if(elseStmtNode == NULL){
		genConditionExpr(condExprNode, 0);
	}
	else{
		genConditionExpr(condExprNode, 1);

		printf("\n\t%s:\n", stringLabel("label_else", Label));

		genMainFunctionCode(elseStmtNode);

		printf("\n\tjal %s\n", stringLabel("label", Label));
	}

	printf("\n\t%s:\n", stringLabel("label_if", Label));
	genMainFunctionCode(ifStmtNode);

	printf("\n\t%s:\n", stringLabel("label", Label));

	Label++;

}

// ----------- 5. Function ----------- //
int arg;
void moveFunArg(tree *node){

	char* nodeName = NodeNames[node->nodeKind];

	if(strcmp(nodeName,"identifier") == 0 || strcmp(nodeName,"integer") == 0){

		int reg;
		if(strcmp(nodeName,"identifier") == 0){
			reg = isRegInHashMap(LOAD, node->val, -1);
		}
		else{
			reg = isRegInHashMap(LOADI, node->val, -1);
		}

		char *strReg = (char*) malloc(sizeof(char) * 3);
		sprintf(strReg, "$a%d", arg++);

		saveRegInHashMap(MOVE, reg, -1, arg-1);
		emit(mipsOp[MOVE], strReg, stringReg(reg), stringValue(-1));
	}

	for (int i = 0; i < node->numChildren; i++){
    	moveFunArg(getChild(node, i));
    }
}

// funCallExpr 	: ID LPAREN argList RPAREN

void genFuncCallExpr(tree *node){

	char* nodeName = NodeNames[node->nodeKind];

	//printf("genFuncCallExpr : %s\n", nodeName);

	printf("\n\t# Call function\n");

	tree *funNameNode = node->children[0];
	char *funName = funNameNode->str_val;

	//printf("fun name : %s\n", funName);

	if(node->numChildren > 1){

		tree *argNode = node->children[1];
		arg = 0;
		moveFunArg(argNode);
	}

	printf("\tjal %s\t\t# Save current PC in $ra, and jump to %s\n", funName, funName);
	printf("\tmove $t%d,$v0\t# Return value saved in $v0.\n", nextReg());

	//printHashReg();
}

// ----------- MAIN ----------- //

void genMainFunctionCode(tree *node){

	char* nodeName = NodeNames[node->nodeKind];

	if(node->reg == NULL){

		// 1. Expression
		if(strcmp(nodeName,"expression") == 0){
			//printf("expression\n");
			genExpression(node);
		}

		// 2. Assignment Statement
		else if(strcmp(nodeName,"assignStmt") == 0){
			//printf("assignStmt\n");
			genAssignmentStmt(node);
		}

		// 3. Condition Statement
		else if(strcmp(nodeName,"condStmt") == 0){
			//printf("assignStmt\n");
			genCondStmt(node);
		}

		// 5. Function
		else if(strcmp(nodeName,"funcCallExpr") == 0){
			//printf("funcCallExpr\n");
			genFuncCallExpr(node);
		}
	}

	for (int i = 0; i < node->numChildren; i++){
    	genMainFunctionCode(getChild(node, i));
    }
}

// ----------- SUB FUNCTION ----------- //

void printActivationRecord(){
	for(int i=0;i<idx;i++){
		printf("%s\n", activationRecord[i]);
	}
}

// ----- Function name is the scope for local varibales
void addVarInActivationRecord(char *funName, int symbol_type){

	//printf("funName : %s\n", funName);

	// 5. Local register

	for(int i = 0; i < MAXIDS; i++){

        if(strTable[i] != NULL){
            
            struct strEntry *entry = strTable[i];
            //printf("ID - %s, ", entry->id);

            while(entry != NULL){

            	struct typeEntry *type = entry->type;

            	if(strcmp(entry->scope, funName) == 0 && type->symbol_type == symbol_type){

            		activationRecord[idx++] = entry->id;
            	}

            	entry = entry->next;
            }
        }
    }

}

void saveRegInActivationRecord(char *funName){

	int savRegCount = -1;

	for(int i = 0; i < MAXIDS; i++){

        if(strTable[i] != NULL){
            
            struct strEntry *entry = strTable[i];
            //printf("ID - %s, ", entry->id);

            while(entry != NULL){

            	struct typeEntry *type = entry->type;

            	if(strcmp(entry->id, funName) == 0){

            		// If anything returns, add one more registers
            		if(entry->type->data_type == INT_TYPE){
            			savRegCount = entry->type->param + 1;
            		}
            		else{
            			savRegCount = entry->type->param;
            		}
            		break;
            	}

            	entry = entry->next;
            }
        }

        if(savRegCount != -1)
        	break;
    }

    for(int i=0;i < savRegCount;i++){
    	activationRecord[idx++] = stringValue(i + 1);
    }
}

// node -> funDecl node
void genActivationRecord(tree *node){

	tree *funNameNode = node->children[0];
	tree *idNode = funNameNode->children[1];

	char *funName = idNode->str_val;

	//printf("Children : %d\n", node->numChildren);

	idx = 0;

	// funDecl 		: funcTypeName LPAREN formalDeclList RPAREN funBody
	// 0 -> funcTypeName, 1 -> formalDeclList, 2 -> funBody
	if(node->numChildren == 3){

		// 1. Add parameter varibales 
		addVarInActivationRecord(funName, ARG_SCALAR);
	}

	// 2. Return value & caller fp
	activationRecord[idx++] = "$v0";
	activationRecord[idx++] = "fp";

	// 3. Save register area
	saveRegInActivationRecord(funName);

    // 4. Global pointer
	activationRecord[idx++] = "global_ptr";

	// 5. Local register
	addVarInActivationRecord(funName, SCALAR);

	//printActivationRecord();
}

void genFunctionCode(tree *node){

	genActivationRecord(node);

	// ----- PUSH onto stack ----- //

	printf("\t# PUSH onto stack\n\n");

	int sp, fp;
	sp = idx * 4;
	for(int i = 0;i < idx;i++){
		if(strcmp(activationRecord[i],"fp") == 0){
			fp = sp - i*4;
			break;
		}
	}

	for(int i = (sp - fp)/4 + 1; i < idx; i++){
		if(strcmp(activationRecord[i],"global_ptr") == 0){
			break;
		}
		printf("\taddi $sp,$sp,-4\t\t# Adjust stack pointer\n");
		printf("\tsw $s%s,0($sp)\t\t# Save $s%s\n",activationRecord[i], activationRecord[i]);
	}

	// ------ FUNCTION BODY ----- //

	printf("\n\t# Function body\n\n");

	genMainFunctionCode(node);

	// ------ RETURN ----- //

	printf("\n\t# Save the return value in $v0\n\n");

	for(int i = (sp - fp)/4 + 1; i < idx; i++){
		if(strcmp(activationRecord[i],"global_ptr") == 0){

			printf("\tmove $v0,$s%s\n", activationRecord[i - 1]);
			break;
		}
	}

	// ----- POP from stack ----- //

	printf("\n\t# POP from stack\n\n");

	for(int i = (sp - fp)/4 + 1; i < idx; i++){
		if(strcmp(activationRecord[i],"global_ptr") == 0){
			break;
		}
		printf("\tlw $s%s,0($sp)\t\t# Restore $s%s\n", activationRecord[i], activationRecord[i]);
		printf("\taddi $sp,$sp,4\t\t# Adjust stack pointer\n");
	}

	printf("\n\t# Return from function\n\n");
	printf("\tjr $ra\t\t# Jump to addr stored in $ra\n");
}

void genHeaderForFunction(char *funName){

	printf("\n\n# ------------------------------------------------------------------\n\n");

	// ----- For main function
	if(strcmp(funName,"main") == 0){
		printf("\t# Declare main as a global function\n");
		printf("\t# The label 'main' represents the starting point\n");
		printf("\t.text\n");
		printf("\t.globl main\n");
	}
	// ----- For other function
	else{
		printf("\t# FUNCTION\n");
		printf("\t# Arguments are stored in $a0\n");
		printf("\t# Return value is stored in $v0\n");
		printf("\t# Return address is stored in $ra (put there by jal instruction)\n");
	}

	printf("%s:\n", funName);
}

// ----------- Traverse AST ----------- //

void traverseAst(tree *node) {

	char* nodeName = NodeNames[node->nodeKind];

	if(strcmp(nodeName,"funDecl") == 0){

		// node -> funDecl node
		// funDecl 		: funcTypeName LPAREN formalDeclList RPAREN funBody
		// funDecl 		: funcTypeName LPAREN RPAREN funBody

		// funcTypeName : typeSpecifier ID

		tree *funNameNode = node->children[0];
		tree *idNode = funNameNode->children[1];

		char *funName = idNode->str_val;

		// ----- Header & Label for function
		genHeaderForFunction(funName);
		
		if(strcmp(funName,"main") == 0){

			funScope = MAIN_FUNCTION;
			initTempVarReg();
			genMainFunctionCode(node);
		}
		else{
			
			funScope = SUB_FUNCTION;
			genFunctionCode(node);
		}
	}

    for (int i = 0; i < node->numChildren; i++){
    	traverseAst(getChild(node, i));
    }
}

// ----------- Exit Code ----------- //

void genExitCode(){

	printf("\n");
	printf("\t# Exit the program by means of a syscall.\n");
	printf("\t# The code for exit is 10. \n");

	printf("\tli $v0, 10 	# Sets $v0 to 10 to select exit syscall\n");
	printf("\tsyscall 	# Exit\n");
	printf("\n");
}

// ----------- Data Code ----------- //

void genDataSection(){

	printf("\n\n# ------------------------------------------------------------------\n\n");
	printf("\t# All memory structures are placed after the .data assembler directive\n");
	printf("\t.data\n");
	printf("\n");

	for(int i = 0; i < MAXIDS; i++){

		struct strEntry *entry = strTable[i];

		while(entry != NULL){

		 	struct typeEntry *type = entry->type;

			if(type->symbol_type != FUNCTION){
				printf("%s:\t.word %d\n", entry->id, entry->value);
			}

			entry = entry->next;
    	}
	}

	printf("\n\n");
}

void codeGenerate(tree *ast){

	//printf("Code Gen\n\n");

	openFile();

	// To keep count of labels with in function for IF ELSE
	Label = 0;
	traverseAst(ast);

	genExitCode();

	genDataSection();

	//printf("%s\n", stringIdentifier("k"));
}
