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
 * Keyword definitions for IDL parsing and code generation
 *
 * --- Revision History --------------------------------------------------
 * $Log$
 * Revision 1.2  1998/02/03 10:04:36  gpauliss
 * - Changed mapping structure for clearer code generation.
 *
 * Revision 1.1  1998/01/25 15:20:21  gpauliss
 * Initial revision
 *
 */

#ifndef _IDL_KEYWORDS_H_
#define _IDL_KEYWORDS_H_

#include "idl_const.h"

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
