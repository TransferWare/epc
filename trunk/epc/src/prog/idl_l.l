%{
/*
 * Filename   		: $RCSfile$
 *
 * Creation date	: 26-JUN-1997
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
 * Revision 1.5  1998/07/27 15:21:09  gert-jan
 * First release.
 *
 * Revision 1.4  1998/05/06 20:24:02  gpauliss
 * Added support for longs
 *
 * Revision 1.3  1998/02/19 16:42:30  gpauliss
 * Using dos filename conventions (8.3)
 *
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
#include "idl_y.h"
#include "idl_defs.h"
#include "idl_prs.h"

int lineno = 1;

void yyerror( char *s );
int yyparse( void );

static char *STRDUP( const char * str )
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

"char"[ \t]*"*"				{ yylval.ival = C_STRING; return STRING; }
"int"					{ yylval.ival = C_INT; return INT; }
"long"					{ yylval.ival = C_LONG; return LONG; }
"float"					{ yylval.ival = C_FLOAT; return FLOAT; }
"double"				{ yylval.ival = C_DOUBLE; return DOUBLE; }
"void"					{ yylval.ival = C_VOID; return VOID; }

[A-Za-z][A-Za-z0-9_]*			{ yylval.sval = STRDUP((const char*)yytext); return NAME; }

[{}(),;]				{ return yytext[0]; }

[ \t\r\n]+				;

.					yyerror("invalid character");

%%

void yyerror( char *s )
{
	printf("%d: %s at '%s', keyword: %d\n", lineno, s, yytext, yylval.ival );
}

int main ( int argc, char **argv )
{
	if ( argc <= 1 ) {
		fprintf( stderr, "Syntax: %s <input file>\n", argv[0] );
		exit (-1);
	}
	if ( (yyin = fopen( argv[1], "r" )) == NULL ) {
		perror(argv[1]);
		exit (-1);
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

