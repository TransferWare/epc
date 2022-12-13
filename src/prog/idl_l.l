%{
/*
 * Filename   		: idl_l.l
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
 * Revision 1.22  2004/12/28 12:18:10  gpaulissen
 * Test on Amazon
 *
 * Revision 1.21  2004/12/20 13:29:16  gpaulissen
 * make lint
 *
 * Revision 1.20  2004/12/16 16:03:08  gpaulissen
 * Web services added
 *
 * Revision 1.19  2004/10/20 13:34:05  gpaulissen
 * make lint
 *
 * Revision 1.18  2004/04/06 11:19:09  gpaulissen
 * \r added as character to ignore
 *
 * Revision 1.17  2004/02/22 17:14:14  gpaulissen
 * bug id 891761
 *
 * Revision 1.16  2002/10/31 22:21:20  gpaulissen
 * Release 3.0.0a
 *
 * Revision 1.15  2002/10/28 14:53:04  gpaulissen
 * Using GNU standards.
 *
 * Revision 1.14  2001/01/24 16:29:09  gpaulissen
 * Release 2.0.0
 *
 * Revision 1.12  2000/08/14 13:47:40  gpaulissen
 * * Added -v command line option for version info.
 * * Updated build support.
 *
 * Revision 1.11  2000/08/14 12:56:36  gpaulissen
 * Added version.
 *
 * Revision 1.10  2000/01/13 16:23:24  gpaulissen
 * Version 1.6.0
 *
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

#if HAVE_CONFIG_H
#include "config.h"
#endif

#if HAVE_STDIO_H
#include <stdio.h>
#endif

#if HAVE_STDLIB_H
#include <stdlib.h>
#endif

#if HAVE_STRING_H
#include <string.h>
#endif

#if HAVE_MALLOC_MALLOC_H
#include <malloc/malloc.h>
#endif

#include "idl_y.h"
#include "idl_defs.h"
#include "idl_prs.h"
#include "dbug.h"

#include "replace.h"

int lineno = 1;

void yyerror( char *s );
int yyparse( void );

#ifndef YY_PROTO
#ifdef __STDC__
#define YY_PROTO(proto) proto
#else
#define YY_PROTO(proto) (void)
#endif
#endif 

/*@unused@*/
static void yyunput YY_PROTO(( int c, char *buf_ptr ));

/* assume strdup() is NOT there */
#ifndef HAVE_STRDUP
#define HAVESTRDUP 0
#endif

#if !HAVE_STRDUP

static
char *
strdup( const char * str )
{
  char *tmp;

  if ( str == NULL )
    return NULL;

  tmp = (char *) malloc( strlen(str)+1 );

  if ( tmp != NULL )
    {
      (void) strcpy( tmp, str );
    }
  return tmp;
}

#endif


/* assume strstr() is there */
#ifndef HAVE_STRSTR
#define HAVE_STRSTR 1
#endif

#if !HAVE_STRSTR

/* find first occurence of s2 in s1 */
static
char *
strstr( const char *s1, const char *s2 )
{
  char *s1_ptr = (char *)s1, *s2_ptr = (char *)s2;

  if ( s1_ptr == NULL || s2_ptr == NULL )
    return NULL;

  while ( *s1_ptr != '\0' && *s2_ptr != '\0' ) 
    {
      if ( *s1_ptr == *s2_ptr ) 
        {
          s1_ptr++;
          s2_ptr++;
        } 
      else 
        {
          s1_ptr++; /* advance */
          s2_ptr = (char *)s2; /* back to start */
        }
    }

  if ( *s2_ptr == '\0' ) /* s2 completey within s1 */
    return s1_ptr;

  return NULL;
}

#endif

/*@null@*/ /*@only@*/ static char *include_text = NULL;
static int size = 0;

static
void
add_include_text( char *line )
{
  include_text = (char*) realloc( include_text, size+strlen(line)+1 );

  if ( include_text != NULL )
    {
      if ( size > 0 )
	(void) strcat( include_text, line );
      else
	(void) strcpy( include_text, line );

      size += strlen(line)+1;
    }
}

/*@-nullassign@*/
/*@-predboolint@*/
/*@-boolops@*/
/*@-unrecog@*/
/*@-onlytrans@*/
/*@-type@*/
/*@-unreachable@*/
/*@-mustfreeonly@*/
/*@-unqualifiedtrans@*/
/*@-branchstate@*/
/*@-nullderef@*/
/*@-predboolothers@*/
/*@-nullstate@*/
/*@-statictrans@*/
/*@-globstate@*/
/*@-casebreak@*/
/*@-nullret@*/
/*@-warnposixheaders@*/
/*@-nullassign@*/
/*@-compmempass@*/
/*@-nullpass@*/
/*@-mustfreefresh@*/
/*@-compdef@*/
/*@-observertrans@*/
/*@-dependenttrans@*/

%}

