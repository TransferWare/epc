REMARK
REMARK  $Header$
REMARK
REMARK  Description:    Oracle package specification for External Procedure Call Toolkit.
REMARK
REMARK  $Log$
REMARK  Revision 1.12  2004/12/28 12:51:14  gpaulissen
REMARK  Test on Amazon
REMARK
REMARK  Revision 1.11  2004/12/28 12:18:11  gpaulissen
REMARK  Test on Amazon
REMARK
REMARK  Revision 1.10  2004/12/17 15:54:21  gpaulissen
REMARK  inline namespaces introduced (xmlns:ns1)
REMARK
REMARK  Revision 1.9  2004/12/16 17:49:16  gpaulissen
REMARK  added dbms_xmlgen.convert for converting HTML entities
REMARK
REMARK  Revision 1.8  2004/12/16 16:03:24  gpaulissen
REMARK  Web services added
REMARK
REMARK  Revision 1.7  2004/10/21 10:37:08  gpaulissen
REMARK  * make lint
REMARK  * error reporting enhanced
REMARK  * oneway functions enhanced
REMARK
REMARK  Revision 1.6  2004/10/20 20:38:44  gpaulissen
REMARK  make lint
REMARK
REMARK  Revision 1.5  2004/10/15 20:41:32  gpaulissen
REMARK  XML namespace bugs solved.
REMARK
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

"xmlns:SOAP-ENV" constant varchar2(1000) := 
  'xmlns:SOAP-ENV="http://schemas.xmlsoap.org/soap/envelope/"';

SOAP_HEADER_START constant varchar2(1000) :=
  '<?xml version="1.0" encoding="UTF-8"?>'
  ||'<SOAP-ENV:Envelope'
  ||' '
  ||'xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"'
  ||' '
  ||'xmlns:SOAP-ENC="http://schemas.xmlsoap.org/soap/encoding/"'
  ||' '
  ||"xmlns:SOAP-ENV"
  ||' '
  ||'xmlns:xsd="http://www.w3.org/2001/XMLSchema"'
  ||' '
  ||'SOAP-ENV:encodingStyle="http://schemas.xmlsoap.org/soap/encoding/"'
  ||'>'
  ||'<SOAP-ENV:Body>';

SOAP_HEADER_END constant varchar2(1000) := 
  '</SOAP-ENV:Body></SOAP-ENV:Envelope>';

subtype connection_method_subtype is pls_integer;

CONNECTION_METHOD_DBMS_PIPE constant connection_method_subtype := 1;
CONNECTION_METHOD_UTL_TCP constant connection_method_subtype := 2;
CONNECTION_METHOD_UTL_HTTP constant connection_method_subtype := 3;

-- types
type epc_info_rectype is record (
  interface_name epc.interface_name_subtype,
  namespace epc.namespace_subtype,
  inline_namespace epc.namespace_subtype,
  connection_method connection_method_subtype default CONNECTION_METHOD_DBMS_PIPE,
  request_pipe epc.pipe_name_subtype default 'epc_request_pipe',
  tcp_connection utl_tcp.connection,
  http_connection http_connection_subtype,
  msg varchar2(32767),
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

   5 - To server: PROTOCOL, MSG SEQ, SOAP REQUEST MESSAGE [, RESULT PIPE ]
       From server: MSG SEQ, SOAP RESPONSE MESSAGE

       GJP 21-10-2004
       RESULT PIPE is empty for oneway functions.

*/
c_msg_protocol  constant pls_integer := 5;
c_max_msg_seq   constant pls_integer := 65535; /* msg seq wraps from 0 up till 65535 */

-- global variables
epc_info_tab epc_info_tabtype;
g_result_pipe epc.pipe_name_subtype := null;
/* The current message sequence number.
   The message sequence number is incremented before a message
   is sent. This (incremented) number if part of the message
   and must be returned as part of the result message. Now 
   this can be checked.
*/
g_msg_seq pls_integer := c_max_msg_seq;
g_cdata_tag_start constant varchar2(9) := '<![CDATA[';
g_cdata_tag_end   constant varchar2(3) := ']]>';

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
      epc_info_tab(l_idx).namespace := p_interface_name;
      epc_info_tab(l_idx).inline_namespace := 'ns1';
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
, p_connection in http_connection_subtype
)
is
begin
  epc_info_tab(p_epc_key).connection_method := CONNECTION_METHOD_UTL_HTTP;
  epc_info_tab(p_epc_key).http_connection := p_connection;
end set_connection_info;

procedure get_connection_info
(
  p_epc_key in epc_key_subtype
, p_connection out http_connection_subtype
)
is
begin
  if epc_info_tab(p_epc_key).connection_method = CONNECTION_METHOD_UTL_HTTP
  then
    p_connection := epc_info_tab(p_epc_key).http_connection;
  else
    raise no_data_found;
  end if;
