REMARK
REMARK  $Header$
REMARK
REMARK  Description:    Oracle package specification for External Procedure Call Toolkit.
REMARK
REMARK  $Log$
REMARK  Revision 1.4  2004/10/15 13:53:40  gpaulissen
REMARK  XML added
REMARK
REMARK  Revision 1.3  2004/05/21 15:04:35  gpaulissen
REMARK  Eerste implementatie
REMARK
REMARK  Revision 1.2  2004/04/21 11:16:55  gpaulissen
REMARK  .
REMARK
REMARK  Revision 1.1  2004/04/05 14:52:33  gpaulissen
REMARK  Interface changed
REMARK
REMARK  Revision 1.1  2004/04/02 10:26:28  gpaulissen
REMARK  New interface for epc
REMARK
REMARK
REMARK

create or replace package body epc_clnt as

subtype connection_method_subtype is pls_integer;

CONNECTION_METHOD_DBMS_PIPE constant connection_method_subtype := 1;
CONNECTION_METHOD_UTL_TCP constant connection_method_subtype := 2;

-- types
type epc_info_rectype is record (
  interface_name epc.interface_name_subtype,
  connection_method connection_method_subtype default CONNECTION_METHOD_DBMS_PIPE,
  request_pipe epc.pipe_name_subtype default 'epc_request_pipe',
  tcp_connection utl_tcp.connection,
  msg varchar2(4000),
  doc xmltype, /* output */
  send_timeout pls_integer default 10,
  recv_timeout pls_integer default 10
);

type epc_info_tabtype is table of epc_info_rectype index by binary_integer;

-- constants
/* 
   History of protocols:
 
   1 - original protocol
       To server: RESULT PIPE, PROTOCOL, INTERFACE, FUNCTION, PARAMETERS IN
       From server: EXEC CODE, SQL CODE, PARAMETERS OUT

   2 - same as protocol 1, but FUNCTION is now the FUNCTION SIGNATURE.

   3 - To server: PROTOCOL, MSG SEQ, INTERFACE, FUNCTION, [ RESULT PIPE, ] PARAMETERS IN
       From server: MSG SEQ, PARAMETERS OUT

       MSG SEQ has been added in order to check for messages which have
       not been (correctly) processed by the server. 

   4 - To server: PROTOCOL, MSG SEQ, INTERFACE, FUNCTION, RESULT PIPE, PARAMETERS IN
       From server: MSG SEQ, PARAMETERS OUT

       GJP 07-04-2004
       RESULT PIPE is v_oneway_result_pipe for oneway functions.

   5 - To server: PROTOCOL, MSG SEQ, SOAP REQUEST MESSAGE, RESULT PIPE
       From server: MSG SEQ, SOAP RESPONSE MESSAGE

       GJP 07-04-2004
       RESULT PIPE is v_oneway_result_pipe for oneway functions.

*/
c_msg_protocol  constant pls_integer := 5;
c_max_msg_seq   constant pls_integer := 65535; /* msg seq wraps from 0 up till 65535 */

-- global variables
epc_info_tab epc_info_tabtype;
g_result_pipe epc.pipe_name_subtype := null;
--v_request_pipe epc.pipe_name_subtype := 'epc_request_pipe';
g_oneway_result_pipe constant epc.pipe_name_subtype := 'N/A';
/* The current message sequence number.
   The message sequence number is incremented before a message
   is sent. This (incremented) number if part of the message
   and must be returned as part of the result message. Now 
   this can be checked.
*/
g_msg_seq pls_integer := c_max_msg_seq;


-- functions
function register( p_interface_name in epc.interface_name_subtype )
return epc_key_subtype
is
  l_idx epc_key_subtype;
begin
  begin
    l_idx := get_epc_key( p_interface_name );
  exception
    when no_data_found
    then
      l_idx := epc_info_tab.count+1;
      epc_info_tab(l_idx).interface_name := p_interface_name;
  end;
  return l_idx;
end register;

function get_epc_key( p_interface_name in epc.interface_name_subtype )
return epc_key_subtype
is
begin
  if epc_info_tab.count > 0 
  then
    for l_idx in epc_info_tab.first .. epc_info_tab.last
    loop
      if epc_info_tab(l_idx).interface_name = p_interface_name
      then
        return l_idx;
      end if;
    end loop;
  end if;
  raise no_data_found;
end get_epc_key;

procedure set_connection_info
(
  p_epc_key in epc_key_subtype
, p_connection in utl_tcp.connection
)
is
begin
  epc_info_tab(p_epc_key).connection_method := CONNECTION_METHOD_UTL_TCP;
  epc_info_tab(p_epc_key).tcp_connection := p_connection;
end set_connection_info;

procedure get_connection_info
(
  p_epc_key in epc_key_subtype
, p_connection out utl_tcp.connection
)
is
begin
  if epc_info_tab(p_epc_key).connection_method = CONNECTION_METHOD_UTL_TCP
  then
    p_connection := epc_info_tab(p_epc_key).tcp_connection;
  else
    raise no_data_found;
  end if;
end get_connection_info;

