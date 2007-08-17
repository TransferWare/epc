set serveroutput on size 1000000 format trunc
set linesize 1000 trimspool on

variable what varchar2(4)

execute :what := '&1';

declare
  g_epc_key constant epc_clnt.epc_key_subtype := epc_clnt.register('epctest');

  i_par1 epc.string_subtype := 'dhsd';
  io_par2 epc.string_subtype := '1234';
  o_par3 epc.string_subtype;
  l_result varchar2(4 byte);

  l_msg_info varchar2(32767);
  l_msg_request varchar2(32767);
begin
  for i_nr in 1..4
  loop
    if instr(:what, to_char(i_nr)) > 0
    then
      case i_nr
        when 1
        then
          dbms_pipe.purge('EPC_REQUEST_PIPE');
          dbms_output.put_line('send request');
          epc_clnt.new_request(g_epc_key, 'proc01', 0);
          epc_clnt.set_request_parameter(g_epc_key, 'i_par1', epc.data_type_string, i_par1, 4);
          epc_clnt.set_request_parameter(g_epc_key, 'io_par2', epc.data_type_string, io_par2, 12);
          epc_clnt.send_request(g_epc_key);

        when 2
        then
          dbms_output.put_line('receive request');
          epc_srvr.recv_request
          ( epc_srvr.register
          , l_msg_info
          , l_msg_request
          );
          dbms_output.put_line('msg info: '||l_msg_info);
          dbms_output.put_line('msg request: '||l_msg_request);

        when 3
        then
          dbms_output.put_line('send response');
          epc_srvr.send_response
          ( epc_srvr.register
          , l_msg_info
          , epc.data_type_int || '01' || to_char(0) ||
            epc.data_type_string || '000C' || 'hjfgeljaujfd' ||
            epc.data_type_string || '001F' || 'dfgfhahfmkghjfvsbvfhjfgeljaujfd' ||
            epc.data_type_string || '0004' || 'abcd'
          );

        when 4
        then
          dbms_output.put_line('receive response');
          epc_clnt.recv_response(g_epc_key);
          epc_clnt.get_response_parameter(g_epc_key, 'io_par2', epc.data_type_string, io_par2, 12);
          epc_clnt.get_response_parameter(g_epc_key, 'o_par3', epc.data_type_string, o_par3, 31);
          epc_clnt.get_response_parameter(g_epc_key, 'result', epc.data_type_string, l_result);

          dbms_output.put_line('io_par2: '||io_par2);
          dbms_output.put_line('o_par3: '||o_par3);
          dbms_output.put_line('result: '||l_result);
      end case;   
    end if;
  end loop;
end;
/
