/*
 * Filename   		: $RCSfile$
 *
 * Creation date  : 25-JUN-1997
 *
 * Created by 		: Huub van der Wouden
 *
 * Company    		: Transfer Solutions bv
 *
 * --- Description -------------------------------------------------------
 * Keyword definitions for IDL parsing and code generation
 *
 * --- Revision History --------------------------------------------------
 * $Log$
 * Revision 1.4  1998/05/06 20:23:43  gpauliss
 * Added support for longs
 *
 * Revision 1.3  1998/02/19 16:42:07  gpauliss
 * Using dos filename conventions (8.3)
 *
 * Revision 1.2  1998/02/03 10:04:36  gpauliss
 * - Changed mapping structure for clearer code generation.
 *
 * Revision 1.1  1998/01/25 15:20:21  gpauliss
 * Initial revision
 *
 */

#ifndef _IDL_KWRD_H_
#define _IDL_KWRD_H_

#include "idl_defs.h"

#define NUM_LANGUAGES 2
#define C 1
#define PLSQL 2

typedef struct {
	int language;
	char *syntax;			/* part of the return value/parameter syntax */
	char *constant_name;	/* Name of constant */
} mapping;

typedef struct {
	int key;
	mapping mappings[NUM_LANGUAGES];
} keyword;

extern keyword keywords[];

#endif
