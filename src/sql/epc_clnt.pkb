REMARK
REMARK  $Header$
REMARK
REMARK  Description:    Oracle package specification for External Procedure Call Toolkit.
REMARK
REMARK  $Log$
REMARK  Revision 1.13  2005/01/03 12:26:44  gpaulissen
REMARK  Release 4.4.0
REMARK
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
/* line 53 */
create or replace package body epc_clnt as

subtype connection_method_subtype is pls_integer;

/*

XMLRPC Wikipedia

<?xml version="1.0"?>
<methodCall>
  <methodName>examples.getStateName</methodName>
  <params>
    <param>
        <value><i4>40</i4></value>
    </param>
  </params>
</methodCall>

An example of a typical XML-RPC response would be:

<?xml version="1.0"?>
<methodResponse>
  <params>
    <param>
        <value><string>South Dakota</string></value>
    </param>
  </params>
</methodResponse>

A typical XML-RPC fault would be:

<?xml version="1.0"?>
<methodResponse>
  <fault>
    <value>
      <struct>
        <member>
          <name>faultCode</name>
          <value><int>4</int></value>
        </member>
        <member>
          <name>faultString</name>
          <value><string>Too many parameters.</string></value>
        </member>
      </struct>
    </value>
  </fault>
</methodResponse>

*/

CONNECTION_METHOD_DBMS_PIPE constant connection_method_subtype := 1;
CONNECTION_METHOD_UTL_TCP constant connection_method_subtype := 2;
CONNECTION_METHOD_UTL_HTTP constant connection_method_subtype := 3;

-- types
type epc_info_rectype is record (
  interface_name epc.interface_name_subtype, /* the key */

  method_name epc.method_name_subtype,
  oneway pls_integer,

  /* Protocol information */
  protocol protocol_subtype default "NATIVE",

  /* SOAP related information */
  namespace epc.namespace_subtype,
  inline_namespace epc.namespace_subtype,

  /* Connection information */
  connection_method connection_method_subtype default CONNECTION_METHOD_DBMS_PIPE,
  request_pipe epc.pipe_name_subtype default 'epc_request_pipe',
  tcp_connection utl_tcp.connection,
  http_connection http_connection_subtype,

  msg epc.xml_subtype, /* input/output */

  doc xmltype, /* output */

  next_out_parameter pls_integer, /* number of next out (or in/out) parameter to read for XMLRPC */

  send_timeout pls_integer default 10,
  recv_timeout pls_integer default 10
);

type epc_info_tabtype is table of epc_info_rectype index by binary_integer;

-- constants
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

g_decimal_char varchar2(1); -- needed to convert numbers to/from strings

/*DBUG
g_indent pls_integer := 1;
/*DBUG*/

-- LOCAL

/*DBUG
procedure enter(p_procname in varchar2)
is
begin
  dbms_output.put_line(substr(lpad('>', g_indent, ' ')||p_procname, 1, 255));
  g_indent := g_indent + 2;
exception
  when others
  then null;
end;

procedure print(p_break_point in varchar2, p_format in varchar2, p_data in varchar2 default null)
is
begin
  dbms_output.put_line(substr(lpad(' ', g_indent-1, ' ')||p_break_point||': '||replace(p_format, '%s', p_data), 1, 255));
exception
  when others
  then null;
end;

procedure leave
is
begin
  g_indent := g_indent - 2;
  dbms_output.put_line(substr(lpad('<', g_indent, ' '), 1, 255));
exception
  when others
  then null;
end;

procedure leave_on_error
is
  l_error_stack constant varchar2(32767) := dbms_utility.format_error_backtrace;
begin
  leave;
  dbms_output.put_line(substr(l_error_stack, 1+0*255, 255));
  dbms_output.put_line(substr(l_error_stack, 1+1*255, 255));
  dbms_output.put_line(substr(l_error_stack, 1+2*255, 255));
exception
  when others
  then null;
end;
/*DBUG*/

procedure set_request_parameter
( p_name in epc.parameter_name_subtype
, p_data_type in epc.data_type_subtype
, p_value in varchar2
, p_max_bytes in integer
, p_epc_info_rec in out nocopy epc_info_rectype
);

procedure get_response_parameter
( p_name in epc.parameter_name_subtype
, p_data_type in epc.data_type_subtype
, p_value out varchar2
, p_max_bytes in integer
, p_epc_info_rec in out nocopy epc_info_rectype
);

procedure new_request
( p_method_name in epc.method_name_subtype
, p_oneway in pls_integer
, p_epc_info_rec in out nocopy epc_info_rectype
)
is
begin
/*DBUG
  enter('epc_clnt.new_request');
  print('input', 'p_method_name: %s', p_method_name);
  print('input', 'p_oneway: %s', p_oneway);
/*DBUG*/

  p_epc_info_rec.msg := null;
  p_epc_info_rec.method_name := p_method_name;
  p_epc_info_rec.oneway := p_oneway;

  if p_epc_info_rec.connection_method = CONNECTION_METHOD_DBMS_PIPE
  then
