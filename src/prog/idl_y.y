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
 * Revision 1.5  1998/07/27 15:21:09  gert-jan
 * First release.
 *
 * Revision 1.4  1998/05/06 20:24:03  gpauliss
 * Added support for longs
 *
 * Revision 1.3  1998/02/19 16:42:31  gpauliss
 * Using dos filename conventions (8.3)
 *
 * Revision 1.2  1998/02/03 10:06:53  gpauliss
 * - Removed obsolete logging
 *
 * Revision 1.1  1998/01/25 15:20:20  gpauliss
 * Initial revision
 *
 *
 */

#include <stdio.h>
#include "idl_prs.h"

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
%token <ival> LONG
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
	|	LONG
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



