#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

/* defined on any system */
#include <stdio.h>

#if HAVE_STDLIB_H
#include <stdlib.h>
#endif

#if HAVE_SIGNAL_H
#include <signal.h>
#endif

#if HAVE_STRING_H
#include <string.h>
#endif

#if HAVE_ASSERT_H
#include <assert.h>
#endif

#include <epc.h>
#include <epc_xml.h>
#include <dbug.h>

/* include dmalloc as last one */
#ifdef WITH_DMALLOC
#include "dmalloc.h"
#endif

#ifdef SERVER_INTERRUPT

static
/*@null@*/
epc__handle_interrupt_t G_epc__handle_interrupt = NULL;

/* Can not supply epc__info to standard signal handlers 
   so supply a global */
static
/*@null@*/
epc__info_t *G_epc__info_interrupt = NULL;

#endif

static int G_signo = 0;

static void handle_signal (int signo);

#define MAX_SIGNO 32

/* index 0 for startup, index 1 in signal handler */
static struct
{
  /*@null@ */ void (*func) (int);
} signal_handler_info[2][MAX_SIGNO] =
{
  {
    {
    handle_signal},
    {
    handle_signal},
    {
    handle_signal},
    {
    handle_signal},
    {
    handle_signal},
    {
    handle_signal},
    {
    handle_signal},
    {
    handle_signal},
    {
    handle_signal},
    {
    handle_signal},
    {
    handle_signal},
    {
    handle_signal},
    {
    handle_signal},
    {
    handle_signal},
    {
    handle_signal},
    {
    handle_signal},
    {
    handle_signal},
    {
    handle_signal},
    {
    handle_signal},
    {
    handle_signal},
    {
    handle_signal},
    {
    handle_signal},
    {
    handle_signal},
    {
    handle_signal},
    {
    handle_signal},
    {
    handle_signal},
    {
    handle_signal},
    {
    handle_signal},
    {
    handle_signal},
    {
    handle_signal},
    {
    handle_signal},
    {
    handle_signal}
  },
  {
    {
    SIG_IGN},
    {
    SIG_IGN},
    {
    SIG_IGN},
    {
    SIG_IGN},
    {
    SIG_IGN},
    {
    SIG_IGN},
    {
    SIG_IGN},
    {
    SIG_IGN},
    {
    SIG_IGN},
    {
    SIG_IGN},
    {
    SIG_IGN},
    {
    SIG_IGN},
    {
    SIG_IGN},
    {
    SIG_IGN},
    {
    SIG_IGN},
    {
    SIG_IGN},
    {
    SIG_IGN},
    {
    SIG_IGN},
    {
    SIG_IGN},
    {
    SIG_IGN},
    {
    SIG_IGN},
    {
    SIG_IGN},
    {
    SIG_IGN},
    {
    SIG_IGN},
    {
    SIG_IGN},
    {
    SIG_IGN},
    {
    SIG_IGN},
    {
    SIG_IGN},
    {
    SIG_IGN},
    {
    SIG_IGN},
    {
    SIG_IGN},
    {
    SIG_IGN}
  }
};

