/*
 * Filename   		: $RCSfile$
 *
 * Creation date	: 25-JUN-1997
 *
 * Created by 		: Huub van der Wouden
 *
 * Company    		: Transfer Solutions bv
 *
 * Notes		: For strings PRO*C STRING constructs are used.
 *
 * --- Description -------------------------------------------------------
 * Parsing and code generation routines
 *
 * --- Revision History --------------------------------------------------
 * $Log$
 * Revision 1.19  2001/09/18 14:34:13  gpaulissen
 * Oracle 8 external routines added.
 *
 * Revision 1.18  2001/01/24 16:29:09  gpaulissen
 * Release 2.0.0
 *
 * Revision 1.17  2000/01/13 16:23:25  gpaulissen
 * Version 1.6.0
 *
 * Revision 1.14  1999/11/25 09:15:18  gpaulissen
 * Release 1.5
 *
 * Revision 1.13  1999/11/23 16:05:38  gpaulissen
 * DBUG interface changed.
 *
 * Revision 1.12  1999/10/20 10:58:32  gpaulissen
 * epc.request_set_parameter and epc.request_get_parameter instead of
 * dbms_pipe.pack_message and dbms_pipe.unpack_message
 *
 * Revision 1.11  1999/08/26 12:35:04  gpaulissen
 * Added DBUG info
 *
 * Revision 1.10  1998/08/02 15:17:40  gjp
 * Removed obsolete local variable fun in generate_header.
 *
 * Revision 1.9  1998/08/02 13:58:26  gjp
 * Moved declarations of external routines to the interface header file. This reduces the chance of conflicts.
 *
 * Revision 1.8  1998/07/31 16:25:32  gert-jan
 * Various changes.
 *
 * Revision 1.7  1998/07/27  15:21:09  gert-jan
 * First release.
 *
 * Revision 1.6  1998/05/26 14:01:51  gpauliss
 * epc_ifc.h not needed
 *
 * Revision 1.5  1998/05/06 20:23:42  gpauliss
 * Added support for longs
 *
 * Revision 1.4  1998/02/19 16:41:09  gpauliss
 * Using dos filename conventions (8.3)
 *
 * Revision 1.3  1998/02/17 20:25:33  gpauliss
 * Interface changed
 *
 * Revision 1.2  1998/02/03 09:51:26  gpauliss
 * - Changed old K&R syntax to ANSI C syntax.
 * - Added extern declaration of IDL function to be called.
 *
 * Revision 1.1  1998/01/25 15:20:21  gpauliss
 * Initial revision
 *
 */

#include <malloc.h> 
#include <stdio.h> 
#include <string.h> 
#include <stdlib.h>
#include <assert.h>
#include "idl_prs.h"
#include "idl_defs.h"
#include "idl_kwrd.h"

#include "dbug.h"

/*
|| Defines
*/
#define EXEC_SQL_BEGIN_DECLARE_SECTION "EXEC SQL BEGIN DECLARE SECTION"
#define EXEC_SQL_END_DECLARE_SECTION "EXEC SQL END DECLARE SECTION"
#define EPC_PREFIX "_epc_"

/*
|| A special epc interface header is not needed, since the interface C file
|| defines the num_functions and functions variables.
||
|| #define GEN_EPC_IFC_H 
||
*/

/*
|| Forward declaration of static procedures
*/

static void init_parameter( idl_parameter_t *parm, char *name, idl_mode_t mode, idl_type_t datatype, dword_t size );
static mapping *get_mapping( idl_type_t type, idl_lang_t language );
static char *get_syntax( idl_type_t type, idl_lang_t language );
static char *get_constant_name( idl_type_t type, idl_lang_t language );
static void print_formal_parameter( FILE *pout, idl_parameter_t *parm, idl_lang_t lang );
static void print_actual_parameter( FILE *pout, idl_parameter_t *parm, idl_lang_t lang );
static void print_variable_definition( FILE *pout, idl_parameter_t *parm, idl_lang_t lang );
static void generate_plsql_parameters( FILE * pout, idl_function_t * fun );
static void generate_plsql_function( FILE * pout, idl_function_t * fun );
static void generate_plsql_function_declaration( FILE * pout, idl_function_t * fun );
static void generate_plsql_header( FILE * pout );
static void generate_plsql_function_body( FILE * pout, idl_function_t * fun );
static void generate_plsql_function_body_ext( FILE * pout, idl_function_t * fun );
static void generate_plsql_body( FILE * pout );
static void generate_plsql_body_ext( FILE * pout );
static void generate_c_parameters( FILE * pout, idl_function_t * fun );
static void print_c_debug_info( FILE *pout, char *name, idl_type_t datatype );
static void generate_c_debug_info( FILE * pout, idl_function_t * fun, idl_mode_t mode );
static void generate_c_function ( FILE * pout, idl_function_t * fun );
static void print_generate_comment( FILE * pout, char * prefix );
static void declare_external_function( FILE * pout, idl_function_t * fun );
static void declare_internal_function( FILE * pout, idl_function_t * fun );
static void generate_c_source ( FILE * pout, const char *include_text );
#ifdef GEN_EPC_IFC_H
static void generate_interface_header ( FILE * pout );
#endif
static void generate_header ( FILE *pout );



