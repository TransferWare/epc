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
 * Constants for IDL parsing and code generation
 *
 * --- Revision History --------------------------------------------------
 * $Log$
 * Revision 1.4  1998/07/27 15:18:54  gert-jan
 * First release.
 *
 * Revision 1.3  1998/05/06 20:23:43  gpauliss
 * Added support for longs
 *
 * Revision 1.2  1998/02/19 16:42:06  gpauliss
 * Using dos filename conventions (8.3)
 *
 * Revision 1.1  1998/01/25 15:20:20  gpauliss
 * Initial revision
 *
 * Revision 1.3  1997/07/10 14:33:19  hgwouden
 * added some constants
 *
 *
 */

#ifndef _IDL_DEFS_H_
#define _IDL_DEFS_H_

#define MAX_PARM_NAME_LEN	32   /* must be a multiple of 4 */
#define MAX_FUNC_NAME_LEN	32   /* must be a multiple of 4 */
#define MAX_INTERFACE_NAME_LEN	32   /* must be a multiple of 4 */
#define MAX_STR_VAL_LEN		4096 /* must be a multiple of 4 */
#define MAX_FUNCTIONS		100
#define MAX_PARAMETERS		20

/* KEYWORDS GENERATED BY IDL COMPILER */

/* type definitions: make sure they are a double word long in order to avoid 
   alignment problems in structures */

#ifndef DWORD_T
#define DWORD_T
typedef long dword_t;
#endif

/* DATA TYPES */
#define C_STRING 1
#define C_INT 2
#define C_LONG 3
#define C_FLOAT 4
#define C_DOUBLE 5
#define C_VOID 6

typedef dword_t idl_type_t; /* one of the values above */

/* PARAMETER MODES */
#define C_IN 101
#define C_OUT 102
#define C_INOUT 103

typedef dword_t idl_mode_t; /* one of the values above */

#endif





