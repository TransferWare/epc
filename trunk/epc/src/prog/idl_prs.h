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
 * Data structures for parsing and code generation
 *
 * --- Revision History --------------------------------------------------
 * $Log$
 * Revision 1.1  1998/01/25 15:20:22  gpauliss
 * Initial revision
 *
 * Revision 1.2  1997/07/02 14:46:14  hgwouden
 * Added c generation code
 *
 * Revision 1.1  1997/06/25 12:22:27  hgwouden
 * Initial revision
 *
 *
 */

#ifndef _IDL_PARSE_H
#define _IDL_PARSE_H

#define MAXLEN 31

typedef struct {
	char name[MAXLEN];
	int mode;
	int datatype;
} parameter;

typedef struct {
	char name[MAXLEN];
	int datatype;
	int num_parameters;
	parameter * parameters[MAXLEN];
} function;

typedef struct {
	char name[MAXLEN];
	int num_functions;
	function * functions[MAXLEN];
} interface;

void set_interface ( char *name );
void add_function ( char *name, int datatype );
void add_parameter ( char *name, int mode, int datatype );

void generate_plsql ();
void generate_c ();

#endif