/*
|| Static global variables
*/
static idl_interface_t _interface;


/*
|| Global variables
*/
keyword keywords[] = {
  { C_VOID, 	
    {	
      { C, 	"void",		"C_VOID" },
      { PLSQL,	"",		"epc.c_void" }
    }
  },
  { C_INT,	
    {	
      { C, 	"int",		"C_INT" },
      /*      { PLSQL,	"INTEGER",	"epc.c_int" }*/
      { PLSQL,	"BINARY_INTEGER",	"epc.c_int" }
    }
  },
  { C_LONG,	
    {	
      { C, 	"long",		"C_LONG" },
      /*      { PLSQL,	"INTEGER",	"epc.c_long" }*/
      { PLSQL,	"BINARY_INTEGER",	"epc.c_long" }
    }
  },
  { C_FLOAT,	
    {	
      { C,	"float",	"C_FLOAT" },
      /*      { PLSQL,	"NUMBER",	"epc.c_float" }*/
      { PLSQL,	"FLOAT",	"epc.c_float" }
    }
  },
  { C_DOUBLE,	
    {	
      { C,	"double",	"C_DOUBLE" },
      /*      { PLSQL,	"NUMBER",	"epc.c_double" }*/
      { PLSQL,	"DOUBLE PRECISION",	"epc.c_double" }
    }
  },
  { C_STRING,	
    {
      /* string is a typedef */
      { C,	"string",	"C_STRING" },
      { PLSQL,	"VARCHAR2",	"epc.c_string" }
    }
  },
  { C_IN,
    {
      { C,	"",		"C_IN" },
      { PLSQL,	"IN",		"epc.c_in" }
    }
  },
  { C_OUT,
    {
      { C,	"*",		"C_OUT" },
      { PLSQL,	"OUT",		"epc.c_out" } 
    }
  },
  { C_INOUT,
    {
      { C,	"*",		"C_INOUT" },
      { PLSQL,	"IN OUT",	"epc.c_inout" }
    }
  }
};

int print_external_function = 0; /* print extern <function> in header */

/*
|| Global functions
*/

void set_interface ( char *name )
{
  strcpy( _interface.name, name );
  _interface.num_functions = 0;
}

static void init_parameter( idl_parameter_t *parm, char *name, idl_mode_t mode, idl_type_t datatype, dword_t size )
{
  strcpy( parm->name, name );
  strcpy( parm->proc_name, name );

  parm->mode = mode;
  switch( mode )
    {
    case C_IN:
    case C_INOUT:
    case C_OUT:
      break;
      
    default:
      fprintf( stderr, "(add_parameter) Mode %ld of parameter %s unknown.\n", 
	       (long)mode, name );
      exit(-1);
    }

  parm->datatype = datatype;
  switch( datatype )
    {
    case C_STRING:
      /*		strcat( parm->proc_name, "_vc" );*/ /* GJP 8-1-2000 VARCHAR is not used anymore */
      parm->size = size;
      break;

    case C_VOID:
    case C_INT:
    case C_LONG:
    case C_DOUBLE:
    case C_FLOAT:
      parm->size = 0;
      break;

    default:
      fprintf( stderr, "(add_parameter) Type %ld of parameter %s unknown.\n", 
	       (long)datatype, name );
      exit(-1);
    }
}

void add_function ( char *name, idl_type_t datatype, const int oneway )
{
  idl_function_t * fun = (idl_function_t*)malloc(sizeof(idl_function_t));

  strcpy( fun->name, name );
  fun->oneway = oneway;
  fun->num_parameters = 0;

  init_parameter( &fun->return_value, "result", C_OUT, datatype, MAX_STR_VAL_LEN );

  _interface.functions[_interface.num_functions] = fun;
  _interface.num_functions++;
}

