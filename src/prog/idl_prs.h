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
 * Data structures for parsing and code generation
 *
 * --- Revision History --------------------------------------------------
 * $Log$
 * Revision 1.5  1998/07/31 16:25:46  gert-jan
 * Various changes.
 *
 * Revision 1.4  1998/07/27  15:21:09  gert-jan
 * First release.
 *
 * Revision 1.3  1998/05/06 20:23:44  gpauliss
 * Added support for longs
 *
 * Revision 1.2  1998/02/19 16:42:09  gpauliss
 * Using dos filename conventions (8.3)
 *
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

#ifndef _IDL_PRS_H
#define _IDL_PRS_H

#ifndef  _IDL_DEFS_H_
#include "idl_defs.h"
#define  _IDL_DEFS_H_
#endif

typedef struct {
	char name[MAX_PARM_NAME_LEN];
	char proc_name[MAX_PARM_NAME_LEN+4];
	idl_mode_t mode;
	idl_type_t datatype;
	dword_t size;
} idl_parameter_t;

typedef struct {
	char name[MAX_FUNC_NAME_LEN];
	idl_parameter_t return_value;
	dword_t num_parameters;
	idl_parameter_t * parameters[MAX_PARAMETERS];
} idl_function_t;

typedef struct {
	char name[MAX_INTERFACE_NAME_LEN];
	dword_t num_functions;
	idl_function_t * functions[MAX_FUNCTIONS];
} idl_interface_t;

void set_interface ( char *name );
void add_function ( char *name, idl_type_t datatype );
void add_parameter ( char *name, idl_mode_t mode, idl_type_t datatype );

void generate_plsql ( void );
void generate_c ( void );

#endif
