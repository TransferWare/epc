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
#ifndef GENERATE_PROC
#define GENERATE_PROC 0
#endif

#if GENEATE_PROC != 0

#define EXEC_SQL_BEGIN_DECLARE_SECTION "EXEC SQL BEGIN DECLARE SECTION"
#define EXEC_SQL_END_DECLARE_SECTION "EXEC SQL END DECLARE SECTION"

#endif

#define EPC_PREFIX "_epc_"

 /* GJP 6-6-2003 Do not send epc_error nor sqlcode back to the client */
#define SEND_EPC_ERROR 0
#define SEND_SQLCODE 0

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
#if GENERATE_PROC != 0
          (void) fprintf( pout,
                          "%s %s = 0",
                          get_syntax( parm->datatype, C ),
                          parm->proc_name );
#else
          (void) fprintf( pout,
                          "%s *%s = (%s *)function->parameters[%d].data",
                          get_syntax( parm->datatype, C ),
                          parm->proc_name,
                          get_syntax( parm->datatype, C ),
                          parameter_nr );
#endif
          break;

        case C_STRING:
#if GENERATE_PROC != 0
          /* Use the STRING PRO*C construct */
          (void) fprintf( pout, "char %s[%ld] = \"\";\n  EXEC SQL VAR %s IS STRING(%ld)", 
                          parm->proc_name, parm->size+1, parm->proc_name, parm->size+1 );
#else
          /* Use the STRING PRO*C construct */
          (void) fprintf( pout,
                          "%s %s = (%s)function->parameters[%d].data", 
                          get_syntax( parm->datatype, C ),
                          parm->proc_name,
                          get_syntax( parm->datatype, C ),
                          parameter_nr );
#endif
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


/* do we need a PL/SQL function for receiving and returning parameters? */
static
int
exists_plsql_function( idl_function_t * fun, const int package_type )
{
  int val = 0;
  int nr;

  DBUG_ENTER( "exists_plsql_function" );

  switch( package_type )
    {
    case STUB:
      val = 1;
      break;

    case SKEL_SEND:
#if GENERATE_PROC != 0
      /* oneway functions do not return any result */
      if ( fun->oneway == 0 )
        val = 1;
#endif
      break;

    case SKEL_RECV:
#if GENERATE_PROC != 0
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
#endif
      break;
    }

  DBUG_LEAVE();

  return val;
}


static
void
generate_plsql_function( FILE * pout, idl_function_t * fun, const int package_type )
{
  int nr, nr_actual_parameters /* for a SEND and RECV less parameters */;

  DBUG_ENTER( "generate_plsql_function" );

  assert( exists_plsql_function( fun, package_type ) != 0 );

  if ( package_type == STUB && fun->return_value.datatype != C_VOID )
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
          (void) fprintf( pout, "%s    ", ( nr_actual_parameters++ == 0 ? "(\n" : ",\n" ) );
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

  /* Add result, msg_info (IN), epc_error (IN) and sqlcode (IN/OUT) */
  if ( package_type == SKEL_SEND )
    {
      /* send the result */
      if ( fun->return_value.datatype != C_VOID )
        {
          (void) fprintf( pout, "%s    ", ( nr_actual_parameters++ == 0 ? "(\n" : ",\n" ) );
          print_formal_parameter( pout,
                                  "result",
                                  C_IN,
                                  fun->return_value.datatype,
                                  PLSQL );
        }

      for ( nr = 0; nr < 3; nr++ )
        {
          switch(nr)
            {
            case 0:
#if SEND_EPC_ERROR != 0
            case 1:
#endif
#if SEND_SQLCODE != 0
            case 2:
#endif
              (void) fprintf( pout, "%s    ", ( nr_actual_parameters++ == 0 ? "(\n" : ",\n" ) );

              switch(nr)
                {
                case 0:
                  print_formal_parameter( pout,
                                          "msg_info",
                                          C_IN,
                                          C_STRING,
                                          PLSQL );
                  break;

#if SEND_EPC_ERROR != 0
                case 1:
                  print_formal_parameter( pout,
                                          "epc_error",
                                          C_IN,
                                          C_INT,
                                          PLSQL );
                  break;
#endif

#if SEND_SQLCODE != 0
                case 2:
                  print_formal_parameter( pout,
                                          "sqlcode",
                                          C_INOUT,
                                          C_INT,
                                          PLSQL );
                  break;
#endif
                }
            }
        }
    }

  if ( nr_actual_parameters > 0 ) 
    {
      (void) fprintf( pout, "\n  )" );
    }

  if ( package_type == STUB && fun->return_value.datatype != C_VOID )
    {
      (void) fprintf( pout, "\n  RETURN %s", get_syntax( fun->return_value.datatype, PLSQL ) );
    }

  DBUG_LEAVE();
}