/*DBUG
    print('info', 'resetting buffer');
/*DBUG*/
    dbms_pipe.reset_buffer;
    dbms_pipe.pack_message( p_epc_info_rec.protocol );
    g_msg_seq := g_msg_seq + 1;
    if g_msg_seq > c_max_msg_seq then g_msg_seq := 0; end if;
/*DBUG
    print('info', 'g_msg_seq: %s', g_msg_seq);
/*DBUG*/
    dbms_pipe.pack_message( g_msg_seq );

    if p_epc_info_rec.protocol = "NATIVE"
    then
      set_request_parameter
      ( p_name => 'interface'
      , p_data_type => epc.data_type_string
      , p_value => p_epc_info_rec.interface_name
      , p_max_bytes => null
      , p_epc_info_rec => p_epc_info_rec
      );
      set_request_parameter
      ( p_name => 'function'
      , p_data_type => epc.data_type_string
      , p_value => p_epc_info_rec.method_name
      , p_max_bytes => null
      , p_epc_info_rec => p_epc_info_rec
      );
    end if;
  end if;

/*DBUG
  leave;
/*DBUG*/
end new_request;

procedure set_request_parameter
( p_name in epc.parameter_name_subtype
, p_data_type in epc.data_type_subtype
, p_value in varchar2
, p_max_bytes in integer
, p_epc_info_rec in out nocopy epc_info_rectype
)
is
begin
/*DBUG
  enter('epc_clnt.set_request_parameter (1)');
  print('input', 'p_name: %s', p_name);
  print('input', 'p_data_type: %s', p_data_type);
  print('input', 'p_value: %s', p_value);
  print('input', 'p_max_bytes: %s', p_max_bytes);
/*DBUG*/

  if p_value is null
  then
    raise epc.e_illegal_null_value;
  elsif p_max_bytes is not null and lengthb(p_value) > p_max_bytes
  then
    raise value_error;
  else
    case p_epc_info_rec.protocol
      when "NATIVE"
      then
        case 
          when p_data_type in (epc.data_type_string, epc.data_type_xml)
          then
            p_epc_info_rec.msg :=
              p_epc_info_rec.msg
              ||to_char(p_data_type)
              ||to_char(nvl(lengthb(p_value), 0), 'FM000X')
              ||p_value;

          else
            p_epc_info_rec.msg :=
              p_epc_info_rec.msg
              ||to_char(p_data_type)
              ||to_char(nvl(lengthb(p_value), 0), 'FM0X')
              ||p_value;
        end case;

      when "SOAP"
      then
        case p_data_type
          when epc.data_type_string
          then
            p_epc_info_rec.msg :=
              p_epc_info_rec.msg
              ||'<'||p_name||' xsi:type="string">'
              ||g_cdata_tag_start
              ||p_value
              ||g_cdata_tag_end
              ||'</'||p_name||'>';

          when epc.data_type_xml
          then
            p_epc_info_rec.msg :=
              p_epc_info_rec.msg
              ||'<'||p_name||'>'
              ||p_value
              ||'</'||p_name||'>';

          else
            raise value_error;
        end case;
      
      when "XMLRPC"
      then
        if p_data_type in (epc.data_type_string, epc.data_type_xml)
        then
          p_epc_info_rec.msg :=
            p_epc_info_rec.msg
            ||'<param><value><string>'
            ||g_cdata_tag_start
            ||p_value
            ||g_cdata_tag_end
            ||'</string></value></param>'
            ||chr(10);
        else
          raise value_error;
        end if; 

      else
        raise program_error;
    end case;            
  end if;

/*DBUG
  print('output', 'msg: %s', p_epc_info_rec.msg);
  leave;
/*DBUG*/
end set_request_parameter;

procedure set_request_parameter
( p_name in epc.parameter_name_subtype
, p_data_type in epc.data_type_subtype
, p_value in number
, p_epc_info_rec in out nocopy epc_info_rectype
)
is
  l_data_type varchar2(10);
