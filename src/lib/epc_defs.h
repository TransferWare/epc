#ifndef _EPC_TYPES_H_
#define _EPC_TYPES_H_

#define MAX_PIPE_NAME_LEN	128  /* must be a multiple of 4 */

#include "idl_defs.h"  /* constants used by idl and epc */

/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 * make sure all structs are double word (4 bytes) aligned 
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */

typedef struct {
  idl_mode_t mode;
  idl_type_t type;
  union {
    int	ival;
    long	lval;
    double	dval;
    float	fval;
    char *	sval;
  } uval;
  void *value;	/* points to one of the union members above */
} epc_parameter_t;


typedef struct {
  char result_pipe[MAX_PIPE_NAME_LEN];
  char interface_name[MAX_INTERFACE_NAME_LEN];
  char function_name[MAX_FUNC_NAME_LEN];
  long epc_error; /* result of call */
  long sqlcode;   /* sql error code */
} epc_call_t;

#define CALL_INIT { "", "", "", OK, NULL }

typedef struct {
  char *name;
  idl_type_t type;
  void (*function) ( epc_call_t * );
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
  char *pipe;
  dword_t num_interfaces;
  epc_interface_t **interfaces; /* pointing to a list of interfaces */
} epc_info_t;

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
  INTERFACE_UNKNOWN = -11
} epc_error_t;

#endif
