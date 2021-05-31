#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "strtab.h"

char *dataTypes[4] = {"int", "char", "string", "void"};
char *symbolTypes[4] = {"scalar", "array", "function", "arg"};

void updateVarScope(char *scope_name);

unsigned long hash(char *str)
{
	unsigned long hash = 5381;
	int c;

	while (c = *str++)
		hash = ((hash << 5) + hash) + c; /*hash *33 + c */

	return hash % MAXIDS;
}

struct strEntry *createStrEntry(char *id, char *scope, struct typeEntry *type){
	struct strEntry *entry = (struct strEntry*)malloc(sizeof(struct strEntry));
	entry->id = id;
	entry->scope = scope;
	entry->type = type;
	entry->next = NULL;
	return entry;
}

struct typeEntry *createTypeEntry(int symbol_type, int data_type){
	struct typeEntry *type_entry = (struct typeEntry*)malloc(sizeof(struct typeEntry));
	type_entry->symbol_type = symbol_type;
	type_entry->data_type = data_type;
	type_entry->param = 0;
	type_entry->param_types[0] = 0;
	return type_entry;
}

int ST_insert(char *id, char *scope, int data_type, int symbol_type, tree *fun_param){

	if(id == NULL)
		return -1;

	unsigned long index = ST_lookup(id, scope);

	// If id does not exist in Symbol table, then insert.

	if(index == UNDECLARED_VAR){

		struct typeEntry *new_type = createTypeEntry(symbol_type, data_type);

		// If FUNCTION, then also insert no of parameter & parameter types info into symbol table

		if(symbol_type == FUNCTION){

			updateFunParam(new_type, fun_param);

			// For FUNCTION, update the scope of local variable
			// scope of loacl var = name of the function

			updateVarScope(id);
		}
		struct strEntry *new_entry = createStrEntry(id, scope, new_type);

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
			entry->type->data_type == data_type && entry->type->symbol_type == symbol_type){

			if(symbol_type == FUNCTION){
				yyerror("Multiple declaration of function ");
			}
			else{
				yyerror("Multiple declaration of varibale ");
			}
		}
		else{
			// If different id's have same hash value, then they are linked next to each other

			struct typeEntry *new_type = createTypeEntry(symbol_type, data_type);
			if(symbol_type == FUNCTION && fun_param != NULL){
				updateFunParam(new_type, fun_param);
			}
			struct strEntry *new_entry = createStrEntry(id, scope, new_type);
			new_entry->next = entry;
			strTable[index] = new_entry;
		}
	}

	return index;
}

int ST_lookup(char *id, char *scope){

	if(id == NULL)
		return -1;

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

		struct typeEntry *type = entry->type;

		if(type->symbol_type == FUNCTION){
			printf("Symbol type : %s, ID : %s, Scope : %s, Return type : %s, No of Param : %d, Param Types : ", 
			symbolTypes[type->symbol_type], entry->id, entry->scope, 
			dataTypes[type->data_type],type->param);

			for(int i = 0;i < type->param; i++){
				printf("%s ", dataTypes[type->param_types[i]]);
			}
			printf("\n");
		}
		else{
			printf("Symbol type : %s, ID : %s, Scope : %s, Data type : %s", 
			symbolTypes[type->symbol_type], entry->id, entry->scope, dataTypes[type->data_type]);
            printf("\n");
		}
		entry = entry->next;
	}
}

void print_sym_tab(){
	printf("\n\nSYMBOL TABLE:\n");
    for(int i = 0; i < MAXIDS; i++){
        if(strTable[i] != NULL){
            output_entry(i);
        }
    }
}

// For FUNCTION, update the scope of local variable
// scope of loacl var = name of the function
void updateVarScope(char *scope_name){

	//printf("updateVarScope : %s\n", scope_name);

	for(int i = 0; i < MAXIDS; i++){

        if(strTable[i] != NULL){
            
            struct strEntry *entry = strTable[i];
            //printf("ID - %s, ", entry->id);

            while(entry != NULL){

            	struct typeEntry *type = entry->type;

            	// Type -> var & scope -> local. Means scope is not updated yet.
            	// scope of loacl var = name of the function

            	//printf("Type - %s\n", symbolTypes[type->symbol_type]);

            	if(type->symbol_type != FUNCTION && strcmp(entry->scope, "local") == 0){

            		int size = strlen(scope_name);
            		char *str_scope = (char*) malloc(sizeof(char) *(size + 1));
					strcpy(str_scope, scope_name);

					entry->scope = str_scope;
            	}

            	entry = entry->next;
            }
        }
    }
}

void saveValueInSymbolTable(char *id, int value){

	for(int i = 0; i < MAXIDS; i++){

        if(strTable[i] != NULL){
            
            struct strEntry *entry = strTable[i];
            //printf("ID - %s, ", entry->id);

            while(entry != NULL){

            	struct typeEntry *type = entry->type;

            	if(strcmp(entry->id, id) == 0 && type->symbol_type == SCALAR){

            		entry->value = value;

            		//printf("ID : %s, val : %d\n", entry->id, entry->value);
            	}

            	entry = entry->next;
            }
        }
    }
}