begin
/*DBUG
  enter('epc_clnt.set_request_parameter (2)');
  print('input', 'p_name: %s', p_name);
  print('input', 'p_data_type: %s', p_data_type);
  print('input', 'p_value: %s', p_value);
/*DBUG*/

  if p_value is null
  then
    raise epc.e_illegal_null_value;
  else
    case p_epc_info_rec.protocol
      when "NATIVE"
      then
        set_request_parameter
        ( p_name => p_name
        , p_data_type => p_data_type
        , p_value => replace(to_char(p_value), g_decimal_char, '.')
        , p_max_bytes => null
        , p_epc_info_rec => p_epc_info_rec
        );

      when "SOAP"
      then
        case p_data_type
          when epc.data_type_int
          then
            l_data_type := 'integer';
          when epc.data_type_long
          then
            l_data_type := 'integer';
          when epc.data_type_float
          then
            l_data_type := 'float';
          when epc.data_type_double
          then
            l_data_type := 'double';
          else
            raise value_error;
        end case;

        p_epc_info_rec.msg :=
          p_epc_info_rec.msg 
          ||'<'||p_name||' xsi:type="xsd:'||l_data_type||'">'
          ||replace(to_char(p_value), g_decimal_char, '.')
          ||'</'||p_name||'>';

      when "XMLRPC"
      then
        case p_data_type
          when epc.data_type_int
          then
            l_data_type := 'int';
          when epc.data_type_long
          then
            l_data_type := 'int';
          when epc.data_type_float
          then
            l_data_type := 'double';
          when epc.data_type_double
          then
            l_data_type := 'double';
          else
            raise value_error;
        end case;

        p_epc_info_rec.msg :=
          p_epc_info_rec.msg
          ||'<param><value><'||l_data_type||'>'
          ||replace(to_char(p_value), g_decimal_char, '.')
          ||'</'||l_data_type||'></value></param>'
          ||chr(10);

      else
        raise program_error;
    end case;
  end if;

/*DBUG
  print('output', 'msg: %s', p_epc_info_rec.msg);
  leave;
/*DBUG*/
end set_request_parameter;

procedure set_request_parameter
( p_name in epc.parameter_name_subtype
, p_data_type in epc.data_type_subtype
, p_value in date
, p_epc_info_rec in out nocopy epc_info_rectype
)
is
  l_value constant varchar2(17) := 
    to_char(p_value, 'yyyymmdd')
    ||'T'
    ||to_char(p_value, 'hh24:mi:ss');
begin
/*DBUG
  enter('epc_clnt.set_request_parameter (3)');
  print('input', 'p_name: %s', p_name);
  print('input', 'p_data_type: %s', p_data_type);
  print('input', 'p_value: %s', l_value);
/*DBUG*/

  if p_value is null
  then
    raise epc.e_illegal_null_value;
  else
    case p_epc_info_rec.protocol
      when "NATIVE"
      then
        set_request_parameter
        ( p_name => p_name
        , p_data_type => p_data_type
        , p_value => l_value
        , p_max_bytes => null
        , p_epc_info_rec => p_epc_info_rec
        );

      when "XMLRPC"
      then
        declare
          l_data_type constant varchar2(16) := 'dateTime.iso8601';
        begin
          case p_data_type
            when epc.data_type_date
            then
              null;

            else
              raise value_error;
          end case;

          p_epc_info_rec.msg :=
            p_epc_info_rec.msg
            ||'<param><value><'||l_data_type||'>'
            ||l_value
            ||'</'||l_data_type||'></value></param>'
            ||chr(10);
        end;

      else
        raise program_error;
    end case;
  end if;

/*DBUG
  print('output', 'msg: %s', p_epc_info_rec.msg);
  leave;
/*DBUG*/
end set_request_parameter;

function get_method_name
( p_epc_info_rec in epc_info_rectype
)
return varchar2
is
begin
  if p_epc_info_rec.inline_namespace is null
  then
    return p_epc_info_rec.method_name;
  else
    return p_epc_info_rec.inline_namespace
    ||':'
    ||p_epc_info_rec.method_name;
  end if;
end get_method_name;

function get_xmlns
( p_epc_info_rec in epc_info_rectype
)
return varchar2
is
begin
  if p_epc_info_rec.inline_namespace is null
  then
    return 'xmlns';
  else
    return 'xmlns:' || p_epc_info_rec.inline_namespace;
  end if;
end get_xmlns;

procedure send_request_dbms_pipe
( p_epc_info_rec in epc_info_rectype
)
is
  l_retval pls_integer := -1;
begin
/*DBUG
  enter('epc_clnt.send_request_dbms_pipe');
/*DBUG*/

  dbms_pipe.pack_message( p_epc_info_rec.msg );
  if p_epc_info_rec.oneway = 0
  then
    dbms_pipe.pack_message( g_result_pipe );
  end if;

  l_retval := 
    dbms_pipe.send_message
    ( 
      p_epc_info_rec.request_pipe
    , p_epc_info_rec.send_timeout 
    );

  case l_retval
    when 0 -- OK
    then null;

    when 1 -- time-out
    then raise_application_error
         ( epc.c_comm_error
         , '(epc_clnt.send_request_dbms_pipe) ' ||
           'Timed out while sending message number ' ||
           to_char(g_msg_seq) ||
           ' for pipe ' || p_epc_info_rec.request_pipe ||
           '.'
         );

    when 3 -- message-interrupted
    then raise_application_error
         ( epc.c_comm_error
         , '(epc_clnt.send_request_dbms_pipe) ' ||
           'Interrupted while sending message number ' ||
           to_char(g_msg_seq) ||
           ' for pipe ' || p_epc_info_rec.request_pipe ||
           '.'
         );

    else
      raise program_error; -- there are no more return codes
  end case;

