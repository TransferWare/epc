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


/*
|| Static global variables
*/
static idl_interface_t _interface;

/* package types */
#define STUB 0
#define SKEL_RECV 1
#define SKEL_SEND 2
  
static char *package_type_str[] = { "", "_recv", "_send" };

#define NR_PACKAGE_TYPE (sizeof(package_type_str)/sizeof(package_type_str[0]))

/*
|| Global variables
*/
keyword keywords[] = {
  { C_VOID,     
    {   
      { C,      "void",         "C_VOID" },
      { PLSQL,  "",             "epc.c_void" }
    }
  },
  { C_INT,      
    {   
      { C,      "int",          "C_INT" },
      /*      { PLSQL,  "INTEGER",      "epc.c_int" }*/
      { PLSQL,  "BINARY_INTEGER",       "epc.c_int" }
    }
  },
  { C_LONG,     
    {   
      { C,      "long",         "C_LONG" },
      /*      { PLSQL,  "INTEGER",      "epc.c_long" }*/
      { PLSQL,  "BINARY_INTEGER",       "epc.c_long" }
    }
  },
  { C_FLOAT,    
    {   
      { C,      "float",        "C_FLOAT" },
      /*      { PLSQL,  "NUMBER",       "epc.c_float" }*/
      { PLSQL,  "FLOAT",        "epc.c_float" }
    }
  },
  { C_DOUBLE,   
    {   
      { C,      "double",       "C_DOUBLE" },
      /*      { PLSQL,  "NUMBER",       "epc.c_double" }*/
      { PLSQL,  "DOUBLE PRECISION",     "epc.c_double" }
    }
  },
  { C_STRING,   
    {
      /* string is a typedef */
      { C,      "string",       "C_STRING" },
      { PLSQL,  "VARCHAR2",     "epc.c_string" }
    }
  },
  { C_IN,
    {
      { C,      "",             "C_IN" },
      { PLSQL,  "IN",           "epc.c_in" }
    }
  },
  { C_OUT,
    {
      { C,      "*",            "C_OUT" },
      { PLSQL,  "OUT",          "epc.c_out" } 
    }
  },
  { C_INOUT,
    {
      { C,      "*",            "C_INOUT" },
      { PLSQL,  "IN OUT",       "epc.c_inout" }
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
  assert( strlen(name) < sizeof(_interface.name) ); /* beware of the trailing '\0' */
  (void) strcpy( _interface.name, name );
  _interface.num_functions = 0;
}


static
void
init_parameter( idl_parameter_t *parm, char *name, idl_mode_t mode, idl_type_t datatype, dword_t size )
{
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
}


void
add_function( char *name, idl_type_t datatype, const int oneway )
{
  idl_function_t * fun = (idl_function_t*)malloc(sizeof(idl_function_t));

  assert( strlen(name) < sizeof(fun->name) ); /* beware of the trailing '\0' */
  (void) strcpy( fun->name, name );
  fun->oneway = oneway;
  fun->num_parameters = 0;

  init_parameter( &fun->return_value, "result", C_OUT, datatype, MAX_STR_VAL_LEN );

  _interface.functions[_interface.num_functions] = fun;
  _interface.num_functions++;
}


void
add_parameter( char *name, idl_mode_t mode, idl_type_t datatype, dword_t size )
{
  idl_function_t * fun = _interface.functions[_interface.num_functions-1];
  idl_parameter_t * parm = (idl_parameter_t*)malloc(sizeof(idl_parameter_t));

  init_parameter( parm, name, mode, datatype, size );

  fun->parameters[fun->num_parameters] = parm;
  fun->num_parameters++;
}


static
mapping *
get_mapping( idl_type_t type, idl_lang_t language )
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
get_syntax( idl_type_t type, idl_lang_t language )
{
  return        get_mapping( type, language )->syntax;
}


static
char *
get_constant_name( const idl_type_t type, const idl_lang_t language )
{
  return        get_mapping( type, language )->constant_name;
}


static
void
print_formal_parameter( FILE *pout,
                        const char *name,
                        const idl_mode_t mode,
                        const idl_type_t datatype,
                        const idl_lang_t lang )
{
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
}


static
void
print_actual_parameter( FILE *pout, idl_parameter_t *parm, idl_lang_t lang )
{
  switch( lang )
    {
    case C:
      (void) fprintf( pout, "%s%s",
                      ( parm->mode != C_IN && parm->datatype != C_STRING ? "&" : "" ),
                      parm->name );
      break;
    case PLSQL:
      (void) fprintf( pout, "%s", 
                      parm->name );
      break;
    }
}


static
void
print_variable_definition( FILE *pout, idl_parameter_t *parm, idl_lang_t lang )
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
          (void) fprintf( pout, "%s %s = 0", get_syntax( parm->datatype, C ), parm->proc_name );
          break;

        case C_STRING: /* Use the STRING PRO*C construct */
          (void) fprintf( pout, "char %s[%d] = \"\";\n\tEXEC SQL VAR %s IS STRING(%d)", 
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
          (void) fprintf( pout, "%s %s", 
                          parm->name, 
                          get_syntax( parm->datatype, PLSQL ) );
          break;
          
        case C_STRING:
          (void) fprintf( pout, "%s %s(%d)", 
                          parm->name, 
                          get_syntax( parm->datatype, PLSQL ), 
                          parm->size );
          break;
        }
      break;
    }
}


