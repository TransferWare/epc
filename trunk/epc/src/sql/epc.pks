REMARK
REMARK  $Header$
REMARK
REMARK  Description:    Oracle package specification for External Procedure Call Toolkit.
REMARK
REMARK  $Log$
REMARK  Revision 1.2  2004/04/05 14:52:33  gpaulissen
REMARK  Interface changed
REMARK
REMARK  Revision 1.1  2004/04/02 10:26:28  gpaulissen
REMARK  New interface for epc
REMARK
REMARK
REMARK

create or replace package epc as
/**
--
-- This package is used to implement RPC like functionality on Oracle.
-- Messages are sent by the client to a server. The transport mechanisms
-- supported are database pipes (DBMS_PIPE) and TCP/IP (UTL_TCP).
--
*/

/* see $EPC_HOME/src/lib/idl_defs.h */
subtype interface_name_subtype is varchar2(32);
subtype routine_name_subtype is varchar2(512);
subtype parameter_name_subtype is varchar2(32);
subtype data_type_subtype is pls_integer;
subtype parameter_mode_subtype is pls_integer;
subtype pipe_name_subtype is varchar2(128);

subtype int_subtype is integer;
subtype long_subtype is integer;
subtype float_subtype is float;
subtype double_subtype is double precision;
subtype string_subtype is varchar2(4096);

/* 
|| EXCEPTIONS
*/

e_illegal_null_value exception;
e_wrong_protocol     exception;
e_comm_error         exception;
e_msg_timed_out      exception;
e_msg_too_big        exception;
e_msg_interrupted    exception;

/*ORA-06558 is raised if the message buffer overflows (currently 4096 bytes)*/

/**
-- Get the string data type constant.
*/
function data_type_string
return data_type_subtype;

/**
-- Get the int data type constant.
*/
function data_type_int
return data_type_subtype;

/**
-- Get the long data type constant.
*/
function data_type_long
return data_type_subtype;

/**
-- Get the float data type constant.
*/
function data_type_float
return data_type_subtype;

/**
-- Get the double data type constant.
*/
function data_type_double
return data_type_subtype;

/* 
|| Connection related functions/procedures.
*/

end epc;
/
