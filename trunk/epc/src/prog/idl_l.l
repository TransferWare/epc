%{
/*
 * Filename   		: $Source$
 *
 * Creation date  : 26-JUN-1997
 *
 * Created by 		: Huub van der Wouden
 *
 * Company    		: Transfer Solutions bv
 *
 * --- Description -------------------------------------------------------
 * Specification of lexer tokens
 *
 * --- Revision History --------------------------------------------------
 * $Log$
 * Revision 1.1  1998/01/25 15:20:19  gpauliss
 * Initial revision
 *
 * Revision 1.3  1997/07/08 14:14:05  hgwouden
 * added DOUBLE datatype
 *
 * Revision 1.2  1997/07/02 14:46:14  hgwouden
 * *** empty log message ***
 *
 * Revision 1.1  1997/06/25 12:22:27  hgwouden
 * Initial revision
 *
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "y.tab.h"
#include "idl_const.h"
#include "idl_parse.h"

int lineno = 1;

void yyerror( char *s );
int yyparse( void );

%}

%%

"interface"				{ return INTERFACE; }
"[in]"					{ yylval.ival = C_IN; return IN; }
"[out]"					{ yylval.ival = C_OUT; return OUT; }
"[inout]"				{ yylval.ival = C_INOUT; return INOUT; }

"char *"				{ yylval.ival = C_STRING; return STRING; }
"int"					{ yylval.ival = C_INT; return INT; }
"float"					{ yylval.ival = C_FLOAT; return FLOAT; }
"double"				{ yylval.ival = C_DOUBLE; return DOUBLE; }
"void"					{ yylval.ival = C_VOID; return VOID; }

[A-Za-z][A-Za-z0-9_]*	{ yylval.sval = strdup(yytext); return NAME; }

[{}(),;]				{ return yytext[0]; }

[ \t\r\n]+				;

.						yyerror("invalid character");

%%

void yyerror( char *s )
{
	printf("%d: %s at %s\n", lineno, s, yytext );
}

int main ( int argc, char **argv )
{
	if ( argc > 1  && (yyin = fopen( argv[1], "r" )) == NULL ) {
		perror(argv[1]);
		exit(1);
	}

	if ( !yyparse () ) {
		generate_plsql ();
		generate_c ();
		printf( "Code generated...\n" );
	}
	else {
		printf( "IDL parse failed\n" );
	}
	return 0;
}