%s NORMAL COMMENT INCLUDE

start_comment   "/*"
end_comment     "*/"
start_include   "%{"
end_include     "%}"

string          "char"[ \t]*"*"|"string"

%%

<COMMENT>{end_comment}                  { DBUG_PRINT( "info", ("<COMMENT>{end_comment} '%s'", yytext)); 
                                          BEGIN(NORMAL); }
<COMMENT>.                              { DBUG_PRINT( "info", ("<COMMENT>. '%s'", yytext)); }
<COMMENT>\n                             { DBUG_PRINT( "info", ("<COMMENT>\\n '%s'", yytext)); lineno++; }

<NORMAL>{start_comment}                 { DBUG_PRINT( "info", ("<NORMAL>{start_comment} '%s'", yytext));
                                          BEGIN(COMMENT); }

<INCLUDE>^{end_include}.*\n             { DBUG_PRINT( "info", ("<INCLUDE>^{end_include}.*\\n '%s'", yytext));
                                          lineno++;
                                          BEGIN(NORMAL); }
<INCLUDE>.*\n                           { add_include_text( yytext ); 
                                          DBUG_PRINT( "info", ("<INCLUDE>.* '%s'", yytext));
                                          lineno++; }
<NORMAL>^{start_include}.*\n            { DBUG_PRINT( "info", ("<NORMAL>^{start_include}.* '%s'", yytext));
                                          lineno++;
                                          BEGIN(INCLUDE); }

<NORMAL>"interface"			{ DBUG_PRINT( "info", ("<NORMAL>\"interface\" '%s'", yytext));
                                          return INTERFACE; }
<NORMAL>"[in]"				{ DBUG_PRINT( "info", ("<NORMAL>\"[in]\" '%s'", yytext));
                                          yylval.mval = C_IN; return IN; }
<NORMAL>"[out]"				{ DBUG_PRINT( "info", ("<NORMAL>\"[out]\" '%s'", yytext));
                                          yylval.mval = C_OUT; return OUT; }
<NORMAL>"[inout]"			{ DBUG_PRINT( "info", ("<NORMAL>\"[inout]\" '%s'", yytext));
                                          yylval.mval = C_INOUT; return INOUT; }

<NORMAL>"xml"				{ DBUG_PRINT( "info", ("<NORMAL>\"xml\" '%s'", yytext));
                                          yylval.tval.datatype = C_XML; yylval.tval.size = MAX_STR_VAL_LEN;
                                          return XML; }
<NORMAL>{string}			{ DBUG_PRINT( "info", ("<NORMAL>{string} '%s'", yytext));
                                          yylval.tval.datatype = C_STRING; yylval.tval.size = MAX_STR_VAL_LEN;
                                          return STRING; }
<NORMAL>"char["[0-9]+"]"		{ DBUG_PRINT( "info", ("<NORMAL>\"char[\"[0-9]+\"]\" '%s'", yytext));
                                          yylval.tval.datatype = C_STRING; yylval.tval.size = atoi(yytext+5); 
                                          return STRING; }
<NORMAL>"int"				{ DBUG_PRINT( "info", ("<NORMAL>\"int\" '%s'", yytext));
                                          yylval.tval.datatype = C_INT; return INT; }
<NORMAL>"long"				{ DBUG_PRINT( "info", ("<NORMAL>\"long\" '%s'", yytext));
                                          yylval.tval.datatype = C_LONG; return LONG; }
<NORMAL>"float"				{ DBUG_PRINT( "info", ("<NORMAL>\"float\" '%s'", yytext));
                                          yylval.tval.datatype = C_FLOAT; return FLOAT; }
<NORMAL>"double"			{ DBUG_PRINT( "info", ("<NORMAL>\"double\" '%s'", yytext));
                                          yylval.tval.datatype = C_DOUBLE; return DOUBLE; }
