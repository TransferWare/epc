/*
 * Filename             : $RCSfile$
 *
 * Creation date        : 25-JUN-1997
 *
 * Created by           : Huub van der Wouden
 *
 * Company              : Transfer Solutions bv
 *
 * Notes                : For strings PRO*C STRING constructs are used.
 *
 * --- Description -------------------------------------------------------
 * Parsing and code generation routines
 *
 * --- Revision History --------------------------------------------------
 * $Log$
 * Revision 1.33  2004/10/19 14:35:09  gpaulissen
 * epc_ -> epc__
 *
 * Revision 1.32  2004/10/18 15:17:33  gpaulissen
 * XML enabling of EPC
 *
 * Revision 1.31  2004/10/15 13:53:40  gpaulissen
 * XML added
 *
 * Revision 1.30  2004/03/28 15:32:25  gpaulissen
 * Generate C code only.
 *
 * Revision 1.29  2004/03/14 07:31:22  gpaulissen
 * Imake support cancelled
 *
 * Revision 1.28  2003/06/08 16:28:20  gpaulissen
 * GNU build system for ts_dbug
 *
 * Revision 1.27  2003/03/29 18:31:42  gpaulissen
 * Various fixes
 *
 * Revision 1.26  2003/03/26 21:47:08  gpaulissen
 * Building the epc
 *
 * Revision 1.25  2002/12/04 17:47:03  gpaulissen
 * Libtool problems
 *
 * Revision 1.24  2002/10/28 14:53:04  gpaulissen
 * Using GNU standards.
 *
 * Revision 1.23  2002/10/19 14:05:19  gpaulissen
 * einde signature
 *
 * Revision 1.22  2002/10/19 13:58:13  gpaulissen
 * function signature instead of function name
 *
 * Revision 1.21  2002/10/19 12:10:30  gpaulissen
 * PL/SQL receive and send functions added.
 *
 * Revision 1.20  2002/10/18 17:07:53  gpaulissen
 * Creating skeleton packages
 *
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

#if HAVE_CONFIG_H
#include "config.h"
#endif

#if HAVE_MALLOC_H
#include <malloc.h> 
#endif

#include <stdio.h> 

#if HAVE_STRING_H
#include <string.h> 
#endif

#if HAVE_STDLIB_H
#include <stdlib.h>
#endif

#if HAVE_ASSERT_H
#include <assert.h>
#endif

#include "idl_prs.h"
#include "idl_defs.h"
#include "idl_kwrd.h"

#include <dbug.h>

/*
|| Defines
*/
#define EPC_PREFIX "_epc_"

/*
|| Forward declaration of static procedures
*/


/*
|| Static global variables
*/
static idl_interface_t _interface;

/*
|| Global variables. See epc.pks
*/
keyword keywords[] = {
  { C_VOID,     
    {   
      { C,      "void",         "C_VOID" },
      { PLSQL,  "??",             "??" }
    }
  },
  { C_INT,      
    {   
      { C,      "idl_int_t",    "C_INT" },
      { PLSQL,  "epc.int_subtype",    "epc.data_type_int" }
    }
  },
  { C_LONG,     
    {   
      { C,      "idl_long_t",   "C_LONG" },
      { PLSQL,  "epc.long_subtype",   "epc.data_type_long" }
    }
  },
  { C_FLOAT,    
    {   
      { C,      "idl_float_t",  "C_FLOAT" },
      { PLSQL,  "epc.float_subtype",  "epc.data_type_float" }
    }
  },
  { C_DOUBLE,   
    {   
      { C,      "idl_double_t", "C_DOUBLE" },
      { PLSQL,  "epc.double_subtype", "epc.data_type_double" }
    }
  },
  { C_STRING,   
    {
      /* string is a typedef */
      { C,      "idl_string_t", "C_STRING" },
      { PLSQL,  "epc.string_subtype",     "epc.data_type_string" }
    }
  },
  { C_IN,
    {
      { C,      "",             "C_IN" },
      { PLSQL,  "IN",           "??epc.c_in" }
    }
  },
  { C_OUT,
    {
      { C,      "*",            "C_OUT" },
      { PLSQL,  "OUT",          "??epc.c_out" } 
    }
  },
  { C_INOUT,
    {
      { C,      "*",            "C_INOUT" },
      { PLSQL,  "IN OUT",       "??epc.c_inout" }
    }
  }
};