static
/*@observer@*/
char *
signal_str (int signo)
{
  switch (signo)
    {
#ifdef SIGHUP
    case SIGHUP:
      return "SIGHUP";
#endif
#ifdef SIGINT
    case SIGINT:
      return "SIGINT";
#endif
#ifdef SIGQUIT
    case SIGQUIT:
      return "SIGQUIT";
#endif
#ifdef SIGILL
    case SIGILL:
      return "SIGILL";
#endif
#ifdef SIGTRAP
    case SIGTRAP:
      return "SIGTRAP";
#endif
#ifdef SIGABRT
    case SIGABRT:
      return "SIGABRT";
#endif
#ifdef SIGEMT
    case SIGEMT:
      return "SIGEMT";
#endif
#ifdef SIGFPE
    case SIGFPE:
      return "SIGFPE";
#endif
#ifdef SIGBUS
    case SIGBUS:
      return "SIGBUS";
#endif
#ifdef SIGSEGV
    case SIGSEGV:
      return "SIGSEGV";
#endif
#ifdef SIGSYS
    case SIGSYS:
      return "SIGSYS";
#endif
#ifdef SIGPIPE
    case SIGPIPE:
      return "SIGPIPE";
#endif
#ifdef SIGALRM
    case SIGALRM:
      return "SIGALRM";
#endif
#ifdef SIGTERM
    case SIGTERM:
      return "SIGTERM";
#endif
#ifdef SIGURG
    case SIGURG:
      return "SIGURG";
#endif
#ifdef SIGSTOP
    case SIGSTOP:
      return "SIGSTOP";
#endif
#ifdef SIGTSTP
    case SIGTSTP:
      return "SIGTSTP";
#endif
#ifdef SIGCONT
    case SIGCONT:
      return "SIGCONT";
#endif
#ifdef SIGCHLD
    case SIGCHLD:
      return "SIGCHLD";
#endif
#ifdef SIGTTIN
    case SIGTTIN:
      return "SIGTTIN";
#endif
#ifdef SIGTTOU
    case SIGTTOU:
      return "SIGTTOU";
#endif
#ifdef SIGIO
    case SIGIO:
      return "SIGIO";
#endif
#ifdef SIGXCPU
    case SIGXCPU:
      return "SIGXCPU";
#endif
#ifdef SIGXFSZ
    case SIGXFSZ:
      return "SIGXFSZ";
#endif
#ifdef SIGVTALRM
    case SIGVTALRM:
      return "SIGVTALRM";
#endif
#ifdef SIGPROF
    case SIGPROF:
      return "SIGPROF";
#endif
#ifdef SIGWINCH
    case SIGWINCH:
      return "SIGWINCH";
#endif
#ifdef SIGLOST
    case SIGLOST:
      return "SIGLOST";
#endif
#ifdef SIGUSR1
    case SIGUSR1:
      return "SIGUSR1";
#endif
#ifdef SIGUSR2
    case SIGUSR2:
      return "SIGUSR2";
#endif
#ifdef SIGBREAK
    case SIGBREAK:
      return "SIGBREAK";
#endif
    default:
      return "Unknown signal";
    }
}

void
epc__set_signal_handlers (const int idx)
{
  int nr;

  DBUG_ENTER ("epc__set_signal_handlers");

  for (nr = 0; nr < MAX_SIGNO; nr++)
    {
      switch (nr)
        {
#ifdef SIGHUP
        case SIGHUP:
          break;
#endif
#ifdef SIGINT
        case SIGINT:
          continue;             /* GJP 21-02-2004 Handled by handle_interrupt() */
#endif
#ifdef SIGQUIT
        case SIGQUIT:
          break;
#endif
#ifdef SIGILL
        case SIGILL:
          break;
#endif
#ifdef SIGTRAP
        case SIGTRAP:
          break;
#endif
#ifdef SIGABRT
        case SIGABRT:
          break;
#endif
#ifdef SIGEMT
        case SIGEMT:
          break;
#endif
#ifdef SIGFPE
        case SIGFPE:
          break;
#endif
#ifdef SIGBUS
        case SIGBUS:
          break;
#endif
#ifdef SIGSEGV
        case SIGSEGV:
          break;
#endif
#ifdef SIGSYS
        case SIGSYS:
          break;
#endif
#ifdef SIGPIPE
        case SIGPIPE:
          break;
#endif
#ifdef SIGALRM
        case SIGALRM:
          break;
#endif
#ifdef SIGTERM
        case SIGTERM:
          break;
#endif
#ifdef SIGURG
        case SIGURG:
          break;
#endif
#ifdef SIGSTOP
        case SIGSTOP:
          break;
#endif
#ifdef SIGTSTP
        case SIGTSTP:
          break;
#endif
#ifdef SIGCONT
        case SIGCONT:
          break;
#endif
#ifdef SIGCHLD
        case SIGCHLD:
          break;
#endif
#ifdef SIGTTIN
        case SIGTTIN:
          break;
#endif
#ifdef SIGTTOU
        case SIGTTOU:
          break;
#endif
#ifdef SIGIO
        case SIGIO:
          break;
#endif
#ifdef SIGXCPU
        case SIGXCPU:
          break;
#endif
#ifdef SIGXFSZ
        case SIGXFSZ:
          break;
#endif
#ifdef SIGVTALRM
        case SIGVTALRM:
          break;
#endif
#ifdef SIGPROF
        case SIGPROF:
          break;
#endif
#ifdef SIGWINCH
        case SIGWINCH:
          break;
#endif
#ifdef SIGLOST
        case SIGLOST:
          break;
#endif
#ifdef SIGUSR1
        case SIGUSR1:
          break;
#endif
#ifdef SIGUSR2
        case SIGUSR2:
          break;
#endif
#ifdef SIGBREAK
        case SIGBREAK:
          break;
#endif
        default:
          continue;             /* not a known signal */
        }

      DBUG_PRINT ("info", ("handler for signal %d (%s): %p",
                           nr,
                           signal_str (nr),
                           signal_handler_info[idx][nr].func));

      /* store old handler */
      signal_handler_info[idx][nr].func =
        signal (nr, signal_handler_info[idx][nr].func);
    }

  DBUG_LEAVE ();
}