<NORMAL>"date"				{ DBUG_PRINT( "info", ("<NORMAL>\"date\" '%s'", yytext));
                                          yylval.tval.datatype = C_DATE; return DATE; }
<NORMAL>"void"				{ DBUG_PRINT( "info", ("<NORMAL>\"void\" '%s'", yytext));
                                          yylval.tval.datatype = C_VOID; return VOID; }
<NORMAL>"oneway"			{ DBUG_PRINT( "info", ("<NORMAL>\"oneway\" '%s'", yytext));
                                          return ONEWAY; }

<NORMAL>[A-Za-z][A-Za-z0-9_]*		{ DBUG_PRINT( "info", ("<NORMAL>[A-Za-z][A-Za-z0-9_]* '%s'", yytext));
                                          yylval.sval = strdup((const char*)yytext); return NAME; }

<NORMAL>[{}(),;]			{ DBUG_PRINT( "info", ("<NORMAL>[{}(),;] '%s'", yytext));
                                          return yytext[0]; }

<NORMAL>[ \t]+			        { DBUG_PRINT( "info", ("<NORMAL>[ \\t]+ '%s'", yytext)); }

<NORMAL>\n			        { DBUG_PRINT( "info", ("<NORMAL>\\n")); lineno++; }

<NORMAL>\r				;

<NORMAL>.				yyerror("invalid character");

%%

void
yyerror( char *s )
{
  printf("%s; line %d: text: '%s'\n", s, lineno, yytext );
}

static
char *
version( void )
{
  return PACKAGE_VERSION;
}

static
void
usage( int argc, char **argv )
{
  (void) fprintf( stdout, "\
Syntax: %s [ -c ] [ -D DBUG_OPTIONS ] [ -h ] [ -p ] [ -v ] <input file>\n\
\n\
OPTIONS\n\
  -c    Only output C code.\n\
  -D    Set DBUG_OPTIONS. See the DBUG package.\n\
  -h    This message.\n\
  -p    Only output PL/SQL code.\n\
  -v    Print the version.\n",
	   argv[0] );
}

int
main( int argc, char **argv )
{
  char *file = NULL;
  char *dbug_options = "";
  int nr;
  int only_c = 0;
  int only_plsql = 0;

  DBUG_INIT( dbug_options, NULL );

  for ( nr = 1; nr < argc; nr++ )
    {
      switch( argv[nr][0] )
        {
        case '-':
          switch( argv[nr][1] )
            {
            case 'c':
              only_c = 1;
              break;
              
            case 'D':
            case '#': /* for compability reasons */
              if ( argv[nr][2] != '\0' )
                dbug_options = &argv[nr][2];
              else
                dbug_options = &argv[++nr][0];
              DBUG_DONE();
              DBUG_INIT( dbug_options, NULL );
              break;

            case 'h':
              usage( argc, argv );
              exit( EXIT_SUCCESS );
              break;

            case 'p':
              only_plsql = 1;
              break;
              
            case 'v':
              fprintf( stdout, "EPC idl version: %s\n", version() );
              exit( EXIT_SUCCESS );
              break;
            }
          break;

        default:
          file = argv[nr];
          break;
        }
    }

  if ( file == NULL ) 
    {
      usage( argc, argv );
      exit( EXIT_FAILURE );
    }

  if ( (yyin = fopen( file, "r" )) == NULL ) 
    {
      perror(file);
      exit( EXIT_FAILURE );
    }
  
  /* a new block for DBUG_ENTER */
  {
    DBUG_ENTER( "main" );
    BEGIN(NORMAL);
  
    if ( !yyparse () ) 
      {
        switch( only_c * 2 + only_plsql )
          {
          case 3: /* only_c and only_plsql both true: conflicting things */
            (void) printf( "Only C code and only PL/SQL code is conflicting, hence no code will be generated...\n" );
            break;
            
          case 2: /* only C */
            generate_c( include_text );
            (void) printf( "Only C code has been generated...\n" );
            break;
            
          case 1: /* only PL/SQL */
            generate_plsql();
            (void) printf( "Only PL/SQL code has been generated...\n" );
            break;

          case 0:
            generate_plsql();
            generate_c( include_text );
            (void) printf( "Both C and PL/SQL code have been generated...\n" );
            break;            
          }
      }
    else 
      {
        (void) printf( "IDL parse failed\n" );
      }
    DBUG_LEAVE();
  }
  DBUG_DONE();

  return 0;
}


