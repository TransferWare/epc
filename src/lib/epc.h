#ifndef _EPC_H_
#define _EPC_H_

#include "epc_defs.h"

/*
|| Because Oracle uses epc_ functions as well on Linux (epc_init),
|| I will rename the EPC functions to ts_epc (Transfer Solutions)
|| Be sure that in ts_epc-def.cpp the ts_ names are used.
*/

#define epc_add_interface ts_epc_add_interface
#define epc_done ts_epc_done
#define epc_handle_request ts_epc_handle_request
#define epc_handle_requests ts_epc_handle_requests
#define epc_init ts_epc_init
#define epc_main ts_epc_main
#define epc_list_main ts_epc_list_main
#define epc_set_logon ts_epc_set_logon
#define epc_set_pipe ts_epc_set_pipe

extern epc_error_t epc_main( int argc, char **argv, epc_interface_t * );

extern epc_error_t epc_list_main( int argc, char **argv, epc_interface_t *, ... );

extern epc_info_t *epc_init( void );

extern void epc_done( epc_info_t **epc_info );

extern epc_error_t epc_set_logon( epc_info_t *epc_info, char *logon );

extern epc_error_t epc_set_pipe( epc_info_t *epc_info, char *pipe );

extern epc_error_t epc_add_interface( epc_info_t *epc_info, epc_interface_t *interface );

extern long epc_handle_request( epc_info_t *epc_info, epc_call_t *call );

extern epc_error_t epc_handle_requests( epc_info_t *epc_info );
 
#endif
