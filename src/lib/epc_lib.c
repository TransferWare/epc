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

#include <dbug.h>
#include "epc.h"
#ifndef XML_OFF
#include "epc_xml.h"
#endif
#include "epc_lib.h"

/* strtof is not a standard function */
extern float strtof(const char* s, /*@null@ */ char** endptr);

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

#ifdef SERVER_INTERRUPT

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
  /* GJP 16-02-2009
     Some defined signals may have the same number hence a switch will not compile.
  */
#ifdef SIGHUP
  if (signo == SIGHUP)
    return "SIGHUP";
#endif
#ifdef SIGINT
  if (signo == SIGINT)
    return "SIGINT";
#endif
#ifdef SIGQUIT
  if (signo == SIGQUIT)
    return "SIGQUIT";
#endif
#ifdef SIGILL
  if (signo == SIGILL)
    return "SIGILL";
#endif
#ifdef SIGTRAP
  if (signo == SIGTRAP)
    return "SIGTRAP";
#endif
#ifdef SIGABRT
  if (signo == SIGABRT)
    return "SIGABRT";
#endif
#ifdef SIGEMT
  if (signo == SIGEMT)
    return "SIGEMT";
#endif
#ifdef SIGFPE
  if (signo == SIGFPE)
    return "SIGFPE";
#endif
#ifdef SIGBUS
  if (signo == SIGBUS)
    return "SIGBUS";
#endif
#ifdef SIGSEGV
  if (signo == SIGSEGV)
    return "SIGSEGV";
#endif
#ifdef SIGSYS
  if (signo == SIGSYS)
    return "SIGSYS";
#endif
#ifdef SIGPIPE
  if (signo == SIGPIPE)
    return "SIGPIPE";
#endif
#ifdef SIGALRM
  if (signo == SIGALRM)
    return "SIGALRM";
#endif
#ifdef SIGTERM
  if (signo == SIGTERM)
    return "SIGTERM";
#endif
#ifdef SIGURG
  if (signo == SIGURG)
    return "SIGURG";
#endif
#ifdef SIGSTOP
  if (signo == SIGSTOP)
    return "SIGSTOP";
#endif
#ifdef SIGTSTP
  if (signo == SIGTSTP)
    return "SIGTSTP";
#endif
#ifdef SIGCONT
  if (signo == SIGCONT)
    return "SIGCONT";
#endif
#ifdef SIGCHLD
  if (signo == SIGCHLD)
    return "SIGCHLD";
#endif
#ifdef SIGTTIN
  if (signo == SIGTTIN)
    return "SIGTTIN";
#endif
#ifdef SIGTTOU
  if (signo == SIGTTOU)
    return "SIGTTOU";
#endif
#ifdef SIGIO
  if (signo == SIGIO)
    return "SIGIO";
#endif
#ifdef SIGXCPU
  if (signo == SIGXCPU)
    return "SIGXCPU";
#endif
#ifdef SIGXFSZ
  if (signo == SIGXFSZ)
    return "SIGXFSZ";
#endif
#ifdef SIGVTALRM
  if (signo == SIGVTALRM)
    return "SIGVTALRM";
#endif
#ifdef SIGPROF
  if (signo == SIGPROF)
    return "SIGPROF";
#endif
#ifdef SIGWINCH
  if (signo == SIGWINCH)
    return "SIGWINCH";
#endif
#ifdef SIGLOST
  if (signo == SIGLOST)
    return "SIGLOST";
#endif
#ifdef SIGUSR1
  if (signo == SIGUSR1)
    return "SIGUSR1";
#endif
#ifdef SIGUSR2
  if (signo == SIGUSR2)
    return "SIGUSR2";
#endif
#ifdef SIGBREAK
  if (signo == SIGBREAK)
    return "SIGBREAK";
#endif
  return "Unknown signal";
}

#endif /* #ifdef SERVER_INTERRUPT */

