/*
 * Filename             : idl_prs.c
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
 * Revision 1.40  2005/01/03 12:26:42  gpaulissen
 * Release 4.4.0
 *
 * Revision 1.39  2004/12/28 12:18:10  gpaulissen
 * Test on Amazon
 *
 * Revision 1.38  2004/12/20 13:29:16  gpaulissen
 * make lint
 *
 * Revision 1.37  2004/12/16 16:03:22  gpaulissen
 * Web services added
 *
 * Revision 1.36  2004/10/21 11:54:32  gpaulissen
 * indent *.c *.h
 *
 * Revision 1.35  2004/10/21 11:02:40  gpaulissen
 * length checking added to PL/SQL
 *
 * Revision 1.34  2004/10/20 13:34:05  gpaulissen
 * make lint
 *
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

#include "epc.h"
#include "idl_prs.h"
#include "idl_defs.h"
#include "idl_kwrd.h"

#include <dbug.h>

/* include dmalloc as last one */
#ifdef WITH_DMALLOC
#include "dmalloc.h"
#endif

/*
|| Defines
*/
#define EXEC_SQL_BEGIN_DECLARE_SECTION "EXEC SQL BEGIN DECLARE SECTION"
#define EXEC_SQL_END_DECLARE_SECTION "EXEC SQL END DECLARE SECTION"
#define EPC_PREFIX "_epc_"

 /* GJP 6-6-2003 Do not send epc__error nor sqlcode back to the client */
#define SEND_EPC__ERROR 0
#define SEND_SQLCODE 0

/*
  GJP 03-01-2005 
  Do not define local variables anymore. 
  Bounds checking is disabled.

  GJP 10-08-2007
  Enable bounds checking again but now as an extra length parameter to 
  set_request_parameter/get_response_parameter
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
#define GENERATE_PROC 0

#if GENERATE_PROC
#define SKEL_RECV 1
#define SKEL_SEND 2
#endif

static char *package_type_str[] = 
  { ""
#if GENERATE_PROC
    , "_recv", "_send"
#endif /* #if GENERATE_PROC */
 };

#define NR_PACKAGE_TYPE ((int)(sizeof(package_type_str)/sizeof(package_type_str[0])))

/*
|| Global variables. See epc.pks
*/
keyword keywords[] = {
  {C_VOID,
   {
    {C, "void", "C_VOID"},
    {PLSQL, "??", "??"}
    }
   },
  {C_INT,
   {
    {C, "int", "C_INT"},
    {PLSQL, "epc.int_subtype", "epc.data_type_int"}
    }
   },
  {C_LONG,
   {
    {C, "long", "C_LONG"},
    {PLSQL, "epc.long_subtype", "epc.data_type_long"}
    }
   },
  {C_FLOAT,
   {
    {C, "float", "C_FLOAT"},
    {PLSQL, "epc.float_subtype", "epc.data_type_float"}
    }
   },
  {C_DOUBLE,
   {
    {C, "double", "C_DOUBLE"},
    {PLSQL, "epc.double_subtype", "epc.data_type_double"}
    }
   },
  {C_STRING,
   {
    /* string is a typedef */
    {C, "char*", "C_STRING"},
    {PLSQL, "epc.string_subtype", "epc.data_type_string"}
    }
   },
  {C_XML,
   {
    {C, "char*", "C_XML"},
    {PLSQL, "epc.xml_subtype", "epc.data_type_xml"}
    }
   },
  {C_DATE,
   {
    {C, "char*", "C_DATE"},
    {PLSQL, "epc.date_subtype", "epc.data_type_date"}
    }
   },
  {C_IN,
   {
    {C, "", "C_IN"},
    {PLSQL, "IN", "??epc.c_in"}
    }
   },
  {C_OUT,
   {
    {C, "*", "C_OUT"},
    {PLSQL, "OUT NOCOPY", "??epc.c_out"}
    }
   },
  {C_INOUT,
   {
    {C, "*", "C_INOUT"},
    {PLSQL, "IN OUT NOCOPY", "??epc.c_inout"}
    }
   }
};

static int print_external_function = 0; /* print extern <function> in header */

/*
|| Global functions
*/

void
set_interface (char *name)
{
  DBUG_ENTER ("set_interface");

  assert (strlen (name) < sizeof (_interface.name));    /* beware of the trailing '\0' */
  (void) strcpy (_interface.name, name);
  _interface.num_functions = 0;

  DBUG_LEAVE ();
}

static void
init_parameter ( /*@out@ */ idl_parameter_t * parm, char *name,
                idl_mode_t mode, idl_type_t datatype, dword_t size)
{
  DBUG_ENTER ("init_parameter");

  assert (strlen (name) < sizeof (parm->name)); /* beware of the trailing '\0' */
  (void) strcpy (parm->name, name);
  assert (strlen (name) < sizeof (parm->proc_name));    /* beware of the trailing '\0' */
  (void) strcpy (parm->proc_name, name);

  parm->mode = mode;
  switch (mode)
    {
    case C_IN:
    case C_INOUT:
    case C_OUT:
      break;
    default:
      (void) fprintf (stderr,
                      "(init_parameter) Mode %ld of parameter %s unknown.\n",
                      (long) mode, name);
      exit (EXIT_FAILURE);
    }

  parm->datatype = datatype;
  switch (datatype)
    {
    case C_STRING:
    case C_XML:
      parm->size = size;
      break;

    case C_DATE:
      parm->size = MAX_DATE_LEN;
      break;

    case C_VOID:
    case C_INT:
    case C_LONG:
    case C_DOUBLE:
    case C_FLOAT:
      parm->size = 0;
      break;

    default:
      (void) fprintf (stderr,
                      "(init_parameter) Type %ld of parameter %s unknown.\n",
                      (long) datatype, name);
      exit (EXIT_FAILURE);
    }

  DBUG_LEAVE ();
}