end get_connection_info;

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

procedure set_namespace
(
  p_epc_key in epc_key_subtype
, p_namespace in varchar2
)
is
begin
  epc_info_tab(p_epc_key).namespace := p_namespace;
end set_namespace;

procedure set_inline_namespace
(
  p_epc_key in epc_key_subtype
, p_inline_namespace in varchar2
)
is
begin
  epc_info_tab(p_epc_key).inline_namespace := p_inline_namespace;
end set_inline_namespace;

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
  if p_value is null
  then
    raise epc.e_illegal_null_value;
  elsif p_data_type = epc.data_type_string
  then
    epc_info_tab(p_epc_key).msg :=
      epc_info_tab(p_epc_key).msg
      ||'<'||p_name||' xsi:type="string">'||g_cdata_tag_start||p_value||g_cdata_tag_end||'</'||p_name||'>';
  elsif p_data_type = epc.data_type_xml
  then
    epc_info_tab(p_epc_key).msg :=
      epc_info_tab(p_epc_key).msg
      ||'<'||p_name||'>'||p_value||'</'||p_name||'>';
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
  if p_value is null
  then
    raise epc.e_illegal_null_value;
  elsif p_data_type in ( epc.data_type_int, epc.data_type_long )
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
    ||'<'||p_name||' xsi:type="xsd:'||l_data_type||'">'||to_char(p_value)||'</'||p_name||'>';
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
  -- dbms_output.put_line('msg protocol: ' || c_msg_protocol);
  dbms_pipe.pack_message( c_msg_protocol );
  g_msg_seq := g_msg_seq + 1;
  if g_msg_seq > c_max_msg_seq then g_msg_seq := 0; end if;
  -- dbms_output.put_line('msg seq: ' || g_msg_seq);
  dbms_pipe.pack_message( g_msg_seq );
  -- dbms_output.put_line('soap request: ' || substr(p_soap_request, 1, 200));
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
  end if;

  l_retval := 
    dbms_pipe.send_message
    ( 
      epc_info_tab(p_epc_key).request_pipe
    , epc_info_tab(p_epc_key).send_timeout 
    );
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

procedure send_request_utl_http( 
  p_epc_key in epc_key_subtype
, p_soap_request in varchar2
, p_soap_action in varchar2
)
is
begin
  epc_info_tab(p_epc_key).http_connection.http_req := 
    utl_http.begin_request
    (
      epc_info_tab(p_epc_key).http_connection.url
    , epc_info_tab(p_epc_key).http_connection.method
    , epc_info_tab(p_epc_key).http_connection.version
    );
  utl_http.set_header
  (
    epc_info_tab(p_epc_key).http_connection.http_req
  , 'Content-Type', 'text/xml'
  );
  utl_http.set_header
  (
    epc_info_tab(p_epc_key).http_connection.http_req
  , 'Content-Length', length(p_soap_request)
  );
  utl_http.set_header
  (
    epc_info_tab(p_epc_key).http_connection.http_req
  , 'SOAPAction', p_soap_action
  );
  utl_http.write_text
  (
    epc_info_tab(p_epc_key).http_connection.http_req
  , p_soap_request
  );
end send_request_utl_http;

procedure show_envelope(p_msg in varchar2) 
is
  l_idx pls_integer;
  l_len pls_integer;
  l_end pls_integer;
begin
  l_idx := 1;
  l_len := length(p_msg);

  while (l_idx <= l_len)
  loop
    l_end := instr(p_msg, ' ', l_idx + 60);
    if l_end = 0
    then
      l_end := l_idx + 60;
    end if;

    while l_idx + 255 < l_end
    loop
      dbms_output.put_line(substr(p_msg, l_idx, 255));
      l_idx := l_idx + 255;
    end loop;
    dbms_output.put_line(substr(p_msg, l_idx, l_end - l_idx));
    l_idx := l_end;
  end loop;
end show_envelope;

function get_method_name
(
  p_epc_key in epc_key_subtype
, p_method_name in epc.method_name_subtype
)
return varchar2
is
begin
  if epc_info_tab(p_epc_key).inline_namespace is null
  then
    return p_method_name;
  else
    return epc_info_tab(p_epc_key).inline_namespace
    ||':'
    ||p_method_name;
  end if;
end get_method_name;

function get_xmlns
(
  p_epc_key in epc_key_subtype
)
return varchar2
is
begin
  if epc_info_tab(p_epc_key).inline_namespace is null
  then
    return 'xmlns';
  else
    return 'xmlns:' || epc_info_tab(p_epc_key).inline_namespace;
  end if;
end get_xmlns;

procedure send_request
( 
  p_epc_key in epc_key_subtype
, p_method_name in epc.method_name_subtype
, p_oneway in pls_integer
)
is
begin
  epc_info_tab(p_epc_key).msg :=
