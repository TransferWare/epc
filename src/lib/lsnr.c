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

#if HAVE_SIGNAL_H
#include <signal.h>
#endif

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
#include <epc_dbg.h>

#ifndef  ORA_PROC    /* skip during precompiling */
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

/* declarations */

/* general signal handler */
static
void
handle_signal( int signo );


static
void
help( char *procname );


static
void
set_signal_handlers( void )
{
  DBUG_ENTER( "set+signal_handlers" );

  signal( SIGINT, handle_signal );
  signal( SIGABRT, handle_signal );
  signal( SIGILL, handle_signal );
  signal( SIGSEGV, handle_signal );
  signal( SIGTERM, handle_signal );
#ifdef WIN32
  signal( SIGBREAK, handle_signal );
#endif
  DBUG_LEAVE();
}


static
char *
version( void )
{
  enum keyword {
    NAME_KW,
    REVISION_KW
  } step_no;
  static const char str[2][100] = { "$Name$", "$Revision$" };
  char *instr[2] = { "Name: ", "Revision: " };
  char *found = NULL;

  for ( step_no = NAME_KW;
        ( found == NULL || *found == '\0' ) && step_no <= REVISION_KW;
        step_no++ )
    {
      found = strstr( str[step_no], instr[step_no] );

      if ( found != NULL )
        {
          char *space;

          found += strlen( instr[step_no] );

          space = strstr( found, " " );

          if ( space != NULL )
            *space = '\0';
        }
    }

  return found != NULL ? found : "";
}


epc_error_t
epc_main( int argc, char **argv, epc_interface_t *epc_interface )
{
  return epc_list_main( argc, argv, epc_interface, NULL );
}


epc_error_t
epc_list_main( int argc, char **argv, epc_interface_t *epc_interface, ... )
{
#if defined(HASULIB) && HASULIB != 0
  unsigned int chk = AllocStartCheckPoint();
#endif
  char *logon = NULL;
  char *request_pipe = NULL;
  int nr;
  epc_info_t *epc_info = NULL;
  epc_error_t ret;
  char *dbug_options = "";

  /* process command line parameters */
  for ( nr = 0; nr < argc; nr++ ) 
    {
      if ( argv[nr][0] == '-' ) 
        {
          switch( argv[nr][1] ) 
            {
            case '#':
              if ( argv[nr][2] != '\0' )
                dbug_options = &argv[nr][2];
              else
                dbug_options = &argv[++nr][0];
              break;

            case 'd':
              dbug_options = "d,t,g";
              break;

            case 'h': 
              help( argv[0] ); 
              return OK;

            case 'p': 
              if ( argv[nr][2] != '\0' )
                request_pipe = &argv[nr][2];
              else
                request_pipe = &argv[++nr][0]; 
              break;

            case 'u': 
              if ( argv[nr][2] != '\0' )
                logon = &argv[nr][2]; 
              else
                logon = &argv[++nr][0];
              break;

            case 'v':
              (void) fprintf( stdout, "EPC listener version: %s\n", version() );
              return OK;
              break;

            default : 
              help( argv[0] ); 
              return OK;
            }
        }
    }

  assert( OK == 0 );

  for ( nr = 0, ret = OK; ret == OK && nr < 7; nr++ )
    {
      switch( nr )
        {
        case 0:
          ret = dbug_init( dbug_options, argv[0] );
          break;
                        
        case 1:
          set_signal_handlers ();
          break;
        
        case 2:
          epc_info = epc_init();
          if ( epc_info == NULL )
            ret = MEMORY_ERROR;
          break;

        case 3:
          ret = epc_set_logon( epc_info, logon );
          break;

        case 4:
          ret = epc_set_pipe( epc_info, request_pipe );
          break;

        case 5:
          {
            va_list ap;

            va_start( ap, epc_interface );
            for ( ; ret == OK && epc_interface != NULL; )
              {
                ret = epc_add_interface( epc_info, epc_interface );
                epc_interface = va_arg(ap, epc_interface_t *);
              }
            va_end( ap );
          }
          break;

        case 6:
          ret = epc_handle_requests( epc_info );
          break;
        }
    }

  switch( nr-1 ) /* last correct step */
    {
    case 5:
    case 4:
    case 3:
    case 2:
      epc_done( &epc_info );
      /* no break */

    case 1:
    case 0:
      dbug_done();
      break;

    default:
      break;
    }

#if defined(HASULIB) && HASULIB != 0
  (void) AllocStopCheckPoint(chk);
#endif

  return OK;
}


static
void 
help( char *procname )
{
  (void) printf( "\
Syntax: %s -# <dbug options> -d -h -p <request pipe> -u <user connect> -v\n\
\n\
Flags:\n\
        #       set dbug options\n\
        d       full debugging, tracing and profiling on\n\
        h       this help\n\
        p       set name of request pipe\n\
        u       user connect string for database logon, e.g. SCOTT/TIGER@DB\n\
        v       display the EPC listener version\n\
", procname );
}


static
void
handle_signal( int signo )
{
  (void) printf( "Signal %d received\n", signo );
  epc_disconnect();
}