procedure set_connection_info
(
  p_epc_key in epc_key_subtype
, p_pipe_name in epc.pipe_name_subtype
)
is
begin
  epc_info_tab(p_epc_key).connection_method := CONNECTION_METHOD_DBMS_PIPE;
  epc_info_tab(p_epc_key).request_pipe := p_pipe_name;
end set_connection_info;

procedure get_connection_info
(
  p_epc_key in epc_key_subtype
, p_pipe_name out epc.pipe_name_subtype
)
is
begin
  if epc_info_tab(p_epc_key).connection_method = CONNECTION_METHOD_DBMS_PIPE
  then
    p_pipe_name := epc_info_tab(p_epc_key).request_pipe;
  else
    raise no_data_found;
  end if;
end get_connection_info;

procedure set_request_send_timeout
(
  p_epc_key in epc_key_subtype
, p_request_send_timeout in pls_integer
)
is
begin
  epc_info_tab(p_epc_key).send_timeout := p_request_send_timeout;
end set_request_send_timeout;

procedure set_response_recv_timeout
(
  p_epc_key in epc_key_subtype
, p_response_recv_timeout in pls_integer
)
is
begin
  epc_info_tab(p_epc_key).recv_timeout := p_response_recv_timeout;
end set_response_recv_timeout;

procedure new_request
(
  p_epc_key in epc_key_subtype
)
is
begin
  epc_info_tab(p_epc_key).msg := null;
end new_request;

procedure set_request_parameter
(
  p_epc_key in epc_key_subtype
, p_name in epc.parameter_name_subtype
, p_data_type in epc.data_type_subtype
, p_value in varchar2
)
is
begin
  if p_data_type = epc.data_type_string
  then
    epc_info_tab(p_epc_key).msg :=
      epc_info_tab(p_epc_key).msg
      ||'<'||p_name||' xsi:type="string">'||p_value||'</'||p_name||'>';
  else
    raise value_error;
  end if;
end set_request_parameter;

procedure set_request_parameter
(
  p_epc_key in epc_key_subtype
, p_name in epc.parameter_name_subtype
, p_data_type in epc.data_type_subtype
, p_value in number
)
is
  l_data_type varchar2(10);
begin
  if p_data_type in ( epc.data_type_int, epc.data_type_long )
  then
    l_data_type := 'integer';
  elsif p_data_type = epc.data_type_float
  then
    l_data_type := 'float';
  elsif p_data_type = epc.data_type_double
  then
    l_data_type := 'double';
  else
    raise value_error;
  end if;

  epc_info_tab(p_epc_key).msg :=
    epc_info_tab(p_epc_key).msg 
    ||'<'||p_name||' xsi:type="'||l_data_type||'">'||to_char(p_value)||'</'||p_name||'>';
end set_request_parameter;

procedure send_request_dbms_pipe( 
  p_epc_key in epc_key_subtype
, p_soap_request in varchar2
, p_oneway in pls_integer 
)
is
  l_retval pls_integer := -1;

  e_send_error exception;
begin
  dbms_pipe.reset_buffer;
  dbms_output.put_line('msg protocol: ' || c_msg_protocol);
  dbms_pipe.pack_message( c_msg_protocol );
  g_msg_seq := g_msg_seq + 1;
  if g_msg_seq > c_max_msg_seq then g_msg_seq := 0; end if;
  dbms_output.put_line('msg seq: ' || g_msg_seq);
  dbms_pipe.pack_message( g_msg_seq );
  dbms_output.put_line('soap request: ' || substr(p_soap_request, 1, 200));
  dbms_pipe.pack_message( p_soap_request );
  if p_oneway = 0
  then
    if g_result_pipe is null
    then
      g_result_pipe := 'EPC$' || dbms_pipe.unique_session_name;

      /* 
      || GJP 08-01-2001 
      || Emptying the result pipe seems to prevent timeouts on receipt. 
      */

      dbms_pipe.purge( g_result_pipe );
    end if;

    dbms_pipe.pack_message( g_result_pipe );
  else
    /* GJP 07-03-2004 Must supply a result pipe */
    dbms_pipe.pack_message( g_oneway_result_pipe );
  end if;

  l_retval := dbms_pipe.send_message( epc_info_tab(p_epc_key).request_pipe, epc_info_tab(p_epc_key).send_timeout );
  if l_retval <> 0
  then
    raise e_send_error;
  end if;
exception
  when e_send_error
  then
    dbms_output.put_line( '(epc_clnt.send_request_dbms_pipe) ' ||
                          'Error sending message.' || chr(10) ||
                          'Return value: ' || to_char(l_retval) || chr(10) ||
                          'Current message number: ' || g_msg_seq );
    raise epc.e_comm_error;
end send_request_dbms_pipe;

procedure show_envelope(p_msg in varchar2) as
  l_idx pls_integer;
  l_len pls_integer;
begin
  l_idx := 1;
  l_len := length(p_msg);

  while (l_idx <= l_len)
  loop
    dbms_output.put_line(substr(p_msg, l_idx, 60));
    l_idx := l_idx + 60;
  end loop;
end show_envelope;