/* do we need a PL/SQL function for receiving and returning parameters? */
static
int
exists_plsql_function( idl_function_t * fun, const int package_type )
{
  int val = 0;
  int nr;

  switch( package_type )
    {
    case STUB:
      val = 1;
      break;

    case SKEL_SEND:
      /* oneway functions do not return any result */
      if ( fun->oneway == 0 )
        val = 1;
      break;

    case SKEL_RECV:
      /* if there are no in or in/out parameters the recv function is not necessary */
      for ( nr = 0; nr < fun->num_parameters; nr++ )
        {
          if ( fun->parameters[nr]->mode != C_OUT )
            {
              /* there is at least one in or in/out parameter */
              val = 1;
              break;
            }
        }
    }
  return val;
}


static
void
generate_plsql_function( FILE * pout, idl_function_t * fun, const int package_type )
{
  int nr, nr_actual_parameters /* for a SEND and RECV less parameters */;

  assert( exists_plsql_function( fun, package_type ) != 0 );

  if ( package_type == STUB && fun->return_value.datatype != C_VOID )
    {
      (void) fprintf( pout, "\tFUNCTION %s", fun->name );
    }
  else
    {
      (void) fprintf( pout, "\tPROCEDURE %s", fun->name );
    }

  /* PARAMETERS */
  for ( nr = nr_actual_parameters = 0; nr < fun->num_parameters; nr++ ) 
    {
      switch(package_type)
        {
        case SKEL_SEND: /* in/out parameters, out parameters back to client */
        case SKEL_RECV: /* in and in/out parameters from client */

          /* a parameter is skipped if the following condition is true */
          if ( ( package_type == SKEL_RECV && fun->parameters[nr]->mode == C_OUT ) ||
               ( package_type == SKEL_SEND && fun->parameters[nr]->mode == C_IN ) )
            break; 

          /*FALLTHROUGH*/
        case STUB:
          (void) fprintf( pout, "%s\t\t", ( nr_actual_parameters++ == 0 ? "(\n" : ",\n" ) );
          print_formal_parameter( pout,
                                  fun->parameters[nr]->name,
                                  ( package_type == STUB ?
                                    fun->parameters[nr]->mode :
                                    ( package_type == SKEL_RECV ? 
                                      C_OUT :
                                      C_IN ) ),
                                  fun->parameters[nr]->datatype,
                                  PLSQL );
        }
    }

  /* Add result, result_pipe (IN), epc_error (IN) and sqlcode (IN/OUT) */
  if ( package_type == SKEL_SEND )
    {
      /* send the result */
      if ( fun->return_value.datatype != C_VOID )
        {
          (void) fprintf( pout, "%s\t\t", ( nr_actual_parameters++ == 0 ? "(\n" : ",\n" ) );
          print_formal_parameter( pout,
                                  "result",
                                  C_IN,
                                  fun->return_value.datatype,
                                  PLSQL );
        }

      for ( nr = 0; nr < 3; nr++ )
        {
          (void) fprintf( pout, "%s\t\t", ( nr_actual_parameters++ == 0 ? "(\n" : ",\n" ) );

          switch(nr)
            {
            case 0:
              print_formal_parameter( pout,
                                      "result_pipe",
                                      C_IN,
                                      C_STRING,
                                      PLSQL );
              break;
            case 1:
              print_formal_parameter( pout,
                                      "epc_error",
                                      C_IN,
                                      C_INT,
                                      PLSQL );
              break;
            case 2:
              print_formal_parameter( pout,
                                      "sqlcode",
                                      C_INOUT,
                                      C_INT,
                                      PLSQL );
              break;
            }
        }
    }

  if ( nr_actual_parameters > 0 ) 
    {
      (void) fprintf( pout, "\n\t)" );
    }

  if ( package_type == STUB && fun->return_value.datatype != C_VOID )
    {
      (void) fprintf( pout, " RETURN %s", get_syntax( fun->return_value.datatype, PLSQL ) );
    }
}


