#include<tree.h>
#include<strtab.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

/* string values for ast node types, makes tree output more readable */
char *nodeNames[42] = {"program", "declList", "decl", "varDecl", "typeSpecifier",
                       "funDecl", "formalDeclList", "formalDecl", "funBody",
                       "localDeclList", "statementList", "statement", "compoundStmt",
                       "assignStmt", "condStmt", "loopStmt", "returnStmt", "unaryStmt", "expression",
                       "relop", "addExpr", "addop", "term", "mulop", "factor",
                       "funcCallExpr", "argList", "integer", "identifier", "var",
                       "arrayDecl", "char", "string", "funcTypeName", "logicalop", "unaryop", "if", "else", "while", "return", "assignOp"};

char *typeNames[4] = {"int", "char", "string", "void"};
char *ops[17] = {"+", "-", "*", "/", "%", "<", "<=", "==", ">=", ">", "!=", "&&", "||", "!", "++", "--", "="};

tree *maketree(int kind) {
      tree *this = (tree *) malloc(sizeof(struct treenode));
      this->nodeKind = kind;
      this->numChildren = 0;
      return this;
}

tree *maketreeWithIntVal(int kind, int val) {

      tree *this = (tree *) malloc(sizeof(struct treenode));
      this->nodeKind = kind;
      this->numChildren = 0;
      this->val = val;
      return this;
}

tree *maketreeWithStrVal(int kind, char *str_val) {
      tree *this = (tree *) malloc(sizeof(struct treenode));
      this->nodeKind = kind;
      this->numChildren = 0;
      this->str_val = str_val;
      return this;
}

void addChild(tree *parent, tree *child) {

      if(parent != NULL && child != NULL){
          if (parent->numChildren == MAXCHILDREN) {
            printf("Cannot add child to parent node\n");
            exit(1);
          }
          child->parent = parent;
          nextAvailChild(parent) = child;
          parent->numChildren++;
      }
}

void printAst(tree *node, int nestLevel) {
      char* nodeName = nodeNames[node->nodeKind];
      if(strcmp(nodeName,"identifier") == 0){
          if(node->val == -1)
              printf("%s,%s\n", nodeName,"undeclared variable");
          else
              printf("%s,%s\n", nodeName,node->str_val);
              //printf("%s,%s\n", nodeName,get_symbol_id(node->val));
      }
      else if(strcmp(nodeName,"integer") == 0){
          printf("%s,%d\n", nodeName,node->val);
      }
      else if(strcmp(nodeName,"char") == 0){
          printf("%s,%c\n", nodeName,node->val);
      }
      else if(strcmp(nodeName,"typeSpecifier") == 0){
          printf("%s,%s\n", nodeName,typeNames[node->val]);
      }
      else if(strcmp(nodeName,"relop") == 0 || strcmp(nodeName,"mulop") == 0 || strcmp(nodeName,"addop") == 0 
           || strcmp(nodeName,"unaryop") == 0 || strcmp(nodeName,"logicalop") == 0 || strcmp(nodeName,"assignOp") == 0){
          printf("%s,%s\n", nodeName,ops[node->val]);
      }
      else{
          printf("%s\n", nodeName);
      }

      int i, j;

      for (i = 0; i < node->numChildren; i++)  {
          for (j = 0; j < nestLevel; j++)
              printf("    ");
          printAst(getChild(node, i), nestLevel + 1);
      }

}

void flattenList(tree *list, tree *subList){
    for(int i=0; i < subList->numChildren; i++){
        addChild(list,getChild(subList,i));
    }
}

// ------ Function Parameter Info ----- //

// Traverse the node & update the parameter info (no of parameter, parameter types) into typeEntry type
void updateFunParam(struct typeEntry *type, tree *node){

  if(node == NULL)
    return;

  char* nodeName = nodeNames[node->nodeKind];
  if(strcmp(nodeName,"identifier") == 0){
    type->param_types[type->param] = node->data_type;
    type->param = type->param + 1;
  }
  else if(strcmp(nodeName,"integer") == 0){
    type->param_types[type->param] = INT_TYPE;
    type->param = type->param + 1;
  }
  else if(strcmp(nodeName,"char") == 0){
    type->param_types[type->param] = CHAR_TYPE;
    type->param = type->param + 1;
  }
  else if(strcmp(nodeName,"void") == 0){
    type->param_types[type->param] = VOID_TYPE;
    type->param = type->param + 1;
  }
  
  for (int i = 0; i < node->numChildren; i++){
    updateFunParam(type, getChild(node, i));
  }
}