void
epc__reset_signal_handlers (const int idx)
{
  epc__set_signal_handlers (idx);
}

void
epc__call_print (epc__call_t * call)
/* ----------------------------------------------------------------------
 * Prints out all fields of a call in readable format
 * ----------------------------------------------------------------------*/
{
  DBUG_ENTER ("epc__call_print");
  DBUG_PRINT ("input", ("msg info: '%s'", call->msg_info));
  DBUG_PRINT ("input",
              ("interface: '%s'; function: '%s'",
               (call != NULL && call->interface != NULL
                && call->interface->name !=
                NULL ? call->interface->name : "(null)"), (call != NULL
                                                           && call->
                                                           function != NULL
                                                           && call->function->
                                                           name !=
                                                           NULL ? call->
                                                           function->
                                                           name : "(null)")));
  DBUG_PRINT ("input",
              ("status: %ld; error code: %ld", (long) call->epc__error,
               (long) call->errcode));
  DBUG_LEAVE ();
}

static int
epc__cmp_function (const void *par1, const void *par2)
{
  epc__function_t *fnc1 = (epc__function_t *) par1;
  epc__function_t *fnc2 = (epc__function_t *) par2;

  return strcmp (fnc1->name, fnc2->name);
}

static int
epc__cmp_interface (const void *par1, const void *par2)
{
  epc__interface_t *ifc1 = *((epc__interface_t **) par1);
  epc__interface_t *ifc2 = *((epc__interface_t **) par2);

  return strcmp (ifc1->name, ifc2->name);
}

static
/*@observer@*/
char *
get_error_str (epc__error_t err)
{
  switch (err)
    {
    case OK:
      return "OK";
    case MSG_TIMED_OUT:
      return "MSG_TIMED_OUT";
    case MSG_TOO_BIG:
      return "MSG_TOO_BIG";
    case MSG_INTERRUPTED:
      return "MSG_INTERRUPTED";
    case RECEIVE_ERROR:
      return "RECEIVE_ERROR";
    case EXEC_ERROR:
      return "EXEC_ERROR";
    case PARSE_ERROR:
      return "PARSE_ERROR";
    case MEMORY_ERROR:
      return "MEMORY_ERROR";
    case DATATYPE_UNKNOWN:
      return "DATATYPE_UNKNOWN";
    case FUNCTION_UNKNOWN:
      return "FUNCTION_UNKNOWN";
    case PARAMETER_UNKNOWN:
      return "PARAMETER_UNKNOWN";
    case INTERFACE_UNKNOWN:
      return "INTERFACE_UNKNOWN";
    case PARAMETER_MODE_UNKNOWN:
      return "PARAMETER_MODE_UNKNOWN";
    case SEND_ERROR:
      return "SEND_ERROR";
    case CONNECT_ERROR:
      return "CONNECT_ERROR";
    case DISCONNECT_ERROR:
      return "DISCONNECT_ERROR";
    default:
      return "Unknown error";
    }
}

epc__info_t *
epc__init (void)
{
  epc__info_t *epc__info = NULL;

  DBUG_ENTER ("epc__init");

  epc__info = (epc__info_t *) malloc (sizeof (epc__info_t));

  if (epc__info == NULL)
    {
      exit (EXIT_FAILURE);
    }

  epc__info->logon = NULL;
  epc__info->connected = 0;
  epc__info->pipe = NULL;
  epc__info->num_interfaces = 0;
  epc__info->interfaces = NULL;
  epc__info->sqlca = NULL;
  epc__info->xml_info = NULL;
  epc__info->purge_pipe = 0;
  epc__info->interrupt = 0;
  epc__info->program = NULL;
  (void) epc__xml_init (epc__info);

  DBUG_PRINT ("info", ("epc__info: %p", (void *) epc__info));

  DBUG_LEAVE ();

  return epc__info;
}