static
void
generate_plsql_function_declaration( FILE * pout, idl_function_t * fun, const int package_type )
{
  if ( exists_plsql_function( fun, package_type ) != 0 )
    {
      generate_plsql_function( pout, fun, package_type );
      (void) fprintf( pout, ";\n\n" );
    }
}


static
void
print_generate_comment( FILE * pout, char * prefix )
{
  (void) fprintf( pout, "%s/*******************************\n", prefix );
  (void) fprintf( pout, "%s ** Generated by EPC compiler **\n", prefix );
  (void) fprintf( pout, "%s *******************************/\n\n", prefix );
}


static
void
generate_plsql_header( FILE * pout )
{
  int nr;
  int package_type;

  print_generate_comment( pout, "REMARK " );

  /* Create three packagas: the STUB and two SKELETON packages (RECV from STUB and SEND) */
  for ( package_type = 0; package_type < NR_PACKAGE_TYPE; package_type++ ) {
    (void) fprintf( pout, "CREATE OR REPLACE PACKAGE %s%s IS\n\n",
                    _interface.name, package_type_str[package_type] );
    
    for ( nr = 0; nr < _interface.num_functions; nr++ ) {
      generate_plsql_function_declaration( pout, _interface.functions[nr], package_type );
    }

    (void) fprintf( pout, "END %s%s;\n/\n\n", _interface.name, package_type_str[package_type] );
  }
}


