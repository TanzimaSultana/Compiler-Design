
/* definitions */

%{
#include<stdio.h>
#include"tokendef.h"

int scancol = 1;
int yycol = 1;
int scanlineno = 1;

char *id;
int num;
char ch;
char *str;

char* yyerror;

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

if 				{updateCol(); return KWD_IF;}
else			{updateCol(); return KWD_ELSE;}
while			{updateCol(); return KWD_WHILE;}
int 			{updateCol(); return KWD_INT;}
string 			{updateCol(); return KWD_STRING;}
char 			{updateCol(); return KWD_CHAR;}
return	    	{updateCol(); return KWD_RETURN;}
void 			{updateCol(); return KWD_VOID;}

"+"				{updateCol(); return OPER_ADD;}
"-"				{updateCol(); return OPER_SUB;}
"*"				{updateCol(); return OPER_MUL;} 
"/"				{updateCol(); return OPER_DIV;} 
"%"				{updateCol(); return OPER_MOD;}
"<"				{updateCol(); return OPER_LT;}
">"				{updateCol(); return OPER_GT;} 
"<="			{updateCol(); return OPER_LTE;}
">=" 			{updateCol(); return OPER_GTE;}
"=="			{updateCol(); return OPER_EQ;}
"!=" 			{updateCol(); return OPER_NEQ;}
"="				{updateCol(); return OPER_ASGN;}
"["				{updateCol(); return LSQ_BRKT;}
"]"				{updateCol(); return RSQ_BRKT;}
"{"				{updateCol(); return LCRLY_BRKT;}
"}"				{updateCol(); return RCRLY_BRKT;}
"(" 			{updateCol(); return LPAREN;}
")" 			{updateCol(); return RPAREN;}
","				{updateCol(); return COMMA;}
";"				{updateCol(); return SEMICLN;}
"@" 			{updateCol(); return OPER_AT;} 
"++"			{updateCol(); return OPER_INC;} 
"--"			{updateCol(); return OPER_DEC;}
"&&"			{updateCol(); return OPER_AND;}
"||"			{updateCol(); return OPER_OR;} 
"!"				{updateCol(); return OPER_NOT;}


{identifier}    	{updateCol(); id = yytext; return ID;}
{illidentifier} 	{updateCol(); yyerror = ILLEGAL_TOKEN_ERROR; return ERROR;}

{integer}       	{updateCol(); num = atoi(yytext); return INTCONST;}
{integerlead0}  	{updateCol(); yyerror = ILLEGAL_TOKEN_ERROR; return ERROR;}

{character}     	{updateCol(); ch = yytext[1]; return CHARCONST;}
{illicharacter}  	{updateCol(); yyerror = ILLEGAL_TOKEN_ERROR; return ERROR;}

{string}        	{updateCol(); return processString();}
{multlnstring}  	{updateCol(); countLines(); yyerror = MULTILINE_STR; return ERROR;}
{untermstring}  	{updateCol(); yyerror = UNTERMINATED_STR; return ERROR;}

{comment}       	{countLines();}
{multlncomment}     {countLines();}
{untermcomment}		{countLines();yyerror = UNTERMINATED_CMMNT; return ERROR;}

{newline}       	{countLines();}
{whitespace}    	{updateCol();}
.             		{yyerror = ILLEGAL_TOKEN_ERROR; return ERROR;}

%%

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
	str = (char*)malloc(sizeof(char) * (yyleng + 1));
	int j = 0;
	for(int i=0; yytext[i]; i++){

		if(yytext[i] == '\\' && yytext[i + 1]){
			if(yytext[i + 1] == 'n'){
				str[j++] = '\n';
				i++;
			}
			else if(yytext[i + 1] == 't'){
				str[j++] = '\t';
				i++;
			}
			else if(yytext[i + 1] == '\"'){
				str[j++] = '\"';
				i++;
			}
			else if(yytext[i + 1] == '\\'){
				str[j++] = '\\';
				i++;
			}
			else{
				yycol = scancol - (yyleng - i) + 1;
				yyerror = UNRECOG_ESC_CHAR_STR;
				return ERROR;
			}
		}
		else{
			str[j++] = yytext[i];
		}
	}
	str[j] = '\0';
	return STRCONST;
}