void
epc__done (epc__info_t * epc__info)
{
  DBUG_ENTER ("epc__done");

  DBUG_PRINT ("input", ("epc__info: %p", epc__info));

  (void) epc__xml_done (epc__info);

  if (epc__info->sqlca != NULL)
    free (epc__info->sqlca);

  if (epc__info->interfaces != NULL)
    {
      dword_t inr, fnr, pnr;

      for (inr = 0; inr < epc__info->num_interfaces; inr++)
        {
          epc__interface_t *interface = epc__info->interfaces[inr];

          /* free memory for the parameters */
          for (fnr = 0; fnr < interface->num_functions; fnr++)
            for (pnr = 0; pnr < interface->functions[fnr].num_parameters;
                 pnr++)
              {
                /* detected by dmalloc */
                if (interface->functions[fnr].parameters[pnr].data != NULL) {
                  free (interface->functions[fnr].parameters[pnr].data);
                  interface->functions[fnr].parameters[pnr].data = NULL;
                }
              }
        }

      free (epc__info->interfaces);
    }

  /* not freeing epc__info->logon detected by dmalloc */
  if (epc__info->logon != NULL)
    free(epc__info->logon);

  if (epc__info->pipe != NULL)
    free(epc__info->pipe);

  free (epc__info);

  DBUG_LEAVE ();
}

epc__error_t
epc__add_interface (epc__info_t * epc__info, epc__interface_t * interface)
{
  epc__error_t status = OK;

  DBUG_ENTER ("epc__add_interface");
  DBUG_PRINT ("input", ("epc__info: %p", (void *) epc__info));

  if (epc__info == NULL)
    {
      status = MEMORY_ERROR;
    }
  else
    {
      epc__info->num_interfaces++;
      epc__info->interfaces =
        (epc__interface_t **)
        realloc ((void *) epc__info->interfaces,
                 (size_t) (epc__info->num_interfaces *
                           sizeof (epc__interface_t **)));

      if (epc__info->interfaces == NULL)
        {
          epc__info->num_interfaces--;
          status = MEMORY_ERROR;
        }
      else
        {
          dword_t fnr, pnr;

          epc__info->interfaces[epc__info->num_interfaces - 1] = interface;

          /* sort the functions */
          qsort (interface->functions,
                 (size_t) interface->num_functions,
                 sizeof (interface->functions[0]), epc__cmp_function);

          /* sort the interfaces */
          qsort (epc__info->interfaces,
                 (size_t) epc__info->num_interfaces,
                 sizeof (epc__info->interfaces[0]), epc__cmp_interface);

          /* allocate memory for the parameters */
          for (fnr = 0; fnr < interface->num_functions; fnr++)
            for (pnr = 0; pnr < interface->functions[fnr].num_parameters;
                 pnr++)
              {
                size_t size = 0;

                switch (interface->functions[fnr].parameters[pnr].type)
                  {
                  case C_STRING:
                  case C_XML:
                  case C_DATE:
                  case C_INT:
                  case C_LONG:
                  case C_FLOAT:
                  case C_DOUBLE:
                    size =
                      (size_t) interface->functions[fnr].parameters[pnr].size;
                    break;

                  case C_VOID:
                    continue;

                  default:
                    assert (interface->functions[fnr].parameters[pnr].type >=
                            C_DATATYPE_MIN
                            && interface->functions[fnr].parameters[pnr].
                            type <= C_DATATYPE_MAX);
                  }
                if ((interface->functions[fnr].parameters[pnr].data =
                     malloc (size)) == NULL)
                  {
                    status = MEMORY_ERROR;
                    break;
                  }
              }
        }
    }

  DBUG_PRINT ("output", ("status: %d", (int) status));
  DBUG_LEAVE ();

  return (status);
}

epc__error_t
epc__set_pipe (epc__info_t * epc__info, char *pipe)
{
  epc__error_t status = OK;

  DBUG_ENTER ("epc__set_pipe");

  DBUG_PRINT ("input", ("epc__info: %p", (void *) epc__info));

  if (epc__info == NULL)
    {
      status = MEMORY_ERROR;
    }
  else if (pipe == NULL)
    {
      if (epc__info->pipe != NULL)
        {
          free (epc__info->pipe);
          epc__info->pipe = NULL;
        }
    }
  else
    {
      epc__info->pipe = (char *) realloc (epc__info->pipe, strlen (pipe) + 1);
      if (epc__info->pipe == NULL)
        status = MEMORY_ERROR;
      else
        strcpy (epc__info->pipe, pipe);
    }

  DBUG_PRINT ("output", ("status: %d", (int) status));

  DBUG_LEAVE ();
  return (status);
}