int print_external_function = 0; /* print extern <function> in header */

/*
|| Global functions
*/

void
set_interface( char *name )
{
  DBUG_ENTER( "set_interface" );

  /*assert( strlen(name) < sizeof(_interface.name) ); /* beware of the trailing '\0' */
  (void) strcpy( _interface.name, name );
  _interface.num_functions = 0;

  DBUG_LEAVE();
}

static
void
init_parameter( idl_parameter_t *parm, char *name, idl_mode_t mode, idl_type_t datatype, dword_t size )
{
  DBUG_ENTER( "init_parameter" );

  assert( strlen(name) < sizeof(parm->name) ); /* beware of the trailing '\0' */
  (void) strcpy( parm->name, name );
  assert( strlen(name) < sizeof(parm->proc_name) ); /* beware of the trailing '\0' */
  (void) strcpy( parm->proc_name, name );

  parm->mode = mode;
  switch( mode )
    {
    case C_IN:
    case C_INOUT:
    case C_OUT:
      break;
      
    default:
      (void) fprintf( stderr, "(add_parameter) Mode %ld of parameter %s unknown.\n", 
                      (long)mode, name );
      exit( EXIT_FAILURE );
    }

  parm->datatype = datatype;
  switch( datatype )
    {
    case C_STRING:
      /*                strcat( parm->proc_name, "_vc" );*/ /* GJP 8-1-2000 VARCHAR is not used anymore */
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
      (void) fprintf( stderr, "(add_parameter) Type %ld of parameter %s unknown.\n", 
                      (long)datatype, name );
      exit( EXIT_FAILURE );
    }
  DBUG_LEAVE();
}


void
add_function( char *name, idl_type_t datatype, const int oneway )
{
  idl_function_t * fun = (idl_function_t*)malloc(sizeof(idl_function_t));

  DBUG_ENTER( "add_function" );

  assert( strlen(name) < sizeof(fun->name) ); /* beware of the trailing '\0' */
  (void) strcpy( fun->name, name );
  fun->oneway = oneway;
  fun->num_parameters = 0;

  init_parameter( &fun->return_value, "result", C_OUT, datatype, MAX_STR_VAL_LEN );

  _interface.functions[_interface.num_functions] = fun;
  _interface.num_functions++;

  DBUG_LEAVE();
}


void
add_parameter( char *name, idl_mode_t mode, idl_type_t datatype, dword_t size )
{
  idl_function_t * fun = _interface.functions[_interface.num_functions-1];
  idl_parameter_t * parm = (idl_parameter_t*)malloc(sizeof(idl_parameter_t));

  DBUG_ENTER( "add_parameter" );

  init_parameter( parm, name, mode, datatype, size );

  fun->parameters[fun->num_parameters] = parm;
  fun->num_parameters++;

  DBUG_LEAVE();
}


static
mapping *
get_mapping( const idl_type_t type, const idl_lang_t language )
{
  int i, j;
  const int num_keywords = sizeof(keywords) / sizeof(keyword);

  for ( i=0; i<num_keywords; i++ ) {
    if ( keywords[i].key == type ) {
      for ( j=0; j<NUM_LANGUAGES; j++ ) {
        if ( keywords[i].mappings[j].language == language ) {
          return &keywords[i].mappings[j];
        }
      }
      (void) fprintf( stderr, "No mapping for %ld in language %ld\n", type, language );
      exit( EXIT_FAILURE );
    }
  }
  fprintf( stderr, "Type %ld not a valid keyword\n", (long)type );
  exit( EXIT_FAILURE );

  return NULL;
}


static
char *
get_syntax( const idl_type_t type, const idl_lang_t language )
{
  return get_mapping( type, language )->syntax;
}