void add_parameter ( char *name, idl_mode_t mode, idl_type_t datatype, dword_t size )
{
  idl_function_t * fun = _interface.functions[_interface.num_functions-1];
  idl_parameter_t * parm = (idl_parameter_t*)malloc(sizeof(idl_parameter_t));

  init_parameter( parm, name, mode, datatype, size );

  fun->parameters[fun->num_parameters] = parm;
  fun->num_parameters++;
}

static mapping *get_mapping( idl_type_t type, idl_lang_t language )
{
  int i, j;
  int num_keywords = sizeof(keywords) / sizeof(keyword);

  for ( i=0; i<num_keywords; i++ ) {
    if ( keywords[i].key == type ) {
      for ( j=0; j<NUM_LANGUAGES; j++ ) {
	if ( keywords[i].mappings[j].language == language ) {
	  return &keywords[i].mappings[j];
	}
      }
      printf( "No mapping for %ld in language %ld\n", type, language );
      exit(1);
    }
  }
  printf( "Type %ld not a valid keyword\n", (long)type );
  exit(1);
  return NULL;
}

static char *get_syntax( idl_type_t type, idl_lang_t language )
{
  return	get_mapping( type, language )->syntax;
}

static char *get_constant_name( idl_type_t type, idl_lang_t language )
{
  return	get_mapping( type, language )->constant_name;
}

static void print_formal_parameter( FILE *pout, idl_parameter_t *parm, idl_lang_t lang )
{
  switch( lang )
    {
    case C:
      fprintf( pout, "%s %s%s",
	       get_syntax( parm->datatype, lang ),
	       ( parm->mode != C_IN && parm->datatype != C_STRING ? "*" : "" ),
	       parm->name );
      break;

    case PLSQL:
      fprintf( pout, "%s %s %s", 
	       parm->name, 
	       get_syntax( parm->mode, lang ),
	       get_syntax( parm->datatype, lang ) );
      break;
    }
}


static void print_actual_parameter( FILE *pout, idl_parameter_t *parm, idl_lang_t lang )
{
  switch( lang )
    {
    case C:
      fprintf( pout, "%s%s",
	       ( parm->mode != C_IN && parm->datatype != C_STRING ? "&" : "" ),
	       parm->name );
      break;
    case PLSQL:
      fprintf( pout, "%s", 
	       parm->name );
      break;
    }
}


static void print_variable_definition( FILE *pout, idl_parameter_t *parm, idl_lang_t lang )
{
  switch( lang )
    {
    case C:
      switch( parm->datatype )
	{
	case C_INT:
	case C_LONG:
	case C_FLOAT:
	case C_DOUBLE:
	  fprintf( pout, "%s %s = 0", get_syntax( parm->datatype, C ), parm->proc_name );
	  break;

	case C_STRING: /* Use the STRING PRO*C construct */
	  fprintf( pout, "char %s[%d] = \"\";\n\tEXEC SQL VAR %s IS STRING(%d)", 
		   parm->proc_name, parm->size+1, parm->proc_name, parm->size+1 );
	  break;
	}
      break;

    case PLSQL:
      switch( parm->datatype )
	{
	case C_INT:
	case C_LONG:
	case C_FLOAT:
	case C_DOUBLE:
	  fprintf( pout, "%s %s", 
		   parm->name, 
		   get_syntax( parm->datatype, PLSQL ) );
	  break;
	  
	case C_STRING:
	  fprintf( pout, "%s %s(%d)", 
		   parm->name, 
		   get_syntax( parm->datatype, PLSQL ), 
		   parm->size
		   );
	  break;
	}
      break;
    }
}


static void generate_plsql_parameters( FILE * pout, idl_function_t * fun )
{
  int i;
  idl_parameter_t * parm;

  fprintf( pout, " (\n" );

  for ( i=0; i<fun->num_parameters; i++) {
    parm = fun->parameters[i];

    if ( i>0 ) {
      fprintf( pout, ",\n" );
    }

    fprintf( pout, "\t\t" );
    print_formal_parameter( pout, parm, PLSQL );
  }
  fprintf( pout, "\n\t)" );
}

