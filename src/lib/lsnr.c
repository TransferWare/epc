/*
 * Filename             : $RCSfile$
 *
 * Creation date        : 25-1-1998
 *
 * Created by           : Huub van der Wouden
 *
 * Company              : Transfer Solutions bv
 *
 * --- Description -------------------------------------------------------
 * Defines default API functions.
 *
 * --- Revision History --------------------------------------------------
 * $Log$
 * Revision 1.12  2004/12/20 13:29:15  gpaulissen
 * make lint
 *
 * Revision 1.11  2004/12/16 18:43:08  gpaulissen
 * generated HTML added
 *
 * Revision 1.10  2004/10/21 11:54:32  gpaulissen
 * indent *.c *.h
 *
 * Revision 1.9  2004/10/21 10:37:07  gpaulissen
 * * make lint
 * * error reporting enhanced
 * * oneway functions enhanced
 *
 * Revision 1.8  2004/10/20 20:38:44  gpaulissen
 * make lint
 *
 * Revision 1.7  2004/10/19 14:35:09  gpaulissen
 * epc_ -> epc__
 *
 * Revision 1.6  2004/10/18 15:17:33  gpaulissen
 * XML enabling of EPC
 *
 * Revision 1.5  2004/10/15 20:41:32  gpaulissen
 * XML namespace bugs solved.
 *
 * Revision 1.4  2004/02/22 17:14:14  gpaulissen
 * bug id 891761
 *
 * Revision 1.3  2003/06/08 16:28:20  gpaulissen
 * GNU build system for ts_dbug
 *
 * Revision 1.2  2003/04/10 19:50:27  gpaulissen
 * Update
 *
 * Revision 1.1  2003/03/29 18:31:42  gpaulissen
 * Various fixes
 *
 * Revision 1.22  2003/03/26 21:47:06  gpaulissen
 * Building the epc
 *
 * Revision 1.21  2002/12/04 17:47:03  gpaulissen
 * Libtool problems
 *
 * Revision 1.20  2002/12/01 20:05:33  gpaulissen
 * Autoconf modifications
 *
 * Revision 1.19  2002/11/20 21:43:25  gpaulissen
 * Autoconf update
 *
 * Revision 1.18  2002/10/31 22:21:20  gpaulissen
 * Release 3.0.0a
 *
 * Revision 1.17  2001/01/24 16:29:09  gpaulissen
 * Release 2.0.0
 *
 * Revision 1.15  2000/08/14 13:47:40  gpaulissen
 * * Added -v command line option for version info.
 * * Updated build support.
 *
 *
 */

const char vcid[] = "$Id$";

#if HAVE_CONFIG_H
#include <config.h>
#endif

/* defined an any system */
#include <stdio.h>

#if HAVE_STDLIB_H
#include <stdlib.h>
#endif

#if HAVE_STRING_H
#include <string.h>
#endif

#if HAVE_ASSERT_H
#include <assert.h>
#endif

#if HAVE_STDARG_H
#include <stdarg.h>
#endif

#include <epc.h>
#include <dbug.h>

#ifndef  ORA_PROC		/* skip during precompiling */
# if defined(HAVE_SQLPROTO_H) && HAVE_SQLPROTO_H
#  include <sqlproto.h>
# elif defined(HAVE_SQLCPR_H) && HAVE_SQLCPR_H
#  include <sqlcpr.h>
# else
#  error No PRO*C prototype header found.
# endif
#endif

/* include dmalloc/ulib as last one */
#if defined(HASDMALLOC) && HASDMALLOC != 0
# include <dmalloc.h>
#elif defined(HASULIB) && HASULIB != 0
# include <u_alloc.h>
#endif

extern
  epc__error_t
epc_recv_request_pipe (epc__info_t * epc__info, epc__call_t * epc__call);
extern
  epc__error_t
epc_send_response_pipe (epc__info_t * epc__info, epc__call_t * epc__call);

/* declarations */

static void help (char *procname);


static
/*@observer@*/
char *
version (void)
{
  return PACKAGE_VERSION;
}


/**
 * @brief  Start the EPC for one interface.
 *
 * See epc__list_main for details.
 *
 * @param argc          - the number of arguments
 * @param argv          - the arguments
 * @param epc_interface - an interface
 *
 * @return
 ******************************************************************************/

epc__error_t
epc__main (int argc, char **argv, epc__interface_t * epc_interface)
{
  return epc__list_main (argc, argv, epc_interface, NULL);
}