static
void
generate_plsql_function_declaration( FILE * pout, idl_function_t * fun, const int package_type )
{
  DBUG_ENTER( "generate_plsql_function_declaration" );

  if ( exists_plsql_function( fun, package_type ) != 0 )
    {
      generate_plsql_function( pout, fun, package_type );
      (void) fprintf( pout, ";\n\n" );
    }

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
  int do_print;
  int package_type;

  DBUG_ENTER( "generate_plsql_header" );

  print_generate_comment( pout, "REMARK " );

  /* Create three packagas: the STUB and two SKELETON packages (RECV from STUB and SEND) */
  for ( package_type = 0; package_type < NR_PACKAGE_TYPE; package_type++ ) {
    /* do not print an empty package */
    for ( do_print = 0, nr = 0; nr < _interface.num_functions; nr++ ) {
      if ( exists_plsql_function( _interface.functions[nr], package_type ) )
        { 
          do_print = 1;
          break;
        }
    }
    
    if ( !do_print )
      continue;

    (void) fprintf( pout, "CREATE OR REPLACE PACKAGE %s%s IS\n\n",
                    _interface.name, package_type_str[package_type] );
    
    for ( nr = 0; nr < _interface.num_functions; nr++ ) {
      generate_plsql_function_declaration( pout, _interface.functions[nr], package_type );
    }

    (void) fprintf( pout, "END %s%s;\n/\n\n", _interface.name, package_type_str[package_type] );
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
generate_plsql_function_body( FILE * pout, idl_function_t * fun, const int package_type )
{
  int nr;
  idl_parameter_t * parm;

  DBUG_ENTER( "generate_plsql_function_body" );

  /* do we need it? */
  if ( exists_plsql_function( fun, package_type ) != 0 )
    {
      generate_plsql_function( pout, fun, package_type );
      (void) fprintf( pout, "\n  IS\n" );

      /* RETURN VARIABLE */
      if ( package_type == STUB && fun->return_value.datatype != C_VOID )
        {
          (void) fprintf( pout, "    " );
          print_variable_definition( pout, &fun->return_value, PLSQL, fun->num_parameters );
          (void) fprintf( pout, ";\n" );
        }

      (void) fprintf( pout, "  BEGIN\n" );

      switch( package_type )
        {
        case STUB:
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
          break;

        case SKEL_SEND:
          /* ----------------
           * Send the results
           * ---------------- */
          (void) fprintf( pout, "\
    epc.receive_set_parameter( epc.get_msg_seq(msg_info) );\n" );

#if SEND_EPC_ERROR != 0
          (void) fprintf( pout, "\
    IF ( epc_error = 0 )\n\
    THEN\n" );
#endif
          /*
           * Set the in/out and out parameters
           */
          for ( nr = 0; nr < fun->num_parameters; nr++ ) {
            idl_parameter_t * parm = fun->parameters[nr];
        
            if ( parm->mode == C_INOUT || parm->mode == C_OUT )
              {
#if SEND_EPC_ERROR != 0
                (void) fputs( "  ", pout ); 
#endif
                (void) fprintf( pout, "    epc.receive_set_parameter( %s );\n", parm->name ); 
              }
          }
          if ( fun->return_value.datatype != C_VOID )
            {
#if SEND_EPC_ERROR != 0
              (void) fputs( "  ", pout ); 
#endif
              (void) fprintf( pout, "    epc.receive_set_parameter( %s );\n", fun->return_value.name ); 
            }

#if SEND_EPC_ERROR != 0
          /*do not forget the NULL statement in case of 0 actual parameters */
          (void) fprintf( pout, "\
      NULL;\n\
    END IF;\n" );
#endif

#if SEND_SQLCODE != 0
          (void) fprintf( pout, "\
    sqlcode := dbms_pipe.send_message( epc.get_result_pipe(msg_info), 1 );\n\
  EXCEPTION\n\
    WHEN  OTHERS\n\
    THEN\n\
      sqlcode := SQLCODE;\n" );
#else
          (void) fprintf( pout, "\
    DECLARE\n\
      v_result PLS_INTEGER;\n\
    BEGIN\n\
      v_result := dbms_pipe.send_message( epc.get_result_pipe(msg_info), 1 );\n\
      IF v_result = 1\n\
      THEN\n\
        RAISE epc.msg_timed_out;\n\
      ELSIF v_result = 3\n\
      THEN\n\
        RAISE epc.msg_interrupted;\n\
      ELSIF v_result <> 0\n\
      THEN\n\
        RAISE VALUE_ERROR;\n\
      END IF;\n\
    END;\n" );
#endif
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
                  (void) fprintf( pout, "    epc.request_get_parameter( %s );\n", parm->name );
                }
            }
          break;
        }

      (void) fprintf( pout, "  END;\n\n" );
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
    }

  DBUG_LEAVE();
}