epc__error_t
epc__set_logon (epc__info_t * epc__info, char *logon)
{
  epc__error_t status = OK;

  DBUG_ENTER ("epc__set_logon");

  DBUG_PRINT ("input", ("epc__info: %p", (void *) epc__info));

  if (epc__info == NULL)
    {
      status = MEMORY_ERROR;
    }
  else if (logon == NULL)
    {
      if (epc__info->logon != NULL)
        {
          free (epc__info->logon);
          epc__info->logon = NULL;
        }
    }
  else
    {
      epc__info->logon =
        (char *) realloc (epc__info->logon, strlen (logon) + 1);
      if (epc__info->logon == NULL)
        status = MEMORY_ERROR;
      else
        strcpy (epc__info->logon, logon);
    }

  DBUG_PRINT ("output", ("status: %d", (int) status));

  DBUG_LEAVE ();

  return (status);
}

static void
epc__response_soap(epc__call_t * epc__call)
{
  dword_t nr;

  assert(epc__call->function != NULL);
  assert(epc__call->interface != NULL);

  if (epc__call->inline_namespace[0] != '\0')
    {
      /* copy */
      (void) snprintf (epc__call->msg_response,
                       MAX_MSG_RESPONSE_LEN + 1,
                       SOAP_HEADER_START
                       "<%s:%sResponse xmlns:%s='%s'>\n",
                       epc__call->inline_namespace,
                       epc__call->function->name,
                       epc__call->inline_namespace,
                       epc__call->interface->name);
    }
  else
    {
      /* copy */
      (void) snprintf (epc__call->msg_response,
                       MAX_MSG_RESPONSE_LEN + 1,
                       SOAP_HEADER_START "<%sResponse xmlns='%s'>\n",
                       epc__call->function->name,
                       epc__call->interface->name);
    }

  for (nr = 0; nr < epc__call->function->num_parameters; nr++)
    {
      if (epc__call->function->parameters[nr].mode != C_IN)
        {
          /* append */
          (void) snprintf (epc__call->msg_response + strlen(epc__call->msg_response),
                           MAX_MSG_RESPONSE_LEN - strlen(epc__call->msg_response) + 1,
                           "<%s>",
                           epc__call->function->parameters[nr].name);

          switch (epc__call->function->parameters[nr].type)
            {
            case C_XML:
            case C_DATE:
              /* append */
              (void) strncat (epc__call->msg_response,
                              (char *) epc__call->function->
                              parameters[nr].data,
                              MAX_MSG_RESPONSE_LEN -
                              strlen(epc__call->msg_response));
              break;

            case C_STRING:
              /* append */
              (void) snprintf (epc__call->msg_response + strlen(epc__call->msg_response),
                               MAX_MSG_RESPONSE_LEN - strlen(epc__call->msg_response) + 1,
                               "<![CDATA[%s]]>",
                               (char *) epc__call->function->
                               parameters[nr].data);
              break;

            case C_INT:
              /* append */
              (void) snprintf (epc__call->msg_response + strlen(epc__call->msg_response),
                               MAX_MSG_RESPONSE_LEN - strlen(epc__call->msg_response) + 1,
                               "%d",
                               *((idl_int_t *) epc__call->function->
                                 parameters[nr].data));
              break;

            case C_LONG:
              /* append */
              (void) snprintf (epc__call->msg_response + strlen(epc__call->msg_response),
                               MAX_MSG_RESPONSE_LEN - strlen(epc__call->msg_response) + 1,
                               "%ld",
                               *((idl_long_t *) epc__call->function->
                                 parameters[nr].data));
              break;

            case C_FLOAT:
              /* append */
              (void) snprintf (epc__call->msg_response + strlen(epc__call->msg_response),
                               MAX_MSG_RESPONSE_LEN - strlen(epc__call->msg_response) + 1,
                               "%f",
                               (double) (*
                                         ((idl_float_t *) epc__call->
                                          function->parameters[nr].
                                          data)));
              break;

            case C_DOUBLE:
              /* append */
              (void) snprintf (epc__call->msg_response + strlen(epc__call->msg_response),
                               MAX_MSG_RESPONSE_LEN - strlen(epc__call->msg_response) + 1,
                               "%f",
                               *((idl_double_t *) epc__call->
                                 function->parameters[nr].data));
              break;

            case C_VOID:        /* procedure */
              assert (epc__call->function->parameters[nr].mode ==
                      C_OUT);
              break;

            default:
              assert (epc__call->function->parameters[nr].type >=
                      C_DATATYPE_MIN
                      && epc__call->function->parameters[nr].type <=
                      C_DATATYPE_MAX);

            }

          /* append */
          (void) snprintf (epc__call->msg_response + strlen(epc__call->msg_response),
                           MAX_MSG_RESPONSE_LEN - strlen(epc__call->msg_response) + 1,
                           "</%s>\n",
                           epc__call->function->parameters[nr].name);
        }
    }

  if (epc__call->inline_namespace[0] != '\0')
    {
      /* append */
      (void) snprintf (epc__call->msg_response + strlen(epc__call->msg_response),
                       MAX_MSG_RESPONSE_LEN - strlen(epc__call->msg_response) + 1,
                       "</%s:%sResponse>" SOAP_HEADER_END "\n",
                       epc__call->inline_namespace,
                       epc__call->function->name);
    }
  else
    {
      /* append */
      (void) snprintf (epc__call->msg_response + strlen(epc__call->msg_response),
                       MAX_MSG_RESPONSE_LEN - strlen(epc__call->msg_response) + 1,
                       "</%sResponse>" SOAP_HEADER_END "\n",
                       epc__call->function->name);
    }
}

