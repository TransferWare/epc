REMARK
REMARK  $Header$
REMARK
REMARK  Description:    Oracle package specification for External Procedure Call Toolkit.
REMARK
REMARK  $Log$
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
--    a) epc_clnt.set_connection_info (optional for database pipes)
--    b) epc_clnt.set_request_send_timeout (optional)
--    c) epc_clnt.set_response_recv_timeout (optional)
-- 2) Marshall a function call into a message
--    a) epc_clnt.set_request_header
--    b) epc_clnt.set_request_parameter (for all IN and IN OUT parameters)
--    c) epc_clnt.set_request_trailer
-- 3) Send the message
--    a) epc_clnt.send_request
-- 4) Receive the response
--    a) epc_clnt.recv_response
-- 5) Unmarshall the message
--    a) epc_clnt.get_response_parameter (for all OUT and IN OUT parameters)
*/

/* 
|| Connection related functions/procedures.
*/

/**
-- Set the connection type to TCP/IP and store the TCP/IP
-- connection for later use.
-- 
-- @param p_epc_info  The interface
-- @param p_connection      An open TCP/IP connection (see utl_tcp.open_connection)
*/
procedure set_connection_info
(
  p_epc_info in epc.epc_info_subtype
, p_connection in utl_tcp.connection
);

/**
-- Set the connection type to database pipes and store the pipe name for
-- later use. Each interface may have a different connection.
-- 
-- @param p_epc_info  The interface
-- @param p_pipe_name       The request pipe name
*/
procedure set_connection_info
(
  p_epc_info in epc.epc_info_subtype
, p_pipe_name in pipe_name_subtype
);

/**
-- Set the request send timeout.
-- 
-- @param p_epc_info        The interface
-- @param p_request_send_timeout  The request send timeout
*/
procedure set_request_send_timeout
(
  p_epc_info in epc.epc_info_subtype
, p_request_send_timeout in pls_integer
);

/**
-- Set the response receive timeout.
-- 
-- @param p_epc_info         The interface
-- @param p_response_recv_timeout  The response receive timeout
*/
procedure set_response_recv_timeout
(
  p_epc_info in epc_info_subtype
, p_response_recv_timeout in pls_integer
);

/**
-- Set the request header.
-- 
-- @param p_interface_name  The interface name
-- @param p_routine_name    The routine name/signature
-- @param p_oneway          Must we wait on a response? (0 = NO)
*/
procedure set_request_header
(
  p_interface_name in epc.interface_name_subtype
, p_routine_name in epc.routine_name_subtype
, p_oneway in pls_integer
);

/**
-- Set a request parameter (IN or IN OUT).
-- 
-- @param p_epc_info  The interface name
-- @param p_name            The name of the parameter
-- @param p_data_type       The data type (should be epc.data_type_string)
-- @param p_value           The value of the parameter
--
-- @exception e_illegal_null_value  p_value is NULL
*/
procedure set_request_parameter
(
  p_epc_info in epc.epc_info_subtype
, p_name in epc.parameter_name_subtype
, p_data_type in epc.data_type_subtype
, p_value in varchar2
);

/**
-- Set a request parameter (IN or IN OUT).
-- 
-- @param p_epc_info  The interface name
-- @param p_name            The name of the parameter
-- @param p_data_type       The data type (should not be epc.data_type_string)
-- @param p_value           The value of the parameter
--
-- @exception e_illegal_null_value  p_value is NULL
*/
procedure set_request_parameter
(
  p_epc_info in epc.epc_info_subtype
, p_name in epc.parameter_name_subtype
, p_data_type in epc.data_type_subtype
, p_value in number
);

/**
-- Set a request trailer. Should be called to after the last
-- set_request_parameter and before send_request.
-- 
-- @param p_epc_info  The interface name
*/
procedure set_request_trailer
(
  p_epc_info in epc.epc_info_subtype
);

/**
-- Send a request.
-- 
-- @param p_epc_info  Connection info can be retrieved by the interface
*/
procedure send_request
( 
  p_epc_info in epc.epc_info_subtype
);

/**
-- Receive a response.
-- 
-- @param p_epc_info  Connection info can be retrieved by the interface
*/
procedure recv_response
( 
  p_epc_info in epc.epc_info_subtype
);

/**
-- Get a response parameter (OUT or IN OUT).
-- 
-- @param p_epc_info  The interface name
-- @param p_name            The name of the parameter
-- @param p_data_type       The data type (should be epc.data_type_string)
-- @param p_value           The value of the parameter
*/
procedure get_response_parameter
(
  p_epc_info in epc.epc_info_subtype
, p_name in epc.parameter_name_t
, p_data_type in epc.data_type_subtype
, p_value out varchar2
);

/**
-- Set a request parameter (OUT or IN OUT).
-- 
-- @param p_epc_info   The interface name
-- @param p_name       The name of the parameter
-- @param p_data_type  The data type (should not be epc.data_type_string)
-- @param p_value      The value of the parameter
*/
procedure get_response_parameter
(
  p_epc_info in epc.epc_info_subtype
, p_name in epc.parameter_name_t
, p_data_type in epc.data_type_subtype
, p_value out number
);

end epc_clnt;
/