SOAP_HEADER_START
||'<'
||get_method_name(p_epc_key, p_method_name)
||' '
||get_xmlns(p_epc_key)
||'="'
||epc_info_tab(p_epc_key).namespace
||'">'
||epc_info_tab(p_epc_key).msg
||'</'
||get_method_name(p_epc_key, p_method_name)
||'>'
||SOAP_HEADER_END;

  epc_info_tab(p_epc_key).doc := 
    xmltype.createxml( epc_info_tab(p_epc_key).msg );
  -- show_envelope(epc_info_tab(p_epc_key).doc.getstringval());

  if epc_info_tab(p_epc_key).connection_method = CONNECTION_METHOD_DBMS_PIPE
  then
    send_request_dbms_pipe(p_epc_key, epc_info_tab(p_epc_key).msg, p_oneway);
  elsif epc_info_tab(p_epc_key).connection_method = CONNECTION_METHOD_UTL_HTTP
  then
    send_request_utl_http
    ( 
      p_epc_key
    , epc_info_tab(p_epc_key).msg
    , epc_info_tab(p_epc_key).namespace || '#' || p_method_name
    );
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
  l_retval :=
    dbms_pipe.receive_message
    ( 
      g_result_pipe
    , epc_info_tab(p_epc_key).recv_timeout 
    );
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

procedure recv_response_utl_http( 
  p_epc_key in epc_key_subtype
)
is
  http_resp utl_http.resp;
begin
  http_resp := 
    utl_http.get_response(epc_info_tab(p_epc_key).http_connection.http_req);
  begin
    utl_http.read_text(http_resp, epc_info_tab(p_epc_key).msg);
    utl_http.end_response(http_resp);
  exception
    when others
    then
      utl_http.end_response(http_resp);
      raise;
  end;
end recv_response_utl_http;

procedure check_fault(p_doc in out nocopy xmltype) as
  fault_node   xmltype;
  fault_code   varchar2(256);
  fault_string varchar2(32767);
begin
   fault_node := p_doc.extract('/SOAP-ENV:Fault', "xmlns:SOAP-ENV");
   if (fault_node is not null) then
     fault_code := 
       fault_node.extract
       (
         '/SOAP-ENV:Fault/faultcode/child::text()'
       , "xmlns:SOAP-ENV"
       ).getstringval();
     fault_string := 
       fault_node.extract
       (
         '/SOAP-ENV:Fault/faultstring/child::text()'
       , "xmlns:SOAP-ENV"
       ).getstringval();
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
  elsif epc_info_tab(p_epc_key).connection_method = CONNECTION_METHOD_UTL_HTTP
  then
    recv_response_utl_http(p_epc_key);
  end if;
  epc_info_tab(p_epc_key).doc := xmltype.createxml( epc_info_tab(p_epc_key).msg );
  epc_info_tab(p_epc_key).doc :=
    epc_info_tab(p_epc_key).doc.extract('/SOAP-ENV:Envelope/SOAP-ENV:Body/child::node()',
      "xmlns:SOAP-ENV");
  -- show_envelope(epc_info_tab(p_epc_key).doc.getstringval());
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
  l_value epc.string_subtype;
  l_xml XMLType;
  l_extract_type varchar2(20);
begin
  if p_data_type = epc.data_type_xml
  then
    l_extract_type := null; -- 'child::node()';
  else
    l_extract_type := '/child::text()';
  end if;

  l_xml := 
    epc_info_tab(p_epc_key).doc.extract
    (
      '//'||p_name||l_extract_type
    , get_xmlns(p_epc_key)||'="'||epc_info_tab(p_epc_key).namespace||'"'
    );

  l_value := l_xml.getstringval();

  if p_data_type = epc.data_type_string
  then
    if instr(l_value, g_cdata_tag_start) = 1
    and instr(l_value, g_cdata_tag_end, length(l_value) - length(g_cdata_tag_end) + 1) > 0
    then
      l_value :=
        substr
        (
          l_value
        , length(g_cdata_tag_start) + 1
        , length(l_value) - length(g_cdata_tag_start) - length(g_cdata_tag_end)
        );
    end if;

    p_value := 
      dbms_xmlgen.convert
      (
        xmlData => l_value
      , flag => dbms_xmlgen.entity_decode
      );
  else
    p_value := l_value;
  end if;
end get_response_parameter;

procedure get_response_parameter
(
  p_epc_key in epc_key_subtype
, p_name in epc.parameter_name_subtype
, p_data_type in epc.data_type_subtype
, p_value out number
)
is
  l_value epc.string_subtype;
begin
  epc_clnt.get_response_parameter
  (
    p_epc_key => p_epc_key
  , p_name => p_name
  , p_data_type => p_data_type
  , p_value => l_value
  );
  p_value := to_number(l_value);
end get_response_parameter;

end epc_clnt;
/

show errors
