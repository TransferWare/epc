#ifndef _EPC_TYPES_H_
#define _EPC_TYPES_H_

#define MAXLEN 255
#define MAXNUMFUNS 100
#define MAXNUMPARMS 20

typedef struct {
	int mode;
	int type;
	void *value;
} parameter_t;

typedef struct {
	char function_name[MAXLEN];
	int return_type;
	void *return_value;
	int num_parameters;
	parameter_t parameters[MAXNUMPARMS];
} call_t;

typedef struct {
	char *name;
	int type;
	void (*function) ( call_t * );
} function_t;

typedef enum {
	OK = 0,
	MSG_TOO_BIG = -1,
	MSG_INTERRUPTED = -2,
	FUNCTION_UNKNOWN = -3,
	PARAMETER_UNKNOWN = -4,
	EMPTY_PIPE = -5,
	NO_RESULT_PIPE = -6,
	NO_FUNCTION = -7,
	NO_PARAMETER = -8,
	RECEIVE_ERROR = -9,
	EXEC_ERROR = -10
} error_t;

#endif
