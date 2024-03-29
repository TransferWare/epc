set serveroutput on size 1000000 format trunc
set linesize 10000 trimspool on
set feedback off
set verify off

define N = &&1
define PROTOCOL = &&2

whenever sqlerror exit failure
whenever oserror exit failure

alter session set nls_numeric_characters = '.,';

variable l_interface_name varchar2(30)

execute :l_interface_name := 'epctest';

REM Just start nothing1 to set interface defaults, change the protocol and set the pipe size
declare
  l_pipe_name epc.pipe_name_subtype;
begin
  epctest.nothing1;
  epc_clnt.set_protocol(:l_interface_name, epc_clnt."&&PROTOCOL");
  epc_clnt.get_connection_info(:l_interface_name, l_pipe_name);
  -- enlarge the pipe
  if 0 = 
     dbms_pipe.create_pipe
     ( pipename => l_pipe_name
     , maxpipesize => 1000000
     , private => false
     )
  then
    null;
  end if;
end;
.

rem list

/

spool epctest.lis

declare
  v_par1  varchar2(4) := 'dhsd';
  v_par2  varchar2(12) := '1234';
  v_par3  varchar2(31);
  v_result varchar2(4);
begin
  dbms_output.put_line( 'proc01' );
  dbms_output.put_line( 'i_par1: ' || v_par1 );
  dbms_output.put_line( 'io_par2: ' || v_par2 );

  v_result := epctest.proc01( v_par1, v_par2, v_par3 );
  dbms_output.put_line( 'result: ' || v_result );
  dbms_output.put_line( 'io_par2: ' || v_par2 );
  dbms_output.put_line( 'o_par3: ' || v_par3 );
end;
.

rem list

/

declare
  v_par1  integer := 99;
  v_par2  integer;
  v_par3  integer := 1;
  v_result integer;
begin
  dbms_output.put_line( 'proc02' );
  dbms_output.put_line( 'io_par1: ' || v_par1 );
  dbms_output.put_line( 'i_par3: ' || v_par3 );

  v_result := epctest.proc02( v_par1, v_par2, v_par3 );
  dbms_output.put_line( 'result: ' || v_result );
  dbms_output.put_line( 'io_par1: ' || v_par1 );
  dbms_output.put_line( 'o_par2: ' || v_par2 );
end;
.

rem list

/

declare
  v_par1  number;
  v_par2  number := 10.111;
  v_par3  number := 99.09;
  v_result number;
begin
  dbms_output.put_line( 'proc03' );
  dbms_output.put_line( 'i_par2: ' || v_par2 );
  dbms_output.put_line( 'io_par3: ' || v_par3 );

  v_result := epctest.proc03( v_par1, v_par2, v_par3 );
  dbms_output.put_line( 'result: ' || v_result );
  dbms_output.put_line( 'o_par1: ' || v_par1 );
  dbms_output.put_line( 'io_par3: ' || v_par3 );
end;
.

rem list

/

declare
  v_par1  number := 232367.44;
  v_par2  number := 1234.99999;
  v_par3  number;
  v_result number;
begin
  dbms_output.put_line( 'proc04' );
  dbms_output.put_line( 'i_par1: ' || v_par1 );
  dbms_output.put_line( 'io_par2: ' || v_par2 );

  v_result := epctest.proc04( v_par1, v_par2, v_par3 );
  dbms_output.put_line( 'result: ' || to_char(v_result, '99.99') );
  dbms_output.put_line( 'io_par2: ' || to_char(v_par2, '99.99') );
  dbms_output.put_line( 'o_par3: ' || to_char(v_par3, '99.99') );
end;
.

rem list

/

prompt Performance test doing a null block

declare
  l_epc_clnt_object epc_clnt_object;
  l_recv_timeout constant integer := 10;
begin
  -- clean up local storage
  std_object_mgr.delete_std_objects;

  -- Store object into PL/SQL table
  epc_clnt.set_response_recv_timeout(:l_interface_name, l_recv_timeout);

  -- Retrieve from PL/SQL table
  l_epc_clnt_object := new epc_clnt_object(:l_interface_name);

  if l_epc_clnt_object.recv_timeout = l_recv_timeout
  then
    null;
  else
    raise value_error;
  end if;
end;
.

rem list

/

define function = nothing2

prompt Performance test doing &&N number of calls doing nothing without results returned.
declare
  l_count pls_integer := 0;
  l_line varchar2(255);
  l_status integer;
begin 
  for v_nr in 1..&&n
  loop
  begin
    epctest.&&function;
    l_count := l_count + 1;
  exception
    when epc.e_comm_error
    then
      epctest.&&function;
      l_count := l_count + 1;
    when others
    then
      dbms_output.put_line(substr(sqlerrm, 1, 255));
  end;
  end loop;
  dbms_output.put_line('&&function count: ' || l_count);
exception
  when others
  then
    dbms_output.put_line('Current date/time: ' || to_char(sysdate, 'yyyy-mm-dd hh24:mi:ss'));   
    raise;
end;
.

rem list

/

define function = nothing1

prompt Performance test doing &&N number of calls doing nothing with results returned.

/

execute std_object_mgr.delete_std_objects

prompt Finished.
spool off

execute epc_clnt.shutdown