procedure send_request
( 
  p_epc_key in epc_key_subtype
, p_method_name in epc.method_name_subtype
, p_oneway in pls_integer
)
is
begin
  epc_info_tab(p_epc_key).msg :=
'<SOAP-ENV:Envelope xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/"
xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/"
xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
xmlns:xsd="http://www.w3.org/2001/XMLSchema">'
||'<SOAP-ENV:Body><'
||p_method_name
||' xmlns="'
||epc_info_tab(p_epc_key).interface_name
||'">'
||epc_info_tab(p_epc_key).msg
||'</'
||p_method_name
||'></SOAP-ENV:Body></SOAP-ENV:Envelope>';

  epc_info_tab(p_epc_key).doc := xmltype.createxml( epc_info_tab(p_epc_key).msg );
  show_envelope(epc_info_tab(p_epc_key).doc.getstringval());

  if epc_info_tab(p_epc_key).connection_method = CONNECTION_METHOD_DBMS_PIPE
  then
    send_request_dbms_pipe(p_epc_key, epc_info_tab(p_epc_key).msg, p_oneway);
  end if;
end send_request;

procedure recv_response_dbms_pipe
( 
  p_epc_key in epc_key_subtype
)
is
  l_retval pls_integer := -1;
  l_msg_seq_result pls_integer;

  e_recv_error exception;
begin
  l_retval := dbms_pipe.receive_message( g_result_pipe, epc_info_tab(p_epc_key).recv_timeout );
  if l_retval <> 0
  then
    raise e_recv_error;
  end if;

  /* Get the message sequence */
  dbms_pipe.unpack_message( l_msg_seq_result );
  if l_msg_seq_result <> g_msg_seq
  then
    raise epc.e_wrong_protocol;
  end if;

  dbms_pipe.unpack_message( epc_info_tab(p_epc_key).msg );
exception
  when e_recv_error
  then
    dbms_output.put_line( '(epc_clnt.recv_response_dbms_pipe) ' ||
                          'Error receiving message.' || chr(10) ||
                          'Return value: ' || to_char(l_retval) || chr(10) ||
                          'Current message number: ' || g_msg_seq );
--  epc.purge( g_result_pipe );
    raise epc.e_comm_error;

  when epc.e_wrong_protocol
  then
    dbms_output.put_line( '(epc_clnt.recv_response_dbms_pipe) ' ||
                          'Wrong message number received.' || chr(10) ||
                          'Current message number: ' || g_msg_seq || chr(10) ||
                          'Received message number: ' || l_msg_seq_result );
--  epc.purge( g_result_pipe );
    raise epc.e_comm_error;
end recv_response_dbms_pipe;

procedure check_fault(p_doc in out nocopy xmltype) as
  fault_node   xmltype;
  fault_code   varchar2(256);
  fault_string varchar2(32767);
begin
   fault_node := p_doc.extract('/SOAP-ENV:Fault',
     'xmlns:soap="http://schemas.xmlsoap.org/soap/envelope/');
   if (fault_node is not null) then
     fault_code := fault_node.extract('/SOAP-ENV:Fault/faultcode/child::text()',
       'xmlns:soap="http://schemas.xmlsoap.org/soap/envelope/').getstringval();
     fault_string := fault_node.extract('/SOAP-ENV:Fault/faultstring/child::text()',
       'xmlns:soap="http://schemas.xmlsoap.org/soap/envelope/').getstringval();
     raise_application_error(-20000, fault_code || ' - ' || fault_string);
   end if;
end check_fault;

procedure recv_response
( 
  p_epc_key in epc_key_subtype
)
is
begin
  if epc_info_tab(p_epc_key).connection_method = CONNECTION_METHOD_DBMS_PIPE
  then
    recv_response_dbms_pipe(p_epc_key);
  end if;
  epc_info_tab(p_epc_key).doc := xmltype.createxml( epc_info_tab(p_epc_key).msg );
  epc_info_tab(p_epc_key).doc :=
    epc_info_tab(p_epc_key).doc.extract('/SOAP-ENV:Envelope/SOAP-ENV:Body/child::node()',
      'xmlns:soap="http://schemas.xmlsoap.org/soap/envelope/"');
  show_envelope(epc_info_tab(p_epc_key).doc.getstringval());
  check_fault(epc_info_tab(p_epc_key).doc);
end recv_response;

procedure get_response_parameter
(
  p_epc_key in epc_key_subtype
, p_name in epc.parameter_name_subtype
, p_data_type in epc.data_type_subtype
, p_value out varchar2
)
is
begin
  p_value := epc_info_tab(p_epc_key).doc.extract('//'||p_name||'/child::text()',
      'xmlns="'||epc_info_tab(p_epc_key).interface_name||'"').getstringval();
end get_response_parameter;

procedure get_response_parameter
(
  p_epc_key in epc_key_subtype
, p_name in epc.parameter_name_subtype
, p_data_type in epc.data_type_subtype
, p_value out number
)
is
begin
  p_value := to_number(epc_info_tab(p_epc_key).doc.extract('//'||p_name||'/child::text()',
      'xmlns="'||epc_info_tab(p_epc_key).interface_name||'"').getstringval());
end get_response_parameter;

end epc_clnt;
/

show errors
