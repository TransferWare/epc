/*
 * Filename   		: $RCSfile$
 *
 * Creation date	: 25-JUN-1997
 *
 * Created by 		: Huub van der Wouden
 *
 * Company    		: Transfer Solutions bv
 *
 * Notes		: For strings PRO*C VARCHAR constructs are used.
 *			  For input then length has to be set to the
 *			  maximum number of characters to receive. For
 *			  output the length is the number of characters
 *			  to send. For ease of use the following invariant
 *			  will be maintained: str[length] == 0
 *
 * --- Description -------------------------------------------------------
 * Parsing and code generation routines
 *
 * --- Revision History --------------------------------------------------
 * $Log$
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
#include "idl_prs.h"
#include "idl_defs.h"
#include "idl_kwrd.h"

/*
|| Defines
*/
#define EXEC_SQL_BEGIN_DECLARE_SECTION "EXEC SQL BEGIN DECLARE SECTION"
#define EXEC_SQL_END_DECLARE_SECTION "EXEC SQL END DECLARE SECTION"

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

static void init_parameter( idl_parameter_t *parm, char *name, idl_mode_t mode, idl_type_t datatype );
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
static void generate_plsql_body( FILE * pout );
static void generate_c_parameters( FILE * pout, idl_function_t * fun );
static void print_c_debug_info( FILE *pout, char *name, idl_type_t datatype );
static void generate_c_debug_info( FILE * pout, idl_function_t * fun, idl_mode_t mode );
static void generate_c_function ( FILE * pout, idl_function_t * fun );
static void print_generate_comment( FILE * pout, char * prefix );
static void declare_external_function( FILE * pout, idl_function_t * fun );
static void declare_internal_function( FILE * pout, idl_function_t * fun );
static void generate_c_source ( FILE * pout );
#ifdef GEN_EPC_IFC_H
static void generate_interface_header ( FILE * pout );
#endif
static void generate_header ( FILE *pout );



/*
|| Static global variables
*/
static idl_interface_t _interface;


/*
|| Global variable
*/
keyword keywords[] = {
	{ C_VOID, 	{	{ C, 		"void",		"C_VOID" },
				{ PLSQL,	"",		"epc.c_void" }
			}
	},
	{ C_INT,	{	{ C, 		"int",		"C_INT" },
				{ PLSQL,	"integer",	"epc.c_int" }
			}
	},
	{ C_LONG,	{	{ C, 		"long",		"C_LONG" },
				{ PLSQL,	"integer",	"epc.c_long" }
			}
	},
	{ C_FLOAT,	{	{ C,		"float",	"C_FLOAT" },
				{ PLSQL,	"number",	"epc.c_float" }
			}
	},
	{ C_DOUBLE,	{	{ C,		"double",	"C_DOUBLE" },
				{ PLSQL,	"number",	"epc.c_double" }
			}
	},
	{ C_STRING,	{	{ C,		"char *",	"C_STRING" },
				{ PLSQL,	"varchar2",	"epc.c_string" }
			}
	},
	{ C_IN,		{	{ C,		"",		"C_IN" },
				{ PLSQL,	"in",		"epc.c_in" }
			}
	},
	{ C_OUT,	{	{ C,		"*",		"C_OUT" },
				{ PLSQL,	"out",		"epc.c_out" } 
			}
	},
	{ C_INOUT,	{	{ C,		"*",		"C_INOUT" },
				{ PLSQL,	"in out",	"epc.c_inout" }
			}
	}
};


void set_interface ( char *name )
{
	strcpy( _interface.name, name );
	_interface.num_functions = 0;
}

static void init_parameter( idl_parameter_t *parm, char *name, idl_mode_t mode, idl_type_t datatype )
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
		strcat( parm->proc_name, "_vc" );
		parm->size = MAX_STR_VAL_LEN;
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

void add_function ( char *name, idl_type_t datatype )
{
	idl_function_t * fun = (idl_function_t*)malloc(sizeof(idl_function_t));

	strcpy( fun->name, name );
	fun->num_parameters = 0;

	init_parameter( &fun->return_value, "result", C_IN, datatype );

	_interface.functions[_interface.num_functions] = fun;
	_interface.num_functions++;
}