static
char *
get_constant_name( const idl_type_t type, const idl_lang_t language )
{
  return get_mapping( type, language )->constant_name;
}

static
char *
get_size( const idl_parameter_t *idl_parameter )
{
  static char size_str[100] = "";

  switch( idl_parameter->datatype )
    {
    case C_STRING:
      (void) snprintf(size_str, sizeof(size_str), "%ld+1", (long)idl_parameter->size);
      break;

    case C_VOID:
      (void) snprintf(size_str, sizeof(size_str), "0");
      break;

    case C_INT:
      (void) snprintf(size_str, sizeof(size_str), "sizeof(int)");
      break;

    case C_LONG:
      (void) snprintf(size_str, sizeof(size_str), "sizeof(long)");
      break;

    case C_DOUBLE:
      (void) snprintf(size_str, sizeof(size_str), "sizeof(double)");
      break;

    case C_FLOAT:
      (void) snprintf(size_str, sizeof(size_str), "sizeof(float)");
      break;
    }

  return size_str;
}

static
void
print_formal_parameter( FILE *pout,
                        const char *name,
                        const idl_mode_t mode,
                        const idl_type_t datatype,
                        const idl_lang_t lang )
{
  DBUG_ENTER( "print_formal_parameter" );

  switch( lang )
    {
    case C:
      (void) fprintf( pout, "%s %s%s",
                      get_syntax( datatype, lang ),
                      ( mode != C_IN && datatype != C_STRING ? "*" : "" ),
                      name );
      break;

    case PLSQL:
      (void) fprintf( pout, "%s %s %s", 
                      name, 
                      get_syntax( mode, lang ),
                      get_syntax( datatype, lang ) );
      break;
    }

  DBUG_LEAVE();
}


static
void
print_actual_parameter( FILE *pout, idl_parameter_t *parm, idl_lang_t lang )
{
  DBUG_ENTER( "print_actual_parameter" );

  switch( lang )
    {
    case C:
      (void) fprintf( pout, "%s%s",
                      ( parm->mode == C_IN && parm->datatype != C_STRING ? "*" : "" ),
                      parm->name );
      break;
    case PLSQL:
      (void) fprintf( pout, "%s", 
                      parm->name );
      break;
    }

  DBUG_LEAVE();
}


static
void
print_variable_definition( FILE *pout, idl_parameter_t *parm, idl_lang_t lang, const int parameter_nr  )
{
  DBUG_ENTER( "print_variable_definition" );

  switch( lang )
    {
    case C:
      switch( parm->datatype )
        {
        case C_INT:
        case C_LONG:
        case C_FLOAT:
        case C_DOUBLE:
          (void) fprintf( pout,
                          "%s *%s = (%s *)function->parameters[%d].data",
                          get_syntax( parm->datatype, C ),
                          parm->proc_name,
                          get_syntax( parm->datatype, C ),
                          parameter_nr );
          break;

        case C_STRING:
          (void) fprintf( pout,
                          "%s %s = (%s)function->parameters[%d].data", 
                          get_syntax( parm->datatype, C ),
                          parm->proc_name,
                          get_syntax( parm->datatype, C ),
                          parameter_nr );
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
          (void) fprintf( pout, "%s %s", 
                          parm->name, 
                          get_syntax( parm->datatype, PLSQL ) );
          break;
          
        case C_STRING:
          (void) fprintf( pout, "%s %s(%ld)", 
                          parm->name, 
                          get_syntax( parm->datatype, PLSQL ), 
                          parm->size );
          break;
        }
      break;
    }

  DBUG_LEAVE();
}


