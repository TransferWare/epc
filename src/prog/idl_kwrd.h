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
 * Revision 1.1  1998/01/25 15:20:21  gpauliss
 * Initial revision
 *
 * Revision 1.1  1997/07/10 14:33:58  hgwouden
 * Initial revision
 *
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
	char * value;
} mapping;

typedef struct {
	int key;
	mapping mappings[NUM_LANGUAGES];
} keyword;

keyword keywords[] = {
	{ C_VOID, 	  { C, 		"void" } },
	{ C_INT,		{ { C, 		"int" },
					  { PLSQL, "number" } } },
	{ C_FLOAT,	{ { C,		"float" },
					  { PLSQL, "number" } } },
	{ C_DOUBLE,	{ { C,		"double" },
					  { PLSQL, "number" } } },
	{ C_STRING,	{ { C,		"char *" },
					  { PLSQL, "varchar2" } } },
	{ C_IN,		{ { C,		"" },
					  { PLSQL, "in" } } },
	{ C_OUT,		{ { C,		"*" },
					  { PLSQL, "out" } } },
	{ C_INOUT,	{ { C,		"*" },
					  { PLSQL, "in out" } } },
};

#endif
