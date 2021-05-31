/* major stuff */
#define ID         251
#define INTCONST   252
#define CHARCONST  253
#define STRCONST   254

/* keywords */
#define KWD_IF     255
#define KWD_ELSE   256
#define KWD_WHILE  257
#define KWD_INT    258
#define KWD_STRING 259
#define KWD_CHAR   260
#define KWD_RETURN 261
#define KWD_VOID   262

/* operators */
#define OPER_ADD    263
#define OPER_SUB    264
#define OPER_MUL    265
#define OPER_DIV    266
#define OPER_LT     267
#define OPER_GT     268
#define OPER_GTE    269
#define OPER_LTE    270
#define OPER_EQ     271
#define OPER_NEQ    272
#define OPER_ASGN    273

#define OPER_AT		    282
#define OPER_MOD		283
#define OPER_INC	 	284
#define OPER_DEC	 	285
#define OPER_AND 		286
#define OPER_OR 		287
#define OPER_NOT 		288

/* brackets & parens */
#define LSQ_BRKT    274
#define RSQ_BRKT    275
#define LCRLY_BRKT  276
#define RCRLY_BRKT  277
#define LPAREN     278
#define RPAREN     279

/* punctuation */
#define COMMA      280
#define SEMICLN    281

/* error */
#define ERROR 					200
#define ILLEGAL_TOKEN_ERROR		"Illegal token"
#define UNTERMINATED_STR 		"Unterminated string"
#define UNRECOG_ESC_CHAR_STR	"Unrecognized escape character in String"
#define MULTILINE_STR			"String spans multiple lines"
#define UNTERMINATED_CMMNT 		"Unterminated comment"


