/*
 * Filename   		: $RCSfile$
 *
 * Creation date	: 25-JUN-1997
 *
 * Created by 		: Huub van der Wouden
 *
 * Company    		: Transfer Solutions bv
 *
 * --- Description -------------------------------------------------------
 * Parsing and code generation routines
 *
 * --- Revision History --------------------------------------------------
 * $Log$
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
#include "epc_defs.h"

static interface _interface;

keyword keywords[] = {
	{ C_VOID, 	{	{ C, 		"void",		"C_VOID" },
					{ PLSQL,	"",			"epc.c_void" }
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
	{ C_IN,		{	{ C,		"",			"C_IN" },
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

void print_parameter( parameter * parm )
{
	printf( "\t\t%s\n", parm->name );
	printf( "\t\t%d\n", parm->mode );
	printf( "\t\t%d\n", parm->datatype );
}

void print_function( function * fun )
{
	int i;

	printf( "\t%s\n", fun->name );
	printf( "\t%d\n", fun->datatype );
	for ( i=0; i<fun->num_parameters; i++ ) {
		print_parameter( fun->parameters[i] );
	}
}

void print_interface ( void )
{
	int i;

	printf( "%s\n", _interface.name );
	printf( "%d\n", _interface.num_functions );
	for ( i=0; i<_interface.num_functions; i++ ) {
		print_function( _interface.functions[i] );
	}
}

void set_interface ( char *name )
{
	strcpy( _interface.name, name );
	_interface.num_functions = 0;
}

void add_function ( char *name, int datatype )
{
	function * fun = malloc(sizeof(function));

	strcpy( fun->name, name );
	fun->num_parameters = 0;
	fun->datatype = datatype;

	_interface.functions[_interface.num_functions] = fun;
	_interface.num_functions++;
}

void add_parameter ( char *name, int mode, int datatype )
{
	function * fun = _interface.functions[_interface.num_functions-1];
	parameter * parm = malloc(sizeof(parameter));

	strcpy( parm->name, name );
	parm->mode = mode;
	parm->datatype = datatype;

	fun->parameters[fun->num_parameters] = parm;
	fun->num_parameters++;
}

mapping *get_mapping( int type, int language )
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
			printf( "No mapping for %d in language %d\n", type, language );
			exit(1);
		}
	}
	printf( "Type %d not a valid keyword\n", type );
	exit(1);
}

char *get_syntax( int type, int language )
{
	return	get_mapping( type, language )->syntax;
}

char *get_constant_name( int type, int language )
{
	return	get_mapping( type, language )->constant_name;
}

void generate_plsql_parameters( FILE * pout, function * fun )
{
	int i;
	parameter * parm;

	fprintf( pout, " (\n" );

	for ( i=0; i<fun->num_parameters; i++) {
		parm = fun->parameters[i];

		if ( i>0 ) {
			fprintf( pout, ",\n" );
		}

		fprintf( pout, "\t\t%s %s %s", 
			parm->name, 
			get_syntax( parm->mode, PLSQL ),
			get_syntax( parm->datatype, PLSQL ) );
	}
	fprintf( pout, "\n\t)" );
}

void generate_plsql_function( FILE * pout, function * fun )
{
	if ( fun->datatype != C_VOID ) {
		fprintf( pout, "\tfunction %s", fun->name );
	}
	else {
		fprintf( pout, "\tprocedure %s", fun->name );
	}

	/* PARAMETERS */
	if ( fun->num_parameters > 0 ) {
		generate_plsql_parameters( pout, fun );
	}

	if ( fun->datatype != C_VOID ) {
		fprintf( pout, " return %s", get_syntax( fun->datatype, PLSQL ) );
	}
}

void generate_plsql_function_declaration( FILE * pout, function * fun )
{
	generate_plsql_function ( pout, fun );
	fprintf( pout, ";\n\n" );
}

void generate_plsql_header( FILE * pout )
{
	int i;

	fprintf( pout, "create or replace package %s is\n", _interface.name );
	fprintf( pout, "\n" );

	for ( i=0; i<_interface.num_functions; i++) {
		generate_plsql_function_declaration( pout, _interface.functions[i] );
	}

	fprintf( pout, "end;\n\n" );
	fprintf( pout, "/\n" );
}

