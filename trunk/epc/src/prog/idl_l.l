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
 * Revision 1.2  1998/02/03 10:06:21  gpauliss
 * - Added strdup because it is not an ANSI C function.
 * - String parameter (char *) may contain whitespace instead of only one space.
 *
 * Revision 1.1  1998/01/25 15:20:19  gpauliss
 * Initial revision
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

static char *strdup( const char * str )
{
	char *tmp = malloc( strlen(str)+1 );
	if ( tmp )
	{
		strcpy( tmp, str );
	}
	return tmp;
}

%}

%%

"interface"				{ return INTERFACE; }
"[in]"					{ yylval.ival = C_IN; return IN; }
"[out]"					{ yylval.ival = C_OUT; return OUT; }
"[inout]"				{ yylval.ival = C_INOUT; return INOUT; }

"char"[ \t]*"*"			{ yylval.ival = C_STRING; return STRING; }
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
	printf("%d: %s at '%s', keyword: %d\n", lineno, s, yytext, yylval.ival );
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