static void generate_plsql_function( FILE * pout, idl_function_t * fun )
{
  if ( fun->return_value.datatype != C_VOID ) {
    fprintf( pout, "\tFUNCTION %s", fun->name );
  }
  else {
    fprintf( pout, "\tPROCEDURE %s", fun->name );
  }

  /* PARAMETERS */
  if ( fun->num_parameters > 0 ) {
    generate_plsql_parameters( pout, fun );
  }

  if ( fun->return_value.datatype != C_VOID ) {
    fprintf( pout, " RETURN %s", get_syntax( fun->return_value.datatype, PLSQL ) );
  }
}

static void generate_plsql_function_declaration( FILE * pout, idl_function_t * fun )
{
  generate_plsql_function ( pout, fun );
  fprintf( pout, ";\n\n" );
}

static void generate_plsql_header( FILE * pout )
{
  int i;

  print_generate_comment( pout, "REMARK " );

  fprintf( pout, "CREATE OR REPLACE PACKAGE %s IS\n", _interface.name );
  fprintf( pout, "\n" );

  for ( i=0; i<_interface.num_functions; i++) {
    generate_plsql_function_declaration( pout, _interface.functions[i] );
  }

  fprintf( pout, "END;\n/\n\n" );
}

static void generate_plsql_function_body( FILE * pout, idl_function_t * fun )
{
  int i;
  idl_parameter_t * parm;

  DBUG_ENTER( "generate_plsql_function_body" );

  generate_plsql_function ( pout, fun );
  fprintf( pout, " IS\n" );

  /* RETURN VARIABLE */
  switch( fun->return_value.datatype )
    {
    case C_VOID:
      break;

    default:
      fprintf( pout, "\t\t" );
      print_variable_definition( pout, &fun->return_value, PLSQL );
      fprintf( pout, ";\n" );
      break;
    }

  fprintf( pout, "\tBEGIN\n" );

  /* SETUP OF CLIENT SIDE FUNCTION CALL */
  fprintf( pout, "\t\tepc.request_set_header( '%s', '%s', %d );\n", 
	   _interface.name,
	   fun->name,
	   fun->oneway );

  for ( i=0; i<fun->num_parameters; i++) {
    parm = fun->parameters[i];

    if ( parm->mode != C_OUT )
      fprintf( pout, "\t\tepc.request_set_parameter( %s );\n", parm->name );
  }

  fprintf( pout, "\t\tepc.request_perform_routine( %d );\n", fun->oneway );

  /* GET THE RESULTS */
  for ( i=0; i<fun->num_parameters; i++) {
    parm = fun->parameters[i];
    if ( parm->mode != C_IN ) {
      fprintf( pout, "\t\tdbms_pipe.unpack_message( %s );\n", parm->name );
    }
  }

  if ( fun->return_value.datatype != C_VOID ) {
    fprintf( pout, "\t\tdbms_pipe.unpack_message( %s );\n", fun->return_value.name );
    fprintf( pout, "\t\tRETURN %s;\n", fun->return_value.name );
  }

  /* EXCEPTIONS */
  fprintf( pout, "\tEXCEPTION\n" );
  fprintf( pout, "\t\tWHEN\tOTHERS\n" );
  fprintf( pout, "\t\tTHEN\tRAISE;\n" );

  fprintf( pout, "\tEND;\n\n" );

  DBUG_LEAVE();
}


static
void
generate_plsql_function_body_ext( FILE * pout, idl_function_t * fun )
{
  int nr;

  DBUG_ENTER( "generate_plsql_function_body_ext" );
  DBUG_PRINT( "input", ( "interface: %s; function: %s", _interface.name, fun->name ) );

  generate_plsql_function( pout, fun );

  (void) fprintf( pout, " AS\n\
\tEXTERNAL LIBRARY %s_lib\n\
\tNAME \"%s\"\n\
\tLANGUAGE C", _interface.name, fun->name );

  DBUG_PRINT( "info",
	      ( "return datatype: %d; # parms: %d",
		(int) fun->return_value.datatype,
		(int) fun->num_parameters ) );

  /* PARAMETERS CLAUSE ? */
  if ( fun->return_value.datatype != C_VOID ||
       fun->num_parameters > 0 )
    {
      (void) fprintf( pout, "\n\
\tPARAMETERS (\n" );

      for ( nr = 0; nr < fun->num_parameters; nr++ ) 
	{
	  (void) fprintf( pout, "\t%s\t%s %s\n",
			  (nr == 0 ? " " : "," ),
			  fun->parameters[nr]->name,
			  get_syntax( fun->parameters[nr]->datatype, C ) ) ;
	}

      /* RETURN VARIABLE */
      switch( fun->return_value.datatype )
	{
	case C_VOID:
	  break;
	  
	default:
	  (void) fprintf( pout, "\t%s\tRETURN %s\n",
			  (nr == 0 ? " " : "," ),
			  get_syntax( fun->return_value.datatype, C ) ) ;
	  break;
	}

      (void) fprintf( pout, "\
\t)" );
    }

  (void) fprintf( pout, ";\n\n" );

  DBUG_LEAVE();
}


