/*
 * Filename   		: $Source$
 *
 * Creation date  : 25-JUN-1997
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
 * Revision 1.1  1998/01/25 15:20:21  gpauliss
 * Initial revision
 *
 * Revision 1.4  1997/07/10 14:33:58  hgwouden
 * stable
 *
 * Revision 1.3  1997/07/08 14:14:47  hgwouden
 * first stable version - generates executable code
 *
 * Revision 1.2  1997/07/02 14:46:14  hgwouden
 * Added c generation code
 *
 * Revision 1.1  1997/06/25 12:22:27  hgwouden
 * Initial revision
 *
 *
 */

#include <malloc.h> 
#include <stdio.h> 
#include <string.h> 
#include <stdlib.h>
#include "idl_parse.h"
#include "idl_const.h"
#include "idl_keywords.h"

static interface _interface;

void print_parameter( parm )
parameter * parm;
{
	printf( "\t\t%s\n", parm->name );
	printf( "\t\t%d\n", parm->mode );
	printf( "\t\t%d\n", parm->datatype );
}

void print_function( fun )
function * fun;
{
	int i;

	printf( "\t%s\n", fun->name );
	printf( "\t%d\n", fun->datatype );
	for ( i=0; i<fun->num_parameters; i++ ) {
		print_parameter( fun->parameters[i] );
	}
}

void print_interface ()
{
	int i;

	printf( "%s\n", _interface.name );
	printf( "%d\n", _interface.num_functions );
	for ( i=0; i<_interface.num_functions; i++ ) {
		print_function( _interface.functions[i] );
	}
}

void set_interface ( name )
char * name;
{
	strcpy( _interface.name, name );
	_interface.num_functions = 0;
}

void add_function ( name, datatype )
char *name;
int datatype;
{
	function * fun = malloc(sizeof(function));

	strcpy( fun->name, name );
	fun->num_parameters = 0;
	fun->datatype = datatype;

	_interface.functions[_interface.num_functions] = fun;
	_interface.num_functions++;
}

void add_parameter ( name, mode, datatype )
char *name;
int mode;
int datatype;
{
	function * fun = _interface.functions[_interface.num_functions-1];
	parameter * parm = malloc(sizeof(parameter));

	strcpy( parm->name, name );
	parm->mode = mode;
	parm->datatype = datatype;

	fun->parameters[fun->num_parameters] = parm;
	fun->num_parameters++;
}

char * 
translate ( type, language )
int type;
int language;
{
	extern keyword keywords[];
	int i, j;
	int num_keywords = sizeof(keywords) / sizeof(keyword);

	for ( i=0; i<num_keywords; i++ ) {
		if ( keywords[i].key == type ) {
			for ( j=0; j<NUM_LANGUAGES; j++ ) {
				if ( keywords[i].mappings[j].language == language ) {
					return keywords[i].mappings[j].value;
				}
			}
			printf( "No value for %d in language %d\n", type, language );
			exit(1);
		}
	}
	printf( "Type %d not a valid keyword\n", type );
	exit(1);
}

void generate_plsql_parameters ( pout, fun )
FILE * pout;
function * fun;
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
			translate( parm->mode, PLSQL ),
			translate( parm->datatype, PLSQL ) );
	}
	fprintf( pout, "\n\t)" );
}

void generate_plsql_function ( pout, fun )
FILE * pout;
function * fun;
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
		fprintf( pout, " return %s", translate( fun->datatype, PLSQL ) );
	}
}

void generate_plsql_function_declaration ( pout, fun )
FILE * pout;
function * fun;
{
	generate_plsql_function ( pout, fun );
	fprintf( pout, ";\n\n" );
}

void generate_plsql_header( pout )
FILE * pout;
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

void generate_plsql_function_body ( pout, fun )
FILE * pout;
function * fun;
{
	int i;
	parameter * parm;

	generate_plsql_function ( pout, fun );
	fprintf( pout, " is\n" );

	/* RETURN VARIABLE */
	if ( fun->datatype != C_VOID ) {
		fprintf( pout, " \t\tresult %s;\n", translate( fun->datatype, PLSQL ) );
	}

	fprintf( pout, "\tbegin\n" );

	/* SETUP OF CLIENT SIDE FUNCTION CALL */
	if ( fun->datatype != C_VOID ) {
		fprintf( pout, "\t\tepc.set_function( '%s', epc.c_%s );\n", 
			fun->name, translate( fun->datatype, PLSQL ) );
	}
	else {
		fprintf( pout, "\t\tepc.set_procedure( '%s' );\n", fun->name );
	}


	for ( i=0; i<fun->num_parameters; i++) {
		parm = fun->parameters[i];
		fprintf( pout, "\t\tepc.set_parameter_value( " );

		switch( parm->mode ) {
			case C_IN:
				fprintf( pout, "epc.c_in, " );
				break;
			case C_INOUT:
				fprintf( pout, "epc.c_inout, " );
				break;
			case C_OUT:
				fprintf( pout, "epc.c_out, " );
				break;
			default:
				fprintf( stderr, 
					"Parameter mode not supported: %d\n", parm->mode );
				exit(1);

		}

		switch( parm->datatype ) {
			case C_STRING:
				fprintf( pout, "epc.c_string" );
				break;
			case C_INT:
				fprintf( pout, "epc.c_int" );
				break;
			case C_FLOAT:
				fprintf( pout, "epc.c_float" );
				break;
			case C_DOUBLE:
				fprintf( pout, "epc.c_double" );
				break;
			default:
				fprintf( stderr, 
					"Parameter datatype not supported: %d\n", parm->datatype );
				exit(1);

		}
		if ( parm->mode != C_OUT ) {
		   fprintf( pout, ", %s );\n", parm->name );
		}
		else {
		   fprintf( pout, " );\n" );
		}
		
	}

	if ( fun->datatype != C_VOID ) {
		fprintf( pout, "\t\tepc.call_function;\n" );
	}
	else {
		fprintf( pout, "\t\tepc.call_procedure;\n" );
	}

	for ( i=0; i<fun->num_parameters; i++) {
		parm = fun->parameters[i];
		if ( parm->mode != C_IN ) {
			fprintf( pout, "\t\tepc.get_parameter_value( %s );\n", parm->name );
		}
	}


	if ( fun->datatype != C_VOID ) {
		fprintf( pout, "\t\tepc.get_return_value( result );\n" );
		fprintf( pout, "\t\treturn result;\n" );
	}

	/* EXCEPTIONS */
	fprintf( pout, "\n\texception\n" );
	fprintf( pout, "\t\twhen others\n" );
	fprintf( pout, "\t\tthen raise;\n" );
	fprintf( pout, "\n" );

	fprintf( pout, "\tend;\n\n" );
}

