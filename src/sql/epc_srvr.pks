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

create or replace package epc_srvr as
/**
--
-- This package is used to implement the server side of RPC like functionality
-- on an Oracle database.
-- Messages are sent by the client to a server. The transport mechanisms
-- supported are database pipes (DBMS_PIPE) and TCP/IP (UTL_TCP). This package
-- is only needed for database pipe transport. TCP/IP servers should not use
-- the TCP/IP functionality of Oracle, but of the OS instead.
--
-- The flow of procedure calls will typically look like this:
-- 1) Set connection information.
--    a) epc.get_connection_handle (once in a database session)
--    b) epc_srvr.set_connection_info
--    c) epc_srvr.set_request_recv_timeout (optional)
-- 2) Receive the request
--    a) epc_srvr.recv_request
-- 3) Process the message (no Oracle calls needed)
-- 4) Send the response
--    a) epc_srvr.send_response
*/

/**
-- Set the connection type to database pipes and store the pipe name for
-- later use.
-- 
-- @param p_connection_handle  A handle to this connection
-- @param p_pipe_name          The request pipe name
*/
procedure set_connection_info
(
  p_connection_handle in epc.connection_handle_subtype
, p_pipe_name in epc.pipe_name_subtype
);

/**
-- Set the request send timeout.
-- 
-- @param p_connection_handle     A handle to this connection
-- @param p_request_recv_timeout  The request receive timeout
*/
procedure set_request_recv_timeout
(
  p_connection_handle in epc.connection_handle_subtype
, p_request_recv_timeout in pls_integer
);

/**
-- Receive a request.
-- 
-- @param p_connection_handle  A handle to this connection
-- @param p_msg_info           The message info
*/
procedure recv_request
( 
  p_connection_handle in epc.connection_handle_subtype
, p_msg_info out epc.msg_info_subtype
);

/**
-- Send a response.
-- 
-- @param p_connection_handle  A handle to this connection
-- @param p_msg_info           The message info
*/
procedure send_response
( 
  p_connection_handle in epc.connection_handle_subtype
, p_msg_info in epc.msg_info_subtype
);

procedure recv_request_interrupt
(
  p_connection_handle in epc.connection_handle_subtype
);

end epc_srvr;
/