static
void
generate_plsql_body( FILE * pout )
{
  int i;

  print_generate_comment( pout, "REMARK " );

  fprintf( pout, "CREATE OR REPLACE PACKAGE BODY %s IS\n", _interface.name );
  fprintf( pout, "\n" );

  for ( i=0; i<_interface.num_functions; i++) {
    generate_plsql_function_body( pout, _interface.functions[i] );
  }

  fprintf( pout, "END;\n" );
  fprintf( pout, "/\n" );
}

static
void
generate_plsql_body_ext( FILE * pout )
{
  int i;

  DBUG_ENTER( "generate_plsql_body_ext" );

  print_generate_comment( pout, "REMARK " );

  (void) fprintf( pout, "\
WHENEVER SQLERROR CONTINUE\n\
DROP LIBRARY %s_lib\n\
/\n\
WHENEVER SQLERROR EXIT FAILURE\n\
CREATE LIBRARY %s_lib AS '&%s_lib'\n\
/\n\
CREATE OR REPLACE PACKAGE BODY %s IS\n\n",
		  _interface.name,
		  _interface.name,
		  _interface.name,
		  _interface.name );

  for ( i=0; i<_interface.num_functions; i++) {
    generate_plsql_function_body_ext( pout, _interface.functions[i] );
  }

  fprintf( pout, "END;\n" );
  fprintf( pout, "/\n" );

  DBUG_LEAVE();
}

void
generate_plsql( void )
{
  char filename[256];
  FILE * pout[4];
  int nr;

  for ( nr = 0; nr < 4; nr++ )
    {
      switch( nr )
	{
	case 0:
	  (void) sprintf( filename, "%s.pks", _interface.name );
	  break;

	case 1:
	  (void) sprintf( filename, "%s.pkb", _interface.name );
	  break;

	case 2:
	  /* for Oracle 8 external routines */
	  (void) sprintf( filename, "%s.pke", _interface.name );
	  break;

	case 3:
	  (void) sprintf( filename, "%s.pls", _interface.name );
	  break;
	}

      if ( ( pout[nr] = fopen( filename, "w" ) ) == NULL ) 
	{
	  (void) fprintf( stderr, "cannot open file %s - exiting...\n", filename );
	  exit( EXIT_FAILURE );
	}
    }

  generate_plsql_header( pout[0] );
  generate_plsql_body( pout[1] );
  generate_plsql_body_ext( pout[2] );

  /* .pls script call .pks and .pkb */
  print_generate_comment( pout[3], "REMARK " );

  (void) fprintf( pout[3], "@@ %s.pks\n\
REMARK Package body using an EPC server.\n\
@@ %s.pkb\n\
REMARK Package body using PL/SQL external routines.\n\
REMARK Uncomment the next line when using PL/SQL external routines (Oracle8 only).\n\
REMARK @@ %s.pke\n", _interface.name, _interface.name, _interface.name );

  for ( nr = 0; nr < 4; nr++ )
    {
      (void) fclose( pout[nr] );
    }
}

static void generate_c_parameters( FILE * pout, idl_function_t * fun )
{
  int i;
  idl_parameter_t * parm;

  /* 
   * declare variables 
   */

  for ( i=0; i<fun->num_parameters; i++) {
    parm = fun->parameters[i];
    fprintf( pout, "\t" );
    print_variable_definition( pout, parm, C );
    fprintf( pout, ";\n" );
  }
}


