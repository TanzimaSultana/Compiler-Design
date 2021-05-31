#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "strtab.h"

unsigned long hash(char *str)
{
	unsigned long hash = 5381;
	int c;

	while (c = *str++)
		hash = ((hash << 5) + hash) + c; /*hash *33 + c */

	return hash % MAXIDS;
}

struct strEntry *createStrEntry(char *id, char *scope, char* data_type, int symbol_type){
	struct strEntry *entry = (struct strEntry*)malloc(sizeof(struct strEntry));
	entry->id = id;
	entry->scope = scope;
	entry->data_type = data_type;
	entry->symbol_type = symbol_type;
	entry->next = NULL;
	return entry;
}

int ST_insert(char *id, char *scope, char* data_type, int symbol_type){

	unsigned long index = ST_lookup(id, scope);

	// If id does not exist in Symbol table, then insert.

	if(index == UNDECLARED_VAR){

		struct strEntry *new_entry = createStrEntry(id, scope, data_type, symbol_type);

		int size = strlen(id) + strlen(scope);
		char *str = (char*) malloc(sizeof(char) *(size + 1));
		strcpy(str, id);
		strcat(str, scope);

		index = hash(str);

		strTable[index] = new_entry;
	}
	else{
		// If an id is declared multiple times.

		struct strEntry *entry = strTable[index];
		if(strcmp(entry->id, id) == 0 && strcmp(entry->scope, scope) == 0 && 
			strcmp(entry->data_type, data_type) == 0 && entry->symbol_type == symbol_type){

			yywarning("Duplicate declaration", id);
		}
		else{
			// If different id's have same hash value, then they are linked next to each other

			struct strEntry *new_entry = createStrEntry(id, scope, data_type, symbol_type);
			new_entry->next = entry;
			strTable[index] = new_entry;
		}
	}

	return index;
}

int ST_lookup(char *id, char *scope){

	int size = strlen(id) + strlen(scope);
	char *str = (char*) malloc(sizeof(char) *(size + 1));
	strcpy(str, id);
	strcat(str, scope);

	unsigned long index = hash(str);

	if(strTable[index] != NULL){
		return index;
	}
	else{
		return UNDECLARED_VAR;
	}
	
}

void output_entry(int i){
	struct strEntry *entry = strTable[i];
	while(entry != NULL){
		printf("ID : %s, Scope : %s, Data type : %s, Symbol type : ", entry->id, entry->scope, entry->data_type);
		switch(entry->symbol_type){
			case SCALAR:
			printf("scalar");
			break;

			case ARRAY:
			printf("array");
			break;

			case FUNCTION:
			printf("function");
			break;

			case ARG:
			printf("arg");
			break;

			default:
			printf("ERROR");
			break;
		}
		printf("\n");

		entry = entry->next;
	}
}