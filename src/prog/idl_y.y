%{

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
 * IDL language specification
 *
 * --- Revision History --------------------------------------------------
 * $Log$
 * Revision 1.1  1998/01/25 15:20:20  gpauliss
 * Initial revision
 *
 * Revision 1.2  1997/07/08 14:14:25  hgwouden
 * added DOUBLE datatype
 *
 * Revision 1.1  1997/06/25 12:22:27  hgwouden
 * Initial revision
 *
 *
 */

#include <stdio.h>
#include "idl_parse.h"

extern void yyerror( char *s );
extern int yylex( void );

%}


%union {
	char * sval;
	int ival;
}

%token <sval> INTERFACE 
%token <sval> NAME 
%token <ival> IN 
%token <ival> OUT 
%token <ival> INOUT 
%token <ival> VOID 
%token <ival> STRING 
%token <ival> INT 
%token <ival> FLOAT
%token <ival> DOUBLE

%type <ival> datatype, parameter_mode
%type <sval> interface_name, function_name, parameter_name

%%

interface:
		INTERFACE interface_name interface_body
	;

interface_name:
		NAME	 { set_interface( $1 ); }
	;

interface_body:
		'{' function_list '}'
	;

function_list:
		/* empty */
	|	function_list function
	;

function:
		datatype 
		function_name 
			{ add_function( $2, $1 ); }
		'(' parameter_list ')' ';'

	;

datatype:
		STRING
	|	INT
	|	FLOAT
	|	DOUBLE
	|	VOID
	;

function_name:
		NAME
	;

parameter_list:
		/* empty */
	|	parameter 
	|	parameter_list ',' parameter
	;

parameter:
		parameter_mode datatype parameter_name
			{ add_parameter( $3, $1, $2 ); }
	;

parameter_mode:
		IN
	|	OUT
	|	INOUT
	;

parameter_name:
		NAME
	;



