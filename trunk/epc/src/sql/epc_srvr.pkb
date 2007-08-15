REMARK
REMARK  $Header$
REMARK
REMARK  Description:    Oracle package specification for External Procedure Call Toolkit.
REMARK
REMARK  $Log$
REMARK  Revision 1.2  2004/10/21 10:37:08  gpaulissen
REMARK  * make lint
REMARK  * error reporting enhanced
REMARK  * oneway functions enhanced
REMARK
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

g_indent pls_integer := 1;
-- LOCAL

procedure enter(p_procname in varchar2)
is
begin
  dbms_output.put_line(lpad('>', g_indent, ' ')||p_procname);
  g_indent := g_indent + 2;
end;

procedure print(p_break_point in varchar2, p_format in varchar2, p_data in varchar2 default null)
is
begin
  dbms_output.put_line(lpad(' ', g_indent, ' ')||replace(p_format, '%s', p_data));
end;

procedure leave
is
begin
  g_indent := g_indent - 2;
  dbms_output.put_line(lpad('<', g_indent, ' '));
end;

procedure leave_on_error
is
  l_error_stack constant varchar2(32767) := dbms_utility.format_error_backtrace;
begin
  leave;
  dbms_output.put_line(substr(l_error_stack, 1+0*255, 255));
  dbms_output.put_line(substr(l_error_stack, 1+1*255, 255));
  dbms_output.put_line(substr(l_error_stack, 1+2*255, 255));
end;

procedure new_response
( 
  p_epc_key in epc_key_subtype
, p_msg_info in epc_srvr.msg_info_subtype
, p_error_code in pls_integer
)
is
  l_msg_seq constant pls_integer := to_number(substr(p_msg_info, 2, 4), 'FM000X');
begin
--/*DBUG
  enter('epc_srvr.new_response');
  print('input', 'p_epc_key: %s', p_epc_key);
  print('input', 'p_msg_info: %s', p_msg_info);
  print('input', 'p_error_code: %s', p_error_code);
--/*DBUG*/

  if p_epc_key = g_epc_key
  then
--/*DBUG
    print('info', 'resetting buffer');
--/*DBUG*/

    dbms_pipe.reset_buffer; -- just to be sure
    dbms_pipe.pack_message(l_msg_seq);
    if p_error_code is not null -- ignore it for XMLRPC/SOAP
    then
      dbms_pipe.pack_message(p_error_code);
    end if;
  end if;

--/*DBUG
  leave;
--/*DBUG*/
end new_response;

-- GLOBAL

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
, p_msg_info out epc_srvr.msg_info_subtype
, p_msg_request out varchar2
, p_interface_name out varchar2
, p_function_name out varchar2
)
is
  l_retval pls_integer;
  l_msg_protocol pls_integer;
  l_msg_seq pls_integer;
  l_result_pipe epc.pipe_name_subtype := null;
begin
--/*DBUG
  enter('epc_srvr.recv_request');
  print('input', 'p_epc_key: %s', p_epc_key);
--/*DBUG*/

  if p_epc_key = g_epc_key
  then
--/*DBUG
    print('info', 'receiving from %s', g_pipe_name);
--/*DBUG*/
    l_retval := dbms_pipe.receive_message(g_pipe_name); /* wait forever */

--/*DBUG
    print('info', 'l_retval: %s', l_retval);
--/*DBUG*/

    case l_retval
      when 0 /* OK */
      then
        dbms_pipe.unpack_message(l_msg_protocol);
        dbms_pipe.unpack_message(l_msg_seq);

--/*DBUG
	print('info', 'protocol: %s', l_msg_protocol);
	print('info', 'msg_seq: %s', l_msg_seq);
--/*DBUG*/

        case 
          when l_msg_protocol = epc_clnt."NATIVE"
          then
            dbms_pipe.unpack_message(p_interface_name);
--/*DBUG
	    print('info', '#1; next item type: %s', dbms_pipe.next_item_type);
--/*DBUG*/
            dbms_pipe.unpack_message(p_function_name);
--/*DBUG
	    print('info', '#2; next item type: %s', dbms_pipe.next_item_type);
--/*DBUG*/
            dbms_pipe.unpack_message(l_result_pipe);
--/*DBUG
	    print('info', '#3; next item type: %s', dbms_pipe.next_item_type);
--/*DBUG*/
            if l_result_pipe = epc_clnt."N/A"
            then
              l_result_pipe := null;
            end if;

          when l_msg_protocol in (epc_clnt."SOAP", epc_clnt."XMLRPC")
          then
            dbms_pipe.unpack_message(p_msg_request);
--/*DBUG
	    print('info', '#4');
--/*DBUG*/
            if dbms_pipe.next_item_type != 0
            then
              dbms_pipe.unpack_message(l_result_pipe);
