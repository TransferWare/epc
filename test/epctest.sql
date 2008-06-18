set serveroutput on size 1000000 format trunc
set linesize 10000 trimspool on
set feedback off
set verify off

define N = &&1
define PROTOCOL = &&2

whenever sqlerror exit failure
whenever oserror exit failure

alter session set nls_numeric_characters = '.,';

REM Just start nothing1 to set interface defaults, change the protocol and set the pipe size
declare
  l_pipe_name epc.pipe_name_subtype;
  l_epc_clnt_object epc_clnt_object;
begin
  epctest.nothing1;
  epc_clnt.get_epc_clnt_object(l_epc_clnt_object, 'epctest');
  epc_clnt.set_protocol(l_epc_clnt_object, epc_clnt."&&PROTOCOL");
  epc_clnt.get_connection_info(l_epc_clnt_object, l_pipe_name);
  epc_clnt.set_epc_clnt_object(l_epc_clnt_object, 'epctest');
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
/

prompt Performance test doing a null block

variable l_group_name varchar2(100)

execute :l_group_name := 'epctest.sql';

declare
  l_epc_clnt_object epc_clnt_object;
  l_recv_timeout constant integer := 10;
begin
  -- Store object into PL/SQL table
  std_object_mgr.set_group_name(null);
  epc_clnt.get_epc_clnt_object(l_epc_clnt_object, 'epctest');
  epc_clnt.set_response_recv_timeout(l_epc_clnt_object, l_recv_timeout);
  epc_clnt.set_epc_clnt_object(l_epc_clnt_object, 'epctest');

  -- Store object into table std_objects
  std_object_mgr.set_group_name(:l_group_name);
  l_epc_clnt_object.recv_timeout := l_epc_clnt_object.recv_timeout * 2;
  epc_clnt.set_epc_clnt_object(l_epc_clnt_object, 'epctest');

  -- Retrieve from PL/SQL table
  std_object_mgr.set_group_name(null);
  epc_clnt.get_epc_clnt_object(l_epc_clnt_object, 'epctest');

  if l_epc_clnt_object.recv_timeout = l_recv_timeout
  then
    null;
  else
    raise value_error;
  end if;

  -- Retrieve from table std_objects
  std_object_mgr.set_group_name(:l_group_name);
  epc_clnt.get_epc_clnt_object(l_epc_clnt_object, 'epctest');

  if l_epc_clnt_object.recv_timeout = l_recv_timeout * 2
  then
    null;
  else
    raise value_error;
  end if;
end;
/

define function = nothing2

prompt Performance test doing &&N number of calls doing nothing without results returned.
DECLARE
  l_count pls_integer := 0;
  l_line varchar2(255);
  l_status integer;
BEGIN 
  FOR v_nr IN 1..&&N
  LOOP
  BEGIN
    epctest.&&function;
    l_count := l_count + 1;
  EXCEPTION
    WHEN epc.e_comm_error
    THEN
      epctest.&&function;
      l_count := l_count + 1;
    WHEN OTHERS
    THEN
      dbms_output.put_line(substr(sqlerrm, 1, 255));
  END;
  END LOOP;
  dbms_output.put_line('&&function count: ' || l_count);
EXCEPTION
  WHEN OTHERS
  THEN
    dbms_output.put_line('Current date/time: ' || to_char(sysdate, 'yyyy-mm-dd hh24:mi:ss'));   
    RAISE;
END;
/

define function = nothing1

prompt Performance test doing &&N number of calls doing nothing with results returned.

/

execute std_object_mgr.delete_std_objects(p_group_name => :l_group_name)

prompt Finished.
spool off

execute epc_clnt.shutdown
