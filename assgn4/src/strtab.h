#include "tree.h"

#ifndef STRTAB_H
#define STRTAB_H
#define MAXIDS 1000

#define UNDECLARED_VAR -100

enum dataType {INT_TYPE, CHAR_TYPE, STRING_TYPE, VOID_TYPE};
enum symbolType {SCALAR, ARRAY, FUNCTION, ARG_SCALAR, ARG_ARRAY};

struct typeEntry{
	int symbol_type; // Var & Function
	int data_type; // For var data_type & for fun, return type
	int param;
	int param_types[MAXIDS];
};

struct strEntry{
    char* id;
    char* scope;
    int value;
    struct typeEntry *type;
    struct strEntry *next;
};

struct strEntry *strTable[MAXIDS];

int ST_insert(char *id, char *scope, int data_type, int symbol_type, tree *fun_param);

int ST_lookup(char *id, char *scope);

void output_entry(int i);

void print_sym_tab();

#endif
