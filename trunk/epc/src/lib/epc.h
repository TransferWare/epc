#ifndef _EPC_H_
#define _EPC_H_

#include "epc_defs.h"

extern epc_error_t epc_main( int argc, char **argv, epc_interface_t * );

extern epc_info_t *epc_init( void );

extern void epc_done( epc_info_t **epc_info );

extern epc_error_t epc_set_logon( epc_info_t *epc_info, char *logon );

extern epc_error_t epc_set_pipe( epc_info_t *epc_info, char *pipe );

extern epc_error_t epc_add_interface( epc_info_t *epc_info, epc_interface_t *interface );

extern long epc_handle_request( epc_info_t *epc_info, epc_call_t *call );

extern epc_error_t epc_handle_requests( epc_info_t *epc_info );
 
#endif
