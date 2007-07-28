REM See also http://www.xmlrpc.com/currentTime

set serveroutput on size 1000000 format truncated
declare
  http_connection epc_clnt.http_connection_subtype;
  l_epc_key constant epc_clnt.epc_key_subtype := 
    epc_clnt.register('currentTime');
  l_date date;
begin
  http_connection.url := 'http://time.xmlrpc.com/RPC2';
  epc_clnt.set_connection_info(l_epc_key, http_connection);
  epc_clnt.set_protocol(l_epc_key, epc_clnt."XMLRPC");
  l_date := currentTime.getCurrentTime;
  dbms_output.put_line('current time: '||to_char(l_date, 'yyyy-mm-dd hh24:mi:ss'));
end;
/