/*DBUG
  leave;
/*DBUG*/
end send_request_dbms_pipe;

procedure send_request_utl_http
( p_epc_info_rec in out nocopy epc_info_rectype
, p_soap_action in varchar2 default null
)
is
begin
  p_epc_info_rec.http_connection.http_req := 
    utl_http.begin_request
    (
      p_epc_info_rec.http_connection.url
    , p_epc_info_rec.http_connection.method
    , p_epc_info_rec.http_connection.version
    );

  case p_epc_info_rec.protocol
    when "SOAP"
    then
      utl_http.set_header
      (
        p_epc_info_rec.http_connection.http_req
      , 'Content-Type', 'text/xml'
      );
      utl_http.set_header
      (
        p_epc_info_rec.http_connection.http_req
      , 'Content-Length', length(p_epc_info_rec.msg)
      );
      utl_http.set_header
      (
        p_epc_info_rec.http_connection.http_req
      , 'SOAPAction', p_soap_action
      );

    when "XMLRPC"
    then
      utl_http.set_header
      (
        p_epc_info_rec.http_connection.http_req
      , 'User-Agent', 'EPC'
      );
      utl_http.set_header
      (
        p_epc_info_rec.http_connection.http_req
      , 'Content-Type', 'text/xml'
      );
      utl_http.set_header
      (
        p_epc_info_rec.http_connection.http_req
      , 'Content-Length', length(p_epc_info_rec.msg)
      );

    else
      raise program_error;
  end case;

  utl_http.write_text
  (
    p_epc_info_rec.http_connection.http_req
  , p_epc_info_rec.msg
  );
end send_request_utl_http;

procedure send_request_utl_tcp
( p_epc_info_rec in out nocopy epc_info_rectype
)
is
  l_length pls_integer := length(p_epc_info_rec.msg);
  l_offset pls_integer := 1;
  l_bytes_written pls_integer;
begin
  while l_length > 0
  loop
    l_bytes_written := 
      utl_tcp.write_text
      ( c => p_epc_info_rec.tcp_connection
      , data => substr(p_epc_info_rec.msg, l_offset, l_length)
      , len => l_length
      );

    exit when nvl(l_bytes_written, 0) <= 0;

    l_offset := l_offset + l_bytes_written;
    l_length := l_length - l_bytes_written;
  end loop;
end send_request_utl_tcp;

procedure send_request
( p_epc_info_rec in out nocopy epc_info_rectype
)
is
begin
  case p_epc_info_rec.protocol
    when "NATIVE"
    then
      null;

    when "SOAP"
    then
      p_epc_info_rec.msg :=
        epc.SOAP_HEADER_START
        ||'<'
        ||get_method_name(p_epc_info_rec)
        ||' '
        ||get_xmlns(p_epc_info_rec)
        ||'="'
        ||p_epc_info_rec.namespace
        ||'">'
        ||p_epc_info_rec.msg
        ||'</'
        ||get_method_name(p_epc_info_rec)
        ||'>'
        ||epc.SOAP_HEADER_END;
    
    when "XMLRPC"
    then 
      p_epc_info_rec.msg :=
        '<methodCall>'
        ||chr(10)
        ||'<methodName>'
        ||p_epc_info_rec.interface_name||'.'||p_epc_info_rec.method_name
        ||'</methodName>'
        ||chr(10)
        ||'<params>'
        ||chr(10)
        ||p_epc_info_rec.msg
        ||'</params>'
        ||chr(10)
        ||'</methodCall>'
        ||chr(10);

    else
      raise program_error;
  end case;

  case p_epc_info_rec.connection_method
    when CONNECTION_METHOD_DBMS_PIPE
    then
      send_request_dbms_pipe(p_epc_info_rec);

    when CONNECTION_METHOD_UTL_TCP
    then
      case p_epc_info_rec.protocol
        when "XMLRPC"
        then
          send_request_utl_tcp(p_epc_info_rec);

        else
          raise program_error;
      end case;  

    when CONNECTION_METHOD_UTL_HTTP
    then
      case p_epc_info_rec.protocol
        when "SOAP"
        then
          send_request_utl_http
          ( p_epc_info_rec => p_epc_info_rec
          , p_soap_action => p_epc_info_rec.namespace || '#' || p_epc_info_rec.method_name
          );

        when "XMLRPC"
        then
          send_request_utl_http
          ( p_epc_info_rec => p_epc_info_rec
          );

        else
          raise program_error;
      end case;  
  end case;
end send_request;

procedure recv_response_dbms_pipe
( p_epc_info_rec in out nocopy epc_info_rectype
)
is
  l_retval pls_integer := -1;
  l_msg_seq_result pls_integer;