void
add_function (const char *name, const idl_type_t datatype, const dword_t size, const dword_t oneway)
{
  _interface.functions[_interface.num_functions] =
    (idl_function_t *) malloc (sizeof (idl_function_t));

  DBUG_ENTER ("add_function");

  assert (_interface.functions[_interface.num_functions] != NULL);
  assert (strlen (name) < sizeof (_interface.functions[_interface.num_functions]->name));       /* beware of the trailing '\0' */
  (void) strcpy (_interface.functions[_interface.num_functions]->name, name);
  assert(oneway == 0L || oneway == 1L);
  _interface.functions[_interface.num_functions]->oneway = oneway;
  assert(_interface.functions[_interface.num_functions]->oneway == 0L || _interface.functions[_interface.num_functions]->oneway == 1L);
  _interface.functions[_interface.num_functions]->num_parameters = 0;

  init_parameter (&_interface.functions[_interface.num_functions]->
                  return_value, "result", C_OUT, datatype, size);

  _interface.num_functions++;
  
  DBUG_LEAVE ();
}


void
add_parameter (char *name, idl_mode_t mode, idl_type_t datatype, dword_t size)
{
  idl_function_t *fun = _interface.functions[_interface.num_functions - 1];
  idl_parameter_t *parm =
    (idl_parameter_t *) malloc (sizeof (idl_parameter_t));

  DBUG_ENTER ("add_parameter");

  assert (parm != NULL);
  init_parameter (parm, name, mode, datatype, size);

  fun->parameters[fun->num_parameters] = parm;
  fun->num_parameters++;

  DBUG_LEAVE ();
}


static
/*@notnull@*/
/*@observer@*/
mapping *
get_mapping (const dword_t key, const idl_lang_t language)
{
  size_t i, j;
  const size_t num_keywords = sizeof (keywords) / sizeof (keyword);

  for (i = 0; i < num_keywords; i++)
    {
      if (keywords[i].key == key)
        {
          for (j = 0; j < NUM_LANGUAGES; j++)
            {
              if (keywords[i].mappings[j].language == language)
                {
                  return &keywords[i].mappings[j];
                }
            }
          (void) fprintf (stderr, "No mapping for %ld in language %ld\n",
                          key, language);
          exit (EXIT_FAILURE);
        }
    }
  fprintf (stderr, "Key %ld not a valid keyword\n", (long) key);
  exit (EXIT_FAILURE);
}


static
/*@observer@*/
char *
get_syntax (const dword_t key, const idl_lang_t language)
{
  return get_mapping (key, language)->syntax;
}


static
/*@observer@*/
char *
get_constant_name (const dword_t key, const idl_lang_t language)
{
  return get_mapping (key, language)->constant_name;
}

static
/*@observer@*/
char *
get_size (const idl_parameter_t * idl_parameter)
{
  static char size_str[100] = "";

  switch (idl_parameter->datatype)
    {
    case C_STRING:
    case C_XML:
    case C_DATE:
      (void) snprintf (size_str, sizeof (size_str), "%ld+1",
                       (long) idl_parameter->size);
      break;

    case C_VOID:
      (void) snprintf (size_str, sizeof (size_str), "0");
      break;

    case C_INT:
      (void) snprintf (size_str, sizeof (size_str), "sizeof(idl_int_t)");
      break;

    case C_LONG:
      (void) snprintf (size_str, sizeof (size_str), "sizeof(idl_long_t)");
      break;

    case C_DOUBLE:
      (void) snprintf (size_str, sizeof (size_str), "sizeof(idl_double_t)");
      break;

    case C_FLOAT:
      (void) snprintf (size_str, sizeof (size_str), "sizeof(idl_float_t)");
      break;
    }

  return size_str;
}

static void
print_formal_parameter (FILE * pout,
                        const char *name,
                        const idl_mode_t mode,
                        const idl_type_t datatype, const idl_lang_t lang)
{
  DBUG_ENTER ("print_formal_parameter");

  switch (lang)
    {
    case C:
      (void) fprintf (pout, "%s %s%s",
                      get_syntax (datatype, lang),
                      (mode != C_IN && ( datatype != C_STRING && datatype != C_XML && datatype != C_DATE ) ? "*" : ""),
                      name);
      break;

    case PLSQL:
      (void) fprintf (pout, "%s %s %s",
                      name,
                      get_syntax (mode, lang), get_syntax (datatype, lang));
      break;
    }

  DBUG_LEAVE ();
}


static void
print_actual_parameter (FILE * pout, idl_parameter_t * parm, idl_lang_t lang)
{
  DBUG_ENTER ("print_actual_parameter");

  switch (lang)
    {
    case C:
      (void) fprintf (pout, "%sl_%s",
                      (parm->mode == C_IN
                       && ( parm->datatype != C_STRING && parm->datatype != C_XML && parm->datatype != C_DATE ) ? "*" : ""),
                      parm->proc_name);
      break;
    case PLSQL:
      (void) fprintf (pout, "%s", parm->name);
      break;
    }

  DBUG_LEAVE ();
}


static void
print_variable_definition (FILE * pout, idl_parameter_t * parm,
                           idl_lang_t lang, const int parameter_nr,
                           const int pc_source)
{
  DBUG_ENTER ("print_variable_definition");

  assert( parm->datatype != C_VOID ); /* impossible */

  switch (lang)
    {
    case C:
      switch (parm->datatype)
        {
        case C_INT:
        case C_LONG:
        case C_FLOAT:
        case C_DOUBLE:
          (void) fprintf (pout,
                          "%s *l_%s = (%s *)function->parameters[%d].data",
                          get_syntax (parm->datatype, C),
                          parm->proc_name,
                          get_syntax (parm->datatype, C), parameter_nr);
          break;

        case C_STRING:
        case C_XML:
        case C_DATE:
          (void) fprintf( pout, "%s l_%s =  (%s)function->parameters[%d].data", 
                          get_syntax (parm->datatype, C),
                          parm->proc_name,
                          get_syntax (parm->datatype, C),
                          parameter_nr );
          /* Use the STRING PRO*C construct to terminate the STRING */
          if (pc_source != 0) {
            (void) fprintf( pout, ";\n  EXEC SQL VAR l_%s IS STRING(%ld)", 
                            parm->proc_name,
                            parm->size+1 );
          }
          break;

        case C_VOID:
          break;
        }
      break;

    case PLSQL:
      switch (parm->datatype)
        {
        case C_INT:
        case C_LONG:
        case C_FLOAT:
        case C_DOUBLE:
          (void) fprintf (pout, "l_%s %s",
                          parm->name, get_syntax (parm->datatype, PLSQL));
          break;

        case C_DATE:
          (void) fprintf (pout, "l_%s DATE", parm->name);
          break;

        case C_STRING:
        case C_XML:
          (void) fprintf (pout, "l_%s VARCHAR2(%ld BYTE)", parm->name, parm->size);
          break;

        case C_VOID:
          break;
        }
      if (parm->mode == C_IN || parm->mode == C_INOUT)
        {
          (void) fprintf (pout, " := %s", parm->name);
        }
      break;
    }

  DBUG_LEAVE ();
}


