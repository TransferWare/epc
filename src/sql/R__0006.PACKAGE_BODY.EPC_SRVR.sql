CREATE OR REPLACE PACKAGE BODY "EPC_SRVR" AS

-- LOCAL

/* !!! SEE ALSO UT_RESET BELOW !!! */

g_epc_key epc_key_subtype := null;

/* dbms_pipe parameters */
g_pipe_name epc.pipe_name_subtype := 'EPC_REQUEST_PIPE';
g_response_send_timeout pls_integer := 10;

$if epc.c_debugging $then

/*
-- GJP 2022-12-04
--
-- We want to debug EPC using DBUG but there is one quirk:
-- you can not debug EPC while using PLSDBUG since that is based on EPC so you get infinite recursion.
--
-- The solution is to disable the PLSDBUG method just before the DBUG call and activate it thereafter.
*/

"PLSDBUG" constant dbug.method_t := 'PLSDBUG';

procedure enter(p_procname in varchar2)
is
begin
  if dbug.active("PLSDBUG")
  then
    dbug.activate("PLSDBUG", false); -- off
    dbug.enter(p_procname);
    dbug.activate("PLSDBUG", true); -- on
  else
    dbug.enter(p_procname);
  end if;
end enter;

procedure print(p_break_point in varchar2, p_format in varchar2, p_data in varchar2 default null)
is
begin
  if dbug.active("PLSDBUG")
  then
    dbug.activate("PLSDBUG", false); -- off
    dbug.print(p_break_point, p_format, p_data);
    dbug.activate("PLSDBUG", true); -- on
  else
    dbug.print(p_break_point, p_format, p_data);
  end if;
end print;

procedure leave
is
begin
  if dbug.active("PLSDBUG")
  then
    dbug.activate("PLSDBUG", false); -- off
    dbug.leave;
    dbug.activate("PLSDBUG", true); -- on
  else
    dbug.leave;
  end if;
end leave;

procedure leave_on_error
is
begin
  if dbug.active("PLSDBUG")
  then
    dbug.activate("PLSDBUG", false); -- off
    dbug.leave_on_error;
    dbug.activate("PLSDBUG", true); -- on
  else
    dbug.leave_on_error;
  end if;
end leave_on_error;

procedure inspect_message
is
  l_number number;
  l_varchar2 varchar2(4000);
  l_rowid rowid;
  l_date date;
  l_raw raw(2000);
  l_item positiven := 1;
begin
  -- dbms_pipe.NEXT_ITEM_TYPE Function Return Values
  -- Return  Description
  --      0  No more items
  --      6  NUMBER
  --      9  VARCHAR2
  --     11  ROWID
  --     12  DATE
  --     23  RAW

  loop
    print('debug', 'item: %s', l_item);
    case dbms_pipe.next_item_type
      when  0 then exit;
      when  6 then dbms_pipe.unpack_message(l_number);   print('debug', 'number: %s'  , l_number);
      when  9 then dbms_pipe.unpack_message(l_varchar2); print('debug', 'varchar2: %s', l_varchar2);
      when 11 then dbms_pipe.unpack_message(l_rowid);    print('debug', 'rowid: %s'   , rowidtochar(l_rowid));
      when 12 then dbms_pipe.unpack_message(l_date);     print('debug', 'date: %s'    , to_char(l_date, 'yyyy-mm-dd hh24:mi:ss'));
      when 23 then dbms_pipe.unpack_message(l_raw);      print('debug', 'raw: %s'     , utl_raw.cast_to_varchar2(l_raw));
    end case;
    l_item := l_item + 1;
  end loop;
end inspect_message;

$end -- $if epc.c_debugging $then

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
, p_pipe_name out nocopy epc.pipe_name_subtype
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
, p_msg_info out nocopy epc_srvr.msg_info_subtype
, p_msg_request out nocopy varchar2
)
is
  l_retval pls_integer;
  l_msg_protocol pls_integer;
  l_msg_seq pls_integer;
  l_result_pipe epc.pipe_name_subtype := null;

  -- ORA-06559: wrong datatype requested, 6, actual datatype is 9
  e_wrong_data_type_requested exception;
  pragma exception_init (e_wrong_data_type_requested, -6559);

