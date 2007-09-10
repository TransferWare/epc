--$NO_KEYWORD_EXPANSION$
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
--    a) epc_srvr.send_response<br />
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
-- @param p_msg_request     The message request
--
-- @throws epc.e_msg_timed_out    Message timed out
-- @throws epc.e_msg_too_big      Message too big
-- @throws epc.e_msg_interrupted  Message interrupted
*/
procedure recv_request
( p_epc_key in epc_key_subtype
, p_msg_info out epc_srvr.msg_info_subtype
, p_msg_request out varchar2
);

/**
-- Send a response to a request.
-- 
-- @param p_epc_key       Needed for the connection info
-- @param p_msg_info      The message information as received by recv_request
-- @param p_msg_response  The message response
--
-- @throws epc.e_msg_timed_out    Message timed out
-- @throws epc.e_msg_interrupted  Message interrupted
*/
procedure send_response
( 
  p_epc_key in epc_key_subtype
, p_msg_info in epc_srvr.msg_info_subtype
, p_msg_response in varchar2
);

/**
-- Interrupt the receipt of a request. When database
-- pipes are used to receive the request, the session can not 
-- easily be interrupted by a user defined interrupt (for example a CTRL-C).
-- The way to do this, is to call this procedure from another session.
-- This will interrupt the server.
--
-- @param p_epc_key   Needed for connection info
--
-- @throws epc.e_msg_timed_out    Message timed out
-- @throws epc.e_msg_interrupted  Message interrupted
*/
procedure send_request_interrupt
(
  p_epc_key in epc_key_subtype
);

/**
-- Ping a server.
--
-- The string 'PING' and the response pipe will be sent into the request pipe
-- (use set_connection_info/get_connection_info to set/get that).  The answer
-- in the response pipe should be 'PONG'.
--
-- @param p_epc_key        Needed for connection info
-- @param p_response_pipe  The response pipe
--
-- @throws epc.e_msg_timed_out    Message timed out
-- @throws epc.e_msg_interrupted  Message interrupted
*/
procedure ping
(
  p_epc_key in epc_key_subtype
, p_response_pipe in varchar2
);

end epc_srvr;
/