static
void
generate_plsql_function( FILE * pout, idl_function_t * fun )
{
  int nr, nr_actual_parameters /* for a SEND and RECV less parameters */;

  DBUG_ENTER( "generate_plsql_function" );

  if ( fun->return_value.datatype != C_VOID )
    {
      (void) fprintf( pout, "  FUNCTION %s", fun->name );
    }
  else
    {
      (void) fprintf( pout, "  PROCEDURE %s", fun->name );
    }

  /* PARAMETERS */
  for ( nr = nr_actual_parameters = 0; nr < fun->num_parameters; nr++ ) 
    {
      (void) fprintf( pout, "%s    ", ( nr_actual_parameters++ == 0 ? "(\n" : ",\n" ) );
      print_formal_parameter( pout,
                              fun->parameters[nr]->name,
                              fun->parameters[nr]->mode,
                              fun->parameters[nr]->datatype,
                              PLSQL );
    }

  if ( nr_actual_parameters > 0 ) 
    {
      (void) fprintf( pout, "\n  )" );
    }

  if ( fun->return_value.datatype != C_VOID )
    {
      (void) fprintf( pout, "\n  RETURN %s", get_syntax( fun->return_value.datatype, PLSQL ) );
    }

  DBUG_LEAVE();
}


static
void
generate_plsql_function_declaration( FILE * pout, idl_function_t * fun )
{
  DBUG_ENTER( "generate_plsql_function_declaration" );

  generate_plsql_function( pout, fun );
  (void) fprintf( pout, ";\n\n" );

  DBUG_LEAVE();
}


static
void
print_generate_comment( FILE * pout, char * prefix )
{
  DBUG_ENTER( "print_generate_comment" );

  (void) fprintf( pout, "%s/*******************************\n", prefix );
  (void) fprintf( pout, "%s ** Generated by EPC compiler **\n", prefix );
  (void) fprintf( pout, "%s *******************************/\n\n", prefix );

  DBUG_LEAVE();
}


static
void
generate_plsql_header( FILE * pout )
{
  int nr;
  DBUG_ENTER( "generate_plsql_header" );

  print_generate_comment( pout, "REMARK " );

  /* do not print an empty package */
  if ( _interface.num_functions > 0 )
    {
      (void) fprintf( pout, "CREATE OR REPLACE PACKAGE %s IS\n\n",
                      _interface.name );
    
      for ( nr = 0; nr < _interface.num_functions; nr++ ) 
        {
          generate_plsql_function_declaration( pout, _interface.functions[nr] );
        }

      (void) fprintf( pout, "END %s;\n/\n\n", _interface.name );
    }

  DBUG_LEAVE();
}


static
void
print_function_signature( FILE *pout, idl_function_t *fun )
{
  int nr;
  const idl_lang_t language = C;
  int tot_length = 0, length;

  DBUG_ENTER( "print_function_signature" );

  length = fprintf( pout, "%s%s %s",
                    ( fun->oneway != 0 ? "oneway " : "" ),
                    get_syntax( fun->return_value.datatype, language ),
                    fun->name );
  if ( length >= 0 )
    tot_length += length;

  for ( nr = 0; nr < fun->num_parameters; nr++ )
    {
      length = fprintf( pout, "%s[%s] %s %s",
                        ( nr == 0 ? "( " : ", " ),
                        ( fun->parameters[nr]->mode == C_IN ?
                          "in" :
                          ( fun->parameters[nr]->mode == C_INOUT ?
                            "inout" :
                            "out" ) ),
                        get_syntax( fun->parameters[nr]->datatype, language ),
                        fun->parameters[nr]->name );
      if ( length >= 0 )
        tot_length += length;
    }

  if ( nr > 0 )
    {
      length = fprintf( pout, "%s", " )" );
      if ( length >= 0 )
        tot_length += length;
    }

  assert( tot_length < MAX_FUNC_NAME_LEN );

  DBUG_LEAVE();
}