static void
epc__response_xmlrpc(epc__call_t * epc__call)
{
  dword_t nr;

  /* copy (strncy does not always add a terminating zero but snprintf does) */
  (void) snprintf (epc__call->msg_response,
                   MAX_MSG_RESPONSE_LEN + 1,
                   "<methodResponse><params>\n");

  assert(epc__call->function != NULL);

  for (nr = 0; nr < epc__call->function->num_parameters; nr++)
    {
      if (epc__call->function->parameters[nr].mode != C_IN)
        {
          /* append */
          (void) strncat (epc__call->msg_response,
                          "<param><value>",
                          MAX_MSG_RESPONSE_LEN - strlen(epc__call->msg_response));

          switch (epc__call->function->parameters[nr].type)
            {
            case C_DATE:
              /* append */
              (void) snprintf (epc__call->msg_response + strlen(epc__call->msg_response),
                               MAX_MSG_RESPONSE_LEN - strlen(epc__call->msg_response) + 1,
                               "<dateTime.iso8601>%s</dateTime.iso8601>",
                               (char *) epc__call->function->parameters[nr].data);
              break;

            case C_STRING:
              /* append */
              (void) snprintf (epc__call->msg_response + strlen(epc__call->msg_response),
                               MAX_MSG_RESPONSE_LEN - strlen(epc__call->msg_response) + 1,
                               "<string><![CDATA[%s]]></string>",
                               (char *) epc__call->function->parameters[nr].data);
              break;

            case C_INT:
              /* append */
              (void) snprintf (epc__call->msg_response + strlen(epc__call->msg_response),
                               MAX_MSG_RESPONSE_LEN - strlen(epc__call->msg_response) + 1,
                               "<int>%d</int>",
                               *((idl_int_t *) epc__call->function->parameters[nr].data));
              break;

            case C_LONG:
              /* append */
              (void) snprintf (epc__call->msg_response + strlen(epc__call->msg_response),
                               MAX_MSG_RESPONSE_LEN - strlen(epc__call->msg_response) + 1,
                               "<int>%ld</int>",
                               *((idl_long_t *) epc__call->function->parameters[nr].data));
              break;

            case C_FLOAT:
              /* append */
              (void) snprintf (epc__call->msg_response + strlen(epc__call->msg_response),
                               MAX_MSG_RESPONSE_LEN - strlen(epc__call->msg_response) + 1,
                               "<double>%f</double>",
                               (double) (*((idl_float_t *) epc__call->function->parameters[nr].data)));
              break;

            case C_DOUBLE:
              /* append */
              (void) snprintf (epc__call->msg_response + strlen(epc__call->msg_response),
                               MAX_MSG_RESPONSE_LEN - strlen(epc__call->msg_response) + 1,
                               "<double>%f</double>",
                               *((idl_double_t *) epc__call->function->parameters[nr].data));
              break;

            case C_VOID:        /* procedure */
              assert (epc__call->function->parameters[nr].mode == C_OUT);
              break;

            default:
              assert (epc__call->function->parameters[nr].type >= C_DATATYPE_MIN
                      && epc__call->function->parameters[nr].type <= C_DATATYPE_MAX);

            }

          /* append */
          (void) strncat (epc__call->msg_response,
                          "</value></param>\n",
                          MAX_MSG_RESPONSE_LEN - strlen(epc__call->msg_response));
        }
    }

  (void) strncat (epc__call->msg_response,
                  "</params></methodResponse>\n",
                  MAX_MSG_RESPONSE_LEN - strlen(epc__call->msg_response));
}