void generate_plsql_function_body( FILE * pout, function * fun )
{
	int i;
	parameter * parm;

	generate_plsql_function ( pout, fun );
	fprintf( pout, " is\n" );

	/* RETURN VARIABLE */
	switch( fun->datatype )
	{
	case C_VOID:
		break;

	case C_INT:
	case C_LONG:
	case C_FLOAT:
	case C_DOUBLE:
		fprintf( pout, " \t\tresult %s;\n", get_syntax( fun->datatype, PLSQL ) );
		break;

	case C_STRING:
		fprintf( 
			pout, 
			" \t\tresult %s(%d);\n", 
			get_syntax( fun->datatype, PLSQL ), 
			MAX_STR_VAL_LEN 
		);
		break;
	}

	fprintf( pout, "\tbegin\n" );

	/* SETUP OF CLIENT SIDE FUNCTION CALL */
	fprintf( pout, "\t\tepc.request_set_header( '%s', '%s', %s );\n", 
		_interface.name,
		fun->name, 
		get_constant_name( fun->datatype, PLSQL ) );

	for ( i=0; i<fun->num_parameters; i++) {
		parm = fun->parameters[i];
		fprintf( pout, "\t\tepc.request_set_parameter( %s, %s",
				get_constant_name( parm->mode, PLSQL ), 
				get_constant_name( parm->datatype, PLSQL ) 
		);
		if ( parm->mode != C_OUT ) {
		   fprintf( pout, ", %s );\n", parm->name );
		}
		else {
		   fprintf( pout, " );\n" );
		}
		
	}

	fprintf( pout, "\t\tepc.request_perform_routine;\n" );

	/* GET THE RESULTS */
	for ( i=0; i<fun->num_parameters; i++) {
		parm = fun->parameters[i];
		if ( parm->mode != C_IN ) {
			fprintf( pout, "\t\tepc.result_get_parameter_value( %s );\n", parm->name );
		}
	}

	if ( fun->datatype != C_VOID ) {
		fprintf( pout, "\t\tepc.result_get_return_value( result );\n" );
		fprintf( pout, "\t\treturn result;\n" );
	}

	/* EXCEPTIONS */
	fprintf( pout, "\n\texception\n" );
	fprintf( pout, "\t\twhen others\n" );
	fprintf( pout, "\t\tthen raise;\n" );
	fprintf( pout, "\n" );

	fprintf( pout, "\tend;\n\n" );
}

void generate_plsql_body( FILE * pout )
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

void generate_c_parameters( FILE * pout, function * fun )
{
	int i;
	parameter * parm;

	for ( i=0; i<fun->num_parameters; i++) {
		parm = fun->parameters[i];
		fprintf( pout, "\t%s", get_syntax( parm->datatype, C ) );

		if ( parm->mode != C_IN && parm->datatype != C_STRING ) {
			fprintf( pout, " *" );
		}

		fprintf( pout, " %s = ", parm->name );

		if ( parm->mode == C_IN && parm->datatype != C_STRING ) {
			fprintf( pout, "* " );
		}

		fprintf( pout, "(" );
		fprintf( pout, "%s", get_syntax( parm->datatype, C ) );
		if ( parm->datatype != C_STRING ) {
			fprintf( pout, " *" );
		}
		fprintf( pout, ")" );
		fprintf( pout, " call->parameters[%d].value;\n", i );
	}

	fprintf( pout, "\n" );
}