static void print_c_debug_info( FILE *pout, char *name, idl_type_t datatype )
{
  fprintf( pout, "\tDBUG_PRINT( \"info\", ( " );
  switch( datatype )
    {
    case C_INT:
      fprintf( pout, "\"%s: %%d\", %s ) );\n", name, name ); break;
    case C_LONG:
      fprintf( pout, "\"%s: %%ld\", %s ) );\n", name, name ); break;
    case C_FLOAT:
      fprintf( pout, "\"%s: %%f\", %s ) );\n", name, name ); break;
    case C_DOUBLE:
      fprintf( pout, "\"%s: %%lf\", %s ) );\n", name, name ); break;
    case C_STRING:
      fprintf( pout, "\"%s: '%%s'\", %s ) );\n", name, name ); break;
    default:
      fprintf( stderr, "print_c_debug_info#Unknown datatype (%d) for %s\n", datatype, name ); break;
    }
}


static void generate_c_debug_info( FILE * pout, idl_function_t * fun, idl_mode_t mode )
{
  int i;
  idl_parameter_t *parm;

  for ( i=0; i<fun->num_parameters; i++) {
    parm = fun->parameters[i];
    
    if ( parm->mode == mode )
      {
	print_c_debug_info( pout, parm->name, parm->datatype );
      }
  }

  parm = &fun->return_value;
  if ( parm->mode == mode && parm->datatype != C_VOID )
    {
      print_c_debug_info( pout, parm->name, parm->datatype );
    }
}