static
void
generate_plsql_function_body( FILE * pout, idl_function_t * fun )
{
  int nr;
  idl_parameter_t * parm;

  DBUG_ENTER( "generate_plsql_function_body" );

  /* do we need it? */
  generate_plsql_function( pout, fun );
  (void) fprintf( pout, "\n  IS\n" );

  /* RETURN VARIABLE */
  if ( fun->return_value.datatype != C_VOID )
    {
      (void) fprintf( pout, "    " );
      print_variable_definition( pout, &fun->return_value, PLSQL, fun->num_parameters );
      (void) fprintf( pout, ";\n" );
    }

  (void) fprintf( pout, "  BEGIN\n" );

  /* SETUP OF CLIENT SIDE FUNCTION CALL */
  (void) fprintf( pout, 
                  "    epc_clnt.new_request(g_epc_key);\n" );

  for ( nr = 0; nr < fun->num_parameters; nr++ ) {
    parm = fun->parameters[nr];

    if ( parm->mode != C_OUT )
      (void) fprintf( pout, 
                      "    epc_clnt.set_request_parameter(g_epc_key, '%s', %s, %s);\n", 
                      parm->name, 
                      get_constant_name(parm->datatype, PLSQL), 
                      parm->name );
  }

  (void) fprintf( pout, 
                  "    epc_clnt.send_request(g_epc_key, '%s', %ld);\n",
                  fun->name,
                  fun->oneway );

  if ( fun->oneway == 0 )
    {
      (void) fprintf( pout, 
                      "    epc_clnt.recv_response(g_epc_key);\n" );
    }

  /* GET THE RESULTS */
  for ( nr = 0; nr < fun->num_parameters; nr++ ) 
    {
      parm = fun->parameters[nr];
      if ( parm->mode != C_IN ) 
        {
          (void) fprintf( pout, 
                          "    epc_clnt.get_response_parameter(g_epc_key, '%s', %s, %s);\n", 
                          parm->name, 
                          get_constant_name(parm->datatype, PLSQL), 
                          parm->name );
        }
    }

  if ( fun->return_value.datatype != C_VOID ) 
    {
      (void) fprintf( pout, 
                      "    epc_clnt.get_response_parameter(g_epc_key, '%s', %s, %s);\n", 
                      fun->return_value.name,
                      get_constant_name(fun->return_value.datatype, PLSQL),
                      fun->return_value.name );
      (void) fprintf( pout, 
                      "    RETURN %s;\n", 
                      fun->return_value.name );
    }

  (void) fprintf( pout, "  END;\n\n" );

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
  EXTERNAL LIBRARY %s_lib\n\
  NAME \"%s\"\n\
  LANGUAGE C", _interface.name, fun->name );

  DBUG_PRINT( "info",
              ( "return datatype: %ld; # parms: %ld",
                (int) fun->return_value.datatype,
                (int) fun->num_parameters ) );

  /* PARAMETERS CLAUSE ? */
  if ( fun->return_value.datatype != C_VOID ||
       fun->num_parameters > 0 )
    {
      (void) fprintf( pout, "\n\
  PARAMETERS (\n" );

      for ( nr = 0; nr < fun->num_parameters; nr++ ) 
        {
          (void) fprintf( pout, "  %s  %s %s\n",
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
          (void) fprintf( pout, "  %s  RETURN %s\n",
                          (nr == 0 ? " " : "," ),
                          get_syntax( fun->return_value.datatype, C ) ) ;
          break;
        }
      
      (void) fprintf( pout, "\
  )" );
    }

  (void) fprintf( pout, ";\n\n" );

  DBUG_LEAVE();
}


static
void
generate_plsql_body( FILE * pout )
{
  int nr;

  DBUG_ENTER( "generate_plsql_body" );

  print_generate_comment( pout, "REMARK " );

  /* do not print an empty package body */
  if ( _interface.num_functions > 0 )
    {
      (void) fprintf( pout, "CREATE OR REPLACE PACKAGE BODY %s IS\n\n\
  g_epc_key epc_clnt.epc_key_subtype;\n\n",
                      _interface.name );
      (void) fprintf( pout, "\n" );

      for ( nr = 0; nr < _interface.num_functions; nr++ ) 
        {
          generate_plsql_function_body( pout, _interface.functions[nr] );
        }
      
      (void) fprintf( pout, "BEGIN\n\
  g_epc_key := epc_clnt.register('%s');\n\
END;\n", _interface.name );
      (void) fprintf( pout, "/\n\n" );
    }

  DBUG_LEAVE();
}