--/*DBUG
	    print('info', '#5');
--/*DBUG*/
            end if;
        end case;

        if l_result_pipe is not null
        then
          p_msg_info := to_char(l_msg_protocol) || to_char(l_msg_seq, 'FM000X') || l_result_pipe;
        else
          p_msg_info := to_char(l_msg_protocol);
        end if;

      when 1
      then
        raise epc.e_msg_timed_out;

      when 2
      then
         raise epc.e_msg_too_big;

      when 3
      then
        raise epc.e_msg_interrupted;

      else
        /* ?? */
        raise value_error;
    end case;
  end if;

--/*DBUG
  print('output', 'p_msg_info: %s', p_msg_info);
  print('output', 'p_msg_request: %s', p_msg_request);
  print('output', 'p_interface_name: %s', p_interface_name);
  print('output', 'p_function_name: %s', p_function_name);
  leave;
exception
  when others
  then
    leave_on_error;
    raise;
--/*DBUG*/
end recv_request;

procedure new_response
( 
  p_epc_key in epc_key_subtype
, p_msg_info in epc_srvr.msg_info_subtype
)
is
begin
  new_response(p_epc_key, p_msg_info, 0);
end new_response;

procedure send_response
( 
  p_epc_key in epc_key_subtype
, p_msg_info in epc_srvr.msg_info_subtype
)
is
  l_retval pls_integer;
  l_result_pipe constant epc.pipe_name_subtype := substr(p_msg_info, 6);
begin
--/*DBUG
  enter('epc_srvr.send_response (1)');
  print('input', 'p_epc_key: %s', p_epc_key);
  print('input', 'p_msg_info: %s', p_msg_info);
--/*DBUG*/

  if p_epc_key = g_epc_key
  then
--/*DBUG
    print('info', 'sending to %s', l_result_pipe);
--/*DBUG*/
    l_retval := dbms_pipe.send_message(l_result_pipe, g_response_send_timeout);

    case l_retval
      when 0 -- OK
      then
        null;

      when 1
      then
        raise epc.msg_timed_out;

      when 3
      then
        raise epc.msg_interrupted;

      else
        raise value_error;
    end case;
  end if;

--/*DBUG
  leave;
--/*DBUG*/
end send_response;

procedure send_response
( 
  p_epc_key in epc_key_subtype
, p_msg_info in epc_srvr.msg_info_subtype
, p_error_code in pls_integer
)
is
begin
--/*DBUG
  enter('epc_srvr.send_response (2)');
  print('input', 'p_epc_key: %s', p_epc_key);
  print('input', 'p_msg_info: %s', p_msg_info);
  print('input', 'p_error_code: %s', p_error_code);
--/*DBUG*/

  new_response(p_epc_key => p_epc_key, p_msg_info => p_msg_info, p_error_code => p_error_code);
  send_response(p_epc_key => p_epc_key, p_msg_info => p_msg_info);

--/*DBUG
  leave;
--/*DBUG*/
end send_response;

procedure send_response
( 
  p_epc_key in epc_key_subtype
, p_msg_info in epc_srvr.msg_info_subtype
, p_msg_response in varchar2
)
is
begin
--/*DBUG
  enter('epc_srvr.send_response (2)');
  print('input', 'p_epc_key: %s', p_epc_key);
  print('input', 'p_msg_info: %s', p_msg_info);
  print('input', 'p_msg_response: %s', p_msg_response);
--/*DBUG*/

  new_response(p_epc_key => p_epc_key, p_msg_info => p_msg_info, p_error_code => null);
  dbms_pipe.pack_message(p_msg_response);
  send_response(p_epc_key => p_epc_key, p_msg_info => p_msg_info);

--/*DBUG
  leave;
--/*DBUG*/
end send_response;

procedure send_request_interrupt
(
  p_epc_key in epc_key_subtype
)
is
  l_retval pls_integer;
begin
--/*DBUG
  enter('epc_srvr.send_request_interrupt');
  print('input', 'p_epc_key: %s', p_epc_key);
--/*DBUG*/

  if p_epc_key = g_epc_key
  then
--/*DBUG
    print('info', 'resetting buffer');
--/*DBUG*/

    dbms_pipe.reset_buffer;
    dbms_pipe.pack_message( 'INTERRUPT' );

--/*DBUG
    print('info', 'sending to %s', g_pipe_name);
--/*DBUG*/

    l_retval := dbms_pipe.send_message(g_pipe_name);
    case l_retval
      when 0 -- OK
      then
        null;

      when 1
      then
        raise epc.msg_timed_out;

      when 3
      then
        raise epc.msg_interrupted;

      else
        raise value_error;
    end case;
  end if;

--/*DBUG
  leave;
--/*DBUG*/
end send_request_interrupt;

end epc_srvr;
/

show errors