begin
/*DBUG
  enter('epc_clnt.recv_response_dbms_pipe');
/*DBUG*/

/*DBUG
  print('info', 'receiving from %s', g_result_pipe);
/*DBUG*/

  l_retval :=
    dbms_pipe.receive_message
    ( 
      g_result_pipe
    , p_epc_info_rec.recv_timeout
    );

  case l_retval
    when 0 -- Success
    then 
      null;

    when 1 -- Timed out. If the pipe was implicitly-created and is empty, then it is removed.
    then
      raise_application_error
      ( epc.c_comm_error
      , '(epc_clnt.recv_response_dbms_pipe) ' ||
        'Timed out while receiving message number ' ||
        to_char(g_msg_seq) ||
        ' for pipe ' ||
        g_result_pipe ||
        '.'
      );

    when 2 -- Record in the pipe is too large for the buffer. (This should not happen.)
    then
      raise_application_error
      ( epc.c_comm_error
      , '(epc_clnt.recv_response_dbms_pipe) ' ||
        'Message too big while receiving message number ' ||
        to_char(g_msg_seq) ||
        ' for pipe ' ||
        g_result_pipe || '.'
      );

    when 3 -- An interrupt occurred.
    then
      raise_application_error
      ( epc.c_comm_error
      , '(epc_clnt.recv_response_dbms_pipe) ' ||
        'Interrupted while receiving message number ' ||
        to_char(g_msg_seq) ||
        ' for pipe ' ||
        g_result_pipe ||
        '.'
      );

    else -- no more return codes according to the documentation
      raise program_error;
  end case;

  /* Get the message sequence */
  dbms_pipe.unpack_message( l_msg_seq_result );

/*DBUG
  print('info', 'l_msg_seq_result: %s', l_msg_seq_result);
/*DBUG*/

  if l_msg_seq_result = g_msg_seq
  then
    dbms_pipe.unpack_message( p_epc_info_rec.msg );

/*DBUG
    print('info', 'p_epc_info_rec.msg: %s', p_epc_info_rec.msg);
/*DBUG*/

    if p_epc_info_rec.protocol = "NATIVE"
    then
      declare
        l_error_code varchar2(100);
      begin
        get_response_parameter
        ( p_name => 'error_code'
        , p_data_type => epc.data_type_int
        , p_value => l_error_code
        , p_max_bytes => null
        , p_epc_info_rec => p_epc_info_rec
        );

        if l_error_code != '0'
        then
          raise_application_error
          ( epc.c_comm_error
          , '(epc_clnt.recv_response_dbms_pipe) ' ||
            'Server error code "' || l_error_code ||
            '" while receiving message number ' || to_char(g_msg_seq) || '.'
          );
        end if;
      end;
    end if;
  else
    raise_application_error
    ( epc.c_wrong_protocol
    , '(epc_clnt.recv_response_dbms_pipe) ' ||
      'Wrong message number received. ' ||
      'Expected "' || to_char(g_msg_seq) || '"' ||
      ' but received "' || to_char(l_msg_seq_result) || '"' || '.'
    );
  end if;

/*DBUG
  leave;
/*DBUG*/
end recv_response_dbms_pipe;

procedure recv_response_utl_http
( p_epc_info_rec in out nocopy epc_info_rectype
)
is
  http_resp utl_http.resp;
begin
  http_resp :=
    utl_http.get_response(p_epc_info_rec.http_connection.http_req);
  begin
    utl_http.read_text(http_resp, p_epc_info_rec.msg);
    utl_http.end_response(http_resp);
  exception
    when others
    then
/*DBUG
      epc.print(p_epc_info_rec.msg);
/*DBUG*/
      utl_http.end_response(http_resp);
      raise;
  end;
end recv_response_utl_http;

procedure recv_response_utl_tcp
( p_epc_info_rec in out nocopy epc_info_rectype
)
is
  l_bytes_read pls_integer;
begin
  l_bytes_read := 
    utl_tcp.read_text
    ( c => p_epc_info_rec.tcp_connection
    , data => p_epc_info_rec.msg
    , len => 32767
    );
exception
  when utl_tcp.transfer_timeout
  then
    raise;
  when utl_tcp.partial_multibyte_char
  then
    raise;
end recv_response_utl_tcp;

procedure check_soap_fault
( p_doc in out nocopy xmltype
)
as
  l_fault_node   xmltype;
  l_fault_code   varchar2(256);
  l_fault_string varchar2(32767);
begin
  l_fault_node := p_doc.extract('/SOAP-ENV:Fault', epc."xmlns:SOAP-ENV");
  if (l_fault_node is not null) then
    l_fault_code := 
      l_fault_node.extract
      (
        '/SOAP-ENV:Fault/faultcode/child::text()'
      , epc."xmlns:SOAP-ENV"
      ).getstringval();
    l_fault_string := 
      l_fault_node.extract
      (
        '/SOAP-ENV:Fault/faultstring/child::text()'
      , epc."xmlns:SOAP-ENV"
      ).getstringval();
    raise_application_error(epc.c_parse_error, l_fault_code || ' - ' || l_fault_string);
  end if;