void
epc__set_signal_handlers (const int idx)
{
#ifdef SERVER_INTERRUPT
  int nr;
#endif

  DBUG_ENTER ("epc__set_signal_handlers");

#ifdef SERVER_INTERRUPT
  for (nr = 0; nr < MAX_SIGNO; nr++)
    {
      /* GJP 16-02-2009
         Some defined signals may have the same number hence a switch will not compile.
      */
      if (0
#ifdef SIGHUP
          || (nr == SIGHUP)
#endif
#ifdef SIGQUIT
          || (nr == SIGQUIT)
#endif
#ifdef SIGILL
          || (nr == SIGILL)
#endif
#ifdef SIGTRAP
          || (nr == SIGTRAP)
#endif
#ifdef SIGABRT
          || (nr == SIGABRT)
#endif
#ifdef SIGEMT
          || (nr == SIGEMT)
#endif
#ifdef SIGFPE
          || (nr == SIGFPE)
#endif
#ifdef SIGBUS
          || (nr == SIGBUS)
#endif
#ifdef SIGSEGV
          || (nr == SIGSEGV)
#endif
#ifdef SIGSYS
          || (nr == SIGSYS)
#endif
#ifdef SIGPIPE
          || (nr == SIGPIPE)
#endif
#ifdef SIGALRM
          || (nr == SIGALRM)
#endif
#ifdef SIGTERM
          || (nr == SIGTERM)
#endif
#ifdef SIGURG
          || (nr == SIGURG)
#endif
#ifdef SIGSTOP
          || (nr == SIGSTOP)
#endif
#ifdef SIGTSTP
          || (nr == SIGTSTP)
#endif
#ifdef SIGCONT
          || (nr == SIGCONT)
#endif
#ifdef SIGCHLD
          || (nr == SIGCHLD)
#endif
#ifdef SIGTTIN
          || (nr == SIGTTIN)
#endif
#ifdef SIGTTOU
          || (nr == SIGTTOU)
#endif
#ifdef SIGIO
          || (nr == SIGIO)
#endif
#ifdef SIGXCPU
          || (nr == SIGXCPU)
#endif
#ifdef SIGXFSZ
          || (nr == SIGXFSZ)
#endif
#ifdef SIGVTALRM
          || (nr == SIGVTALRM)
#endif
#ifdef SIGPROF
          || (nr == SIGPROF)
#endif
#ifdef SIGWINCH
          || (nr == SIGWINCH)
#endif
#ifdef SIGLOST
          || (nr == SIGLOST)
#endif
#ifdef SIGUSR1
          || (nr == SIGUSR1)
#endif
#ifdef SIGUSR2
          || (nr == SIGUSR2)
#endif
#ifdef SIGBREAK
          || (nr == SIGBREAK)
#endif
          )
        {
          DBUG_PRINT ("info", ("handler for signal %d (%s): %p",
                               nr,
                               signal_str (nr),
                               signal_handler_info[idx][nr].func));

          /* store old handler */
          signal_handler_info[idx][nr].func =
            signal (nr, signal_handler_info[idx][nr].func);
        }
      else
        {
          /* SIGINT or other */
          continue;             /* GJP 21-02-2004 Handled by handle_interrupt() */
        }
    }
#endif /* #ifdef SERVER_INTERRUPT */

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
  DBUG_PRINT ("input", ("msg info: '%s'; protocol: %c", call->msg_info, EPC__CALL_PROTOCOL(call)));
  DBUG_PRINT ("input",
              ("interface name: '%s'; function: %p",
               (call != NULL && call->interface != NULL
                && call->interface->name !=
                NULL ? call->interface->name : "(null)"),
               (void*) call->function));
  if (call->function != NULL)
    {
      epc__function_print(call->function);
    }
  DBUG_PRINT ("output",
              ("status: %ld; error code: %ld", (long) call->epc__error,
               (long) call->errcode));
  DBUG_LEAVE ();
}

void
epc__function_print (epc__function_t * function)
/* ----------------------------------------------------------------------
 * Prints out all fields of a function in readable format
 * ----------------------------------------------------------------------*/
{
  dword_t nr;
  
  DBUG_ENTER ("epc__function_print");
  DBUG_PRINT ("input", ("name: '%s'; oneway: %ld; # parameters: %ld",
                        function->name, function->oneway, function->num_parameters));
  for (nr = 0L; nr < function->num_parameters; nr++)
    {
      DBUG_PRINT ("input",
                  ("parameter[%ld]: name: %s; mode: %d; type: %d; size: %ld",
                   nr,
                   function->parameters[nr].name,
                   function->parameters[nr].mode,
                   function->parameters[nr].type,
                   function->parameters[nr].size));
    }
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

/*@observer@*/
char *
epc__get_error_str (epc__error_t err)
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
#ifndef XML_OFF
  epc__info->xml_info = NULL;
#endif
  epc__info->purge_pipe = 0;
  epc__info->interrupt = 0;
  epc__info->program = NULL;
  /* GJP 2022-12-14 It must be possible to create a private request pipe with a custom maximum pipe size. */
  epc__info->max_pipe_size = 8192;
  epc__info->private = 0;
#ifndef XML_OFF
  (void) epc__xml_init (epc__info);
#endif

  DBUG_PRINT ("info", ("epc__info: %p", (void *) epc__info));

  DBUG_LEAVE ();

  return epc__info;
}

