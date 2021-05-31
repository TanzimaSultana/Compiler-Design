#ifndef STRTAB_H
#define STRTAB_H
#define MAXIDS 1000

#define UNDECLARED_VAR -1

enum dataType {INT_TYPE, CHAR_TYPE, STRING_TYPE, VOID_TYPE};
enum symbolType {SCALAR, ARRAY, FUNCTION, ARG};

struct strEntry{
    char* id;
    char* scope;
    char* data_type;
    int   symbol_type;
    struct strEntry *next;
};

struct strEntry *strTable[MAXIDS];

int ST_insert(char *id, char *scope, char* data_type, int symbol_type);

int ST_lookup(char *id, char *scope);

void output_entry(int i);

#endif