static
void
generate_plsql_body_ext( FILE * pout )
{
  int nr;

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

  for ( nr = 0; nr < _interface.num_functions; nr++ ) {
    generate_plsql_function_body_ext( pout, _interface.functions[nr] );
  }

  (void) fprintf( pout, "END;\n" );
  (void) fprintf( pout, "/\n" );

  DBUG_LEAVE();
}


void
generate_plsql( void )
{
  char filename[256];
#define NR_PLSQL_FILES 4
  FILE * pout[NR_PLSQL_FILES];
  int nr;

  DBUG_ENTER( "generate_plsql" );

  for ( nr = 0; nr < NR_PLSQL_FILES; nr++ )
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

        default:
          assert( nr >= 0 && nr < 4 );
        }

      if ( ( pout[nr] = fopen( filename, "w" ) ) == NULL ) 
        {
          goto open_error;
        }
      else
        {
          (void) fprintf( stdout, "Creating %s\n", filename );
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

  for ( nr = 0; nr < NR_PLSQL_FILES; nr++ )
    {
      (void) fclose( pout[nr] );
    }

  DBUG_LEAVE();

  return;

 open_error:
  (void) fprintf( stderr, "Cannot write file %s\n", filename );
  exit( EXIT_FAILURE );
}


static
void
generate_c_parameters( FILE * pout, idl_function_t * fun )
{
  int nr;
  idl_parameter_t * parm;

  DBUG_ENTER( "generate_c_parameters" );

  /* 
   * declare variables 
   */

  for ( nr = 0; nr < fun->num_parameters; nr++ ) {
    parm = fun->parameters[nr];
    (void) fprintf( pout, "  " );
    print_variable_definition( pout, parm, C, nr );
    (void) fprintf( pout, ";\n" );
  }

  DBUG_LEAVE();
}


static
void
print_c_debug_info( FILE *pout, char *name, idl_type_t datatype )
{
  DBUG_ENTER( "print_c_debug_info" );

  (void) fprintf( pout, "  DBUG_PRINT( \"info\", ( " );
  switch( datatype )
    {
    case C_INT:
      (void) fprintf( pout, "\"%s: %%d\", *%s ) );\n", name, name ); break;
    case C_LONG:
      (void) fprintf( pout, "\"%s: %%ld\", *%s ) );\n", name, name ); break;
    case C_FLOAT:
      (void) fprintf( pout, "\"%s: %%f\", *%s ) );\n", name, name ); break;
    case C_DOUBLE:
      (void) fprintf( pout, "\"%s: %%lf\", *%s ) );\n", name, name ); break;
    case C_STRING:
      (void) fprintf( pout, "\"%s: '%%s'\", %s ) );\n", name, name ); break;
    default:
      (void) fprintf( stderr, "print_c_debug_info#Unknown datatype (%ld) for %s\n", datatype, name ); break;
    }

  DBUG_LEAVE();
}


