#ifndef EPC_H
#define EPC_H

#include <epc_defs.h>

/*
|| Because Oracle uses epc_ functions as well on Linux (epc_init),
|| the epc functions are renamed to epc__. 
*/

typedef void (*epc__handle_interrupt_t)(epc__info_t*);

extern
void
epc__set_signal_handlers( const int idx );

extern
void
epc__reset_signal_handlers( const int idx );

extern
void
epc__call_print ( epc__call_t * call );

extern
int
epc__get_signo(void);

extern
void
epc__set_handle_interrupt(epc__handle_interrupt_t handle_interrupt, epc__info_t *epc__info);

extern
epc__error_t
epc__main( int argc, char **argv, epc__interface_t * );

extern
epc__error_t
epc__list_main( int argc, char **argv, epc__interface_t *, ... );

extern
epc__info_t *
epc__init( void );

extern
void
epc__done( epc__info_t **epc__info );

extern
epc__error_t
epc__set_logon( epc__info_t *epc__info, char *logon );

extern
epc__error_t
epc__set_pipe( epc__info_t *epc__info, char *pipe );

extern
epc__error_t
epc__add_interface( epc__info_t *epc__info, epc__interface_t *interface );

extern
long
epc__handle_request( epc__info_t *epc__info, 
                    epc__call_t *call,
                    epc__error_t (*recv_request)( epc__info_t *, epc__call_t * ),
                    epc__error_t (*send_response)( epc__info_t *, epc__call_t * ));

extern
epc__error_t
epc__handle_requests( epc__info_t *epc__info,
                     epc__error_t (*recv_request)( epc__info_t *, epc__call_t * ),
                     epc__error_t (*send_response)( epc__info_t *, epc__call_t * ) );
 
extern
void
epc__abort( char *msg );

extern
epc__error_t
epc__connect( epc__info_t *epc__info );

extern
epc__error_t
epc__disconnect( epc__info_t *epc__info );

#endif