static
void
generate_plsql_body( FILE * pout )
{
  int nr;
  int do_print;
  int package_type;

  DBUG_ENTER( "generate_plsql_body" );

  print_generate_comment( pout, "REMARK " );

  for ( package_type = 0; package_type < NR_PACKAGE_TYPE; package_type++ ) 
    {
      /* do not print an empty package body */
      for ( do_print = 0, nr = 0; nr < _interface.num_functions; nr++ ) {
        if ( exists_plsql_function( _interface.functions[nr], package_type ) )
          { 
            do_print = 1;
            break;
          }
      }
    
      if ( !do_print )
        continue;

      (void) fprintf( pout, "CREATE OR REPLACE PACKAGE BODY %s%s IS\n\n\
  g_epc_key epc_clnt.epc_key_subtype;\n\n",
                      _interface.name, package_type_str[package_type] );
      (void) fprintf( pout, "\n" );

      for ( nr = 0; nr < _interface.num_functions; nr++ ) 
        {
          generate_plsql_function_body( pout, _interface.functions[nr], package_type );
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

  (void) fprintf( pout, "void %s%s ( epc_function_t *function )\n", EPC_PREFIX, fun->name );
  (void) fprintf( pout, "{\n" );

#if GENERATE_PROC != 0
  (void) fprintf( pout, "\
#ifdef SQLCA\n\
#undef SQLCA\n\
#endif\n\
EXEC SQL INCLUDE sqlca;\n\n" );

  /* 
   * VARIABLE TO HOLD RETURN VALUE 
   */

  (void) fprintf( pout, "  %s;\n", EXEC_SQL_BEGIN_DECLARE_SECTION );
  (void) fprintf( pout, "  long epc_error = OK;\n" );
  (void) fprintf( pout, "  long sqlcode = 0;\n" );
  (void) fprintf( pout, "  const char *msg_info = call->msg_info;\n" );
#endif

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

#if GENERATE_PROC != 0
  (void) fprintf( pout, "  %s;\n", EXEC_SQL_END_DECLARE_SECTION );
#endif

  (void) fprintf( pout, "\n  DBUG_ENTER( \"%s\" );\n",
                  fun->name );

#if GENERATE_PROC != 0
  /*
   * Get the in and in/out parameters
   */
  if ( exists_plsql_function( fun, SKEL_RECV ) != 0 )
    {
      /* call the recv function */
      (void) fprintf( pout, "\
  EXEC SQL WHENEVER SQLERROR DO epc_abort(\"-- Oracle error --\");\n\
  EXEC SQL EXECUTE\n\
  BEGIN\n\
    %s%s.%s", _interface.name, package_type_str[SKEL_RECV], fun->name );

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
  END;\n\
  END-EXEC;\n\n",
                      ( nr_actual_parameters == 0 ? "" : " )" ) );
    }
#endif

  /* ---------------------------------------------------------------
   * Print the in and in/out parameters, including Oracle error code
   * --------------------------------------------------------------- */

  generate_c_debug_info( pout, fun, C_IN );
  generate_c_debug_info( pout, fun, C_INOUT );

  (void) fprintf( pout, "\n" );

#if GENERATE_PROC != 0
  print_c_debug_info( pout, "sqlcode", C_LONG );

  /* -----------------
   * HANDLE SQL ERRORS
   * ----------------- */

  (void) fprintf( pout, "\
  if ( sqlcode != 0 )\n\
  {\n\
    epc_error = RECEIVE_ERROR;\n\
  }\n\
  else\n\
  {\n  " );
#endif

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
#if GENERATE_PROC != 0
                      "  ",
#else
                      "",
#endif                      
                      fun->return_value.proc_name, 
                      fun->return_value.size );
      break;

    default:
      break;
    }

  (void) fprintf( pout, ";\n" );

#if GENERATE_PROC == 0

  (void) fprintf( pout, "\n" );

