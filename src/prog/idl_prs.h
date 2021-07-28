/*
 * Filename             : idl_prs.h
 *
 * Creation date        : 25-JUN-1997
 *
 * Created by           : Huub van der Wouden
 *
 * Company              : Transfer Solutions bv
 *
 * --- Description -------------------------------------------------------
 * Data structures for parsing and code generation
 *
 * --- Revision History --------------------------------------------------
 * $Log$
 * Revision 1.14  2005/01/03 12:26:43  gpaulissen
 * Release 4.4.0
 *
 * Revision 1.13  2004/12/28 12:18:11  gpaulissen
 * Test on Amazon
 *
 * Revision 1.12  2004/12/20 13:29:16  gpaulissen
 * make lint
 *
 * Revision 1.11  2004/12/16 16:03:23  gpaulissen
 * Web services added
 *
 * Revision 1.10  2004/10/21 11:54:32  gpaulissen
 * indent *.c *.h
 *
 * Revision 1.9  2004/10/20 13:34:06  gpaulissen
 * make lint
 *
 * Revision 1.8  2002/10/28 14:53:05  gpaulissen
 * Using GNU standards.
 *
 * Revision 1.7  2001/01/24 16:29:10  gpaulissen
 * Release 2.0.0
 *
 * Revision 1.6  1999/11/23 16:05:39  gpaulissen
 * DBUG interface changed.
 *
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

#ifndef IDL_PRS_H
#define IDL_PRS_H

#include <idl_defs.h>

typedef /*@only@ */ struct
{
  /*@unique@ */ char name[MAX_PARM_NAME_LEN];
  /*@unique@ */ char proc_name[MAX_PARM_NAME_LEN + 4];
  idl_mode_t mode;
  idl_type_t datatype;
  dword_t size;
} idl_parameter_t;

typedef /*@only@ */ struct
{
  /*@unique@ */ char name[MAX_FUNC_NAME_LEN];
  idl_parameter_t return_value;
  int oneway;
  dword_t num_parameters;
  idl_parameter_t *parameters[MAX_PARAMETERS];
} idl_function_t;

typedef struct
{
  /*@unique@ */ char name[MAX_INTERFACE_NAME_LEN];
  dword_t num_functions;
  idl_function_t *functions[MAX_FUNCTIONS];
} idl_interface_t;


extern void set_interface (char *name);

extern void add_function (const char *name, const idl_type_t datatype, const dword_t size, const dword_t oneway);

extern void
add_parameter (char *name, idl_mode_t mode, idl_type_t datatype, dword_t size);

extern void generate_plsql (void);

extern void generate_c (const char *include_text);

#endif