void matchFunParam(struct typeEntry *type1, struct typeEntry *type2){

  // type1 - Function declaration parameter
  // type2 - function call parameter

  // 1. Check no of parameter matches
  if(type2->param > type1->param){
    yyerror("Too many arguments provided in function call.");
  }
  else if(type2->param < type1->param){
    yyerror("Too few arguments provided in function call.");
  }

  // 2. Check type mis-match in parameter
  if(type1->param == type2->param){

      for(int i = 0; i < type1->param; i++){
        if(type1->param_types[i] != type2->param_types[i]){
          yyerror("Argument type mismatch in function call.");
        }
      }
  }
}

// ------ Error checking for Array ------ //

int arrayIndex;

void checkArrayIndex(tree *node){

  char* nodeName = nodeNames[node->nodeKind];

  if(strcmp(nodeName,"integer") == 0){
      
      // 1. Array index size is zero
      if(node->val == 0){
        arrayIndex = ARRAY_INDEX_ZERO;
      }

      // 2. Array index size is out of bound
      else if(node->val == -1){
        arrayIndex = ARRAY_INDEX_OUT_OF_BOUND;
      }

      // 3. Array index size is valid
      else{
        arrayIndex = ARRAY_INDEX_VALID;
      }
  }

  for (int i = 0; i < node->numChildren; i++){
    checkArrayIndex(getChild(node, i));
  }
}

int isValidArrayIndex(tree *node){
  arrayIndex = ARRAY_INDEX_NON_INT;
  checkArrayIndex(node);
  return arrayIndex;
}

// ------ Type Mis-Match ------ //

int leftType, rightType;

// Traverse the partial AST tree & find left data type
void getLeftDataType(tree *node){

  char* nodeName = nodeNames[node->nodeKind];

  if(strcmp(nodeName,"identifier") == 0){
    leftType = node->data_type;
    //printf("LHS data type %s\n", typeNames[leftType]);
    return;
  }

  for (int i = 0; i < node->numChildren; i++){
    getLeftDataType(getChild(node, i));
  }
}

// Traverse the partial AST tree & match right data type with left data type
void matchRightDataType(tree *node){

  char* nodeName = nodeNames[node->nodeKind];
  rightType = -1;

  if(strcmp(nodeName,"identifier") == 0){
      rightType = node->data_type;
  }
  else if(strcmp(nodeName,"integer") == 0){
      rightType = INT_TYPE;
  }
  else if(strcmp(nodeName,"char") == 0){
      rightType = CHAR_TYPE;
  }
  else if(strcmp(nodeName,"void") == 0){
      rightType = VOID_TYPE;
  }

  if(rightType != -1){

    //printf("RHS data type %s\n", typeNames[rightType]);

    // valid - int = int/int const/char/char const/void
    // invalid - int = void
    if(leftType == INT_TYPE){
      if(rightType == VOID_TYPE){
        rightType = -1;
        return;
      }
    }

    // valid - char = char/char const
    // invalid - char = int/void
    else if(leftType == CHAR_TYPE){
      if(rightType == INT_TYPE || rightType == VOID_TYPE){
        rightType = -1;
        return;
      }
    }

    // valid - void = void/int/int const
    // invalid - void = char
    else if(leftType == VOID_TYPE){
      if(rightType == CHAR_TYPE){
        rightType = -1;
        return;
      }
    }
  }

  for (int i = 0; i < node->numChildren; i++){
    matchRightDataType(getChild(node, i));
  }

}

int isTypeMatch(tree *leftNode, tree *rightNode){

  leftType = -1, rightType = -1;

  getLeftDataType(leftNode);
  matchRightDataType(rightNode);

  return rightType;

}