$if epc.c_debugging $then

  procedure show_msg_request
  is
    l_data_type epc.data_type_subtype;
    l_data_length pls_integer;
    l_data_value varchar2(32767);
    l_start pls_integer := 1;
    l_part pls_integer := 1;
  begin
    while l_start <= length(p_msg_request)
    loop
      l_data_type := to_number(substr(p_msg_request, l_start, 1));
      case 
        when l_data_type in (epc.data_type_string, epc.data_type_xml)
        then
          l_data_length := to_number(substr(p_msg_request, l_start+1, 4), 'FM000X'); -- see EPC_CLNT
          l_data_value := substrb(p_msg_request, l_start+1+4, l_data_length);
          l_start := l_start+1+4+nvl(length(l_data_value), 0);
        else
          l_data_length := to_number(substr(p_msg_request, l_start+1, 2), 'FM0X'); -- see EPC_CLNT
          l_data_value := substrb(p_msg_request, l_start+1+2, l_data_length);
          l_start := l_start+1+2+nvl(length(l_data_value), 0);
      end case;
      print
      ( 'info'
      , 'msg request: %s'
      , utl_lms.format_message
        ( 'part: %d; data type: %d; data length: %d; data value: "%s"'
        , l_part
        , l_data_type
        , l_data_length
        , l_data_value
        )
      );
      l_part := l_part + 1;
    end loop;
  exception
    when others
    then print('error', 'sqlerrm: %s', sqlerrm);
  end show_msg_request;

$end
begin
$if epc.c_debugging $then
  enter('epc_srvr.recv_request');
  print('input', 'p_epc_key: %s', p_epc_key);
$end

  if p_epc_key = g_epc_key
  then
$if epc.c_debugging $then
    print('info', 'receiving from %s', g_pipe_name);
$end
    l_retval := dbms_pipe.receive_message(g_pipe_name); /* wait forever */

$if epc.c_debugging $then
    print('info', 'l_retval: %s', l_retval);
$end

    case l_retval
      when 0 /* OK */
      then
        dbms_pipe.unpack_message(l_msg_protocol);
        dbms_pipe.unpack_message(l_msg_seq);

$if epc.c_debugging $then
        print('info', 'protocol: %s', l_msg_protocol);
        print('info', 'msg_seq: %s', l_msg_seq);
$end

        dbms_pipe.unpack_message(p_msg_request);

$if epc.c_debugging $then
        show_msg_request;
$end        

        if dbms_pipe.next_item_type != 0
        then
          dbms_pipe.unpack_message(l_result_pipe);
        end if;

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

$if epc.c_debugging $then
  print('output', 'p_msg_info: %s', p_msg_info);
  print('output', 'p_msg_request: %s', p_msg_request);
  leave;
exception
  when e_wrong_data_type_requested
  then
    inspect_message;
    raise;
    
  when others
  then
    leave_on_error;
    raise;
$end
end recv_request;

procedure send_response
(
  p_epc_key in epc_key_subtype
, p_msg_info in epc_srvr.msg_info_subtype
, p_msg_response in varchar2
)
is
  l_retval pls_integer;

  l_msg_seq constant pls_integer := to_number(substr(p_msg_info, 2, 4), 'FM000X');
  l_result_pipe constant epc.pipe_name_subtype := substr(p_msg_info, 6);
begin
$if epc.c_debugging $then
  enter('epc_srvr.send_response');
  print('input', 'p_epc_key: %s', p_epc_key);
  print('input', 'p_msg_info: %s', p_msg_info);
  print('input', 'p_msg_response: %s', p_msg_response);
$end

  if p_epc_key = g_epc_key
  then
$if epc.c_debugging $then
    print('info', 'resetting buffer');
$end

    dbms_pipe.reset_buffer; -- just to be sure
    dbms_pipe.pack_message(l_msg_seq);
    dbms_pipe.pack_message(p_msg_response);

$if epc.c_debugging $then
    print('info', 'sending to %s', l_result_pipe);
$end

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

$if epc.c_debugging $then
  leave;
exception
  when others
  then
    leave_on_error;
    raise;
$end
end send_response;

procedure send_request_interrupt
(
  p_epc_key in epc_key_subtype
)
is
  l_retval pls_integer;
begin
$if epc.c_debugging $then
  enter('epc_srvr.send_request_interrupt');
  print('input', 'p_epc_key: %s', p_epc_key);
$end

  if p_epc_key = g_epc_key
  then
$if epc.c_debugging $then
    print('info', 'resetting buffer');
$end

    dbms_pipe.reset_buffer;
    dbms_pipe.pack_message( 'INTERRUPT' );

$if epc.c_debugging $then
    print('info', 'sending to %s', g_pipe_name);
$end

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

$if epc.c_debugging $then
  leave;
exception
  when others
  then
    leave_on_error;
    raise;
$end
end send_request_interrupt;

procedure ping
(
  p_epc_key in epc_key_subtype
, p_response_pipe in varchar2
)
is
  l_retval pls_integer;