static epc__error_t
epc__exec_call (epc__info_t * epc__info, epc__call_t * epc__call)
{
  unsigned int result;

  DBUG_ENTER ("epc__exec_call");

  assert (epc__call->epc__error == OK);

  switch (EPC__CALL_PROTOCOL(epc__call))
    {
    case PROTOCOL_DBMS_PIPE:
      break;

    case PROTOCOL_SOAP:
    case PROTOCOL_XMLRPC:
      result = epc__xml_parse (epc__info, epc__call, epc__call->msg_request,
                               strlen (epc__call->msg_request));
      break;

    default:
      assert(EPC__CALL_PROTOCOL(epc__call) >= PROTOCOL_MIN && EPC__CALL_PROTOCOL(epc__call) <= PROTOCOL_MAX);
    }

  if (result != 0 || epc__call->epc__error != OK)
    {
      assert (epc__call->epc__error == PARSE_ERROR ||
              epc__call->epc__error == INTERFACE_UNKNOWN ||
              epc__call->epc__error == FUNCTION_UNKNOWN ||
              epc__call->epc__error == PARAMETER_UNKNOWN);
    }
  else
    {
      assert (epc__call->interface != NULL);
      assert (epc__call->function != NULL);

      (*epc__call->function->function) (epc__call->function);

      /* construct the response for non oneway functions */
      if (epc__call->function->oneway == 0)
        {
          switch (EPC__CALL_PROTOCOL(epc__call))
            {
            case PROTOCOL_DBMS_PIPE:
              break;

            case PROTOCOL_SOAP:
              epc__response_soap(epc__call);
              break;

            case PROTOCOL_XMLRPC:
              epc__response_xmlrpc(epc__call);
              break;
            }
        }
    }

  DBUG_LEAVE ();

  return epc__call->epc__error;
}

/* =====================================================================
 *
 * EXPORTED FUNCTIONS
 *
 *======================================================================*/

long
epc__handle_request (epc__info_t * epc__info,
                     epc__call_t * epc__call,
                     epc__error_t (*recv_request) (epc__info_t *,
                                                   epc__call_t *),
                     epc__error_t (*send_response) (epc__info_t *,
                                                    epc__call_t *))
     /* ----------------------------------------------------------------------
      * Handles a single request 
      * - receives a request message
      * - executes the call
      * - sends back the results
      * ----------------------------------------------------------------------*/
{
  long retval = -1L;

  DBUG_ENTER ("epc__handle_request");

  DBUG_PRINT ("input", ("epc__info: %p", (void *) epc__info));

  do
    {
      /* receive the request */
      (void) (*recv_request) (epc__info, epc__call);

      DBUG_PRINT ("info", ("msg_request: %s", epc__call->msg_request));

      /* do the call */
      if (epc__call->epc__error != OK)
        break;

      (void) epc__exec_call (epc__info, epc__call);

      if (!(epc__call->epc__error == OK ||
            epc__call->epc__error == PARSE_ERROR ||
            epc__call->epc__error == INTERFACE_UNKNOWN ||
            epc__call->epc__error == FUNCTION_UNKNOWN ||
            epc__call->epc__error == PARAMETER_UNKNOWN))
        {
          break;
        }

      assert (epc__call->epc__error != OK ||
              (epc__call->function != NULL && epc__call->interface != NULL));

      /* send the response in case of errors or for non oneway functions  */
      if (epc__call->epc__error != OK ||
          (epc__call->function != NULL && epc__call->function->oneway == 0))
        {
          DBUG_PRINT ("info", ("msg_response: %s", epc__call->msg_response));

          (void) (*send_response) (epc__info, epc__call);
        }

      if (epc__call->epc__error != OK)
        break;

      retval = 0L;
    }
  while (0);                    /* loop is used only to be able to break out earlier */

  /* GJP 18-10-2004 Do not know why dbms_pipe errors should be ignored */
  /*#define OBSOLETE 1 */
#ifdef OBSOLETE
  switch (epc__call->epc__error)
    {
    case MSG_TIMED_OUT:
    case RECEIVE_ERROR:
    case SEND_ERROR:
    case OK:
      switch (epc__call->errcode)
        {
        case -6556:             /* the pipe is empty, cannot fulfill the unpack_message request */
        case -6558:             /* buffer in dbms_pipe package is full. No more items allowed */
        case -6559:             /* wrong datatype requested, string, actual datatype is string */
        case 0:
          retval = 0;
          break;

        default:
          break;
        }
      break;

    default:
      break;
    }
#endif

  DBUG_PRINT ("output",
              ("retval: %ld; error code: %ld; epc status: %s",
               retval, (long) epc__call->errcode,
               get_error_str (epc__call->epc__error)));
  DBUG_LEAVE ();

  return retval;
}