/* do we need a PL/SQL function for receiving and returning parameters? */
static
int
exists_plsql_function( /*@unused@ */ idl_function_t * fun, const int package_type )
{
  int val = 0;
#if GENERATE_PROC
  dword_t nr;
#endif /* #if GENERATE_PROC */

  switch( package_type )
    {
    case STUB:
      val = 1;
      break;

#if GENERATE_PROC
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
      break;
#endif /* #if GENERATE_PROC */

    default:
      break;
    }

  return val;
}


static void
generate_plsql_function (FILE * pout, idl_function_t * fun, const int package_type)
{
  dword_t nr;
  dword_t nr_actual_parameters = 0;

  DBUG_ENTER ("generate_plsql_function");

  assert( exists_plsql_function( fun, package_type ) != 0 );

  if (package_type == STUB && fun->return_value.datatype != C_VOID)
    {
      (void) fprintf (pout, "  FUNCTION %s", fun->name);
    }
  else
    {
      (void) fprintf (pout, "  PROCEDURE %s", fun->name);
    }

  /* PARAMETERS */
  for (nr = 0; nr < fun->num_parameters; nr++)
    {
      switch(package_type)
        {
#if GENERATE_PROC

        case SKEL_SEND: /* in/out parameters, out parameters back to client */
        case SKEL_RECV: /* in and in/out parameters from client */

          /* a parameter is skipped if the following condition is true */
          if ( ( package_type == SKEL_RECV && fun->parameters[nr]->mode == C_OUT ) ||
               ( package_type == SKEL_SEND && fun->parameters[nr]->mode == C_IN ) )
            break; 

          /*@fallthrough@*/
#endif /* #if GENERATE_PROC */
        case STUB:
          (void) fprintf (pout, "%s    ", ( nr_actual_parameters++ == 0 ? "(\n" : ",\n" ));
          print_formal_parameter (pout,
                                  fun->parameters[nr]->name,
                                  ( package_type == STUB ?
                                    fun->parameters[nr]->mode :
                                    ( 
#if GENERATE_PROC
                                     package_type == SKEL_RECV ? C_OUT :
#endif /* #if GENERATE_PROC */
                                     C_IN ) ),
                                  fun->parameters[nr]->datatype,
                                  PLSQL);
        }
    }

#if GENERATE_PROC
  /* Add result, msg_info (IN), epc__error (IN) and sqlcode (IN/OUT) */
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
#if SEND_EPC__ERROR != 0
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

#if SEND_EPC__ERROR != 0
                case 1:
                  print_formal_parameter( pout,
                                          "error_code",
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
#endif /* #if GENERATE_PROC */

  if (nr_actual_parameters > 0)
    {
      (void) fprintf (pout, "\n  )");
    }

  if (package_type == STUB && fun->return_value.datatype != C_VOID)
    {
      (void) fprintf (pout, "\n  RETURN %s",
                      get_syntax (fun->return_value.datatype, PLSQL));
    }

  DBUG_LEAVE ();
}


static void
generate_plsql_function_declaration (FILE * pout, idl_function_t * fun, const int package_type)
{
  DBUG_ENTER ("generate_plsql_function_declaration");

  if (exists_plsql_function( fun, package_type ) != 0)
    {
      generate_plsql_function (pout, fun, package_type);
      (void) fprintf (pout, ";\n\n");
    }

  DBUG_LEAVE ();
}


static void
print_generate_comment (FILE * pout, char *prefix)
{
  DBUG_ENTER ("print_generate_comment");

  (void) fprintf (pout, "%s/*******************************\n", prefix);
  (void) fprintf (pout, "%s ** Generated by EPC compiler **\n", prefix);
  (void) fprintf (pout, "%s *******************************/\n\n", prefix);

  DBUG_LEAVE ();
}


static void
generate_plsql_header (FILE * pout)
{
  dword_t nr;
  int package_type;

  DBUG_ENTER ("generate_plsql_header");

  print_generate_comment (pout, "REMARK ");

  /* Create three packagas: the STUB and two SKELETON packages (RECV from STUB and SEND) */
  for ( package_type = 0; package_type < NR_PACKAGE_TYPE; package_type++ ) {
    /* do not print an empty package */
    if (_interface.num_functions > 0)
      {
        (void) fprintf( pout, "CREATE OR REPLACE PACKAGE %s%s AUTHID DEFINER IS\n\n",
                        _interface.name, package_type_str[package_type] );
    
        for ( nr = 0; nr < _interface.num_functions; nr++ ) {
          generate_plsql_function_declaration( pout, _interface.functions[nr], package_type );
        }

        (void) fprintf( pout, "END %s%s;\n/\n\n", _interface.name, package_type_str[package_type] );
      }
  }

  DBUG_LEAVE ();
}


static void
generate_plsql_function_body (FILE * pout, idl_function_t * fun, const int package_type)
{
  dword_t nr;
  idl_parameter_t *parm;

  DBUG_ENTER ("generate_plsql_function_body");

  /* do we need it? */
  if ( exists_plsql_function( fun, package_type ) != 0 )
    {
      generate_plsql_function (pout, fun, package_type);
      (void) fprintf (pout, "\n  IS\n");

      /* RETURN VARIABLE */
      if (package_type == STUB) 
        {
          if (fun->return_value.datatype != C_VOID)
            {
              (void) fprintf (pout, "    ");
              print_variable_definition (pout, &fun->return_value, PLSQL,
                                         (int) fun->num_parameters, 0);
              (void) fprintf (pout, ";\n");
            }
          (void) fprintf (pout, 
                          "    l_epc_clnt_object epc_clnt_object := new epc_clnt_object('%s');\n",
                          _interface.name);
        }

      (void) fprintf (pout, "  BEGIN\n");

      switch (package_type)
        {
        case STUB:
          /* SETUP OF CLIENT SIDE FUNCTION CALL */
          (void) fprintf (pout, 
                          "    BEGIN\n");
          (void) fprintf (pout,
                          "      epc_clnt.new_request(l_epc_clnt_object, '%s', %ld);\n",
                          fun->name, (long) fun->oneway);

          for (nr = 0; nr < fun->num_parameters; nr++)
            {
              parm = fun->parameters[nr];

              if (parm->mode != C_OUT) 
                {
                  switch (parm->datatype)
                    {
                    case C_INT:
                    case C_LONG:
                    case C_FLOAT:
                    case C_DOUBLE:
                    case C_DATE:
                      (void) fprintf (pout,
                                      "      epc_clnt.set_request_parameter(l_epc_clnt_object, '%s', %s, %s);\n",
                                      parm->name,
                                      get_constant_name (parm->datatype, PLSQL),
                                      parm->name);
                      break;

                    case C_STRING:
                    case C_XML:
                      (void) fprintf (pout,
                                      "      epc_clnt.set_request_parameter(l_epc_clnt_object, '%s', %s, %s, %ld);\n",
                                      parm->name,
                                      get_constant_name (parm->datatype, PLSQL),
                                      parm->name,
                                      parm->size);
                      break;

                    case C_VOID:
                      break;
                    }
                }
            }

          (void) fprintf (pout,
                          "      epc_clnt.send_request(l_epc_clnt_object);\n");

          if (fun->oneway == 0)
            {
              (void) fprintf (pout, "      epc_clnt.recv_response(l_epc_clnt_object);\n");
            }

          /* GET THE RESULTS */
          for (nr = 0; nr < fun->num_parameters; nr++)
            {
              parm = fun->parameters[nr];
              if (parm->mode != C_IN)
                {
                  switch (parm->datatype)
                    {
                    case C_INT:
                    case C_LONG:
                    case C_FLOAT:
                    case C_DOUBLE:
                    case C_DATE:
                      (void) fprintf (pout,
                                      "      epc_clnt.get_response_parameter(l_epc_clnt_object, '%s', %s, %s);\n",
                                      parm->name,
                                      get_constant_name (parm->datatype, PLSQL),
                                      parm->name);
                      break;
                      
                    case C_STRING:
                    case C_XML:
                      (void) fprintf (pout,
                                      "      epc_clnt.get_response_parameter(l_epc_clnt_object, '%s', %s, %s, %ld);\n",
                                      parm->name,
                                      get_constant_name (parm->datatype, PLSQL),
                                      parm->name,
                                      parm->size);
                      break;

                    case C_VOID:
                      break;
                    }
                }
            }

          if (fun->return_value.datatype != C_VOID)
            {
              (void) fprintf (pout,
                              "      epc_clnt.get_response_parameter(l_epc_clnt_object, '%s', %s, l_%s);\n",
                              fun->return_value.name,
                              get_constant_name (fun->return_value.datatype, PLSQL),
                              fun->return_value.name);
            }

          (void) fprintf (pout,
                          "    EXCEPTION\n");
          (void) fprintf (pout,
                          "      WHEN OTHERS\n");
          (void) fprintf (pout,
                          "      THEN\n");
          (void) fprintf (pout,
                          "        l_epc_clnt_object.store();\n");
          (void) fprintf (pout,
                          "        RAISE;\n");
          (void) fprintf (pout,
                          "    END;\n");

          (void) fprintf (pout,
                          "    l_epc_clnt_object.store();\n");

          if (fun->return_value.datatype != C_VOID)
            {
              (void) fprintf (pout, "    RETURN l_%s;\n", fun->return_value.name);
            }
          break;

#if GENERATE_PROC
        case SKEL_SEND:
          /* ----------------
           * Send the results
           * ---------------- */
          (void) fprintf( pout, "\
    epc_srvr.new_response( p_epc_key => epc_srvr.get_epc_key, p_msg_info => msg_info );\n" );

#if SEND_EPC__ERROR != 0
          (void) fprintf( pout, "\
    IF ( error_code = %d )\n\
    THEN\n", OK );
#endif
          /*
           * Set the in/out and out parameters
           */
          for ( nr = 0; nr < fun->num_parameters; nr++ ) {
            parm = fun->parameters[nr];
        
            if ( parm->mode == C_INOUT || parm->mode == C_OUT )
              {
#if SEND_EPC__ERROR != 0
                (void) fputs( "  ", pout ); 
#endif
                (void) fprintf( pout, "    epc_srvr.set_response_parameter( epc_srvr.get_epc_key, '%s', %s );\n", parm->name, parm->name ); 
              }
          }
          if ( fun->return_value.datatype != C_VOID )
            {
#if SEND_EPC__ERROR != 0
              (void) fputs( "  ", pout ); 
#endif
              (void) fprintf( pout, "    epc_srvr.set_response_parameter( epc_srvr.get_epc_key, '%s', %s );\n", fun->return_value.name, fun->return_value.name ); 
            }

#if SEND_EPC__ERROR != 0
          /*do not forget the NULL statement in case of 0 actual parameters */
          (void) fprintf( pout, "\
      NULL;\n\
    END IF;\n" );
#endif

#if SEND_SQLCODE != 0
          (void) fprintf( pout, "\
    epc_srvr.send_response( p_epc_key => epc_srvr.get_epc_key, p_msg_info => msg_info );\n\
  EXCEPTION\n\
    WHEN  OTHERS\n\
    THEN\n\
      sqlcode := SQLCODE;\n" );
#else
          (void) fprintf( pout, "\
    epc_srvr.send_response( p_epc_key => epc_srvr.get_epc_key, p_msg_info => msg_info );\n" );
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
                  (void) fprintf( pout, "    epc_srvr.get_request_parameter( epc_srvr.get_epc_key, '%s', %s );\n", parm->name, parm->name );
                }
            }
          break;
#endif /* #if GENERATE_PROC */
        }

      (void) fprintf( pout, "  END;\n\n" );
    }

  DBUG_LEAVE();
}


