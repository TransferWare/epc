REMARK
REMARK  $Header$
REMARK
REMARK  Description:    Oracle package specification for External Procedure Call Toolkit.
REMARK
REMARK  $Log$
REMARK  Revision 1.4  2004/05/21 15:04:34  gpaulissen
REMARK  Eerste implementatie
REMARK
REMARK  Revision 1.3  2004/04/21 11:16:56  gpaulissen
REMARK  .
REMARK
REMARK  Revision 1.2  2004/04/05 14:52:33  gpaulissen
REMARK  Interface changed
REMARK
REMARK  Revision 1.1  2004/04/02 10:26:28  gpaulissen
REMARK  New interface for epc
REMARK
REMARK
REMARK

create or replace package epc_clnt as
/**
--
-- This package is used to implement the client side of RPC like functionality
-- on an Oracle database.
-- Messages are sent by the client to a server. The transport mechanisms
-- supported are database pipes (DBMS_PIPE) and TCP/IP (UTL_TCP).
--
-- The flow of procedure calls will typically look like this:
-- 1) Set connection information.
--    a) epc_clnt.register
--    b) epc_clnt.set_connection_info (optional for database pipes)
--    c) epc_clnt.set_request_send_timeout (optional)
--    d) epc_clnt.set_response_recv_timeout (optional)
-- 2) Marshall a function call into a message
--    a) epc_clnt.new_request
--    b) epc_clnt.set_request_parameter (for all IN and IN OUT parameters)
-- 3) Send the message
--    a) epc_clnt.send_request
-- 4) Receive the response
--    a) epc_clnt.recv_response
-- 5) Unmarshall the message
--    a) epc_clnt.get_response_parameter (for all OUT and IN OUT parameters)
*/

subtype epc_key_subtype is binary_integer;

/**
-- Register an interface
--
-- @param p_interface_name  The name of the interface to register.
--
-- @return A unique key which has to be used in all other epc_clnt calls.
*/
function register( p_interface_name in epc.interface_name_subtype )
return epc_key_subtype;

/**
-- Get the key
--
-- @param p_interface_name  The name of the interface to register.
--
-- @exception no_data_found
--
-- @return The unique key for the interface
*/
function get_epc_key( p_interface_name in epc.interface_name_subtype )
return epc_key_subtype;

/* 
|| Connection related functions/procedures.
*/

/**
-- Set the connection type to TCP/IP and store the open TCP/IP
-- connection for later use. The EPC will not open or close
-- the TCP/IP connection. It is the responsability of the client
-- program to open and close the connection.
-- 
-- @param p_epc_key     The key
-- @param p_connection  An open TCP/IP connection (see utl_tcp.open_connection)
*/
procedure set_connection_info
(
  p_epc_key in epc_key_subtype
, p_connection in utl_tcp.connection
);

/**
-- Get the TCP/IP connection. 
-- 
-- @param p_epc_key     The key
-- @param p_connection  An open TCP/IP connection
--                      (see utl_tcp.open_connection)
--
-- @exception no_data_found  connection method is not utl_tcp
*/
procedure get_connection_info
(
  p_epc_key in epc_key_subtype
, p_connection out utl_tcp.connection
);

/**
-- Set the connection type to database pipes and store the pipe name for
-- later use. Each interface may have a different connection.
-- 
-- @param p_epc_key    The key
-- @param p_pipe_name  The request pipe name
*/
procedure set_connection_info
(
  p_epc_key in epc_key_subtype
, p_pipe_name in epc.pipe_name_subtype
);

/**
-- Get the database request pipe.
-- 
-- @param p_epc_key    The key
-- @param p_pipe_name  The request pipe name
--
-- @exception no_data_found  connection method is not dbms_pipe
*/
procedure get_connection_info
(
  p_epc_key in epc_key_subtype
, p_pipe_name out epc.pipe_name_subtype
);

/**
-- Set the request send timeout.
-- 
-- @param p_epc_key               The key
-- @param p_request_send_timeout  The request send timeout
*/
procedure set_request_send_timeout
(
  p_epc_key in epc_key_subtype
, p_request_send_timeout in pls_integer
);

/**
-- Set the response receive timeout.
-- 
-- @param p_epc_key                The key
-- @param p_response_recv_timeout  The response receive timeout
*/
procedure set_response_recv_timeout
(
  p_epc_key in epc_key_subtype
, p_response_recv_timeout in pls_integer
);

/**
-- Start a new request
-- 
-- @param p_epc_key    The key
*/
procedure new_request
(
  p_epc_key in epc_key_subtype
);

/**
-- Set a request parameter (IN or IN OUT).
-- 
-- @param p_epc_key    The key
-- @param p_name       The name of the parameter
-- @param p_data_type  The data type (should be epc.data_type_string)
-- @param p_value      The value of the parameter
--
-- @exception e_illegal_null_value  p_value is NULL
*/
procedure set_request_parameter
(
  p_epc_key in epc_key_subtype
, p_name in epc.parameter_name_subtype
, p_data_type in epc.data_type_subtype
, p_value in varchar2
);

/**
-- Set a request parameter (IN or IN OUT).
-- 
-- @param p_epc_key    The key
-- @param p_name       The name of the parameter
-- @param p_data_type  The data type (should not be epc.data_type_string)
-- @param p_value      The value of the parameter
--
-- @exception e_illegal_null_value  p_value is NULL
*/
procedure set_request_parameter
(
  p_epc_key in epc_key_subtype
, p_name in epc.parameter_name_subtype
, p_data_type in epc.data_type_subtype
, p_value in number
);

/**
-- Send a request.
-- 
-- @param p_epc_key      Connection info can be retrieved by the key
-- @param p_method_name  The method name
-- @param p_oneway       Must we wait on a response? (0 = NO)
*/
procedure send_request
( 
  p_epc_key in epc_key_subtype
, p_method_name in epc.method_name_subtype
, p_oneway in pls_integer
);

/**
-- Receive a response.
-- 
-- @param p_epc_key  Connection info can be retrieved by the key
*/
procedure recv_response
( 
  p_epc_key in epc_key_subtype
);

/**
-- Get a response parameter (OUT or IN OUT).
-- 
-- @param p_epc_key    The key
-- @param p_name       The name of the parameter
-- @param p_data_type  The data type (should be epc.data_type_string)
-- @param p_value      The value of the parameter
*/
procedure get_response_parameter
(
  p_epc_key in epc_key_subtype
, p_name in epc.parameter_name_subtype
, p_data_type in epc.data_type_subtype
, p_value out varchar2
);

/**
-- Get a response parameter (OUT or IN OUT).
-- 
-- @param p_epc_key    The key
-- @param p_name       The name of the parameter
-- @param p_data_type  The data type (should not be epc.data_type_string)
-- @param p_value      The value of the parameter
*/
procedure get_response_parameter
(
  p_epc_key in epc_key_subtype
, p_name in epc.parameter_name_subtype
, p_data_type in epc.data_type_subtype
, p_value out number
);

end epc_clnt;
/
