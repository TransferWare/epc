%{

/*
 * Filename   		: $RCSfile$
 *
 * Creation date  	: 25-JUN-1997
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
 * Revision 1.10  2002/10/28 14:53:05  gpaulissen
 * Using GNU standards.
 *
 * Revision 1.9  2001/01/24 16:29:10  gpaulissen
 * Release 2.0.0
 *
 * Revision 1.8  1999/11/23 16:05:39  gpaulissen
 * DBUG interface changed.
 *
 * Revision 1.7  1998/11/27 08:17:25  gjp
 * PSK-EPC-10. Clash between types of INTERFACE keyword and interface rule.
 *
 * Revision 1.6  1998/08/11 21:07:07  gjp
 * Changed RCS keyword Source by RCSfile to prevent differences between checkouts.
 *
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

#include "idl_defs.h"
#include "idl_prs.h"

extern
void
yyerror( char *s );

extern
int
yylex( void );

%}


%union {
  char * sval;
  int mval;
  struct {
    int datatype;
    unsigned int size;
  } tval;
}

%token <sval> INTERFACE 
%token <sval> NAME 
%token <mval> IN 
%token <mval> OUT 
%token <mval> INOUT 
%token <tval> VOID 
%token ONEWAY
%token <tval> STRING 
%token <tval> INT 
%token <tval> LONG
%token <tval> FLOAT
%token <tval> DOUBLE

%type <tval> datatype
%type <mval> parameter_mode
%type <sval> interface, interface_name, function_name, parameter_name

%%

interface: INTERFACE interface_name interface_body;

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
		{ add_function( $2, $1.datatype, 0 ); }
		'(' parameter_list ')' ';'
	|       ONEWAY
		VOID
		function_name 
		{ add_function( $3, $2.datatype, 1 ); }
		'(' in_parameter_list ')' ';'

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
		{ add_parameter( $3, $1, $2.datatype, $2.size ); }
	;

parameter_mode:
		IN
	|	OUT
	|	INOUT
	;

parameter_name:
		NAME
	;

in_parameter_list:
		/* empty */
	|	in_parameter 
	|	in_parameter_list ',' parameter
	;

in_parameter:
		IN datatype parameter_name
		{ add_parameter( $3, $1, $2.datatype, $2.size ); }
	;




