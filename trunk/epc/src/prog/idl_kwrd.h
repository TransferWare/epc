#ifndef IDL_KWRD_H
#define IDL_KWRD_H

/*
 * Filename             : $RCSfile$
 *
 * Creation date  : 25-JUN-1997
 *
 * Created by           : Huub van der Wouden
 *
 * Company              : Transfer Solutions bv
 *
 * --- Description -------------------------------------------------------
 * Keyword definitions for IDL parsing and code generation
 *
 * --- Revision History --------------------------------------------------
 * $Log$
 * Revision 1.9  2004/12/20 13:29:16  gpaulissen
 * make lint
 *
 * Revision 1.8  2004/10/21 11:54:32  gpaulissen
 * indent *.c *.h
 *
 * Revision 1.7  2004/10/20 13:34:05  gpaulissen
 * make lint
 *
 * Revision 1.6  2002/10/28 14:53:04  gpaulissen
 * Using GNU standards.
 *
 * Revision 1.5  1998/07/27 15:21:09  gert-jan
 * First release.
 *
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

#include <idl_defs.h>

#define NUM_LANGUAGES 2
#define C 1
#define PLSQL 2

typedef dword_t idl_lang_t;

typedef struct
{
  idl_lang_t language;
  /*@observer@ */ char *syntax;
  /* part of the return value/parameter syntax */
  /*@observer@ */ char *constant_name;
  /* Name of constant */
} mapping;

typedef struct
{
  dword_t key;
  /*@observer@ */ mapping mappings[NUM_LANGUAGES];
} keyword;

extern keyword keywords[];

#endif
