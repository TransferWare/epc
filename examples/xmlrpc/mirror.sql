REM See also http://www.xmlrpc.com/currentTime

set serveroutput on size 1000000 format truncated
declare
  http_connection epc_clnt.http_connection_subtype;
  l_epc_key constant epc_clnt.epc_key_subtype := 
    epc_clnt.register('mirror');
  l_xml epc.xml_subtype;
begin
  http_connection.url := 'http://www.mirrorproject.com/xmlrpc/';
  epc_clnt.set_connection_info(l_epc_key, http_connection);
  epc_clnt.set_protocol(l_epc_key, epc_clnt."XMLRPC");
  l_xml := mirror.Random;
  epc.print(l_xml);
end;
/