begin
$if epc.c_debugging $then
  enter('epc_srvr.ping');
  print('input', 'p_epc_key: %s', p_epc_key);
  print('input', 'p_response_pipe: %s', p_response_pipe);
$end

  if p_epc_key = g_epc_key
  then
$if epc.c_debugging $then
    print('info', 'resetting buffer');
$end

    dbms_pipe.reset_buffer;
    dbms_pipe.pack_message( 'PING' );
    dbms_pipe.pack_message( p_response_pipe );

$if epc.c_debugging $then
    print('info', 'sending to %s', g_pipe_name);
$end

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

$if epc.c_debugging $then
  leave;
exception
  when others
  then
    leave_on_error;
    raise;
$end
end ping;

procedure create_pipe
(
  p_pipe_name in epc.pipe_name_subtype
, p_max_pipe_size in integer default 8192
, p_private in boolean default true
)
is
begin
  if dbms_pipe.create_pipe(p_pipe_name, p_max_pipe_size, p_private) = 0
  then
    null;
  else
    raise program_error;
  end if;
end create_pipe;

procedure purge_pipe
(
  p_pipe_name in epc.pipe_name_subtype
)
is
begin
  dbms_pipe.purge(p_pipe_name);
end purge_pipe;

$if epc.c_testing $then

procedure ut_reset
is
begin
  g_epc_key := null;
  g_pipe_name := 'EPC_REQUEST_PIPE';
  g_response_send_timeout := 10;
end ut_reset;

--%test
procedure ut_register
is
begin
  ut.expect(g_epc_key).to_be_null();
  for i_idx in 1..2
  loop
    ut.expect(register, to_char(i_idx)).to_equal(1);
    ut.expect(g_epc_key, to_char(i_idx)).to_equal(1);
  end loop;
end;

--%test
procedure ut_get_epc_key
is
begin
  ut.expect(get_epc_key).to_be_null();
  if register is null
  then
    raise program_error;
  end if;
  for i_idx in 1..2
  loop
    ut.expect(get_epc_key, to_char(i_idx)).to_equal(register);
  end loop;
end;  

--%test
procedure ut_set_connection_info
is
begin
  ut.expect(g_pipe_name).to_equal('EPC_REQUEST_PIPE');
  if register is null
  then
    raise program_error;
  end if;
  for i_idx in 1..2
  loop
    set_connection_info(get_epc_key, to_char(i_idx));
    ut.expect(g_pipe_name, to_char(i_idx)).to_equal(to_char(i_idx));
  end loop;
end;

--%test
procedure ut_get_connection_info
is
  l_pipe_name epc.pipe_name_subtype;
begin
  for i_idx in 1..2
  loop
    get_connection_info
    ( p_epc_key => get_epc_key
    , p_pipe_name => l_pipe_name
    );
    case i_idx
      when 1
      then
        ut.expect(l_pipe_name, to_char(i_idx)).to_be_null();
        if register is null
        then
          raise program_error;
        end if;
      when 2
      then
        ut.expect(l_pipe_name, to_char(i_idx)).to_equal(g_pipe_name);
    end case;
  end loop;
end;

procedure ut_set_response_send_timeout
is
begin
  ut.expect(g_response_send_timeout).to_equal(10);
  if register is null
  then
    raise program_error;
  end if;
  for i_idx in 1..2
  loop
    set_response_send_timeout(get_epc_key, i_idx);
    ut.expect(g_response_send_timeout, to_char(i_idx)).to_equal(i_idx);
  end loop;
end;

procedure ut_recv_request
is
  l_msg_info epc_srvr.msg_info_subtype := '0';
  l_msg_request varchar2(32767) := rpad('x', 32767, 'y');
begin
  -- without register nothing happens
  recv_request
  ( get_epc_key
  , l_msg_info
  , l_msg_request
  );
  raise epc.e_not_tested;
end;

procedure ut_send_response
is
  l_msg_info epc_srvr.msg_info_subtype := null;
  l_msg_request varchar2(32767) := rpad('x', 32767, 'y');
begin
  -- without register nothing happens
  send_response
  ( get_epc_key
  , l_msg_info
  , l_msg_request
  );
  raise epc.e_not_tested;
end;

procedure ut_send_request_interrupt
is
begin
  -- without register nothing happens
  send_request_interrupt
  ( get_epc_key
  );
  raise epc.e_not_tested;
end;

procedure ut_ping
is
begin
  -- without register nothing happens
  ping(get_epc_key, rpad('x', 32767, 'y'));
  raise epc.e_not_tested;
end;

procedure ut_create_pipe
is
begin
  raise epc.e_not_tested;
end;

procedure ut_purge_pipe
is
begin
  raise epc.e_not_tested;
end;

$end

end epc_srvr;
/