void
epc__done (epc__info_t * epc__info)
{
  DBUG_ENTER ("epc__done");
  DBUG_PRINT ("input", ("epc__info: %p", epc__info));

#ifndef XML_OFF
  (void) epc__xml_done (epc__info);
#endif

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

void
epc__chk_parameter (epc__parameter_t * parameter)
{
  assert(parameter != NULL);
  assert(parameter->name != NULL);
  assert(parameter->mode >= C_PARAMETER_MODE_MIN && parameter->mode <= C_PARAMETER_MODE_MAX);
  assert(parameter->type >= C_DATATYPE_MIN && parameter->type <= C_DATATYPE_MAX);
  assert(parameter->size >= 0);
}

void
epc__chk_function (epc__function_t * function)
{
  dword_t nr;
  
  assert(function != NULL);
  assert(function->name != NULL);
  assert(function->oneway == 0 || function->oneway == 1);
  assert(function->num_parameters >= 0);
  for (nr = 0; nr < function->num_parameters; nr++)
    {
      epc__chk_parameter(&function->parameters[nr]);
    }
}

void
epc__chk_interface (epc__interface_t * interface)
{
  dword_t nr;

  assert(interface != NULL);
  assert(interface->name != NULL);
  assert(interface->num_functions > 0);
  for (nr = 0; nr < interface->num_functions; nr++)
    {
      epc__chk_function(&interface->functions[nr]);
    }
}

epc__error_t
epc__add_interface (epc__info_t * epc__info, epc__interface_t * interface)
{
  epc__error_t status = OK;

  DBUG_ENTER ("epc__add_interface");

  epc__chk_interface (interface);
    
  DBUG_PRINT ("input", ("epc__info: %p; interface: %s", (void *) epc__info, interface->name));

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
  DBUG_PRINT ("input", ("epc__info: %p; pipe: %s", (void *) epc__info, pipe));

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


static
void
epc__response_native(epc__call_t * epc__call)
{
  dword_t nr;
  char num[100] = "";

  DBUG_ENTER ("epc__response_native");

  epc__call->msg_response[0] = '\0';

  /* error code */
  (void) snprintf (num,
                   sizeof(num),
                   "%d",
                   (int) epc__call->epc__error);
  /* append */
  (void) snprintf (epc__call->msg_response + strlen(epc__call->msg_response),
                   MAX_MSG_RESPONSE_LEN - strlen(epc__call->msg_response) + 1,
                   "%1d%02X%s",
                   (int) C_INT,
                   (unsigned int) strlen(num),
                   num);

  for (nr = 0; epc__call->function != NULL && nr < epc__call->function->num_parameters; nr++)
    {
      if (epc__call->function->parameters[nr].mode != C_IN)
        {
          switch (epc__call->function->parameters[nr].type)
            {
            case C_DATE:
              /* append */
              (void) snprintf (epc__call->msg_response + strlen(epc__call->msg_response),
                               MAX_MSG_RESPONSE_LEN - strlen(epc__call->msg_response) + 1,
                               "%1d%02X%s",
                               (int) epc__call->function->parameters[nr].type,
                               (unsigned int) strlen((char *) epc__call->function->parameters[nr].data),
                               (char *) epc__call->function->parameters[nr].data);
              break;

            case C_STRING:
            case C_XML:
              /* append */
              (void) snprintf (epc__call->msg_response + strlen(epc__call->msg_response),
                               MAX_MSG_RESPONSE_LEN - strlen(epc__call->msg_response) + 1,
                               "%1d%04X%s",
                               (int) epc__call->function->parameters[nr].type,
                               (unsigned int) strlen((char *) epc__call->function->parameters[nr].data),
                               (char *) epc__call->function->parameters[nr].data);
              break;

            case C_INT:
              (void) snprintf (num,
                               sizeof(num),
                               "%d",
                               *((idl_int_t *) epc__call->function->parameters[nr].data));
              /* append */
              (void) snprintf (epc__call->msg_response + strlen(epc__call->msg_response),
                               MAX_MSG_RESPONSE_LEN - strlen(epc__call->msg_response) + 1,
                               "%1d%02X%s",
                               (int) epc__call->function->parameters[nr].type,
                               (unsigned int) strlen(num),
                               num);
              break;

            case C_LONG:
              (void) snprintf (num,
                               sizeof(num),
                               "%ld",
                               *((idl_long_t *) epc__call->function->parameters[nr].data));
              /* append */
              (void) snprintf (epc__call->msg_response + strlen(epc__call->msg_response),
                               MAX_MSG_RESPONSE_LEN - strlen(epc__call->msg_response) + 1,
                               "%1d%02X%s",
                               (int) epc__call->function->parameters[nr].type,
                               (unsigned int) strlen(num),
                               num);
              break;

            case C_FLOAT:
              (void) snprintf (num,
                               sizeof(num),
                               "%f",
                               (double) *((idl_float_t *) epc__call->function->parameters[nr].data));
              /* append */
              (void) snprintf (epc__call->msg_response + strlen(epc__call->msg_response),
                               MAX_MSG_RESPONSE_LEN - strlen(epc__call->msg_response) + 1,
                               "%1d%02X%s",
                               (int) epc__call->function->parameters[nr].type,
                               (unsigned int) strlen(num),
                               num);
              break;

            case C_DOUBLE:
              (void) snprintf (num,
                               sizeof(num),
                               "%f",
                               *((idl_double_t *) epc__call->function->parameters[nr].data));
              /* append */
              (void) snprintf (epc__call->msg_response + strlen(epc__call->msg_response),
                               MAX_MSG_RESPONSE_LEN - strlen(epc__call->msg_response) + 1,
                               "%1d%02X%s",
                               (int) epc__call->function->parameters[nr].type,
                               (unsigned int) strlen(num),
                               num);
              break;

            case C_VOID:        /* procedure */
              assert (epc__call->function->parameters[nr].mode == C_OUT);
              break;

            default:
              assert (epc__call->function->parameters[nr].type >= C_DATATYPE_MIN
                      && epc__call->function->parameters[nr].type <= C_DATATYPE_MAX);

            }
        }
    }

  DBUG_PRINT ("output", ("epc__call->msg_response: %s", epc__call->msg_response));
  DBUG_LEAVE();
}


static void
epc__response_soap(epc__call_t * epc__call)
{
  dword_t nr;

  DBUG_ENTER ("epc__response_soap");

#ifndef XML_OFF
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
#endif
  
  DBUG_LEAVE();
}

static void
epc__response_xmlrpc(epc__call_t * epc__call)
{
  dword_t nr;

  DBUG_ENTER ("epc__response_xmlrpc");

#ifndef XML_OFF
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
#endif

  DBUG_LEAVE();
}

static
int
epc__native_parse_argument (idl_type_t type, char **msg_request, /*@out@ */unsigned int *len)
{
  int retval;

  DBUG_ENTER ("epc__native_parse_argument");
  DBUG_PRINT ("input", ("type: %d; msg_request: %s", (int)type, *msg_request));

  /* read an argument:
     a) verify data type
     b) read length in hexadecimal format (4 bytes for string/xml and 2 for others)
     c) read data
  */

  /* verify that the actual data type matches the expected data type */
  assert((idl_type_t)(**msg_request - '0') == type);

  /* read length */

  (*msg_request)++;

  switch (type)
    {
    case C_XML:
    case C_STRING:
      /*@+ignoresigns@*/
      retval = sscanf(*msg_request, "%04X", len);
      /*@=ignoresigns@*/
      (*msg_request) += 4;
      break;

    default:
      /*@+ignoresigns@*/
      retval = sscanf(*msg_request, "%02X", len);
      /*@=ignoresigns@*/
      (*msg_request) += 2;
      break;
    }

  DBUG_PRINT("output", ("msg_request: %.*s", *len, *msg_request));
  DBUG_LEAVE();

  return retval;
}

static
epc__error_t
epc__native_parse (epc__info_t *epc__info, epc__call_t *epc__call, const char *msg_request)
{
  dword_t nr;
  unsigned int len;
  char *ptr = (char *)msg_request;
  char interface_name[MAX_INTERFACE_NAME_LEN+1] = "";
  char function_name[MAX_FUNC_NAME_LEN+1] = "";

  DBUG_ENTER("epc__native_parse");
  DBUG_PRINT("input", ("msg_request: %s", msg_request));

  do {
    if (epc__call->epc__error != OK)
      {
        break;
      }

    if (1 !=
        epc__native_parse_argument (C_STRING, &ptr, &len))
      {
        epc__call->epc__error = PARSE_ERROR;
        break;
      }
    
    if ((size_t) len >= sizeof(interface_name))
      {
        epc__call->epc__error = BUFFER_OVERFLOW;
        break;
      }

    (void) strncpy (interface_name, ptr, (size_t)len);
    interface_name[len] = '\0';
    ptr += len;

    if (1 !=
        epc__native_parse_argument (C_STRING, &ptr, &len))
      {
        epc__call->epc__error = PARSE_ERROR;
        break;
      }
    
    if ((size_t) len >= sizeof(function_name))
      {
        epc__call->epc__error = BUFFER_OVERFLOW;
        break;
      }

    (void) strncpy (function_name, ptr, (size_t)len);
    function_name[len] = '\0';
    ptr += len;

    epc__lookup_interface (interface_name, epc__info, epc__call);
    epc__lookup_function (function_name, epc__info, epc__call);

    switch (epc__call->epc__error)
      {
      case INTERFACE_UNKNOWN:
        break;
          
      case FUNCTION_UNKNOWN:
        break;

      case OK:
        assert(epc__call->function != NULL);

        for (nr = 0; nr < epc__call->function->num_parameters; nr++)
          {
            if (epc__call->function->parameters[nr].mode != C_OUT)
              {
                DBUG_PRINT("info", ("name: %s", epc__call->function->parameters[nr].name));
                
                if (1 !=
                    epc__native_parse_argument (epc__call->function->parameters[nr].type, &ptr, &len))
                  {
                    epc__call->epc__error = PARSE_ERROR;
                    break;
                  }

                if (OK !=
                    epc__set_parameter (ptr, (size_t) len, &epc__call->function->parameters[nr]))
                  {
                    epc__call->epc__error = PARSE_ERROR;
                    break;
                  }

                ptr += len;
              }
          }
        break;

      default:
        assert (epc__call->epc__error == INTERFACE_UNKNOWN
                || epc__call->epc__error == FUNCTION_UNKNOWN
                || epc__call->epc__error == OK);
      }
  } while (0);

  epc__call_print(epc__call);
    
  DBUG_LEAVE();

  return epc__call->epc__error;
}


static
epc__error_t
epc__exec_call (epc__info_t * epc__info, epc__call_t * epc__call)
{
  unsigned int result = 0;

  DBUG_ENTER ("epc__exec_call");

  assert (epc__call->epc__error == OK);

  switch (EPC__CALL_PROTOCOL(epc__call))
    {
    case PROTOCOL_NATIVE:
      result = (unsigned int) epc__native_parse (epc__info, epc__call, epc__call->msg_request);
      break;

    case PROTOCOL_SOAP:
    case PROTOCOL_XMLRPC:
#ifndef XML_OFF
      result = epc__xml_parse (epc__info, epc__call, epc__call->msg_request,
                               strlen (epc__call->msg_request));
#endif
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

      (*epc__call->function->function) (epc__call);
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

      /* do the call */
      if (epc__call->epc__error == OK)
        {
          (void) epc__exec_call (epc__info, epc__call);
        }

      /* construct the response when oneway is equal to 0 (no function counts 
         too since we must report back the error) */
      if (!(epc__call->function != NULL && epc__call->function->oneway != 0))
        {
          switch (EPC__CALL_PROTOCOL(epc__call))
            {
            case PROTOCOL_NATIVE:
            case PROTOCOL_SOAP:
            case PROTOCOL_XMLRPC:
              
              switch (EPC__CALL_PROTOCOL(epc__call))
                {
                case PROTOCOL_NATIVE:
                  epc__response_native(epc__call);
                  break;
                  
                case PROTOCOL_SOAP:
                  epc__response_soap(epc__call);
                  break;

                case PROTOCOL_XMLRPC:
                  epc__response_xmlrpc(epc__call);
                  break;
                  
                default:
                  assert(EPC__CALL_PROTOCOL(epc__call) >= PROTOCOL_MIN && EPC__CALL_PROTOCOL(epc__call) <= PROTOCOL_MAX);
                  break;
                }
              DBUG_PRINT ("info", ("msg_response: %s", epc__call->msg_response));

              (void) (*send_response) (epc__info, epc__call);
              break;

            default:
              break;
            }
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
               epc__get_error_str (epc__call->epc__error)));
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

  DBUG_PRINT ("output",
              ("error code: %ld; epc status: %s",
               (long) epc__call.errcode,
               epc__get_error_str (epc__call.epc__error)));

  DBUG_LEAVE ();

  return epc__call.epc__error;
}

#ifdef SERVER_INTERRUPT

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

#endif /* #ifdef SERVER_INTERRUPT */

void
epc__set_handle_interrupt (/*@unused@ */ epc__handle_interrupt_t handle_interrupt,
                           /*@unused@ */ epc__info_t * epc__info)
{
#ifdef SERVER_INTERRUPT
  G_epc__handle_interrupt = handle_interrupt;
  G_epc__info_interrupt = epc__info;
#endif
}

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

epc__error_t
epc__set_parameter (const char *ch, size_t len, epc__parameter_t *parameter)
{
  epc__error_t result = OK;
  char num[100] = ""; /* ch is not null terminated, hence copy it to num for numbers and use strtoX */

  DBUG_ENTER("epc__set_parameter");
  DBUG_PRINT("input", ("parameter: %s", parameter->name));

  switch (parameter->type)
    {
    case C_XML:
      /* append data */
      if ((dword_t) (len + strlen((char *) parameter->data)) < parameter->size)
        {
          (void) snprintf( ((char *) parameter->data) + 
                           strlen((char *) parameter->data),
                           (size_t)(parameter->size -
                                    strlen((char *) parameter->data)),
                           "%.*s", (int)len, (char *)ch);
          DBUG_PRINT("info", ("value: %s", (char *) parameter->data));
        }
      else
        {
          result = BUFFER_OVERFLOW;
        }
      break;

    case C_STRING:
    case C_DATE:
      if ((dword_t) len < parameter->size)
        {
          (void) strncpy ((char *) parameter->data,
                          (char *) ch, len);
          ((char *) parameter->data)[len] = '\0';
          DBUG_PRINT("info", ("value: %s", (char *) parameter->data));
        }
      else
        {
          result = BUFFER_OVERFLOW;
        }
      break;

    case C_INT:
    case C_LONG:
    case C_DOUBLE:
    case C_FLOAT:
      assert(len < sizeof(num));
      (void) strncpy (num, ch, len);
      num[len] = '\0';
      switch (parameter->type)
        {
        case C_INT:
          *((idl_int_t *) parameter->data) =
            (int) strtol (num, NULL, 10);
          DBUG_PRINT("info", ("value: %d", *((idl_int_t *) parameter->data)));
          break;

        case C_LONG:
          *((idl_long_t *) parameter->data) =
            strtol (num, NULL, 10);
          DBUG_PRINT("info", ("value: %ld", *((idl_long_t *) parameter->data)));
          break;

        case C_FLOAT:
          *((idl_float_t *) parameter->data) =
            strtof (num, NULL);
          DBUG_PRINT("info", ("value: %f", (double) *((idl_float_t *) parameter->data)));
          break;

        case C_DOUBLE:
          *((idl_double_t *) parameter->data) =
            strtod (num, NULL);
          DBUG_PRINT("info", ("value: %f", *((idl_double_t *) parameter->data)));
          break;

        default:
          break;
        }
      break;

    case C_VOID:                /* impossible */
      assert (parameter->type != C_VOID);
      break;

    default:
      assert (parameter->type >= C_DATATYPE_MIN &&
              parameter->type <= C_DATATYPE_MAX);
    }

  DBUG_LEAVE();

  return result;
}
