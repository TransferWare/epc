#ifndef EPC_TYPES_H
#define EPC_TYPES_H

/* Oracle pipe name length is 128.
   Add 1 for the terminating zero and round till next multiple of 4.
   msg_info contains pipe name and the message sequence number as a 4 
   character hexadecimal string.
 */
#define MAX_PIPE_NAME_LEN       128 
#define MAX_MSG_INFO_LEN        (4+MAX_PIPE_NAME_LEN)
#define MSG_INFO_SIZE           (MAX_MSG_INFO_LEN+1+3)
#define MAX_MSG_REQUEST_LEN     4046
#define MSG_REQUEST_SIZE        (MAX_MSG_REQUEST_LEN+1+1) /* MAX_MSG_REQUEST_LEN+1 rounded till next multiple of 4 */
#define MAX_MSG_RESPONSE_LEN    4082
#define MSG_RESPONSE_SIZE       (MAX_MSG_RESPONSE_LEN+1+1) /* MAX_MSG_RESPONSE_LEN+1 rounded till next multiple of 4 */

#include <idl_defs.h> /* constants used by idl and epc */

/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 * make sure all structs are double word (4 bytes) aligned 
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */

typedef struct epc__parameter {
  char *name;
  idl_mode_t mode;
  idl_type_t type;
  dword_t size; /* for a string including the terminating zero */
  void *data;
} epc__parameter_t;

struct epc__call;

typedef struct epc__function {
  char *name;
  void (*function)( struct epc__function * );
  dword_t oneway;
  dword_t num_parameters;
  epc__parameter_t *parameters;
} epc__function_t;

typedef struct epc__interface {
  char *name;
  dword_t num_functions;
  epc__function_t *functions;
} epc__interface_t;

/*
 * epc__info_t: general info for running an EPC server
 */
typedef struct epc__info {
  char *logon;
  dword_t connected;
  char *pipe;
  dword_t num_interfaces;
  epc__interface_t **interfaces; /* pointing to a list of interfaces */
  struct sqlca *sqlca; /* SQLCA area */
  void *xml_info;
} epc__info_t;

typedef struct epc__call {
  char msg_info[MSG_INFO_SIZE];
  char msg_request[MSG_REQUEST_SIZE];
  char msg_response[MSG_RESPONSE_SIZE];
  epc__interface_t *interface;
  epc__function_t *function;
  long epc__error; /* result of call */
  long errcode;   /* error code returned by transport medium */
} epc__call_t;

#define EPC__CALL_INIT { "", "", "", NULL, NULL, OK, 0L }

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
  PARAMETER_UNKNOWN = -10,
  FUNCTION_UNKNOWN = -11,
  INTERFACE_UNKNOWN = -12,

  /* connect/disconnect problems */
  CONNECT_ERROR = -13,
  DISCONNECT_ERROR = -14
} epc__error_t;

#endif