void generate_c_function ( FILE * pout, function * fun )
{
	int i;

	fprintf( pout, "void _%s ( call_t * call )\n", fun->name );
	fprintf( pout, "{\n" );

	/* VARIABLE TO HOLD RETURN VALUE */
	if ( fun->datatype != C_VOID ) {
		fprintf( pout, "\t%s res;\n", 
			get_syntax( fun->datatype, C )
		);
	}

	/* DECLARE THE FUNCTION CALLED, BECAUSE THE PRECOMPILER NEEDS IT!!! */
	fprintf( pout, "\textern %s %s( ", 
		get_syntax( fun->datatype, C ),
		fun->name
	);
	if ( fun->num_parameters > 0 ) {
		for ( i=0; i<fun->num_parameters; i++) {
			parameter * parm = fun->parameters[i];

			fprintf( pout, "%s", get_syntax( parm->datatype, C ) );

			if ( parm->mode != C_IN && parm->datatype != C_STRING ) {
				fprintf( pout, " *" );
			}

			if ( i < fun->num_parameters - 1 )
				fprintf( pout, ", " );
		}
	}
	else /* 0 parameters */
		fprintf( pout, "void" );

	fprintf( pout, " );\n" );

	/* PARAMETERS */
	if ( fun->num_parameters > 0 ) {
		generate_c_parameters( pout, fun );
	}

	/* THE ACTUAL CALL */
	fprintf( pout, "\t" );
	if( fun->datatype != C_VOID ) {
		fprintf( pout, "res = " );
	}
	fprintf( pout, "%s ( ", fun->name );

	for ( i=0; i<fun->num_parameters; i++) {
		if ( i>0 ) {
			fprintf( pout, ", " );
		}
		fprintf( pout, "%s", fun->parameters[i]->name );
	}
	fprintf( pout, " );\n" );

	/* BINDING TO RETVAL PARAMETER */
	switch ( fun->datatype ) {
		case C_STRING:
			fprintf( pout, "\tmemcpy( call->return_value.value, res, strlen(res)+1 );\n" );
			break;
		case C_INT:
		case C_LONG:
		case C_FLOAT:
		case C_DOUBLE:
			fprintf( pout, "\tmemcpy( call->return_value.value, &res, sizeof(res) );\n" );
			break;
		case C_VOID:
			/* do nothing */
			break;
		default:
			fprintf( stderr, "Datatype not supported: %d\n", fun->datatype );
			exit(1);
	}

	fprintf( pout, "}\n\n" );
}

void generate_c_source ( FILE * pout )
{
	int i;

	fprintf( pout, "/*******************************\n" );
	fprintf( pout, " ** Generated by EPC compiler **\n" );
	fprintf( pout, " *******************************/\n\n" );

	fprintf( pout, "#include <string.h>\n" );
	fprintf( pout, "#include \"%s.h\"\n\n", _interface.name );

	for ( i=0; i<_interface.num_functions; i++) {
		generate_c_function( pout, _interface.functions[i] );
	}
}

void generate_interface_header ( FILE * pout )
{
	fprintf( pout, "/*******************************\n" );
	fprintf( pout, " ** Generated by EPC compiler **\n" );
	fprintf( pout, " *******************************/\n\n" );

	fprintf( pout, "#include \"%s.h\"\n\n", _interface.name );
}

void generate_header ( FILE *pout )
{
	int i;
	function * fun;

	fprintf( pout, "/*******************************\n" );
	fprintf( pout, " ** Generated by EPC compiler **\n" );
	fprintf( pout, " *******************************/\n\n" );

	fprintf( pout, "#include \"epc_defs.h\"\n\n" );

	/* forward references to functions */
	for ( i=0; i<_interface.num_functions; i++) {
		fun = _interface.functions[i];
		fprintf( pout, "void _%s( call_t * );\n", fun->name );
	}

	/* function array */
	fprintf( pout, "\nint num_functions = %d;\n\n", _interface.num_functions );
	fprintf( pout, "function_t functions[] = {\n" );
	for ( i=0; i<_interface.num_functions; i++) {
		fun = _interface.functions[i];
		fprintf( pout, "\t{ \"%s\", %s, _%s }%s\n", 
			fun->name, 
			get_constant_name( fun->datatype, C ), 
			fun->name,
			( i < _interface.num_functions-1 ? "," : "" ) 
		);
	}
	fprintf( pout, "};\n" );
}

void generate_c ( void )
{
	char filename[256];
	FILE *pout_i, * pout_c, * pout_h;

	sprintf( filename, "epc_ifc.h" );
	if ( ( pout_i = fopen( filename, "w" ) ) == NULL ) {
		printf( "cannot open file %s - exiting...\n", filename );
	}

	sprintf( filename, "%s.h", _interface.name );
	if ( ( pout_h = fopen( filename, "w" ) ) == NULL ) {
		printf( "cannot open file %s - exiting...\n", filename );
	}

	sprintf( filename, "%s.c", _interface.name );
	if ( ( pout_c = fopen( filename, "w" ) ) == NULL ) {
		printf( "cannot open file %s - exiting...\n", filename );
	}

	generate_interface_header ( pout_i ); 
	generate_header ( pout_h ); 
	generate_c_source ( pout_c );
}