void add_parameter ( char *name, idl_mode_t mode, idl_type_t datatype )
{
	idl_function_t * fun = _interface.functions[_interface.num_functions-1];
	idl_parameter_t * parm = (idl_parameter_t*)malloc(sizeof(idl_parameter_t));

	init_parameter( parm, name, mode, datatype );

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

		case C_STRING: /* Use the VARCHAR PRO*C construct */
			fprintf( pout, "VARCHAR %s[%d+1] = { %d, \"\" };\n\tchar *%s = (char*)%s.arr", 
				parm->proc_name, parm->size, parm->size, parm->name, parm->proc_name );
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
		fprintf( pout, "\tfunction %s", fun->name );
	}
	else {
		fprintf( pout, "\tprocedure %s", fun->name );
	}

	/* PARAMETERS */
	if ( fun->num_parameters > 0 ) {
		generate_plsql_parameters( pout, fun );
	}

	if ( fun->return_value.datatype != C_VOID ) {
		fprintf( pout, " return %s", get_syntax( fun->return_value.datatype, PLSQL ) );
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

	fprintf( pout, "create or replace package %s is\n", _interface.name );
	fprintf( pout, "\n" );

	for ( i=0; i<_interface.num_functions; i++) {
		generate_plsql_function_declaration( pout, _interface.functions[i] );
	}

	fprintf( pout, "end;\n/\n\n" );
}

static void generate_plsql_function_body( FILE * pout, idl_function_t * fun )
{
	int i;
	idl_parameter_t * parm;

	generate_plsql_function ( pout, fun );
	fprintf( pout, " is\n" );

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

	fprintf( pout, "\tbegin\n" );

	/* SETUP OF CLIENT SIDE FUNCTION CALL */
	fprintf( pout, "\t\tepc.request_set_header( '%s', '%s' );\n", 
		_interface.name,
		fun->name );

	for ( i=0; i<fun->num_parameters; i++) {
		parm = fun->parameters[i];

		if ( parm->mode != C_OUT )
			fprintf( pout, "\t\tdbms_pipe.pack_message( %s );\n", parm->name );
	}

	fprintf( pout, "\t\tepc.request_perform_routine;\n" );

	/* GET THE RESULTS */
	for ( i=0; i<fun->num_parameters; i++) {
		parm = fun->parameters[i];
		if ( parm->mode != C_IN ) {
			fprintf( pout, "\t\tdbms_pipe.unpack_message( %s );\n", parm->name );
		}
	}

	if ( fun->return_value.datatype != C_VOID ) {
		fprintf( pout, "\t\tdbms_pipe.unpack_message( %s );\n", fun->return_value.name );
		fprintf( pout, "\t\treturn %s;\n", fun->return_value.name );
	}

	/* EXCEPTIONS */
	fprintf( pout, "\texception\n" );
	fprintf( pout, "\t\twhen others\n" );
	fprintf( pout, "\t\tthen raise;\n" );

	fprintf( pout, "\tend;\n\n" );
}

static void generate_plsql_body( FILE * pout )
{
	int i;

	fprintf( pout, "create or replace package body %s is\n", _interface.name );
	fprintf( pout, "\n" );

	for ( i=0; i<_interface.num_functions; i++) {
		generate_plsql_function_body( pout, _interface.functions[i] );
	}

	fprintf( pout, "end;\n" );
	fprintf( pout, "/\n" );
}

void generate_plsql ( void )
{
	char filename[256];
	FILE * pout;

	sprintf( filename, "%s.pls", _interface.name );
	if ( ( pout = fopen( filename, "w" ) ) == NULL ) {
		printf( "cannot open file %s - exiting...\n", filename );
	}
	generate_plsql_header ( pout );
	generate_plsql_body ( pout );
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
	switch( datatype )
	{
	case C_INT:
		fprintf( pout, "\tepc_debug( \"%s= %%d\\n\", (int)%s );\n", name, name ); break;
	case C_LONG:
		fprintf( pout, "\tepc_debug( \"%s= %%ld\\n\", (long)%s );\n", name, name ); break;
	case C_FLOAT:
		fprintf( pout, "\tepc_debug( \"%s= %%f\\n\", (float)%s );\n", name, name ); break;
	case C_DOUBLE:
		fprintf( pout, "\tepc_debug( \"%s= %%lf\\n\", (double)%s );\n", name, name ); break;
	case C_STRING:
		fprintf( pout, "\tepc_debug( \"%s= '%%s'\\n\", (char*)%s );\n", name, name ); break;
	default:
		fprintf( stderr, "print_c_debug_info#Unknown datatype (%d) for %s\n", datatype, name ); break;
	}
}


