#ifndef EPC_TYPES_H
#define EPC_TYPES_H

/* Oracle pipe name length is 128.
   Add 1 for the terminating zero and round till next multiple of 4.
   msg_info contains:
   - protocol (5 for SOAP and 6 for XMLRPC, see also package epc_clnt).

   For calls which expect a response (non oneway) this is added to msg_info:
   - the message sequence number as a 4 character hexadecimal string.
   - pipe name
 */
#define MAX_PIPE_NAME_LEN       128
#define MAX_MSG_INFO_LEN        (1+4+MAX_PIPE_NAME_LEN)
#define MSG_INFO_SIZE           (MAX_MSG_INFO_LEN+1+2)
#define MAX_MSG_REQUEST_LEN     4046
#define MSG_REQUEST_SIZE        (MAX_MSG_REQUEST_LEN+1+1)       /* MAX_MSG_REQUEST_LEN+1 rounded till next multiple of 4 */
#define MAX_MSG_RESPONSE_LEN    4082
#define MSG_RESPONSE_SIZE       (MAX_MSG_RESPONSE_LEN+1+1)      /* MAX_MSG_RESPONSE_LEN+1 rounded till next multiple of 4 */
#define INLINE_NAMESPACE_SIZE   32

#include <idl_defs.h>           /* constants used by idl and epc */

/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 * make sure all structs are double word (4 bytes) aligned 
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */

typedef struct epc__parameter
{
  /*@observer@ */ char *name;
  idl_mode_t mode;
  idl_type_t type;
  dword_t size;                 /* for a string including the terminating zero */
  void *data;
} epc__parameter_t;

struct epc__call;

typedef struct epc__function
{
  /*@observer@ */ char *name;
  void (*function) (struct epc__call *);
  dword_t oneway;
  dword_t num_parameters;
  /*@dependent@ */ epc__parameter_t *parameters;
} epc__function_t;

typedef struct epc__interface
{
  /*@observer@ */ char *name;
  dword_t num_functions;
  /*@dependent@ */ epc__function_t *functions;
} epc__interface_t;

#ifndef XML_OFF
/* forward reference */
struct xml_info;
#endif

/*
 * epc__info_t: general info for running an EPC server
 */
typedef struct epc__info
{
  /*@only@ *//*@null@ */ char *logon;
  dword_t connected;
  /*@only@ *//*@null@ */ char *pipe;
  dword_t num_interfaces;
  /*@only@ *//*@null@ */ epc__interface_t **interfaces;
  /* pointing to a list of interfaces */
  /*@only@ *//*@null@ */ struct sqlca *sqlca;
  /* SQLCA area */
#ifndef XML_OFF
  /*@only@ *//*@null@ */ struct xml_info *xml_info;
#endif
  dword_t purge_pipe;
  dword_t interrupt;
  /*@null@ */ /*@observer@ */ char *program;
} epc__info_t;

typedef /*@null@ *//*@only@ */ epc__info_t *epc__info_ptr_t;

typedef struct epc__call
{
  char msg_info[MSG_INFO_SIZE];
  char msg_request[MSG_REQUEST_SIZE];
  char msg_response[MSG_RESPONSE_SIZE];
  /*@temp@ *//*@null@ */ epc__interface_t *interface;
  /*@temp@ *//*@null@ */ epc__function_t *function;
#ifndef XML_OFF  
  char inline_namespace[INLINE_NAMESPACE_SIZE]; /* ns1 in xmlns:ns1="<interface>" */
#else  
  char dummy[INLINE_NAMESPACE_SIZE]; /* ns1 in xmlns:ns1="<interface>" */
#endif
  long epc__error;              /* result of call */
  long errcode;                 /* error code returned by transport medium */
} epc__call_t;

#define PROTOCOL_SOAP '5'
#define PROTOCOL_MIN PROTOCOL_SOAP
#define PROTOCOL_XML_MIN PROTOCOL_SOAP

#define PROTOCOL_XMLRPC '6'
#define PROTOCOL_XML_MAX PROTOCOL_XMLRPC

#define PROTOCOL_NATIVE '7'
#define PROTOCOL_MAX PROTOCOL_NATIVE


#define EPC__CALL_PROTOCOL(epc__call) (epc__call->msg_info[0])

#define EPC__CALL_INIT { "", "", "", NULL, NULL, "", OK, 0L }

typedef enum
{
  OK = 0,

  /* dbms_pipe.send_message/dbms_pipe.receive_message status */
  MSG_TIMED_OUT = -1,
  MSG_TOO_BIG = -2,
  MSG_INTERRUPTED = -3,

  /* miscellaneous send/receive errors */
  RECEIVE_ERROR = -4,
  SEND_ERROR = -5,

  /* message could not be parsed */
  PARSE_ERROR = -6,

  /* error during execution of function requested */
  EXEC_ERROR = -7,

  /* could not allocate a value */
  MEMORY_ERROR = -8,

  /* illegal values in message */
  DATATYPE_UNKNOWN = -9,
  PARAMETER_MODE_UNKNOWN = -10,
  PARAMETER_UNKNOWN = -11,
  FUNCTION_UNKNOWN = -12,
  INTERFACE_UNKNOWN = -13,

  /* connect/disconnect problems */
  CONNECT_ERROR = -14,
  DISCONNECT_ERROR = -15,

  /* value to large to be held */
  BUFFER_OVERFLOW = -16
} epc__error_t;

#endif