end check_soap_fault;

procedure check_xmlrpc_fault
( p_doc in out nocopy xmltype
)
as
  l_fault_node   xmltype;
  l_fault_code   varchar2(256);
  l_fault_string varchar2(32767);
begin
  l_fault_node := p_doc.extract('/methodResponse/fault');
  if (l_fault_node is not null) then
    l_fault_code := 
      l_fault_node.extract
      (
        '/value/struct/member/value/int/child::text()'
      ).getstringval();
    l_fault_string := 
      l_fault_node.extract
      (
        '/value/struct/member/value/string/child::text()'
      ).getstringval();
    raise_application_error(epc.c_parse_error, l_fault_code || ' - ' || l_fault_string);
  end if;
end check_xmlrpc_fault;

procedure recv_response
( p_epc_info_rec in out nocopy epc_info_rectype
)
is
begin
/*DBUG
  enter('epc_clnt.recv_response');
/*DBUG*/

  case p_epc_info_rec.connection_method
    when CONNECTION_METHOD_DBMS_PIPE
    then
      recv_response_dbms_pipe(p_epc_info_rec);

    when CONNECTION_METHOD_UTL_TCP
    then
      recv_response_utl_tcp(p_epc_info_rec);

    when CONNECTION_METHOD_UTL_HTTP
    then
      recv_response_utl_http(p_epc_info_rec);
  end case;

  case p_epc_info_rec.protocol
    when "NATIVE"
    then
      null;

    when "SOAP"
    then
/*DBUG
      epc.print(p_epc_info_rec.msg);
/*DBUG*/
      p_epc_info_rec.doc :=
        xmltype.createxml(p_epc_info_rec.msg).extract
        ( '/SOAP-ENV:Envelope/SOAP-ENV:Body/child::node()'
        , epc."xmlns:SOAP-ENV"
        );
/*DBUG
      epc.print(p_epc_info_rec.doc.getstringval());
/*DBUG*/
      check_soap_fault(p_epc_info_rec.doc);
    
    when "XMLRPC"
    then 
/*DBUG
      epc.print(p_epc_info_rec.msg);
/*DBUG*/
      p_epc_info_rec.doc := xmltype.createxml(p_epc_info_rec.msg);

/*DBUG
      epc.print(p_epc_info_rec.doc.getstringval());
/*DBUG*/
      check_xmlrpc_fault(p_epc_info_rec.doc);

      p_epc_info_rec.next_out_parameter := 1;

    else
      raise program_error;
  end case;

/*DBUG
  print('output', 'p_epc_info_rec.msg: %s', p_epc_info_rec.msg);
  leave;
/*DBUG*/
end recv_response;

procedure get_response_parameter
( p_name in epc.parameter_name_subtype
, p_data_type in epc.data_type_subtype
, p_value out varchar2
, p_max_bytes in integer
, p_epc_info_rec in out nocopy epc_info_rectype
)
is
  l_value epc.string_subtype;
  l_xml XMLType;
  l_extract_type varchar2(100);
begin
/*DBUG
  enter('epc_clnt.get_response_parameter');
  print('input', 'p_name: %s', p_name);
  print('input', 'p_data_type: %s', p_data_type);
  print('input', 'p_max_bytes: %s', p_max_bytes);
/*DBUG*/

  if p_epc_info_rec.protocol = "NATIVE"
  then
    declare
      l_length pls_integer;
      e_wrong_data_type_requested exception;
      pragma exception_init (e_wrong_data_type_requested, -6559);
    begin