#else

  (void) fprintf( pout, "\
  }\n\
\n" );

  /* ----------------
   * Send the results
   * ---------------- */
  if ( exists_plsql_function( fun, SKEL_SEND ) != 0 )
    {
      /* call the send function */
      (void) fprintf( pout, "\
  EXEC SQL WHENEVER SQLERROR DO epc_abort(\"-- Oracle error --\");\n\
  EXEC SQL EXECUTE\n\
  BEGIN\n\
    %s%s.%s",
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
                      ( nr_actual_parameters++ == 0 ? "( " : ", " ), "msg_info" ); 
#if SEND_EPC_ERROR != 0
      (void) fprintf( pout, "%s:%s",
                      ( nr_actual_parameters++ == 0 ? "( " : ", " ), "epc_error" ); 
#endif
#if SEND_SQLCODE != 0
      (void) fprintf( pout, "%s:%s",
                      ( nr_actual_parameters++ == 0 ? "( " : ", " ), "sqlcode" );
#endif

      (void) fprintf( pout, " );\n\
  EXCEPTION\n\
    WHEN epc.msg_timed_out\n\
    THEN :epc_error := :msg_timed_out;\n\
    WHEN epc.msg_interrupted\n\
    THEN :epc_error := :msg_interrupted;\n\
    WHEN OTHERS\n\
    THEN :epc_error := :send_error; :sqlcode := SQLCODE;\n\
  END;\n\
  END-EXEC;\n\n" );
    }

  (void) fprintf( pout, "\
  call->epc_error = epc_error;\n\
  call->sqlcode = sqlcode;\n" );
#endif

  /*
   * Print the in/out and out parameters, including Oracle error code
   */
  generate_c_debug_info( pout, fun, C_INOUT );
  generate_c_debug_info( pout, fun, C_OUT );
#if GENERATE_PROC != 0
  print_c_debug_info( pout, "sqlcode", C_LONG );
  print_c_debug_info( pout, "epc_error", C_LONG );
#endif

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

  (void) fprintf( pout, "extern void %s%s( epc_function_t *function );\n", EPC_PREFIX, fun->name );

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

#if GENERATE_PROC != 0

  (void) fprintf( pout, "\
EXEC SQL INCLUDE \"epc_defs.h\";\n\
EXEC SQL BEGIN DECLARE SECTION;\n\
static const int msg_timed_out = MSG_TIMED_OUT;\n\
static const int msg_interrupted = MSG_INTERRUPTED;\n\
static const int send_error = SEND_ERROR;\n\
EXEC SQL END DECLARE SECTION;\n" );

#endif

  (void) fprintf( pout, "\
#include <epc_dbg.h>\n\
#include \"%s.h\"\n\n", _interface.name );

#if GENERATE_PROC != 0

  (void) fprintf( pout, "\
#define SQLCA_INIT\n\n" );

#endif

  /* parameter array */
  for ( fnr = 0; fnr < _interface.num_functions; fnr++ ) 
    {
      fun = _interface.functions[fnr];
      (void) fprintf( pout, "\nstatic epc_parameter_t %s_parameters[] = {\n", fun->name );
      for ( pnr = 0; pnr < fun->num_parameters; pnr++ )
        {
          (void) fprintf( pout, "  { \"%s\", %s, %s, %ld, NULL },\n",
                          fun->parameters[pnr]->name,
                          get_constant_name( fun->parameters[pnr]->mode, C ), 
                          get_constant_name( fun->parameters[pnr]->datatype, C ),
                          fun->parameters[pnr]->size );
        }
      (void) fprintf( pout, "  { \"%s\", %s, %s, %ld, NULL }\n", 
                      fun->return_value.name,
                      get_constant_name( fun->return_value.mode, C ), 
                      get_constant_name( fun->return_value.datatype, C ),
                      fun->return_value.size );
      (void) fprintf( pout, "};\n" );
  }

  /* function array */
  (void) fprintf( pout, "\nstatic epc_function_t functions[] = {\n" );
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
  (void) fprintf( pout, "\nepc_interface_t ifc_%s = {\n", _interface.name );
  (void) fprintf( pout, "  \"%s\",\n", _interface.name );
  (void) fprintf( pout, "  %ld,\n", _interface.num_functions );
  (void) fprintf( pout, "  functions\n" );
  (void) fprintf( pout, "};\n\n" );


  for ( fnr = 0; fnr < _interface.num_functions; fnr++ ) {
    generate_c_function( pout, _interface.functions[fnr] );
  }

  DBUG_LEAVE();
}


#ifdef GEN_EPC_IFC_H
static
void
generate_interface_header( FILE * pout )
{
  DBUG_ENTER( "generate_interface_header" );

  print_generate_comment( pout, "" );

  (void) fprintf( pout, "#include \"%s.h\"\n\n", _interface.name );

  DBUG_LEAVE();
}
#endif


static
void
generate_header( FILE *pout )
{
  int nr;

  DBUG_ENTER( "generate_header" );

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


#if GENERATE_PROC != 0
  /* Generate a PRO*C file */
  (void) sprintf( filename, "%s.pc", _interface.name );
#else
  (void) sprintf( filename, "%s.c", _interface.name );
#endif

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


