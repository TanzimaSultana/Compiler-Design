/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     ID = 258,
     INTCONST = 259,
     CHARCONST = 260,
     STRCONST = 261,
     KWD_IF = 262,
     KWD_ELSE = 263,
     KWD_WHILE = 264,
     KWD_INT = 265,
     KWD_STRING = 266,
     KWD_CHAR = 267,
     KWD_VOID = 268,
     KWD_RETURN = 269,
     OPER_ADD = 270,
     OPER_SUB = 271,
     OPER_MUL = 272,
     OPER_DIV = 273,
     OPER_MOD = 274,
     OPER_LT = 275,
     OPER_GT = 276,
     OPER_LTE = 277,
     OPER_GTE = 278,
     OPER_EQ = 279,
     OPER_NEQ = 280,
     OPER_ASGN = 281,
     OPER_AT = 282,
     OPER_INC = 283,
     OPER_DEC = 284,
     LSQ_BRKT = 285,
     RSQ_BRKT = 286,
     LCRLY_BRKT = 287,
     RCRLY_BRKT = 288,
     LPAREN = 289,
     RPAREN = 290,
     COMMA = 291,
     SEMICLN = 292,
     OPER_AND = 293,
     OPER_OR = 294,
     OPER_NOT = 295,
     ERROR = 296,
     IFX = 297
   };
#endif
/* Tokens.  */
#define ID 258
#define INTCONST 259
#define CHARCONST 260
#define STRCONST 261
#define KWD_IF 262
#define KWD_ELSE 263
#define KWD_WHILE 264
#define KWD_INT 265
#define KWD_STRING 266
#define KWD_CHAR 267
#define KWD_VOID 268
#define KWD_RETURN 269
#define OPER_ADD 270
#define OPER_SUB 271
#define OPER_MUL 272
#define OPER_DIV 273
#define OPER_MOD 274
#define OPER_LT 275
#define OPER_GT 276
#define OPER_LTE 277
#define OPER_GTE 278
#define OPER_EQ 279
#define OPER_NEQ 280
#define OPER_ASGN 281
#define OPER_AT 282
#define OPER_INC 283
#define OPER_DEC 284
#define LSQ_BRKT 285
#define RSQ_BRKT 286
#define LCRLY_BRKT 287
#define RCRLY_BRKT 288
#define LPAREN 289
#define RPAREN 290
#define COMMA 291
#define SEMICLN 292
#define OPER_AND 293
#define OPER_OR 294
#define OPER_NOT 295
#define ERROR 296
#define IFX 297




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 32 "src/parser.y"
{
    int value;
    struct treenode *node;
    char *strval;
}
/* Line 1529 of yacc.c.  */
#line 139 "obj/y.tab.h"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