/*DBUG
      print('debug', 'p_epc_info_rec.msg: %s', p_epc_info_rec.msg);
/*DBUG*/

      if substr(p_epc_info_rec.msg, 1, 1) = to_char(p_data_type)
      then
        null;
      else
        raise e_wrong_data_type_requested;
      end if;

      case
        when p_data_type in (epc.data_type_xml, epc.data_type_string)
        then
          l_length := to_number(substr(p_epc_info_rec.msg, 2, 4), 'FM000X');
          p_value := substr(p_epc_info_rec.msg, 6, l_length);
          p_epc_info_rec.msg := substr(p_epc_info_rec.msg, 1 + 1 + 4 + l_length);

        else
          l_length := to_number(substr(p_epc_info_rec.msg, 2, 2), 'FM0X');
          p_value := substr(p_epc_info_rec.msg, 4, l_length);
          p_epc_info_rec.msg := substr(p_epc_info_rec.msg, 1 + 1 + 2 + l_length);
      end case;
    end;
  else
    if p_data_type = epc.data_type_xml
    then
      l_extract_type := null; -- 'child::node()';
    else
      l_extract_type := '/child::text()';
    end if;

    case p_epc_info_rec.protocol
      when "SOAP"
      then
        l_xml := 
          p_epc_info_rec.doc.extract
          (
            '//'||p_name||l_extract_type
          , get_xmlns(p_epc_info_rec)||'="'||p_epc_info_rec.namespace||'"'
          );
      
      when "XMLRPC"
      then 
        case p_data_type
          when epc.data_type_xml
          then
            null;
          when epc.data_type_string
          then
            l_extract_type := 'string' || l_extract_type;
          when epc.data_type_int
          then
            l_extract_type := 'int' || l_extract_type;
          when epc.data_type_long
          then
            l_extract_type := 'int' || l_extract_type;
          when epc.data_type_float
          then
            l_extract_type := 'double' || l_extract_type;
          when epc.data_type_double
          then
            l_extract_type := 'double' || l_extract_type;
          when epc.data_type_date
          then
            l_extract_type := 'dateTime.iso8601' || l_extract_type;
          else
            raise value_error;
        end case;

        l_xml := 
          p_epc_info_rec.doc.extract
          (
            '/methodResponse/params/param['
            ||p_epc_info_rec.next_out_parameter
            ||']/value/'
            ||l_extract_type
          );

        p_epc_info_rec.next_out_parameter :=
          p_epc_info_rec.next_out_parameter + 1;

      else
        raise program_error;
    end case;   

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
  end if;

  if p_max_bytes is not null and lengthb(p_value) > p_max_bytes
  then
    raise value_error;
  end if;

/*DBUG
  print('output', 'p_epc_info_rec.msg: %s', p_epc_info_rec.msg);
  print('output', 'p_value: %s', p_value);
  leave;
/*DBUG*/
end get_response_parameter;

procedure get_response_parameter
( p_name in epc.parameter_name_subtype
, p_data_type in epc.data_type_subtype
, p_value out number
, p_epc_info_rec in out nocopy epc_info_rectype
)
is
  l_value epc.string_subtype;
begin
  epc_clnt.get_response_parameter
  ( p_name => p_name
  , p_data_type => p_data_type
  , p_value => l_value
  , p_max_bytes => null
  , p_epc_info_rec => p_epc_info_rec
  );
  p_value := to_number(replace(l_value, '.', g_decimal_char));
end get_response_parameter;

procedure get_response_parameter
( p_name in epc.parameter_name_subtype
, p_data_type in epc.data_type_subtype
, p_value out date
, p_epc_info_rec in out nocopy epc_info_rectype
)
is
  l_value epc.string_subtype;
begin
  epc_clnt.get_response_parameter
  ( p_name => p_name
  , p_data_type => p_data_type
  , p_value => l_value
  , p_max_bytes => null
  , p_epc_info_rec => p_epc_info_rec
  );
  p_value := to_date(replace(l_value, 'T'), 'yyyymmddhh24:mi:ss');
end get_response_parameter;

-- GLOBAL
function register
( p_interface_name in epc.interface_name_subtype
)
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

function get_epc_key
( p_interface_name in epc.interface_name_subtype
)
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

procedure set_protocol
( p_epc_key in epc_key_subtype
, p_protocol in protocol_subtype
)
is
begin
  if p_protocol in ( "NATIVE", "SOAP", "XMLRPC" )
  then
    epc_info_tab(p_epc_key).protocol := p_protocol;
  else
    raise value_error;
  end if;
end set_protocol;

procedure get_protocol
( p_epc_key in epc_key_subtype
, p_protocol out protocol_subtype
)
is
begin
  p_protocol := epc_info_tab(p_epc_key).protocol;
end get_protocol;

procedure set_connection_info
( p_epc_key in epc_key_subtype
, p_connection in http_connection_subtype
)
is
begin
  epc_info_tab(p_epc_key).connection_method := CONNECTION_METHOD_UTL_HTTP;
  epc_info_tab(p_epc_key).http_connection := p_connection;
  epc_info_tab(p_epc_key).protocol := epc_clnt."SOAP";
end set_connection_info;