epc__error_t
epc__handle_requests (epc__info_t * epc__info,
                      epc__error_t (*recv_request) (epc__info_t *,
                                                    epc__call_t *),
                      epc__error_t (*send_response) (epc__info_t *,
                                                     epc__call_t *))
     /* ----------------------------------------------------------------------
      * Handles all requests received over the specified database pipe:
      * - receives a request message
      * - unpacks the specifications of the requested call
      * - executes the call
      * - packs status and results of the call
      * - sends back the results
      * ----------------------------------------------------------------------*/
{
  epc__call_t epc__call = EPC__CALL_INIT;

  DBUG_ENTER ("epc__handle_requests");

  DBUG_PRINT ("input", ("epc__info: %p", (void *) epc__info));

  for (epc__call.errcode = 0; G_signo == 0; epc__call.errcode = 0)
    {
      if (epc__handle_request
          (epc__info, &epc__call, recv_request, send_response) != 0)
        break;
    }

  if (G_signo != 0)
    {
      (void) printf ("Signal %d received\n", G_signo);
    }

  DBUG_PRINT ("output",
              ("error code: %ld; epc status: %s",
               (long) epc__call.errcode,
               get_error_str (epc__call.epc__error)));

  DBUG_LEAVE ();

  return epc__call.epc__error;
}

static void
handle_signal (int signo)
{
  epc__set_signal_handlers (1);

  (void) fprintf (stderr, "Received signal %d\n", signo);

#ifdef SERVER_INTERRUPT

  if (G_epc__handle_interrupt != NULL)
    {
      (*G_epc__handle_interrupt) (G_epc__info_interrupt);
    }

#endif

  G_signo = signo;

  epc__reset_signal_handlers (1);
}

#ifdef SERVER_INTERRUPT

void
epc__set_handle_interrupt (epc__handle_interrupt_t handle_interrupt,
                           epc__info_t * epc__info)
{
  G_epc__handle_interrupt = handle_interrupt;
  G_epc__info_interrupt = epc__info;
}

#endif

int
epc__get_signo (void)
{
  return G_signo;
}


/**
 * @brief Lookup the interface in the list of interfaces.
 *
 * @param interface_name  The interface name
 * @param epc__info       The EPC run-time information
 * @param epc__call       The interface member is set if the interface is found.
 *                        If not found the epc__error member is set to
 *                        INTERFACE_UNKNOWN.
 *
 ******************************************************************************/
void
epc__lookup_interface (const char *interface_name, epc__info_t * epc__info,
                       epc__call_t * epc__call)
{
  dword_t inr;
  int result;

  assert (epc__info->interfaces != NULL);
  assert ( interface_name != NULL );

  epc__call->interface = NULL;
  /* get the interface */
  for (inr = 0; inr < epc__info->num_interfaces; inr++)
    if ((result =
         strcmp (interface_name, epc__info->interfaces[inr]->name)) == 0)
      {
        epc__call->interface = epc__info->interfaces[inr];
        break;
      }
    else if (result < 0)        /* interfaces sorted ascending */
      {
        break;
      }

  if (epc__call->interface == NULL)
    {
      /* interface not found */
      fprintf (stderr, "ERROR: interface %s not found\n", interface_name);
      epc__call->epc__error = INTERFACE_UNKNOWN;
    }
}

/**
 * @brief Lookup the function in the list of functions of an interface.
 *
 * @param function_name   The function name
 * @param epc__info       The EPC run-time information
 * @param epc__call       The interface member is set if the interface is found.
 *                        If not found the epc__error member is set to 
 *                        FUNCTION_UNKNOWN.
 *
 ******************************************************************************/
void
epc__lookup_function (const char *function_name,
                      /*@unused@ */ epc__info_t * epc__info,
                      epc__call_t * epc__call)
{
  dword_t fnr;
  int result;

  assert(function_name != NULL);

  epc__call->function = NULL;

  if (epc__call->interface != NULL)
    {
      /* get the function */
      for (fnr = 0;
           fnr < epc__call->interface->num_functions; fnr++)
        if ((result =
             strcmp (function_name,
                     epc__call->interface->functions[fnr].name)) == 0)
          {
            epc__call->function = &epc__call->interface->functions[fnr];
            break;
          }
        else if (result < 0)    /* interface functions sorted ascending */
          {
            break;
          }
    }

  if (epc__call->function == NULL)
    {
      /* interface not found */
      fprintf (stderr, "ERROR: function '%s' not found\n", function_name);
      epc__call->epc__error = FUNCTION_UNKNOWN;
    }
}