static
void
print_function_signature( FILE *pout, idl_function_t *fun )
{
  int nr;
  const idl_lang_t language = C;
  int tot_length = 0, length;

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
                        ( fun->parameters[nr]->datatype == C_IN ?
                          "in" :
                          ( fun->parameters[nr]->datatype == C_INOUT ?
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
}

static
void
generate_plsql_function_body( FILE * pout, idl_function_t * fun, const int package_type )
{
  int nr;
  idl_parameter_t * parm;

  DBUG_ENTER( "generate_plsql_function_body" );

  /* do we need it? */
  if ( exists_plsql_function( fun, package_type ) != 0 )
    {
      generate_plsql_function( pout, fun, package_type );
      (void) fprintf( pout, " IS\n" );

      /* RETURN VARIABLE */
      if ( package_type == STUB && fun->return_value.datatype != C_VOID )
        {
          (void) fprintf( pout, "\t\t" );
          print_variable_definition( pout, &fun->return_value, PLSQL );
          (void) fprintf( pout, ";\n" );
        }

      (void) fprintf( pout, "\tBEGIN\n" );

      switch( package_type )
        {
        case STUB:
          /* SETUP OF CLIENT SIDE FUNCTION CALL */
          (void) fprintf( pout, "\t\tepc.request_set_header( '%s', '", 
                          _interface.name );
          print_function_signature( pout, fun );
          (void) fprintf( pout, "', %d );\n", fun->oneway );

          for ( nr = 0; nr < fun->num_parameters; nr++ ) {
            parm = fun->parameters[nr];

            if ( parm->mode != C_OUT )
              (void) fprintf( pout, "\t\tepc.request_set_parameter( %s );\n", parm->name );
          }

          (void) fprintf( pout, "\t\tepc.request_perform_routine( %d );\n", fun->oneway );

          /* GET THE RESULTS */
          for ( nr = 0; nr < fun->num_parameters; nr++ ) 
            {
              parm = fun->parameters[nr];
              if ( parm->mode != C_IN ) {
                (void) fprintf( pout, "\t\tdbms_pipe.unpack_message( %s );\n", parm->name );
              }
            }

          if ( fun->return_value.datatype != C_VOID ) 
            {
              (void) fprintf( pout, "\t\tdbms_pipe.unpack_message( %s );\n", fun->return_value.name );
              (void) fprintf( pout, "\t\tRETURN %s;\n", fun->return_value.name );
            }
          break;

        case SKEL_SEND:
          /* ----------------
           * Send the results
           * ---------------- */
          (void) fprintf( pout, "\
\t\tdbms_pipe.reset_buffer;\n\
\t\tdbms_pipe.pack_message( epc_error );\n\
\t\tdbms_pipe.pack_message( sqlcode );\n\
\t\tIF ( epc_error = 0 )\n\
\t\tTHEN\n" );

          /*
           * Set the in/out and out parameters
           */
          for ( nr = 0; nr < fun->num_parameters; nr++ ) {
            idl_parameter_t * parm = fun->parameters[nr];
        
            if ( parm->mode == C_INOUT || parm->mode == C_OUT )
              {
                (void) fprintf( pout, "\t\t\tdbms_pipe.pack_message( %s );\n", parm->name ); 
              }
          }
          if ( fun->return_value.datatype != C_VOID )
            {
              (void) fprintf( pout, "\t\t\tdbms_pipe.pack_message( %s );\n", fun->return_value.name ); 
            }

          /*do not forget the NULL statement in case of 0 actual parameters */
          (void) fprintf( pout, "\
\t\t\tNULL;\n\
\t\tEND IF;\n\
\t\tsqlcode := dbms_pipe.send_message( result_pipe, 1 );\n\
\tEXCEPTION\n\
\t\tWHEN\tOTHERS\n\
\t\tTHEN\n\
\t\t\tsqlcode := SQLCODE;\n" );
          break;

        case SKEL_RECV:
          /*
           * Get the in and in/out parameters
           */
          for ( nr = 0; nr < fun->num_parameters; nr++ ) 
            {
              parm = fun->parameters[nr];

              if ( parm->mode == C_IN || parm->mode == C_INOUT )
                {
                  (void) fprintf( pout, "\t\tepc.request_get_parameter( %s );\n", parm->name );
                }
            }
          break;
        }

      (void) fprintf( pout, "\tEND;\n\n" );
    }

  DBUG_LEAVE();
}


static
void
generate_plsql_function_body_ext( FILE * pout, idl_function_t * fun )
{
  int nr;
  const int package_type = STUB;

  DBUG_ENTER( "generate_plsql_function_body_ext" );
  DBUG_PRINT( "input", ( "interface: %s; function: %s", _interface.name, fun->name ) );

  /* do we need it? */
  if ( exists_plsql_function( fun, package_type ) != 0 )
    {
      generate_plsql_function( pout, fun, package_type );

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
    }

  DBUG_LEAVE();
}


static
void
generate_plsql_body( FILE * pout )
{
  int nr;
  int package_type;

  print_generate_comment( pout, "REMARK " );

  for ( package_type = 0; package_type < NR_PACKAGE_TYPE; package_type++ ) 
    {
      (void) fprintf( pout, "CREATE OR REPLACE PACKAGE BODY %s%s IS\n",
                      _interface.name, package_type_str[package_type] );
      (void) fprintf( pout, "\n" );

      for ( nr = 0; nr < _interface.num_functions; nr++ ) 
        {
          generate_plsql_function_body( pout, _interface.functions[nr], package_type );
        }

      (void) fprintf( pout, "END;\n" );
      (void) fprintf( pout, "/\n\n" );
    }
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

  /* 
   * declare variables 
   */

  for ( nr = 0; nr < fun->num_parameters; nr++ ) {
    parm = fun->parameters[nr];
    (void) fprintf( pout, "\t" );
    print_variable_definition( pout, parm, C );
    (void) fprintf( pout, ";\n" );
  }
}


static
void
print_c_debug_info( FILE *pout, char *name, idl_type_t datatype )
{
  (void) fprintf( pout, "\tDBUG_PRINT( \"info\", ( " );
  switch( datatype )
    {
    case C_INT:
      (void) fprintf( pout, "\"%s: %%d\", %s ) );\n", name, name ); break;
    case C_LONG:
      (void) fprintf( pout, "\"%s: %%ld\", %s ) );\n", name, name ); break;
    case C_FLOAT:
      (void) fprintf( pout, "\"%s: %%f\", %s ) );\n", name, name ); break;
    case C_DOUBLE:
      (void) fprintf( pout, "\"%s: %%lf\", %s ) );\n", name, name ); break;
    case C_STRING:
      (void) fprintf( pout, "\"%s: '%%s'\", %s ) );\n", name, name ); break;
    default:
      (void) fprintf( stderr, "print_c_debug_info#Unknown datatype (%d) for %s\n", datatype, name ); break;
    }
}


static
void
generate_c_debug_info( FILE * pout, idl_function_t * fun, idl_mode_t mode )
{
  int nr;
  idl_parameter_t *parm;

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
}


static
void
generate_c_function( FILE * pout, idl_function_t * fun )
{
  int nr, nr_actual_parameters;
  int exec_sql_printed = 0;

  (void) fprintf( pout, "void %s%s ( epc_call_t *call )\n", EPC_PREFIX, fun->name );
  (void) fprintf( pout, "{\n\
#ifdef SQLCA\n\
#undef SQLCA\n\
#endif\n\
EXEC SQL INCLUDE sqlca;\n\n" );

  /* 
   * VARIABLE TO HOLD RETURN VALUE 
   */

  (void) fprintf( pout, "\t%s;\n", EXEC_SQL_BEGIN_DECLARE_SECTION );
  (void) fprintf( pout, "\tlong epc_error = OK;\n" );
  (void) fprintf( pout, "\tlong sqlcode = 0;\n" );
  (void) fprintf( pout, "\tconst char *result_pipe = call->result_pipe;\n" );

  switch( fun->return_value.datatype )
    {
    case C_VOID:
      break;

    default:
      (void) fprintf( pout, "\t" );
      print_variable_definition( pout, &fun->return_value, C );
      (void) fprintf( pout, ";\n" );
      break;
    }

  /* PARAMETERS */
  generate_c_parameters( pout, fun );

  (void) fprintf( pout, "\t%s;\n\n\tDBUG_ENTER(\"%s\");\n\n",
                  EXEC_SQL_END_DECLARE_SECTION, fun->name );

  /*
   * Get the in and in/out parameters
   */
  if ( exists_plsql_function( fun, SKEL_RECV ) != 0 )
    {
      /* call the recv function */
      (void) fprintf( pout, "\
\tEXEC SQL WHENEVER SQLERROR DO epc_abort(\"-- Oracle error --\");\n\
\tEXEC SQL EXECUTE\n\
\tBEGIN\n\
\t\t%s%s.%s", _interface.name, package_type_str[SKEL_RECV], fun->name );

      for ( nr = nr_actual_parameters = 0; nr < fun->num_parameters; nr++ )
        {
          idl_parameter_t * parm = fun->parameters[nr];

          if ( parm->mode == C_IN || parm->mode == C_INOUT )
            {
              (void) fprintf( pout, "%s:%s",
                              ( nr_actual_parameters++ == 0 ? "( " : ", " ), parm->proc_name ); 
            }
        }

      /* close the parameter list if there are actual parameters */
      (void) fprintf( pout, "%s;\n\
\tEND;\n\
\tEND-EXEC;\n\n",
                      ( nr_actual_parameters == 0 ? "" : " )" ) );
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

  (void) fprintf( pout, "\n\
\tif ( sqlcode != 0 )\n\
\t{\n\
\t\tepc_error = RECEIVE_ERROR;\n\
\t}\n\
\telse\n\
\t{\n" );

  /* ---------------
   * THE ACTUAL CALL 
   * --------------- */
  (void) fprintf( pout, "\t\t" );

  /* set return value if any. Use strncpy for strings */

  switch( fun->return_value.datatype )
    {
    case C_VOID:
      break;

    case C_STRING:
      (void) fprintf( pout, "(void) strncpy( %s, ", fun->return_value.name );
      break;

    default:
      (void) fprintf( pout, "%s = ", fun->return_value.name );
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
      (void) fprintf( pout, ", %d );\n\t\t%s[%d] = 0", 
                      fun->return_value.size, 
                      fun->return_value.proc_name, 
                      fun->return_value.size );
      break;

    default:
      break;
    }

  (void) fprintf( pout, ";\n\
\t}\n\
\n" );

  /* ----------------
   * Send the results
   * ---------------- */
  if ( exists_plsql_function( fun, SKEL_SEND ) != 0 )
    {
      /* call the send function */
      (void) fprintf( pout, "\
\tEXEC SQL WHENEVER SQLERROR DO epc_abort(\"-- Oracle error --\");\n\
\tEXEC SQL EXECUTE\n\
\tBEGIN\n\
\t\t%s%s.%s",
                      _interface.name, package_type_str[SKEL_SEND], fun->name );

      /*
       * Set the in/out and out parameters
       */
      for ( nr = nr_actual_parameters = 0; nr < fun->num_parameters; nr++ ) 
        {
          idl_parameter_t * parm = fun->parameters[nr];
        
          if ( parm->mode != C_IN )
            {
              (void) fprintf( pout, "%s:%s",
                              ( nr_actual_parameters++ == 0 ? "( " : ", " ), parm->proc_name ); 
            }
        }
      if ( fun->return_value.datatype != C_VOID )
        {
          (void) fprintf( pout, "%s:%s",
                          ( nr_actual_parameters++ == 0 ? "( " : ", " ), fun->return_value.proc_name ); 
        }
      (void) fprintf( pout, "%s:%s",
                      ( nr_actual_parameters++ == 0 ? "( " : ", " ), "result_pipe" ); 
      (void) fprintf( pout, "%s:%s",
                      ( nr_actual_parameters++ == 0 ? "( " : ", " ), "epc_error" ); 
      (void) fprintf( pout, "%s:%s",
                      ( nr_actual_parameters++ == 0 ? "( " : ", " ), "sqlcode" ); 

      (void) fprintf( pout, " );\n\
\tEND;\n\
\tEND-EXEC;\n\n\
\tsqlcode = sqlca.sqlcode;\n\
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

  (void) fprintf( pout, "\
\tcall->epc_error = epc_error;\n\
\tcall->sqlcode = sqlcode;\n" );

  /*
   * Print the in/out and out parameters, including Oracle error code
   */
  generate_c_debug_info( pout, fun, C_INOUT );
  generate_c_debug_info( pout, fun, C_OUT );
  print_c_debug_info( pout, "sqlcode", C_LONG );
  print_c_debug_info( pout, "epc_error", C_LONG );

  (void) fprintf( pout, "\tDBUG_LEAVE();\n}\n\n" );
}


static
void
declare_external_function( FILE * pout, idl_function_t * fun )
{
  int nr;

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
}


static
void
declare_internal_function( FILE * pout, idl_function_t * fun )
{
  (void) fprintf( pout, "extern void %s%s( epc_call_t *call );\n", EPC_PREFIX, fun->name );
}


static
void
generate_c_source ( FILE * pout, const char *include_text )
{
  int nr;
  idl_function_t * fun;

  print_generate_comment( pout, "" );

  if ( include_text != NULL )
    {
      (void) fprintf( pout, "%s\n", include_text );
    }

  (void) fprintf( pout, "#include <string.h>\n" );
  (void) fprintf( pout, "#include <stdlib.h>\n" );
  (void) fprintf( pout, "#include <epc.h>\n" );
  (void) fprintf( pout, "#include <epc_defs.h>\n" );
  (void) fprintf( pout, "#include <epc_dbg.h>\n" );
  (void) fprintf( pout, "#include \"%s.h\"\n\n", _interface.name );
  (void) fprintf( pout, "#define SQLCA_INIT\n\n" );

  /* function array */
  (void) fprintf( pout, "\nstatic epc_function_t functions[] = {\n" );
  for ( nr = 0; nr < _interface.num_functions; nr++ ) 
    {
      fun = _interface.functions[nr];
      (void) fprintf( pout, "\t{ \"" );
      print_function_signature( pout, fun );
      (void) fprintf( pout, "\", %s, %s%s }%s\n", 
                      get_constant_name( fun->return_value.datatype, C ), 
                      EPC_PREFIX,
                      fun->name,
                      ( nr < _interface.num_functions-1 ? "," : "" ) );
  }
  (void) fprintf( pout, "};\n" );

  /* interface */
  (void) fprintf( pout, "\nepc_interface_t ifc_%s = {\n", _interface.name );
  (void) fprintf( pout, "\t\"%s\",\n", _interface.name );
  (void) fprintf( pout, "\t%ld,\n", _interface.num_functions );
  (void) fprintf( pout, "\tfunctions\n" );
  (void) fprintf( pout, "};\n\n" );


  for ( nr = 0; nr < _interface.num_functions; nr++ ) {
    generate_c_function( pout, _interface.functions[nr] );
  }
}


#ifdef GEN_EPC_IFC_H
static
void
generate_interface_header( FILE * pout )
{
  print_generate_comment( pout, "" );

  (void) fprintf( pout, "#include \"%s.h\"\n\n", _interface.name );
}
#endif


static
void
generate_header( FILE *pout )
{
  int nr;

  print_generate_comment( pout, "" );

#ifdef GEN_EPC_IFC_H
  (void) fprintf( pout, "#include <epc_defs.h>\n\n" );
#endif

  /* DECLARE THE INTERNAL FUNCTIONS CALLED, BECAUSE THE COMPILER NEEDS THEM!!! */
  /* HOWEVER DO NOT DECLARE EXTERNAL FUNCTIONS WHEN THEY DO NOT NEED TO BE PRINTED */
  for ( nr = 0; nr < _interface.num_functions; nr++ ) {
    if ( print_external_function != 0 )
      declare_external_function( pout, _interface.functions[nr] );
    declare_internal_function( pout, _interface.functions[nr] );
  }

  /* interface declaration */
  (void) fprintf( pout, "\nextern epc_interface_t ifc_%s;\n", _interface.name );
}


void
generate_c( const char *include_text )
{
  char filename[256];
  FILE *pout = NULL;

  if ( print_external_function == 0 )
    print_external_function = (include_text == NULL);

  /* Create interface file */

#ifdef GEN_EPC_IFC_H
  (void) sprintf( filename, "epc_ifc.h" );
  if ( ( pout = fopen( filename, "w" ) ) == NULL ) 
    {
      goto open_error;
    }
  else
    {
      (void) fprintf( stdout, "Creating %s\n", filename );
      generate_interface_header( pout );
      (void) fclose( pout );
    }
#endif

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


  /* Generate a PRO*C file */

  (void) sprintf( filename, "%s.pc", _interface.name );
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

  return;

 open_error:
  (void) fprintf( stderr, "Cannot write file %s\n", filename );
  exit( EXIT_FAILURE );
}