static void
generate_plsql_function_body_ext (FILE * pout, idl_function_t * fun)
{
  dword_t nr;
  const int package_type = STUB;

  DBUG_ENTER ("generate_plsql_function_body_ext");
  DBUG_PRINT ("input",
              ("interface: %s; function: %s", _interface.name, fun->name));

  /* do we need it? */
  if ( exists_plsql_function( fun, package_type ) != 0 )
    {
      generate_plsql_function(pout, fun, package_type);

      (void) fprintf (pout, " AS\n\
  EXTERNAL LIBRARY %s_lib\n\
  NAME \"%s\"\n\
  LANGUAGE C", _interface.name, fun->name);

      DBUG_PRINT ("info",
                  ("return datatype: %ld; # parms: %ld",
                   (int) fun->return_value.datatype, (int) fun->num_parameters));

      /* PARAMETERS CLAUSE ? */
      if (fun->return_value.datatype != C_VOID || fun->num_parameters > 0)
        {
          (void) fprintf (pout, "\n\
  PARAMETERS (\n");

          for (nr = 0; nr < fun->num_parameters; nr++)
            {
              (void) fprintf (pout, "  %s  %s %s\n",
                              (nr == 0 ? " " : ","),
                              fun->parameters[nr]->name,
                              get_syntax (fun->parameters[nr]->datatype, C));
            }

          /* RETURN VARIABLE */
          switch (fun->return_value.datatype)
            {
            case C_VOID:
              break;

            default:
              (void) fprintf (pout, "  %s  RETURN %s\n",
                              (nr == 0 ? " " : ","),
                              get_syntax (fun->return_value.datatype, C));
              break;
            }

          (void) fprintf (pout, "\
  )");
        }

      (void) fprintf (pout, ";\n\n");
    }

  DBUG_LEAVE ();
}