static void generate_c_debug_info( FILE * pout, idl_function_t * fun, idl_mode_t mode )
{
	int i;

	for ( i=0; i<fun->num_parameters; i++) {
		idl_parameter_t * parm = fun->parameters[i];

		if ( parm->mode == mode )
		{
			print_c_debug_info( pout, parm->name, parm->datatype );
		}
   	}
}


static void generate_c_function( FILE * pout, idl_function_t * fun )
{
	int i;
	int exec_sql_printed = 0;

	fprintf( pout, "void _%s ( epc_call_t *call )\n", fun->name );
	fprintf( pout, "{\n" );

		/* 
		 * VARIABLE TO HOLD RETURN VALUE 
		 */

	fprintf( pout, "\t%s;\n", EXEC_SQL_BEGIN_DECLARE_SECTION );
	fprintf( pout, "\tlong epc_error = OK;\n" );
	fprintf( pout, "\tlong sqlcode = 0;\n" );

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

 	fprintf( pout, "\t%s;\n\n", EXEC_SQL_END_DECLARE_SECTION );

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
			fprintf( pout, "\t\tdbms_pipe.unpack_message( :%s );\n", parm->proc_name ); 
		}
   	}

	if ( exec_sql_printed )
	{
		fprintf( pout, "\
\tEXCEPTION\n\
\t\tWHEN OTHERS THEN :sqlcode := SQLCODE;\n\
\tEND;\n\
\tEND-EXEC;\n\n" );
	}

		/* -----------------------------------------------------------
		 * Make PRO*C variable a zero-terminated field of length field
		 * ----------------------------------------------------------- */
	for ( i=0; i<fun->num_parameters; i++) {
		idl_parameter_t *parm = fun->parameters[i];

		if ( parm->datatype != C_STRING ) continue;

		switch( parm->mode )
		{
		case C_OUT:
			fprintf( pout, "\tmemset(%s, ' ', %d);\n", parm->name, parm->size );
			/* !!! do not use break !!! */
			/* break; */

		case C_IN:
		case C_INOUT:
			fprintf( pout, "\t%s.arr[%s.len] = 0;\n", parm->proc_name, parm->proc_name );
			break;
		}
	}

		/* ---------------------------------------------------------------
		 * Print the in and in/out parameters, including Oracle error code
		 * --------------------------------------------------------------- */
	generate_c_debug_info( pout, fun, C_IN );
	generate_c_debug_info( pout, fun, C_INOUT );
	print_c_debug_info( pout, "sqlcode", C_LONG );
	fprintf( pout, "\n\tif ( sqlcode != 0 ) goto receive_error;\n\n" );

		/* ---------------
		 * THE ACTUAL CALL 
		 * --------------- */
	fprintf( pout, "\t" );
	if( fun->return_value.datatype != C_VOID ) {
		fprintf( pout, "%s = ", fun->return_value.name );
	}
	fprintf( pout, "%s ( ", fun->name );

	for ( i=0; i<fun->num_parameters; i++) {
		if ( i>0 ) {
			fprintf( pout, ", " );
		}
		print_actual_parameter( pout, fun->parameters[i], C );
	}
	fprintf( pout, " );\n\n" );

		/* ------------------------------------------------------
		 * Let PRO*C variable have the length indicated by strlen
		 * ------------------------------------------------------ */
	for ( i=0; i<fun->num_parameters; i++) {
		idl_parameter_t *parm = fun->parameters[i];
		if ( ( parm->mode == C_INOUT || parm->mode == C_OUT ) && parm->datatype == C_STRING ) 
			fprintf( pout, "\t%s.len = strlen(%s);\n", parm->proc_name, parm->name );
	}

		/* ----------------
		 * Send the results
		 * ---------------- */
	fprintf( pout, "\
\tEXEC SQL WHENEVER SQLERROR CONTINUE;\n\
\tEXEC SQL EXECUTE\n\
\tBEGIN\n\
\t\tdbms_pipe.reset_buffer;\n\
\t\tdbms_pipe.pack_message( :epc_error );\n\
\t\tdbms_pipe.pack_message( :sqlcode );\n" ); 

		/*
		 * Set the in/out and out parameters
		 */
	for ( i=0; i<fun->num_parameters; i++) {
		idl_parameter_t * parm = fun->parameters[i];

		if ( parm->mode == C_INOUT || parm->mode == C_OUT )
		{
			fprintf( pout, "\t\tdbms_pipe.pack_message( :%s );\n", parm->name ); 
		}
   	}
	if ( fun->return_value.datatype != C_VOID )
	{
		fprintf( pout, "\t\tdbms_pipe.pack_message( :%s );\n", fun->return_value.name ); 
	}

	fprintf( pout, "\
\tEXCEPTION\n\
\t\tWHEN OTHERS THEN :sqlcode := SQLCODE;\n\
\tEND;\n\
\tEND-EXEC;\n\n\
\tif ( sqlcode != 0 ) goto send_error;\n\n" );

	fprintf( pout, "\
ok:\n\
\tgoto end;\n\
\n\
memory_error:\n\
\tepc_error = MEMORY_ERROR;\n\
\tgoto end;\n\
\n\
receive_error:\n\
\tepc_error = RECEIVE_ERROR;\n\
\tgoto end;\n\
\n\
send_error:\n\
\tepc_error = SEND_ERROR;\n\
\tgoto end;\n\
\n\
end:\n\
\tcall->epc_error = epc_error;\n\
\tcall->sqlcode = sqlcode;\n" );

		/*
		 * Print the in/out and out parameters, including Oracle error code
		 */
	generate_c_debug_info( pout, fun, C_INOUT );
	generate_c_debug_info( pout, fun, C_OUT );
	print_c_debug_info( pout, "sqlcode", C_LONG );
	print_c_debug_info( pout, "epc_error", C_LONG );

	fprintf( pout, "}\n\n" );
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
	fprintf( pout, "extern void _%s( epc_call_t *call );\n", fun->name );
}

