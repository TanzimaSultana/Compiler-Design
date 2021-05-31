
/* definitions */

%{
#include "y.tab.h"
#include <stdio.h>

int scancol = 1;
int yycol = 1;
int scanlineno = 1;

// To pass error message to yyerror function is parser.y
extern int yyerror(char * msg);

extern char* id;
extern char* funName;

void updateCol();
void countLines();
int processString();

%}

newline         \n
whitespace      [ \t]+	
integer         0|[1-9][0-9]*
integerlead0    0[0-9]+	

character       '[a-zA-Z0-9]'
illicharacter	'[a-zA-Z0-9]++'

string       	["].+["]
multlnstring    ["][^"]+["]
untermstring    ["]([^"\\])+

comment         "/*"((\*+[^/*])|[^\n])*\**"*/"
multlncomment   "/*"((\*+[^/*])|([^*]))*\**"*/"
untermcomment   "/*".+

identifier      [a-zA-Z][a-zA-Z0-9]*
illidentifier   [0-9]+[a-zA-Z]+[a-zA-Z0-9]*

yylineno = 0;

%%

 /* rules */ 

if 					{updateCol(); return KWD_IF;}
else				{updateCol(); return KWD_ELSE;}
while 				{updateCol(); return KWD_WHILE;}
int 				{updateCol(); return KWD_INT;}
string 				{updateCol(); return KWD_STRING;}
char 				{updateCol(); return KWD_CHAR;}
return 				{updateCol(); return KWD_RETURN;}
void 				{updateCol(); return KWD_VOID;}

"+"					{updateCol(); return OPER_ADD;}
"-"					{updateCol(); return OPER_SUB;}
"*"					{updateCol(); return OPER_MUL;} 
"/"					{updateCol(); return OPER_DIV;} 
"%"					{updateCol(); return OPER_MOD;}
"<"					{updateCol(); return OPER_LT;}
">"					{updateCol(); return OPER_GT;} 
"<="				{updateCol(); return OPER_LTE;}
">=" 				{updateCol(); return OPER_GTE;}
"=="				{updateCol(); return OPER_EQ;}
"!=" 				{updateCol(); return OPER_NEQ;}
"="					{updateCol(); return OPER_ASGN;}
"["					{updateCol(); return LSQ_BRKT;}
"]"					{updateCol(); return RSQ_BRKT;}
"{"					{updateCol(); return LCRLY_BRKT;}
"}"					{updateCol(); return RCRLY_BRKT;}
"(" 				{updateCol(); funName = yylval.strval; return LPAREN;}
")" 				{updateCol(); return RPAREN;}
","					{updateCol(); return COMMA;}
";"					{updateCol(); return SEMICLN;}
"@"                 {updateCol(); return OPER_AT;} 
"++"                {updateCol(); return OPER_INC;} 
"--"                {updateCol(); return OPER_DEC;}
"&&"				{updateCol(); return OPER_AND;}
"||"				{updateCol(); return OPER_OR;} 
"!"					{updateCol(); return OPER_NOT;}

{identifier}    	{
					updateCol(); 

                    // Saving the id in yylval for using in parser.y

					yylval.strval = (char *) malloc(yyleng * sizeof(char)); 
                 	strcpy(yylval.strval, yytext); 
                    id = yylval.strval;

                 	return ID;
                 	}

{illidentifier} 	{
                    updateCol(); 

                    // Passing error message to yyerror function is parser.y

                    yyerror("An identifier that starts with number"); 
                    return ERROR;
                    }

{integer}       	{
                    updateCol(); 

                    // Saving the int value in yylval for using in parser.y

                    yylval.value = atoi(yytext); 

                    return INTCONST;
                    }

{integerlead0}  	{updateCol(); yyerror("An integer with leading zero"); return ERROR;}

{character}     	{
					updateCol(); 

                    // Saving the ASCII value of the char in yylval for using in parser.y

					yylval.value = yytext[1];  
                 	return CHARCONST;
                 	}
{illicharacter}  	{updateCol(); yyerror("Illegal token"); return ERROR;}

{string}        	{updateCol(); return processString();}
{multlnstring}  	{updateCol(); countLines(); yyerror("String spans multiple lines"); return ERROR;}
{untermstring}  	{updateCol(); yyerror("Unterminated string"); return ERROR;}

{comment}       	{countLines();}
{multlncomment}     {countLines();yyerror("Multiple line comment"); return ERROR;}
{untermcomment}		{countLines();yyerror("Unterminated comment"); return ERROR;}

{newline}       	{countLines();}
{whitespace}    	{updateCol();}
.             		{yyerror("Illegal token"); return ERROR;}

%%

// user routines 

void updateCol(){
	scancol += yyleng;
	yycol = scancol - yyleng;
}

void countLines(){
	for(int i=0; yytext[i]; i++){
		if(yytext[i] == '\n'){
			scanlineno++;
		}
	}
	yylineno = scanlineno;
	scancol = 1;
}

int processString(){
    char* temp = malloc(yyleng * sizeof('a'));

    int tempcursor = 0;

    // Traverse yytext from 1 to yyleng - 2 to avoid beginning and trailing quotes.

    for (int yycursor = 1; yycursor < yyleng - 1; yycursor++){
        char currentChar = yytext[yycursor];
        if (currentChar == '\\'){

            // The next character is escaped. Add appropriate character to our temp string.

            currentChar = yytext[++yycursor];
            if (currentChar == '"'){
                temp[tempcursor++] = '"';
            }
            else if (currentChar == 'n'){
                temp[tempcursor++] = '\n';
            }
            else if (currentChar == 't'){
                temp[tempcursor++] = '\t';
            }
            else if (currentChar == '\\'){
                temp[tempcursor++] = '\\';
            }
            else{
                yycol += yycursor;
                yyerror("Unrecognized escape character in String");
                return ERROR;
            }
        }
        else{
            // If the character isn't escaped, add it to our temp string.
            temp[tempcursor++] = currentChar;
        }
    }

    strcpy(yylval.strval,temp);
    free(temp);
    yyleng = tempcursor;

    return STRCONST;
}