static void
generate_plsql_body (FILE * pout)
{
  dword_t nr;
  int package_type;

  DBUG_ENTER ("generate_plsql_body");

  print_generate_comment (pout, "REMARK ");

  for ( package_type = 0; package_type < NR_PACKAGE_TYPE; package_type++ ) 
    {
      /* do not print an empty package body */
      if (_interface.num_functions > 0)
        {
          (void) fprintf (pout, "CREATE OR REPLACE PACKAGE BODY %s%s IS\n",
                          _interface.name, package_type_str[package_type]);
          (void) fprintf (pout, "\n");

          for (nr = 0; nr < _interface.num_functions; nr++)
            {
              generate_plsql_function_body (pout, _interface.functions[nr], package_type);
            }

          (void) fprintf( pout, "END;\n" );

          (void) fprintf( pout, "/\n\nSHOW ERRORS\n\n" );
        }
    }

  DBUG_LEAVE ();
}


static void
generate_plsql_body_ext (FILE * pout)
{
  dword_t nr;

  DBUG_ENTER ("generate_plsql_body_ext");

  print_generate_comment (pout, "REMARK ");

  (void) fprintf (pout, "\
WHENEVER SQLERROR CONTINUE\n\
DROP LIBRARY %s_lib\n\
/\n\
WHENEVER SQLERROR EXIT FAILURE\n\
CREATE LIBRARY %s_lib AS '&%s_lib'\n\
/\n\
CREATE OR REPLACE PACKAGE BODY %s IS\n\n", _interface.name, _interface.name, _interface.name, _interface.name);

  for (nr = 0; nr < _interface.num_functions; nr++)
    {
      generate_plsql_function_body_ext (pout, _interface.functions[nr]);
    }

  (void) fprintf (pout, "END;\n");
  (void) fprintf (pout, "/\n");

  DBUG_LEAVE ();
}


void
generate_plsql (void)
{
  char filename[256] = "";
#define NR_PLSQL_FILES 4
  FILE * pout;
  int nr;

  DBUG_ENTER ("generate_plsql");

  for (nr = 0; nr < NR_PLSQL_FILES; nr++)
    {
      switch (nr)
        {
        case 0:
          (void) snprintf (filename, sizeof (filename), "%s.pks",
                           _interface.name);
          break;

        case 1:
          (void) snprintf (filename, sizeof (filename), "%s.pkb",
                           _interface.name);
          break;

        case 2:
          /* for Oracle 8 external routines */
          (void) snprintf (filename, sizeof (filename), "%s.pke",
                           _interface.name);
          break;

        case 3:
          (void) snprintf (filename, sizeof (filename), "%s.pls",
                           _interface.name);
          break;

        default:
          assert (nr >= 0 && nr < 4);
        }

      if ((pout = fopen (filename, "w")) == NULL)
        {
          goto open_error;
        }
      else
        {
          (void) fprintf (stdout, "Creating %s\n", filename);
        }

      switch (nr)
        {
        case 0:
          generate_plsql_header (pout);
          break;

        case 1:
          generate_plsql_body (pout);
          break;

        case 2:
          generate_plsql_body_ext (pout);
          break;

        case 3:
          /* .pls script call .pks and .pkb */
          print_generate_comment (pout, "REMARK ");

          (void) fprintf (pout, "@@ %s.pks\n\
REMARK Package body using an EPC server.\n\
@@ %s.pkb\n\
REMARK Package body using PL/SQL external routines.\n\
REMARK Uncomment the next line when using PL/SQL external routines (Oracle8 only).\n\
REMARK @@ %s.pke\n", _interface.name, _interface.name, _interface.name);
          break;

        default:
          assert (nr >= 0 && nr < 4);
        }

      (void) fclose (pout);
    }

  DBUG_LEAVE ();

  return;

open_error:
  (void) fprintf (stderr, "Cannot write file %s\n", filename);
  exit (EXIT_FAILURE);
}


static void
generate_c_parameters (FILE * pout, idl_function_t * fun, const int pc_source)
{
  dword_t nr;
  idl_parameter_t *parm;

  DBUG_ENTER ("generate_c_parameters");

  /* 
   * declare variables 
   */

  for (nr = 0; nr < fun->num_parameters; nr++)
    {
      parm = fun->parameters[nr];
      (void) fprintf (pout, "  ");
      print_variable_definition (pout, parm, C, (int) nr, pc_source);
      (void) fprintf (pout, ";\n");
    }

  DBUG_LEAVE ();
}


