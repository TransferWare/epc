#ifndef EPC_TYPES_H
#define EPC_TYPES_H

/* Oracle pipe name length is 128.
   Add 1 for the terminating zero and round till next multiple of 4.
 */
#define MAX_PIPE_NAME_LEN       132  
#define MAX_MSG_INFO_LEN        (4+MAX_PIPE_NAME_LEN)

#include <idl_defs.h> /* constants used by idl and epc */

/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 * make sure all structs are double word (4 bytes) aligned 
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */

typedef struct {
  idl_mode_t mode;
  idl_type_t type;
  dword_t size;
  void *data;
} epc_parameter_t;

struct epc_call;

typedef struct epc_function {
  char *name;
  void (*function)( struct epc_call *, struct epc_function * );
  dword_t oneway;
  dword_t num_parameters;
  epc_parameter_t *parameters;
} epc_function_t;

typedef struct {
  char *name;
  dword_t num_functions;
  epc_function_t *functions;
} epc_interface_t;

/*
 * epc_info_t: general info for running an EPC server
 */
typedef struct {
  char *logon;
  dword_t connected;
  char *pipe;
  dword_t first_time; /* first time epc_handle_request() is called */
  dword_t num_interfaces;
  epc_interface_t **interfaces; /* pointing to a list of interfaces */
  struct sqlca *sqlca; /* SQLCA area */
} epc_info_t;

typedef struct epc_call {
  char msg_info[MAX_MSG_INFO_LEN];
  epc_interface_t *interface;
  epc_function_t *function;
  long epc_error; /* result of call */
  long sqlcode;   /* sql error code */
} epc_call_t;

#define CALL_INIT { "", NULL, NULL, OK, 0L }

typedef enum {
  OK = 0,

  /* dbms_pipe.send_message/dbms_pipe.receive_message status */
  MSG_TIMED_OUT = -1,
  MSG_TOO_BIG = -2,
  MSG_INTERRUPTED = -3,

  /* miscellaneous send/receive errors */
  RECEIVE_ERROR = -4,
  SEND_ERROR = -5,

  /* error during execution of function requested */
  EXEC_ERROR = -6,

  /* could not allocate a value */
  MEMORY_ERROR = -7,

  /* illegal values in message */
  DATATYPE_UNKNOWN = -8,
  PARAMETER_MODE_UNKNOWN = -9,
  FUNCTION_UNKNOWN = -10,
  INTERFACE_UNKNOWN = -11,

  /* connect/disconnect problems */
  CONNECT_ERROR = -12,
  DISCONNECT_ERROR = -13
} epc_error_t;

#endif