void generate_plsql_body ( pout )
FILE * pout;
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

void generate_plsql ()
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

void generate_c_parameters ( pout, fun )
FILE * pout;
function * fun;
{
	int i;
	parameter * parm;

	for ( i=0; i<fun->num_parameters; i++) {
		parm = fun->parameters[i];
		fprintf( pout, "\t%s", translate( parm->datatype, C ) );

		if ( parm->mode != C_IN && parm->datatype != C_STRING ) {
			fprintf( pout, " *" );
		}

		fprintf( pout, " %s = ", parm->name );

		if ( parm->mode == C_IN && parm->datatype != C_STRING ) {
			fprintf( pout, "* " );
		}

		fprintf( pout, "(" );
		fprintf( pout, "%s", translate( parm->datatype, C ) );
		if ( parm->datatype != C_STRING ) {
			fprintf( pout, " *" );
		}
		fprintf( pout, ")" );
		fprintf( pout, " call->parameters[%d].value;\n", i );
	}

	fprintf( pout, "\n" );
}

void generate_c_function ( pout, fun )
FILE * pout;
function * fun;
{
	int i;

	fprintf( pout, "void _%s ( call_t * call )\n", fun->name );
	fprintf( pout, "{\n" );

	/* VARIABLE TO HOLD RETURN VALUE */
	if ( fun->datatype != C_VOID ) {
		fprintf( pout, "\t%s res;\n\t%s %s();\n", 
			translate( fun->datatype, C ),
			translate( fun->datatype, C ), 
			fun->name );
	}

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
			fprintf( pout, "\tmemcpy( call->return_value, res, strlen(res)+1 );\n" );
			break;
		case C_INT:
		case C_FLOAT:
		case C_DOUBLE:
			fprintf( pout, "\tmemcpy( call->return_value, &res, sizeof(res) );\n" );
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

void generate_c_source ( pout )
FILE * pout;
{
	int i;

	fprintf( pout, "/*******************************\n" );
	fprintf( pout, " ** Generated by EPC compiler **\n" );
	fprintf( pout, " *******************************/\n\n" );

	fprintf( pout, "#include \"%s.h\"\n\n", _interface.name );

	for ( i=0; i<_interface.num_functions; i++) {
		generate_c_function( pout, _interface.functions[i] );
	}
}

void generate_interface_header ( pout )
FILE * pout;
{
	fprintf( pout, "/*******************************\n" );
	fprintf( pout, " ** Generated by EPC compiler **\n" );
	fprintf( pout, " *******************************/\n\n" );

	fprintf( pout, "#include \"%s.h\"\n\n", _interface.name );
}

void generate_header ( pout )
FILE * pout;
{
	int i;
	function * fun;

	fprintf( pout, "/*******************************\n" );
	fprintf( pout, " ** Generated by EPC compiler **\n" );
	fprintf( pout, " *******************************/\n\n" );

	fprintf( pout, "#include <epc_types.h>\n\n" );

	/* forward references to functions */
	for ( i=0; i<_interface.num_functions; i++) {
		fun = _interface.functions[i];
		fprintf( pout, "void _%s( call_t * );\n", fun->name );
	}

	/* function array */
	fprintf( pout, "\nint num_functions = %d;\n\n", _interface.num_functions );
	fprintf( pout, "function functions[] = {\n" );
	for ( i=0; i<_interface.num_functions; i++) {
		fun = _interface.functions[i];
		fprintf( pout, "\t{ \"%s\", %d, _%s },\n", 
			fun->name, fun->datatype, fun->name );
	}
	fprintf( pout, "};\n" );
}

void generate_c ()
{
	char filename[256];
	FILE *pout_i, * pout_c, * pout_h;

	sprintf( filename, "epc_interface.h" );
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