static void
print_c_debug_info (FILE * pout, char *name, idl_type_t datatype, const int indent)
{
  DBUG_ENTER ("print_c_debug_info");

  (void) fprintf (pout, "%*sDBUG_PRINT( \"info\", ( ", indent, "");
  switch (datatype)
    {
    case C_INT:
      (void) fprintf (pout, "\"%s: %%d\", *l_%s ) );\n", name, name);
      break;

    case C_LONG:
      (void) fprintf (pout, "\"%s: %%ld\", *l_%s ) );\n", name, name);
      break;

    case C_FLOAT:
      (void) fprintf (pout, "\"%s: %%f\", *l_%s ) );\n", name, name);
      break;

    case C_DOUBLE:
      (void) fprintf (pout, "\"%s: %%lf\", *l_%s ) );\n", name, name);
      break;

    case C_STRING:
    case C_XML:
    case C_DATE:
      (void) fprintf (pout, "\"%s: '%%s'\", l_%s ) );\n", name, name);
      break;

    default:
      (void) fprintf (stderr,
                      "print_c_debug_info#Unknown datatype (%ld) for %s\n",
                      datatype, name);
      break;
    }

  DBUG_LEAVE ();
}


static void
generate_c_debug_info (FILE * pout, idl_function_t * fun, idl_mode_t mode, const int indent)
{
  dword_t nr;
  idl_parameter_t *parm;

  DBUG_ENTER ("generate_c_debug_info");

  for (nr = 0; nr < fun->num_parameters; nr++)
    {
      parm = fun->parameters[nr];

      if (parm->mode == mode)
        {
          print_c_debug_info (pout, parm->name, parm->datatype, indent);
        }
    }

  parm = &fun->return_value;
  if (parm->mode == mode && parm->datatype != C_VOID)
    {
      print_c_debug_info (pout, parm->name, parm->datatype, indent);
    }

  DBUG_LEAVE ();
}