static void generate_c_function( FILE * pout, idl_function_t * fun )
{
  int i;
  int exec_sql_printed = 0;

  fprintf( pout, "void %s%s ( epc_call_t *call )\n", EPC_PREFIX, fun->name );
  fprintf( pout, "{\n\
#ifdef SQLCA\n\
#undef SQLCA\n\
#endif\n\
EXEC SQL INCLUDE sqlca;\n\n" );

  /* 
   * VARIABLE TO HOLD RETURN VALUE 
   */

  fprintf( pout, "\t%s;\n", EXEC_SQL_BEGIN_DECLARE_SECTION );
  fprintf( pout, "\tlong epc_error = OK;\n" );
  fprintf( pout, "\tlong sqlcode = 0;\n" );
  fprintf( pout, "\tconst char *result_pipe = call->result_pipe;\n" );

  switch( fun->return_value.datatype )
    {
    case C_VOID:
      break;

    default:
      fprintf( pout, "\t" );
      print_variable_definition( pout, &fun->return_value, C );
      fprintf( pout, ";\n" );
      break;
    }

  /* PARAMETERS */
  generate_c_parameters( pout, fun );

  fprintf( pout, "\t%s;\n\n\tDBUG_ENTER(\"%s\");\n\n", EXEC_SQL_END_DECLARE_SECTION, fun->name );

  /*
   * Get the in and in/out parameters
   */
  for ( i=0; i<fun->num_parameters; i++) {
    idl_parameter_t * parm = fun->parameters[i];

    if ( parm->mode == C_IN || parm->mode == C_INOUT )
      {
	if ( !exec_sql_printed )
	  {
	    exec_sql_printed = 1;
	    fprintf( pout, "\
\tEXEC SQL WHENEVER SQLERROR CONTINUE;\n\
\tEXEC SQL EXECUTE\n\
\tBEGIN\n" );
	  }
	fprintf( pout, "\t\tepc.request_get_parameter( :%s );\n", parm->proc_name ); 
      }
  }

  if ( exec_sql_printed )
    {
      fprintf( pout, "\
\tEXCEPTION\n\
\t\tWHEN\tOTHERS\n\
\t\tTHEN\t:sqlcode := SQLCODE;\n\
\tEND;\n\
\tEND-EXEC;\n\n" );
    }

  /* ---------------------------------------------------------------
   * Print the in and in/out parameters, including Oracle error code
   * --------------------------------------------------------------- */

  generate_c_debug_info( pout, fun, C_IN );
  generate_c_debug_info( pout, fun, C_INOUT );
  print_c_debug_info( pout, "sqlcode", C_LONG );

  /* -----------------
   * HANDLE SQL ERRORS
   * ----------------- */

  fprintf( pout, "\n\
\tif ( sqlcode != 0 )\n\
\t{\n\
\t\tepc_error = RECEIVE_ERROR;\n\
\t}\n\
\telse\n\
\t{\n" );

  /* ---------------
   * THE ACTUAL CALL 
   * --------------- */
  fprintf( pout, "\t\t" );

  /* set return value if any. Use strncpy for strings */

  switch( fun->return_value.datatype )
    {
    case C_VOID:
      break;

    case C_STRING:
      fprintf( pout, "strncpy( %s, ", fun->return_value.name );
      break;

    default:
      fprintf( pout, "%s = ", fun->return_value.name );
      break;
    }

  fprintf( pout, "%s( ", fun->name );

  for ( i=0; i<fun->num_parameters; i++) {
    if ( i>0 ) {
      fprintf( pout, ", " );
    }
    print_actual_parameter( pout, fun->parameters[i], C );
  }
  fprintf( pout, " )" );

  switch( fun->return_value.datatype )
    {
    case C_VOID:
      break;

    case C_STRING:
      /* zero terminate to be sure */
      fprintf( pout, ", %d );\n\t\t%s[%d] = 0", 
	       fun->return_value.size, 
	       fun->return_value.proc_name, 
	       fun->return_value.size );
      break;

    default:
      break;
    }

  fprintf( pout, ";\n\
\t}\n\
\n" );

  /* ----------------
   * Send the results
   * ---------------- */
  if ( fun->oneway == 0 )
    {
      fprintf( pout, "\
\tEXEC SQL WHENEVER SQLERROR CONTINUE;\n\
\tEXEC SQL EXECUTE\n\
\tBEGIN\n\
\t\tdbms_pipe.reset_buffer;\n\
\t\tdbms_pipe.pack_message( :epc_error );\n\
\t\tdbms_pipe.pack_message( :sqlcode );\n\
\t\tIF ( :epc_error = 0 )\n\
\t\tTHEN\n" );

      /*
       * Set the in/out and out parameters
       */
      for ( i=0; i<fun->num_parameters; i++) {
	idl_parameter_t * parm = fun->parameters[i];
	
	if ( parm->mode == C_INOUT || parm->mode == C_OUT )
	  {
	    fprintf( pout, "\t\t\tdbms_pipe.pack_message( :%s );\n", parm->proc_name ); 
	  }
      }
      if ( fun->return_value.datatype != C_VOID )
	{
	  fprintf( pout, "\t\t\tdbms_pipe.pack_message( :%s );\n", fun->return_value.proc_name ); 
	}

      fprintf( pout, "\
\t\t\tNULL;\n\
\t\tEND IF;\n\
\t\t:sqlcode := dbms_pipe.send_message( :result_pipe, 1 );\n\
\tEXCEPTION\n\
\t\tWHEN\tOTHERS\n\
\t\tTHEN\n\
\t\t\t:sqlcode := SQLCODE;\n\
\tEND;\n\
\tEND-EXEC;\n\n\
\tswitch ( sqlcode )\n\
\t{\n\
\tcase 0:\n\
\t\tbreak;\n\
\tcase 1: /* dbms_pipe.send_message return value: timed out */\n\
\t\tepc_error = MSG_TIMED_OUT;\n\
\t\tsqlcode = 0;\n\
\t\tbreak;\n\
\tcase 3: /* dbms_pipe.send_message return value: interrupted */;\n\
\t\tepc_error = MSG_INTERRUPTED;\n\
\t\tsqlcode = 0;\n\
\t\tbreak;\n\
\tdefault: /* SQLCODE: negative */\n\
\t\tepc_error = SEND_ERROR;\n\
\t\tbreak;\n\
\t}\n" );
    }

  fprintf( pout, "\
\tcall->epc_error = epc_error;\n\
\tcall->sqlcode = sqlcode;\n" );

  /*
   * Print the in/out and out parameters, including Oracle error code
   */
  generate_c_debug_info( pout, fun, C_INOUT );
  generate_c_debug_info( pout, fun, C_OUT );
  print_c_debug_info( pout, "sqlcode", C_LONG );
  print_c_debug_info( pout, "epc_error", C_LONG );

  fprintf( pout, "\tDBUG_LEAVE();\n}\n\n" );
}

static void print_generate_comment( FILE * pout, char * prefix )
{
  fprintf( pout, "%s/*******************************\n", prefix );
  fprintf( pout, "%s ** Generated by EPC compiler **\n", prefix );
  fprintf( pout, "%s *******************************/\n\n", prefix );
}

static void declare_external_function( FILE * pout, idl_function_t * fun )
{
  int i;

  fprintf( pout, "extern %s %s( ", 
	   get_syntax( fun->return_value.datatype, C ),
	   fun->name
	   );
  if ( fun->num_parameters > 0 ) {
    for ( i=0; i<fun->num_parameters; i++) {
      idl_parameter_t * parm = fun->parameters[i];

      print_formal_parameter( pout, parm, C );

      if ( i < fun->num_parameters - 1 )
	fprintf( pout, ", " );
    }
  }
  else /* 0 parameters */
    fprintf( pout, "void" );
  fprintf( pout, " );\n" );
}

