/*
REMARK
REMARK  $Header$
REMARK
REMARK  Description:    Oracle package specification for External Procedure Call Toolkit.
REMARK
REMARK  $Log$
REMARK  Revision 1.5  2004/12/16 18:43:08  gpaulissen
REMARK  generated HTML added
REMARK
REMARK  Revision 1.4  2004/12/16 17:50:31  gpaulissen
REMARK  REMARK blocks commented for PLDoc
REMARK
REMARK  Revision 1.3  2004/10/15 13:53:40  gpaulissen
REMARK  XML added
REMARK
REMARK  Revision 1.2  2004/04/05 14:52:33  gpaulissen
REMARK  Interface changed
REMARK
REMARK  Revision 1.1  2004/04/02 10:26:28  gpaulissen
REMARK  New interface for epc
REMARK
REMARK
REMARK
*/

create or replace package epc_srvr is
/**
--
-- This package is used to implement the server side of RPC like functionality
-- on an Oracle database. Messages are sent by the client to a server. The 
-- transport mechanisms supported are database pipes (DBMS_PIPE),
-- HTTP (UTL_HTTP) and TCP/IP (UTL_TCP). This package is only needed for 
-- database pipe transport. TCP/IP servers should not use
-- the TCP/IP functionality of Oracle, but of the OS instead.
--
-- The flow of procedure calls will typically look like this:<br />
-- 1) Set connection information.<br />
--    a) epc_srvr.register (once in a database session)<br />
--    b) epc_srvr.set_connection_info<br />
--    c) epc_srvr.set_request_recv_timeout (optional)<br />
-- 2) Receive the request<br />
--    a) epc_srvr.recv_request<br />
-- 3) Process the message<br />
-- 4) Send the response<br />
--    a) epc_srvr.send_response(epc_srvr.get_epc_key, p_msg_response, p_msg_info)<br /> OR
--    b) epc_srvr.new_response(epc_srvr.get_epc_key, p_msg_info)<br />
--       dbms_pipe.pack_message(output argument 1)
--       .
--       dbms_pipe.pack_message(output argument N)
--       epc_srvr.send_response(epc_srvr.get_epc_key, p_msg_info)<br />
--
-- @headcom
*/

subtype epc_key_subtype is binary_integer;
subtype msg_info_subtype is epc.string_subtype;

/**
-- Register an interface
--
-- @return A unique key which has to be used in all other epc_srvr calls.
*/
function register
return epc_key_subtype;

/**
-- Get the key
--
-- @return The unique key for this session
*/
function get_epc_key
return epc_key_subtype;

/**
-- Set the connection type to database pipes and store the pipe name for
-- later use. Each interface may have a different connection.
-- 
-- @param p_epc_key    The key
-- @param p_pipe_name  The request pipe name
*/
procedure set_connection_info
( p_epc_key in epc_key_subtype
, p_pipe_name in epc.pipe_name_subtype
);

/**
-- Get the database request pipe.
-- 
-- @param p_epc_key    The key
-- @param p_pipe_name  The request pipe name
*/
procedure get_connection_info
( p_epc_key in epc_key_subtype
, p_pipe_name out epc.pipe_name_subtype
);

/**
-- Set the response send timeout.
-- 
-- @param p_epc_key                The key
-- @param p_response_send_timeout  The response send timeout
*/
procedure set_response_send_timeout
( p_epc_key in epc_key_subtype
, p_response_send_timeout in pls_integer
);

/**
-- Receive a request.
-- 
-- @param p_epc_key         Needed for the connection info
-- @param p_msg_info        The message information
-- @param p_msg_request     The XML describing the request
--                          (when protocol stored in p_msg_info is SOAP/XMLRPC)
-- @param p_interface_name  The interface name of the request
--                          (when protocol stored in p_msg_info is NATIVE)
-- @param p_function_name   The function name of the request
--                          (when protocol stored in p_msg_info is NATIVE)
*/
procedure recv_request
( p_epc_key in epc_key_subtype
, p_msg_info out epc_srvr.msg_info_subtype
, p_msg_request out varchar2
, p_interface_name out varchar2
, p_function_name out varchar2
);

/**
-- Create a new response, i.e. clear the database pipe and put the metadata into it.
-- 
-- @param p_epc_key     Needed for the connection info
-- @param p_msg_info    The message information as received by recv_request
-- @param p_error_code  The error code string (OK means no error)
*/
procedure new_response
( p_epc_key in epc_key_subtype
, p_msg_info in epc_srvr.msg_info_subtype
, p_error_code in varchar2
);

/**
-- Send a response after all metadata and date has been put into the database pipe.
-- 
-- @param p_epc_key   Needed for the connection info
-- @param p_msg_info  The message information as received by recv_request
*/
procedure send_response
( p_epc_key in epc_key_subtype
, p_msg_info in epc_srvr.msg_info_subtype
);

/**
-- Send a response.
-- 
-- @param p_epc_key       Needed for the connection info
-- @param p_msg_response  The XML describing the response
-- @param p_msg_info      The message information as received by recv_request
*/
procedure send_response
( 
  p_epc_key in epc_key_subtype
, p_msg_response in varchar2
, p_msg_info in epc_srvr.msg_info_subtype
);

/**
-- Interrupt the receipt of a request. When database
-- pipes are used to receive the request, the session can not 
-- easily be interrupted by a user defined interrupt (for example a CTRL-C).
-- The way to do this, is to start a second session in the server which waits
-- for a user defined interrupt (osnsui call). Then send_request_interrupt 
-- will send database pipe message to the main server session.
--
-- @param p_epc_key   Needed for connection info
*/
procedure send_request_interrupt
(
  p_epc_key in epc_key_subtype
);

end epc_srvr;
/
