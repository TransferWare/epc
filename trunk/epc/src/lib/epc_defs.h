#ifndef _EPC_TYPES_H_
#define _EPC_TYPES_H_

#define MAX_PIPE_NAME_LEN	255
#define MAX_FUNC_NAME_LEN	255
#define MAX_INTERFACE_NAME_LEN	255
#define MAX_STR_VAL_LEN		4096
#define MAX_FUNCTIONS		100
#define MAX_PARAMETERS		20

#include "idl_defs.h"  /* constants used by idl and epc */

typedef struct {
	int mode;
	int type;
	union {
		int	ival;
		long	lval;
		double	dval;
		float	fval;
		char *	sval;
	} uval;
	void *value;	/* points to one of the union members above */
} parameter_t;

typedef struct {
	char result_pipe[MAX_PIPE_NAME_LEN];
	char function_name[MAX_FUNC_NAME_LEN];
	parameter_t return_value;	/* return value is considered a parameter */
	int num_parameters;
	parameter_t parameters[MAX_PARAMETERS];
} call_t;

#define CALL_INIT { "", "", { 0 }, 0, { { 0 } } }

typedef struct {
	char *name;
	int type;
	void (*function) ( call_t * );
} function_t;

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
	FUNCTION_UNKNOWN = -10
} error_t;

#endif
