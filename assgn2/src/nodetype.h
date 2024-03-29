// nodeTypes are non-terminal AST tree nodes //

enum nodeTypes {PROGRAM, DECLLIST, DECL, VARDECL, TYPESPEC, FUNDECL,
                FORMALDECLLIST, FORMALDECL, FUNBODY, LOCALDECLLIST,
                STATEMENTLIST, STATEMENT, COMPOUNDSTMT, ASSIGNSTMT,
                CONDSTMT, LOOPSTMT, RETURNSTMT, EXPRESSION, RELOP,
                ADDEXPR, ADDOP, TERM, MULOP, FACTOR, FUNCCALLEXPR,
                ARGLIST, INTEGER, IDENTIFIER, VAR, ARRAYDECL,
                FUNCTYPENAME, FUNCLIST, IF, ELSE, WHILE, RETURN, CHAR, STRING, ASSIGNOP, LOGICALOP,
            	UNARYSTMT, UNARYOP};