static void
generate_c_function (FILE * pout, idl_function_t * fun, const int pc_source)
{
  dword_t nr;
#if GENERATE_PROC
  dword_t nr_actual_parameters;
#endif
  int indent = 0;

  DBUG_ENTER ("generate_c_function");

  (void) fprintf (pout, "void %s%s ( epc__call_t *call )\n{\n",
                  EPC_PREFIX, fun->name);

  if (pc_source != 0)
    {
      (void) fprintf( pout, "\
#ifdef SQLCA\n\
#undef SQLCA\n\
#endif\n\
EXEC SQL INCLUDE sqlca;\n\n" );
    }

  indent += 2;
  (void) fprintf( pout, "%*sepc__function_t *function = call->function;\n", indent, "" );

  if (pc_source != 0) 
    {
      (void) fprintf( pout, "\
%*s%s;\n\
%*slong *error_code = &call->epc__error;\n\
%*slong sqlcode = 0;\n\
%*sconst char *msg_info = call->msg_info;\n\
%*sEXEC SQL VAR msg_info IS STRING;\n",
                      indent, "", EXEC_SQL_BEGIN_DECLARE_SECTION,
                      indent, "",
                      indent, "",
                      indent, "",
                      indent, "" );
    }

  /* PARAMETERS */
  generate_c_parameters (pout, fun, pc_source);

  switch (fun->return_value.datatype)
    {
    case C_VOID:
      break;

    default:
      (void) fprintf (pout, "%*s", indent, "");
      print_variable_definition (pout, &fun->return_value, C,
                                 (int) fun->num_parameters, pc_source);
      (void) fprintf (pout, ";\n");
      break;
    }

  if (pc_source != 0)
    {
      (void) fprintf( pout, "%*s%s;\n", indent, "", EXEC_SQL_END_DECLARE_SECTION );
    }

  (void) fprintf (pout,
                  "\n%*sDBUG_ENTER( \"%s\" );\n%*sdo\n%*s{\n",
                  indent, "", fun->name,
                  indent, "",
                  indent, "");

  indent += 2;

#if GENERATE_PROC

  /*
   * Get the in and in/out parameters
   */
  if (pc_source == 0)
    {
      (void) fprintf( pout, "\
%*sif (EPC__CALL_PROTOCOL(call) == PROTOCOL_NATIVE)\n\
%*s{\n\
%*s  epc__request_native(call);\n\
%*s}\n", 
                      indent, "", 
                      indent, "", 
                      indent, "", 
                      indent, "" );
    }
  else
    {
      if ( exists_plsql_function( fun, SKEL_RECV ) != 0 )
        {
          (void) fprintf( pout, "\
%*sif (EPC__CALL_PROTOCOL(call) == PROTOCOL_NATIVE)\n\
%*s{\n", 
                          indent, "",
                          indent, "" );

          indent += 2;

          /* call the recv function */
          (void) fprintf( pout, "\
%*sEXEC SQL WHENEVER SQLERROR DO epc__abort(\"-- Oracle error --\");\n\
%*sEXEC SQL EXECUTE\n\
%*sBEGIN\n\
%*s  %s%s.%s", 
                          indent, "", 
                          indent, "", 
                          indent, "", 
                          indent, "", _interface.name, package_type_str[SKEL_RECV], fun->name );

          for ( nr = nr_actual_parameters = 0; nr < fun->num_parameters; nr++ )
            {
              idl_parameter_t * parm = fun->parameters[nr];

              if ( parm->mode == C_IN || parm->mode == C_INOUT )
                {
                  (void) fprintf( pout, "%s:l_%s",
                                  ( nr_actual_parameters++ == 0 ? "( " : ", " ), parm->proc_name ); 
                }
            }

          /* close the parameter list if there are actual parameters */
          (void) fprintf( pout, "%s;\n\
%*sEXCEPTION\n\
%*s  WHEN OTHERS\n\
%*s  THEN :sqlcode := SQLCODE;\n\
%*sEND;\n\
%*sEND-EXEC;\n\
%*s}\n",
                          ( nr_actual_parameters == 0 ? "" : " )" ),
                          indent, "",
                          indent, "",
                          indent, "",
                          indent, "",
                          indent, "",
                          indent - 2, "" );
          indent -= 2;
        }
    }

#endif /* #if GENERATE_PROC */

  /* ---------------------------------------------------------------
   * Print the in and in/out parameters, including Oracle error code
   * --------------------------------------------------------------- */

  generate_c_debug_info (pout, fun, C_IN, indent);
  generate_c_debug_info (pout, fun, C_INOUT, indent);

  (void) fprintf (pout, "\n");

  if (pc_source != 0)
    {
      (void) fprintf( pout, "\
%*sif ( sqlcode != 0 )\n\
%*s{\n\
%*s  *error_code = RECEIVE_ERROR;\n\
%*s  break;\n\
%*s}\n\
\n", 
                      indent, "",
                      indent, "",
                      indent, "",
                      indent, "",
                      indent, "" );
    }
  else
    {
      (void) fprintf( pout, "\
%*sif ( call->epc__error != OK )\n\
%*s{\n\
%*s  break;\n\
%*s}\n\
\n", 
                      indent, "",
                      indent, "",
                      indent, "",
                      indent, "" );
    }
  /* ---------------
   * The actual call 
   * --------------- */
  /* set return value if any. Use strncpy for strings */

  switch (fun->return_value.datatype)
    {
    case C_VOID:
      break;

    case C_STRING:
    case C_XML:
    case C_DATE:
      (void) fprintf (pout,
                      "%*s(void) strncpy( l_%s, ",
                      indent, "",
                      fun->return_value.proc_name);
      break;

    default:
      (void) fprintf (pout,
                      "%*s*l_%s = ",
                      indent, "",
                      fun->return_value.proc_name);
      break;
    }

  (void) fprintf (pout, "%s( ", fun->name);

  for (nr = 0; nr < fun->num_parameters; nr++)
    {
      if (nr > 0)
        {
          (void) fprintf (pout, ", ");
        }
      print_actual_parameter (pout, fun->parameters[nr], C);
    }
  (void) fprintf (pout, " )");

  switch (fun->return_value.datatype)
    {
    case C_VOID:
      break;

    case C_STRING:
    case C_XML:
    case C_DATE:
      /* zero terminate to be sure */
      (void) fprintf (pout, ", %ld );\n%*sl_%s[%ld] = '\\0'",
                      fun->return_value.size,
                      indent, "",
                      fun->return_value.proc_name,
                      fun->return_value.size);
      break;

    default:
      break;
    }

  (void) fprintf (pout, ";\n\n");

#if GENERATE_PROC

  /* ----------------
   * Send the results
   * ---------------- */
  if (pc_source == 0) {
    (void) fprintf( pout, "\
%*sif (EPC__CALL_PROTOCOL(call) == PROTOCOL_NATIVE && call->function->oneway == 0)\n\
%*s{\n\
%*s  epc__response_native(call);\n\
%*s}\n", indent, "", indent, "", indent, "", indent, "");
  } else {
    if ( exists_plsql_function( fun, SKEL_SEND ) != 0 )
      {
        (void) fprintf( pout, "\
%*sif (EPC__CALL_PROTOCOL(call) == PROTOCOL_NATIVE && call->function->oneway == 0)\n\
%*s{\n", indent, "", indent, "" );
        indent += 2;
        /* call the send function */
        (void) fprintf( pout, "\
%*sEXEC SQL WHENEVER SQLERROR DO epc__abort(\"-- Oracle error --\");\n\
%*sEXEC SQL EXECUTE\n\
%*sBEGIN\n\
%*s  %s%s.%s", 
                        indent, "",
                        indent, "",
                        indent, "",
                        indent, "",
                        _interface.name,
                        package_type_str[SKEL_SEND],
                        fun->name );

        /*
         * Set the in/out and out parameters
         */
        for ( nr = nr_actual_parameters = 0; nr < fun->num_parameters; nr++ ) 
          {
            idl_parameter_t * parm = fun->parameters[nr];
            
            if ( parm->mode != C_IN )
              {
                (void) fprintf( pout, "%s:l_%s",
                                ( nr_actual_parameters++ == 0 ? "( " : ", " ),
                                parm->proc_name ); 
              }
          }
        if ( fun->return_value.datatype != C_VOID )
          {
            (void) fprintf( pout, "%s:l_%s",
                            ( nr_actual_parameters++ == 0 ? "( " : ", " ),
                            fun->return_value.proc_name ); 
          }
        (void) fprintf( pout, "%s:%s",
                        ( nr_actual_parameters++ == 0 ? "( " : ", " ),
                        "msg_info" ); 
#if SEND_EPC__ERROR != 0
        (void) fprintf( pout, "%s:%s",
                        ( nr_actual_parameters++ == 0 ? "( " : ", " ),
                        "error_code" ); 
#endif
#if SEND_SQLCODE != 0
        (void) fprintf( pout, "%s:%s",
                      ( nr_actual_parameters++ == 0 ? "( " : ", " ),
                        "sqlcode" );
#endif

        (void) fprintf( pout, " );\n\
%*sEXCEPTION\n\
%*s  WHEN OTHERS\n\
%*s  THEN :sqlcode := SQLCODE;\n\
%*sEND;\n\
%*sEND-EXEC;\n\
%*s}\n", 
                        indent, "",
                        indent, "",
                        indent, "",
                        indent, "",
                        indent, "",
                        indent - 2, "");
        indent -= 2;
      }
  }

#endif /* #if GENERATE_PROC */

  /*
   * Print the in/out and out parameters, including Oracle error code
   */
  generate_c_debug_info (pout, fun, C_INOUT, indent);
  generate_c_debug_info (pout, fun, C_OUT, indent);

  indent -= 2;
  (void) fprintf (pout, "%*s} while (0);\n%*sDBUG_LEAVE();\n}\n\n", indent, "", indent, "");

  DBUG_LEAVE ();
}


static void
declare_external_function (FILE * pout, idl_function_t * fun)
{
  dword_t nr;

  DBUG_ENTER ("declare_external_function");

  (void) fprintf (pout, "extern %s %s( ",
                  get_syntax (fun->return_value.datatype, C), fun->name);
  if (fun->num_parameters > 0)
    {
      for (nr = 0; nr < fun->num_parameters; nr++)
        {
          idl_parameter_t *parm = fun->parameters[nr];

          print_formal_parameter (pout, parm->name, parm->mode,
                                  parm->datatype, C);

          if (nr < fun->num_parameters - 1)
            (void) fprintf (pout, ", ");
        }
    }
  else                          /* 0 parameters */
    (void) fprintf (pout, "void");

  (void) fprintf (pout, " );\n");

  DBUG_LEAVE ();
}


