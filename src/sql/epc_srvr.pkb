REMARK
REMARK  $Header$
REMARK
REMARK  Description:    Oracle package specification for External Procedure Call Toolkit.
REMARK
REMARK  $Log$
REMARK  Revision 1.1  2004/10/15 13:53:40  gpaulissen
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

create or replace package body epc_srvr as

g_epc_key epc_key_subtype := null;

/* dbms_pipe parameters */
g_pipe_name epc.pipe_name_subtype := 'EPC_REQUEST_PIPE';
g_response_send_timeout pls_integer := 10;

function register
return epc_key_subtype
is
begin
  g_epc_key := 1;
  return g_epc_key;
end register;

function get_epc_key
return epc_key_subtype
is
begin
  return g_epc_key;
end get_epc_key;

procedure set_connection_info
(
  p_epc_key in epc_key_subtype
, p_pipe_name in epc.pipe_name_subtype
)
is
begin
  if p_epc_key = g_epc_key
  then
    g_pipe_name := p_pipe_name;
  end if;
end set_connection_info;

procedure get_connection_info
(
  p_epc_key in epc_key_subtype
, p_pipe_name out epc.pipe_name_subtype
)
is
begin
  if p_epc_key = g_epc_key
  then
    p_pipe_name := g_pipe_name;
  else
    p_pipe_name := null;
  end if;
end get_connection_info;

procedure set_response_send_timeout
(
  p_epc_key in epc_key_subtype
, p_response_send_timeout in pls_integer
)
is
begin
  if p_epc_key = g_epc_key
  then
    g_response_send_timeout := p_response_send_timeout;
  end if;
end set_response_send_timeout;

procedure recv_request
( 
  p_epc_key in epc_key_subtype
, p_msg_request out varchar2
, p_msg_info out epc_srvr.msg_info_subtype
)
is
  l_retval pls_integer;
  l_msg_protocol pls_integer;
  l_msg_seq pls_integer;
  l_result_pipe epc.pipe_name_subtype;
begin
  if p_epc_key = g_epc_key
  then
    l_retval := dbms_pipe.receive_message(g_pipe_name); /* wait forever */
    if l_retval = 0 /* OK */
    then
      dbms_pipe.unpack_message(l_msg_protocol);
      dbms_pipe.unpack_message(l_msg_seq);
      dbms_pipe.unpack_message(p_msg_request);
      dbms_pipe.unpack_message(l_result_pipe);
      p_msg_info := to_char(l_msg_seq, 'FM000X') || l_result_pipe;
    elsif l_retval = 1
    then
      raise epc.e_msg_timed_out;
    elsif l_retval = 2
    then
      raise epc.e_msg_too_big;
    elsif l_retval = 3
    then
      raise epc.e_msg_interrupted;
    else
      /* ?? */
      raise value_error;
    end if;
  end if;
end recv_request;

procedure send_response
( 
  p_epc_key in epc_key_subtype
, p_msg_response in varchar2
, p_msg_info in epc_srvr.msg_info_subtype
)
is
  l_retval pls_integer;
  l_msg_seq constant pls_integer := to_number(substr(p_msg_info, 1, 4), 'FM000X');
  l_result_pipe constant epc.pipe_name_subtype := substr(p_msg_info, 5);
begin
  if p_epc_key = g_epc_key
  then
    dbms_pipe.pack_message(l_msg_seq);
    dbms_pipe.pack_message(p_msg_response);
    l_retval := dbms_pipe.send_message(l_result_pipe, g_response_send_timeout);
    if l_retval <> 0
    then
      raise epc.e_comm_error;
    end if;
  end if;
end send_response;

procedure send_request_interrupt
(
  p_epc_key in epc_key_subtype
)
is
  l_retval pls_integer;
begin
  if p_epc_key = g_epc_key
  then
    dbms_pipe.reset_buffer;
    dbms_pipe.pack_message( 'INTERRUPT' );
    l_retval := dbms_pipe.send_message(g_pipe_name);
  end if;
end send_request_interrupt;

end epc_srvr;
/