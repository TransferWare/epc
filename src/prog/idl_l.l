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
 * Revision 1.7  1999/11/25 09:15:18  gpaulissen
 * Release 1.5
 *
 * Revision 1.6  1999/11/23 16:05:38  gpaulissen
 * DBUG interface changed.
 *
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
#include "dbug.h"

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

static char *include_text = NULL;
static int size = 0;

static void add_include_text( char *line )
{
	include_text = realloc( include_text, size+strlen(line)+1 );

	if ( include_text )
	{
		if ( size > 0 )
			strcat( include_text, line );
		else
			strcpy( include_text, line );

		size += strlen(line)+1;
	}
}

%}

%s NORMAL COMMENT INCLUDE

start_comment   "/*"
end_comment     "*/"
start_include   "%{"
end_include     "%}"

%%

<COMMENT>{end_comment}                  { DBUG_PRINT((__LINE__, "info", "<COMMENT>{end_comment} '%s'", yytext)); 
                                          BEGIN(NORMAL); }
<COMMENT>.                              { DBUG_PRINT((__LINE__, "info", "<COMMENT>. '%s'", yytext)); }
<COMMENT>\n                             { DBUG_PRINT((__LINE__, "info", "<COMMENT>\\n '%s'", yytext)); lineno++; }

<NORMAL>{start_comment}                 { DBUG_PRINT((__LINE__, "info", "<NORMAL>{start_comment} '%s'", yytext));
                                          BEGIN(COMMENT); }

<INCLUDE>^{end_include}.*\n             { DBUG_PRINT((__LINE__, "info", "<INCLUDE>^{end_include}.*\\n '%s'", yytext));
                                          lineno++;
                                          BEGIN(NORMAL); }
<INCLUDE>.*\n                           { add_include_text( yytext ); 
                                          DBUG_PRINT((__LINE__, "info", "<INCLUDE>.* '%s'", yytext));
                                          lineno++; }
<NORMAL>^{start_include}.*\n            { DBUG_PRINT((__LINE__, "info", "<NORMAL>^{start_include}.* '%s'", yytext));
                                          lineno++;
                                          BEGIN(INCLUDE); }

<NORMAL>"interface"			{ DBUG_PRINT((__LINE__, "info", "<NORMAL>\"interface\" '%s'", yytext));
                                          return INTERFACE; }
<NORMAL>"[in]"				{ DBUG_PRINT((__LINE__, "info", "<NORMAL>\"[in]\" '%s'", yytext));
                                          yylval.ival = C_IN; return IN; }
<NORMAL>"[out]"				{ DBUG_PRINT((__LINE__, "info", "<NORMAL>\"[out]\" '%s'", yytext));
                                          yylval.ival = C_OUT; return OUT; }
<NORMAL>"[inout]"			{ DBUG_PRINT((__LINE__, "info", "<NORMAL>\"[inout]\" '%s'", yytext));
                                          yylval.ival = C_INOUT; return INOUT; }

<NORMAL>"char"[ \t]*"*"			{ DBUG_PRINT((__LINE__, "info", "<NORMAL>\"char\"[ \\t]*\"*\" '%s'", yytext));
                                          yylval.ival = C_STRING; return STRING; }
<NORMAL>"int"				{ DBUG_PRINT((__LINE__, "info", "<NORMAL>\"int\" '%s'", yytext));
                                          yylval.ival = C_INT; return INT; }
<NORMAL>"long"				{ DBUG_PRINT((__LINE__, "info", "<NORMAL>\"long\" '%s'", yytext));
                                          yylval.ival = C_LONG; return LONG; }
<NORMAL>"float"				{ DBUG_PRINT((__LINE__, "info", "<NORMAL>\"float\" '%s'", yytext));
                                          yylval.ival = C_FLOAT; return FLOAT; }
<NORMAL>"double"			{ DBUG_PRINT((__LINE__, "info", "<NORMAL>\"double\" '%s'", yytext));
                                          yylval.ival = C_DOUBLE; return DOUBLE; }
<NORMAL>"void"				{ DBUG_PRINT((__LINE__, "info", "<NORMAL>\"void\" '%s'", yytext));
                                          yylval.ival = C_VOID; return VOID; }

<NORMAL>[A-Za-z][A-Za-z0-9_]*		{ DBUG_PRINT((__LINE__, "info", "<NORMAL>[A-Za-z][A-Za-z0-9_]* '%s'", yytext));
                                          yylval.sval = STRDUP((const char*)yytext); return NAME; }

<NORMAL>[{}(),;]			{ DBUG_PRINT((__LINE__, "info", "<NORMAL>[{}(),;] '%s'", yytext));
                                          return yytext[0]; }

<NORMAL>[ \t]+			        { DBUG_PRINT((__LINE__, "info", "<NORMAL>[ \\t]+ '%s'", yytext)); }

<NORMAL>\n			        { DBUG_PRINT((__LINE__, "info", "<NORMAL>\\n")); lineno++; }

<NORMAL>.				yyerror("invalid character");

%%

void yyerror( char *s )
{
  printf("%s; line %d: text: '%s'\n", s, lineno, yytext );
}

int main ( int argc, char **argv )
{
  char *file = NULL;
  char *dbug_options = "";
  int nr;

  for ( nr = 1; nr < argc; nr++ )
    {
      switch( argv[nr][0] )
	{
	case '-':
	  switch( argv[nr][1] )
	    {
	    case '#':
	      dbug_options = &argv[nr][2];
	      break;
	    }
	  break;
	default:
	  file = argv[nr];
	  break;
	}
    }

  if ( file == NULL ) {
    fprintf( stderr, "Syntax: %s [ -#<dbug options> ] <input file>\n", argv[0] );
    exit (-1);
  }

  if ( (yyin = fopen( file, "r" )) == NULL ) {
    perror(file);
    exit (-1);
  }

  DBUG_INIT( dbug_options, NULL );
  {
    DBUG_ENTER( "main" );
    BEGIN(NORMAL);
  
    if ( !yyparse () ) {
      generate_plsql();
      generate_c( include_text );
      printf( "Code generated...\n" );
    }
    else {
      printf( "IDL parse failed\n" );
    }
    DBUG_LEAVE();
  }
  DBUG_DONE();

  return 0;
}