static void declare_internal_function( FILE * pout, idl_function_t * fun )
{
  fprintf( pout, "extern void %s%s( epc_call_t *call );\n", EPC_PREFIX, fun->name );
}

static void generate_c_source ( FILE * pout, const char *include_text )
{
  int i;
  idl_function_t * fun;

  print_generate_comment( pout, "" );

  if ( include_text )
    fprintf( pout, "%s\n", include_text );
  fprintf( pout, "#include <string.h>\n" );
  fprintf( pout, "#include <stdlib.h>\n" );
  fprintf( pout, "#include \"epc_defs.h\"\n" );
  fprintf( pout, "#include \"epc_dbg.h\"\n" );
  fprintf( pout, "#include \"%s.h\"\n\n", _interface.name );
  fprintf( pout, "#define SQLCA_INIT\n\n" );

  /* function array */
  fprintf( pout, "\nstatic epc_function_t functions[] = {\n" );
  for ( i=0; i<_interface.num_functions; i++) {
    fun = _interface.functions[i];
    fprintf( pout, "\t{ \"%s\", %s, %s%s }%s\n", 
	     fun->name, 
	     get_constant_name( fun->return_value.datatype, C ), 
	     EPC_PREFIX,
	     fun->name,
	     ( i < _interface.num_functions-1 ? "," : "" ) 
	     );
  }
  fprintf( pout, "};\n" );

  /* interface */
  fprintf( pout, "\nepc_interface_t ifc_%s = {\n", _interface.name );
  fprintf( pout, "\t\"%s\",\n", _interface.name );
  fprintf( pout, "\t%ld,\n", _interface.num_functions );
  fprintf( pout, "\tfunctions\n" );
  fprintf( pout, "};\n\n" );


  for ( i=0; i<_interface.num_functions; i++) {
    generate_c_function( pout, _interface.functions[i] );
  }
}

#ifdef GEN_EPC_IFC_H
static void generate_interface_header ( FILE * pout )
{
  print_generate_comment( pout, "" );

  fprintf( pout, "#include \"%s.h\"\n\n", _interface.name );
}
#endif

static void generate_header ( FILE *pout )
{
  int i;

  print_generate_comment( pout, "" );

#ifdef GEN_EPC_IFC_H
  fprintf( pout, "#include \"epc_defs.h\"\n\n" );
#endif

  /* DECLARE THE INTERNAL FUNCTIONS CALLED, BECAUSE THE COMPILER NEEDS THEM!!! */
  /* HOWEVER DO NOT DECLARE EXTERNAL FUNCTIONS WHEN THEY DO NOT NEED TO BE PRINTED */
  for ( i=0; i<_interface.num_functions; i++) {
    if ( print_external_function != 0 )
      declare_external_function( pout, _interface.functions[i] );
    declare_internal_function( pout, _interface.functions[i] );
  }

  /* interface declaration */
  fprintf( pout, "\nextern epc_interface_t ifc_%s;\n", _interface.name );
}

void generate_c ( const char *include_text )
{
  char filename[256];
#ifdef GEN_EPC_IFC_H
  FILE * pout_i;
#endif
  FILE * pout_c, * pout_h;

#ifdef GEN_EPC_IFC_H
  sprintf( filename, "epc_ifc.h" );
  if ( ( pout_i = fopen( filename, "w" ) ) == NULL ) {
    printf( "cannot open file %s - exiting...\n", filename );
  }
#endif

  sprintf( filename, "%s.h", _interface.name );
  if ( ( pout_h = fopen( filename, "w" ) ) == NULL ) {
    printf( "cannot open file %s - exiting...\n", filename );
  }

  /* Generate a PRO*C file */
  sprintf( filename, "%s.pc", _interface.name );
  if ( ( pout_c = fopen( filename, "w" ) ) == NULL ) {
    printf( "cannot open file %s - exiting...\n", filename );
  }

  if ( print_external_function == 0 )
    print_external_function = (include_text == NULL);

#ifdef GEN_EPC_IFC_H
  generate_interface_header ( pout_i ); 
#endif
  generate_header ( pout_h ); 
  generate_c_source ( pout_c, include_text );
}