static void generate_c_source ( FILE * pout )
{
	int i;
	idl_function_t * fun;

	print_generate_comment( pout, "" );

	fprintf( pout, "#include <string.h>\n" );
	fprintf( pout, "#include <stdlib.h>\n" );
	fprintf( pout, "#include \"epc_defs.h\"\n" );
	fprintf( pout, "#include \"epc_dbg.h\"\n" );
	fprintf( pout, "#include \"%s.h\"\n\n", _interface.name );
	fprintf( pout, "\
#define SQLCA_STORAGE_CLASS static\n\
/* force initialisation of any sqlca struct */\n\
#define SQLCA_INIT\n\
\n\
#ifdef SQLCA\n\
#undef SQLCA\n\
#endif\n\
EXEC SQL INCLUDE sqlca;\n\n" );

		/* function array */
	fprintf( pout, "\nstatic epc_function_t functions[] = {\n" );
	for ( i=0; i<_interface.num_functions; i++) {
		fun = _interface.functions[i];
		fprintf( pout, "\t{ \"%s\", %s, _%s }%s\n", 
			fun->name, 
			get_constant_name( fun->return_value.datatype, C ), 
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

		/* DECLARE THE FUNCTIONS CALLED, BECAUSE THE COMPILER NEEDS THEM!!! */
	for ( i=0; i<_interface.num_functions; i++) {
		declare_external_function( pout, _interface.functions[i] );
		declare_internal_function( pout, _interface.functions[i] );
	}

		/* interface declaration */
	fprintf( pout, "\nextern epc_interface_t ifc_%s;\n", _interface.name );
}

void generate_c ( void )
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

#ifdef GEN_EPC_IFC_H
	generate_interface_header ( pout_i ); 
#endif
	generate_header ( pout_h ); 
	generate_c_source ( pout_c );
}