procedure get_connection_info
( p_epc_key in epc_key_subtype
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
( p_epc_key in epc_key_subtype
, p_connection in utl_tcp.connection
)
is
begin
  epc_info_tab(p_epc_key).connection_method := CONNECTION_METHOD_UTL_TCP;
  epc_info_tab(p_epc_key).tcp_connection := p_connection;
  epc_info_tab(p_epc_key).protocol := epc_clnt."XMLRPC";
end set_connection_info;

procedure get_connection_info
( p_epc_key in epc_key_subtype
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
( p_epc_key in epc_key_subtype
, p_pipe_name in epc.pipe_name_subtype
)
is
begin
  epc_info_tab(p_epc_key).connection_method := CONNECTION_METHOD_DBMS_PIPE;
  epc_info_tab(p_epc_key).request_pipe := p_pipe_name;
  epc_info_tab(p_epc_key).protocol := epc_clnt."NATIVE";
end set_connection_info;

procedure get_connection_info
( p_epc_key in epc_key_subtype
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
( p_epc_key in epc_key_subtype
, p_request_send_timeout in pls_integer
)
is
begin
  epc_info_tab(p_epc_key).send_timeout := p_request_send_timeout;
end set_request_send_timeout;

procedure set_response_recv_timeout
( p_epc_key in epc_key_subtype
, p_response_recv_timeout in pls_integer
)
is
begin
  epc_info_tab(p_epc_key).recv_timeout := p_response_recv_timeout;
end set_response_recv_timeout;

procedure set_namespace
( p_epc_key in epc_key_subtype
, p_namespace in varchar2
)
is
begin
  epc_info_tab(p_epc_key).namespace := p_namespace;
end set_namespace;

procedure set_inline_namespace
( p_epc_key in epc_key_subtype
, p_inline_namespace in varchar2
)
is
begin
  epc_info_tab(p_epc_key).inline_namespace := p_inline_namespace;
end set_inline_namespace;

procedure new_request
( p_epc_key in epc_key_subtype
, p_method_name in epc.method_name_subtype
, p_oneway in pls_integer
)
is
begin
  new_request
  ( p_method_name
  , p_oneway
  , epc_info_tab(p_epc_key)
  );
end new_request;

procedure set_request_parameter
( p_epc_key in epc_key_subtype
, p_name in epc.parameter_name_subtype
, p_data_type in epc.data_type_subtype
, p_value in varchar2
, p_max_bytes in integer
)
is
begin
  set_request_parameter
  ( p_name
  , p_data_type
  , p_value
  , p_max_bytes
  , epc_info_tab(p_epc_key)
  );
end set_request_parameter;

procedure set_request_parameter
( p_epc_key in epc_key_subtype
, p_name in epc.parameter_name_subtype
, p_data_type in epc.data_type_subtype
, p_value in number
)
is
begin
  set_request_parameter
  ( p_name
  , p_data_type
  , p_value
  , epc_info_tab(p_epc_key)
  );
end set_request_parameter;

procedure set_request_parameter
( p_epc_key in epc_key_subtype
, p_name in epc.parameter_name_subtype
, p_data_type in epc.data_type_subtype
, p_value in date
)
is
begin
  set_request_parameter
  ( p_name
  , p_data_type
  , p_value
  , epc_info_tab(p_epc_key)
  );
end set_request_parameter;

procedure send_request
( p_epc_key in epc_key_subtype
)
is
begin
  send_request(epc_info_tab(p_epc_key));
end send_request;

procedure recv_response
( p_epc_key in epc_key_subtype
)
is
begin
  recv_response(epc_info_tab(p_epc_key));
end recv_response;

procedure get_response_parameter
( p_epc_key in epc_key_subtype
, p_name in epc.parameter_name_subtype
, p_data_type in epc.data_type_subtype
, p_value out varchar2
, p_max_bytes in integer
)
is
begin
 get_response_parameter
 ( p_name
 , p_data_type
 , p_value
 , p_max_bytes
 , epc_info_tab(p_epc_key)
 );
end get_response_parameter;

procedure get_response_parameter
( p_epc_key in epc_key_subtype
, p_name in epc.parameter_name_subtype
, p_data_type in epc.data_type_subtype
, p_value out number
)
is
begin
  get_response_parameter
  ( p_name
  , p_data_type
  , p_value
  , epc_info_tab(p_epc_key)
  );
end get_response_parameter;

procedure get_response_parameter
( p_epc_key in epc_key_subtype
, p_name in epc.parameter_name_subtype
, p_data_type in epc.data_type_subtype
, p_value out date
)
is
begin
  get_response_parameter
  ( p_name
  , p_data_type
  , p_value
  , epc_info_tab(p_epc_key)
  );
end get_response_parameter;

function get_response_pipe
(
  p_epc_key in epc_key_subtype
)
return epc.pipe_name_subtype
is
begin
  return g_result_pipe;
end get_response_pipe;

procedure shutdown
is
begin
  -- either 0 is returned or an exception (ORA-23322)
  if dbms_pipe.remove_pipe(g_result_pipe) = 0
  then
    null;
  end if;
end shutdown;

begin
  select  substr(value, 1, 1) as decimal_char
  into    g_decimal_char
  from    nls_session_parameters
  where   parameter = 'NLS_NUMERIC_CHARACTERS';

  g_result_pipe := 'EPC$' || dbms_pipe.unique_session_name;

  /* 
  || GJP 08-01-2001 
  || Emptying the result pipe seems to prevent timeouts on receipt. 
  */

  dbms_pipe.purge( g_result_pipe );
end epc_clnt;
/

show errors