/**
 * @brief  Start the EPC for one or more interfaces.
 *
 * This is usually called from main().
 *
 * The following command line arguments are recognised:
 * <table>
 *   <tr><td>-D OPTIONS</td><td>Turn on debugging using the DBUG library.</td></tr>
 *   <tr><td>-h</td><td>Print a help message.</td></tr>
 *   <tr><td>-p REQUEST_PIPE</td><td>Set the request pipe.</td></tr>
 *   <tr><td>-u USERID</td><td>Use this connect string for the Oracle logon.</td></tr>
 *   <tr><td>-v</td><td>Displays the EPC listener version.</td></tr>
 * </table>
 *
 * @param argc          - the number of arguments
 * @param argv          - the arguments
 * @param epc_interface - a NULL terminated list of interfaces
 *
 * @return
 ******************************************************************************/

epc__error_t
epc__list_main (int argc, char **argv, epc__interface_t * epc_interface, ...)
{
#if defined(HASULIB) && HASULIB != 0
  unsigned int chk = AllocStartCheckPoint ();
#endif
  char *logon = NULL;
  char *request_pipe = NULL;
  int nr;
  /*@only@ *//*@null@ */ epc__info_t *epc__info = NULL;
  epc__error_t ret;
  /*@only@ *//*@null@ */ char *dbug_options = NULL;

  /* process command line parameters */
  for (nr = 0; nr < argc; nr++)
    {
      if (argv[nr][0] == '-')
	{
	  switch (argv[nr][1])
	    {
	    case 'D':
	      if (argv[nr][2] != '\0')
		{
		  dbug_options = (char *) malloc (strlen (&argv[nr][2]) + 1);
		  assert (dbug_options != NULL);
		  strcpy (dbug_options, &argv[nr][2]);
		}
	      else
		{
		  ++nr;
		  dbug_options = (char *) malloc (strlen (&argv[nr][0]) + 1);
		  assert (dbug_options != NULL);
		  strcpy (dbug_options, &argv[nr][0]);
		}
	      break;

	    case 'h':
	      help (argv[0]);
	      return OK;

	    case 'p':
	      if (argv[nr][2] != '\0')
		request_pipe = &argv[nr][2];
	      else
		request_pipe = &argv[++nr][0];
	      break;

	    case 'u':
	      if (argv[nr][2] != '\0')
		logon = &argv[nr][2];
	      else
		logon = &argv[++nr][0];
	      break;

	    case 'v':
	      (void) fprintf (stdout, "EPC listener version: %s\n",
			      version ());
	      return OK;

	    default:
	      help (argv[0]);
	      return OK;
	    }
	}
    }

  if (dbug_options == NULL)
    {
      dbug_options = (char *) malloc (1);
      assert (dbug_options != NULL);
      dbug_options[0] = '\0';
    }

  assert (OK == 0);

  do
    {
      nr = 0;
      if ((ret = dbug_init (dbug_options, argv[0])) != OK)
	break;

      nr++;			/* 1 */
      epc__info = epc__init ();
      if (epc__info == NULL)
	{
	  ret = MEMORY_ERROR;
	  break;
	}

      nr++;			/* 2 */
      if ((ret = epc__set_logon (epc__info, logon)) != OK)
	break;

      nr++;			/* 3 */
      if ((ret = epc__set_pipe (epc__info, request_pipe)) != OK)
	break;

      nr++;			/* 4 */
      {
	va_list ap;

	va_start (ap, epc_interface);
	for (; ret == OK && epc_interface != NULL;)
	  {
	    ret = epc__add_interface (epc__info, epc_interface);
	    epc_interface = va_arg (ap, epc__interface_t *);
	  }
	va_end (ap);
      }
      if (ret != OK)
	break;

      nr++;			/* 5 */
      if ((ret = epc__connect (epc__info)) != OK)
	break;

      nr++;			/* 6 */
      if ((ret =
	   epc__handle_requests (epc__info, epc_recv_request_pipe,
				 epc_send_response_pipe)) != OK)
	break;

      nr++;
    }
  while (0);

  /*@-branchstate@ */
  switch (nr)			/* last step */
    {
    case 7:
    case 6:
      ret = epc__disconnect (epc__info);
      /*@fallthrough@ */

    case 5:
    case 4:
    case 3:
    case 2:
      epc__done (epc__info);
      /*@fallthrough@ */

    case 1:
      (void) dbug_done ();
      /*@fallthrough@ */

    case 0:
      break;

    default:
      assert (nr >= 0 && nr <= 7);
      break;
    }
  /*@=branchstate@ */

  free (dbug_options);

#if defined(HASULIB) && HASULIB != 0
  (void) AllocStopCheckPoint (chk);
#endif

  return ret;
}


static void
help (char *procname)
{
  (void) printf ("\
Syntax: %s -D <dbug options> -d -h -p <request pipe> -u <user connect> -v\n\
\n\
Flags:\n\
        D       set dbug options\n\
        h       this help\n\
        p       set name of request pipe\n\
        u       user connect string for database logon, e.g. SCOTT/TIGER@DB\n\
        v       display the EPC listener version\n\
", procname);
}
