#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include "tree.h"
#include "strtab.h"

#define ACTIVATION_RECORD 50
#define TEMP_VAR 20
#define SAVD_VAR 8

#define MAIN_FUNCTION 100
#define SUB_FUNCTION 200
#define ARG 300

int tempVar;
int idx;

char *activationRecord[ACTIVATION_RECORD];

typedef struct instruction_node instruction;

struct instruction_node
{
	int operation;
	int dest_reg;
	int reg1;
	int reg2;
	instruction *next;
};

instruction *RegHash[TEMP_VAR];
instruction *SavdRegHash[SAVD_VAR];

void codeGenerate(tree *ast);