static
void
generate_c_debug_info( FILE * pout, idl_function_t * fun, idl_mode_t mode )
{
  int nr;
  idl_parameter_t *parm;

  DBUG_ENTER( "generate_c_debug_info" );

  for ( nr = 0; nr < fun->num_parameters; nr++ ) {
    parm = fun->parameters[nr];
    
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

  DBUG_LEAVE();
}


static
void
generate_c_function( FILE * pout, idl_function_t * fun )
{
  int nr, nr_actual_parameters;
  int exec_sql_printed = 0;

  DBUG_ENTER( "generate_c_function" );

  (void) fprintf( pout, "void %s%s ( epc__function_t *function )\n", EPC_PREFIX, fun->name );
  (void) fprintf( pout, "{\n" );

  /* PARAMETERS */
  generate_c_parameters( pout, fun );

  switch( fun->return_value.datatype )
    {
    case C_VOID:
      break;

    default:
      (void) fprintf( pout, "  " );
      print_variable_definition( pout, &fun->return_value, C, fun->num_parameters );
      (void) fprintf( pout, ";\n" );
      break;
    }

  (void) fprintf( pout, "\n  DBUG_ENTER( \"%s\" );\n",
                  fun->name );

  /* ---------------------------------------------------------------
   * Print the in and in/out parameters, including Oracle error code
   * --------------------------------------------------------------- */

  generate_c_debug_info( pout, fun, C_IN );
  generate_c_debug_info( pout, fun, C_INOUT );

  (void) fprintf( pout, "\n" );

  /* ---------------
   * THE ACTUAL CALL 
   * --------------- */
  (void) fprintf( pout, "  " );

  /* set return value if any. Use strncpy for strings */

  switch( fun->return_value.datatype )
    {
    case C_VOID:
      break;

    case C_STRING:
      (void) fprintf( pout, "(void) strncpy( %s, ", fun->return_value.name );
      break;

    default:
      (void) fprintf( pout, "*%s = ", fun->return_value.name );
      break;
    }

  (void) fprintf( pout, "%s( ", fun->name );

  for ( nr = 0; nr < fun->num_parameters; nr++ ) {
    if ( nr > 0 ) {
      (void) fprintf( pout, ", " );
    }
    print_actual_parameter( pout, fun->parameters[nr], C );
  }
  (void) fprintf( pout, " )" );

  switch( fun->return_value.datatype )
    {
    case C_VOID:
      break;

    case C_STRING:
      /* zero terminate to be sure */
      (void) fprintf( pout, ", %ld );\n%s  %s[%ld] = '\\0'",
                      fun->return_value.size, 
                      "",
                      fun->return_value.proc_name, 
                      fun->return_value.size );
      break;

    default:
      break;
    }

  (void) fprintf( pout, ";\n" );

  (void) fprintf( pout, "\n" );

  /*
   * Print the in/out and out parameters, including Oracle error code
   */
  generate_c_debug_info( pout, fun, C_INOUT );
  generate_c_debug_info( pout, fun, C_OUT );

  (void) fprintf( pout, "  DBUG_LEAVE();\n}\n\n" );

  DBUG_LEAVE();
}


static
void
declare_external_function( FILE * pout, idl_function_t * fun )
{
  int nr;

  DBUG_ENTER( "declare_external_function" );

  (void) fprintf( pout, "extern %s %s( ", 
                  get_syntax( fun->return_value.datatype, C ),
                  fun->name );
  if ( fun->num_parameters > 0 ) {
    for ( nr = 0; nr < fun->num_parameters; nr++ ) {
      idl_parameter_t * parm = fun->parameters[nr];

      print_formal_parameter( pout, parm->name, parm->mode, parm->datatype, C );

      if ( nr < fun->num_parameters - 1 )
        (void) fprintf( pout, ", " );
    }
  }
  else /* 0 parameters */
    (void) fprintf( pout, "void" );

  (void) fprintf( pout, " );\n" );

  DBUG_LEAVE();
}


static
void
declare_internal_function( FILE * pout, idl_function_t * fun )
{
  DBUG_ENTER( "declare_internal_function" );

  (void) fprintf( pout, "extern void %s%s( epc__function_t *function );\n", EPC_PREFIX, fun->name );

  DBUG_LEAVE();
}


static
void
generate_c_source ( FILE * pout, const char *include_text )
{
  int fnr /* function number */, pnr /* parameter number */;
  idl_function_t * fun;

  DBUG_ENTER( "generate_c_source" );

  print_generate_comment( pout, "" );

  if ( include_text != NULL )
    {
      (void) fprintf( pout, "%s\n", include_text );
    }

  (void) fprintf( pout, "\
#include <string.h>\n\
#include <stdlib.h>\n\
#include <epc.h>\n" );

  (void) fprintf( pout, "\
#include <dbug.h>\n\
#include \"%s.h\"\n\n", _interface.name );

  /* parameter array */
  for ( fnr = 0; fnr < _interface.num_functions; fnr++ ) 
    {
      fun = _interface.functions[fnr];
      (void) fprintf( pout, "\nstatic epc__parameter_t %s_parameters[] = {\n", fun->name );
      for ( pnr = 0; pnr < fun->num_parameters; pnr++ )
        {
          (void) fprintf( pout, "  { \"%s\", %s, %s, %s, NULL },\n",
                          fun->parameters[pnr]->name,
                          get_constant_name( fun->parameters[pnr]->mode, C ), 
                          get_constant_name( fun->parameters[pnr]->datatype, C ),
                          get_size( fun->parameters[pnr] ) );
        }
      (void) fprintf( pout, "  { \"%s\", %s, %s, %s, NULL }\n", 
                      fun->return_value.name,
                      get_constant_name( fun->return_value.mode, C ), 
                      get_constant_name( fun->return_value.datatype, C ),
                      get_size( &fun->return_value ) );
      (void) fprintf( pout, "};\n" );
  }

  /* function array */
  (void) fprintf( pout, "\nstatic epc__function_t functions[] = {\n" );
  for ( fnr = 0; fnr < _interface.num_functions; fnr++ ) 
    {
      fun = _interface.functions[fnr];
      (void) fprintf( pout, "%s { \"%s", ( fnr > 0 ? "," : " " ), fun->name );
      /*print_function_signature( pout, fun );*/
      (void) fprintf( pout, "\",\n    %s%s, %ld, %ld, %s_parameters }\n", 
                      EPC_PREFIX,
                      fun->name,
                      fun->oneway,
                      fun->num_parameters+1, /* return value is a parameter too */
                      fun->name
                      );
  }
  (void) fprintf( pout, "};\n" );

  /* interface */
  (void) fprintf( pout, "\nepc__interface_t ifc_%s = {\n", _interface.name );
  (void) fprintf( pout, "  \"%s\",\n", _interface.name );
  (void) fprintf( pout, "  %ld,\n", _interface.num_functions );
  (void) fprintf( pout, "  functions\n" );
  (void) fprintf( pout, "};\n\n" );


  for ( fnr = 0; fnr < _interface.num_functions; fnr++ ) {
    generate_c_function( pout, _interface.functions[fnr] );
  }

  DBUG_LEAVE();
}


static
void
generate_header( FILE *pout )
{
  int nr;

  DBUG_ENTER( "generate_header" );

  print_generate_comment( pout, "" );

  /* DECLARE THE INTERNAL FUNCTIONS CALLED, BECAUSE THE COMPILER NEEDS THEM!!! */
  /* HOWEVER DO NOT DECLARE EXTERNAL FUNCTIONS WHEN THEY DO NOT NEED TO BE PRINTED */
  for ( nr = 0; nr < _interface.num_functions; nr++ ) {
    if ( print_external_function != 0 )
      declare_external_function( pout, _interface.functions[nr] );
    declare_internal_function( pout, _interface.functions[nr] );
  }

  /* interface declaration */
  (void) fprintf( pout, "\nextern epc__interface_t ifc_%s;\n", _interface.name );

  DBUG_LEAVE();
}


void
generate_c( const char *include_text )
{
  char filename[256];
  FILE *pout = NULL;

  DBUG_ENTER( "generate_c" );

  if ( print_external_function == 0 )
    print_external_function = (include_text == NULL);

  /* Create header file */

  (void) sprintf( filename, "%s.h", _interface.name );
  if ( ( pout = fopen( filename, "w" ) ) == NULL ) 
    {
      goto open_error;
    }
  else
    {
      (void) fprintf( stdout, "Creating %s\n", filename );
      generate_header( pout );
      (void) fclose( pout );
    }

  (void) sprintf( filename, "%s.c", _interface.name );

  if ( ( pout = fopen( filename, "w" ) ) == NULL ) 
    {
      goto open_error;
    }
  else
    {
      (void) fprintf( stdout, "Creating %s\n", filename );
      generate_c_source( pout, include_text );
      (void) fclose( pout );
    }

  DBUG_LEAVE();
  return;

 open_error:
  (void) fprintf( stderr, "Cannot write file %s\n", filename );
  exit( EXIT_FAILURE );
}


