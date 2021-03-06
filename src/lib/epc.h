#ifndef EPC_H
#define EPC_H

#include <epc_defs.h>

/*
|| Because Oracle uses epc_ functions as well on Linux (epc_init),
|| the epc functions are renamed to epc__. 
*/

typedef void (*epc__handle_interrupt_t) ( /*@null@ */ epc__info_t *);

extern void epc__set_signal_handlers (const int idx);

extern void epc__reset_signal_handlers (const int idx);

extern void epc__call_print ( /*@notnull@ */ epc__call_t * call);

extern void epc__function_print ( /*@notnull@ */ epc__function_t * function);

extern int epc__get_signo (void);

extern void
epc__lookup_interface (const char *interface_name, epc__info_t * epc__info,
                       epc__call_t * epc__call);

extern void
epc__lookup_function (const char *function_name,
                      /*@unused@ */ epc__info_t * epc__info,
                      epc__call_t * epc__call);

extern void
epc__set_handle_interrupt (epc__handle_interrupt_t handle_interrupt,
                           epc__info_t * epc__info);

extern epc__error_t
epc__main (int argc, char **argv, /*@notnull@ */ epc__interface_t *);

extern epc__error_t
epc__list_main (int argc, char **argv, /*@notnull@ */ epc__interface_t *,
                ...);

extern /*@observer@*/ char *
epc__get_error_str (epc__error_t err);

extern /*@notnull@ *//*@only@ */ epc__info_t *
epc__init (void);

extern void
epc__done ( /*@notnull@ *//*@only@ */ epc__info_t *epc__info) /*@modifies epc__info */ ;

extern epc__error_t
epc__set_logon ( /*@notnull@ */ epc__info_t * epc__info,        
                 /*@null@ */  char *logon) /*@modifies epc__info->logon */ ;

extern epc__error_t
epc__set_pipe ( /*@notnull@ */ epc__info_t * epc__info, 
                /*@null@ */ char *pipe) /*@modifies epc__info->pipe */ ;

extern void
epc__chk_parameter (epc__parameter_t * parameter);

extern void
epc__chk_function (epc__function_t * function);

extern void
epc__chk_interface (epc__interface_t * interface);

extern epc__error_t
epc__add_interface ( /*@notnull@ */ epc__info_t * epc__info,
                     /*@notnull@ */ epc__interface_t * interface);

extern long epc__handle_request ( /*@notnull@ */ epc__info_t * epc__info,
                                  /*@notnull@ */ epc__call_t * call,
                                  epc__error_t (*recv_request)(epc__info_t *, epc__call_t *),
                                  epc__error_t (*send_response)(epc__info_t *, epc__call_t *));

extern epc__error_t
epc__handle_requests ( /*@notnull@ */ epc__info_t * epc__info,
                       epc__error_t (*recv_request) (epc__info_t *, epc__call_t *),
                       epc__error_t (*send_response) (epc__info_t *, epc__call_t *));

extern void
epc__abort( char *msg );

extern epc__error_t
epc__connect ( /*@notnull@ */ epc__info_t * epc__info);

extern epc__error_t
epc__interrupt( /*@notnull@ */ epc__info_t *epc__info );

extern epc__error_t
epc__disconnect ( /*@notnull@ */ epc__info_t * epc__info);

extern epc__error_t
epc__recv_request_pipe( epc__info_t *epc__info, epc__call_t *epc__call );

extern epc__error_t
epc__send_response_pipe( epc__info_t * epc__info, epc__call_t * epc__call );

#endif