static void
declare_internal_function (FILE * pout, idl_function_t * fun)
{
  DBUG_ENTER ("declare_internal_function");

  (void) fprintf (pout, "extern void %s%s( epc__call_t *call );\n",
                  EPC_PREFIX, fun->name);

  DBUG_LEAVE ();
}


static void
generate_c_source (FILE * pout, /*@null@ */ const char *include_text, const int pc_source)
{
  dword_t fnr /* function number */ , pnr /* parameter number */ ;
  idl_function_t *fun;

  DBUG_ENTER ("generate_c_source");

  print_generate_comment (pout, "");

  if (include_text != NULL)
    {
      (void) fprintf (pout, "%s\n", include_text);
    }

  (void) fprintf (pout, "\
#if HAVE_CONFIG_H\n\
#include <config.h>\n\
#endif\n\
#include <string.h>\n\
#include <stdlib.h>\n\
#include <epc.h>\n");

  if (pc_source != 0) {
    (void) fprintf (pout, "\
EXEC SQL INCLUDE \"idl_defs.h\";\n\
EXEC SQL INCLUDE \"epc_defs.h\";\n\
EXEC SQL BEGIN DECLARE SECTION;\n\
static const int msg_timed_out = MSG_TIMED_OUT;\n\
static const int msg_interrupted = MSG_INTERRUPTED;\n\
static const int send_error = SEND_ERROR;\n\
EXEC SQL END DECLARE SECTION;\n\
#define SQLCA_INIT\n\n" );
  }

  (void) fprintf (pout, "\
#include <dbug.h>\n\
#include \"%s.h\"\n\n", _interface.name);

  /* parameter array */
  for (fnr = 0; fnr < _interface.num_functions; fnr++)
    {
      fun = _interface.functions[fnr];
      (void) fprintf (pout, "\nstatic epc__parameter_t %s_parameters[] = {\n",
                      fun->name);
      for (pnr = 0; pnr < fun->num_parameters; pnr++)
        {
          (void) fprintf (pout, "  { \"%s\", %s, %s, %s, NULL },\n",
                          fun->parameters[pnr]->name,
                          get_constant_name (fun->parameters[pnr]->mode, C),
                          get_constant_name (fun->parameters[pnr]->datatype,
                                             C),
                          get_size (fun->parameters[pnr]));
        }
      (void) fprintf (pout, "  { \"%s\", %s, %s, %s, NULL }\n",
                      fun->return_value.name,
                      get_constant_name (fun->return_value.mode, C),
                      get_constant_name (fun->return_value.datatype, C),
                      get_size (&fun->return_value));
      (void) fprintf (pout, "};\n");
    }

  /* function array */
  (void) fprintf (pout, "\nstatic epc__function_t functions[] = {\n");
  for (fnr = 0; fnr < _interface.num_functions; fnr++)
    {
      fun = _interface.functions[fnr];
      assert(fun->oneway == 0 || fun->oneway == 1);
      (void) fprintf (pout, "%s { \"%s", (fnr > 0 ? "," : " "), fun->name);
      (void) fprintf (pout,
                      "\",\n    %s%s, %d, %ld, %s_parameters }\n",
                      EPC_PREFIX,
                      fun->name,
                      (int) fun->oneway, /* GJP 2018-08-21 Even though we specify %ld it does not print well */
                      (dword_t) (fun->num_parameters + 1),  /* return value is a parameter too */
                      fun->name);
    }
  (void) fprintf (pout, "};\n");

  /* interface */
  (void) fprintf (pout, "\nepc__interface_t ifc_%s = {\n", _interface.name);
  (void) fprintf (pout, "  \"%s\",\n", _interface.name);
  (void) fprintf (pout, "  %ld,\n", _interface.num_functions);
  (void) fprintf (pout, "  functions\n");
  (void) fprintf (pout, "};\n\n");

  for (fnr = 0; fnr < _interface.num_functions; fnr++)
    {
      generate_c_function (pout, _interface.functions[fnr], pc_source);
    }

  DBUG_LEAVE ();
}


static void
generate_header (FILE * pout)
{
  dword_t nr;

  DBUG_ENTER ("generate_header");

  print_generate_comment (pout, "");

  /* DECLARE THE INTERNAL FUNCTIONS CALLED, BECAUSE THE COMPILER NEEDS THEM!!! */
  /* HOWEVER DO NOT DECLARE EXTERNAL FUNCTIONS WHEN THEY DO NOT NEED TO BE PRINTED */
  for (nr = 0; nr < _interface.num_functions; nr++)
    {
      if (print_external_function != 0)
        declare_external_function (pout, _interface.functions[nr]);
      declare_internal_function (pout, _interface.functions[nr]);
    }

  /* interface declaration */
  (void) fprintf (pout, "\nextern epc__interface_t ifc_%s;\n",
                  _interface.name);

  DBUG_LEAVE ();
}


void
generate_c (const char *include_text)
{
  char filename[256];
  FILE *pout = NULL;

  DBUG_ENTER ("generate_c");

  if (print_external_function == 0)
    print_external_function = (include_text == NULL ? 1 : 0);

  /* Create header file */

  (void) snprintf (filename, sizeof (filename), "%s.h", _interface.name);
  if ((pout = fopen (filename, "w")) == NULL)
    {
      goto open_error;
    }
  else
    {
      (void) fprintf (stdout, "Creating %s\n", filename);
      generate_header (pout);
      (void) fclose (pout);
    }

  /* Generate a C file */

  (void) snprintf (filename, sizeof (filename), "%s.c", _interface.name);

  if ((pout = fopen (filename, "w")) == NULL)
    {
      goto open_error;
    }
  else
    {
      (void) fprintf (stdout, "Creating %s\n", filename);
      generate_c_source (pout, include_text, 0);
      (void) fclose (pout);
    }

  /* Generate a PRO*C file */

  (void) snprintf (filename, sizeof (filename), "%s.pc", _interface.name);

  if ((pout = fopen (filename, "w")) == NULL)
    {
      goto open_error;
    }
  else
    {
      (void) fprintf (stdout, "Creating %s\n", filename);
      generate_c_source (pout, include_text, GENERATE_PROC);
      (void) fclose (pout);
    }

  DBUG_LEAVE ();
  return;

open_error:
  (void) fprintf (stderr, "Cannot write file %s\n", filename);
  exit (EXIT_FAILURE);
}
