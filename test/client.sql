set serveroutput on size 1000000
set verify off

declare
  v_request_pipe varchar2(128) := epc.get_request_pipe;
  c_result_pipe constant varchar2(128) := dbms_pipe.unique_session_name;
  c_msg_protocol constant integer := 3;
  v_send_wait_time pls_integer := 10;
  v_receive_wait_time pls_integer := 10;

  retval pls_integer;

  send_error exception;
  recv_error exception;
  exec_error exception;
  sql_error exception;
  wrong_protocol exception;

  procedure request_set_header
  (
    i_interface in varchar2
  , i_routine_name in varchar2
  , i_oneway in pls_integer
  )
  is
  begin
    dbms_pipe.reset_buffer;
    dbms_pipe.pack_message( c_msg_protocol );
    dbms_pipe.pack_message( i_interface );
    dbms_pipe.pack_message( i_routine_name );
    if i_oneway = 0
    then
      dbms_pipe.pack_message( c_result_pipe );
    end if;
  end request_set_header;

  procedure request_perform_routine( i_oneway in binary_integer default 0 )
  is
    v_result binary_integer;
    exec_status binary_integer;
    sqlcode binary_integer;
  begin
    dbms_pipe.purge( c_result_pipe );

    <<retry_loop>>
    loop
      begin
        retval := dbms_pipe.send_message( v_request_pipe, v_send_wait_time );

        if retval != 0
        then
          raise send_error;
        end if;

        if i_oneway = 0
        then
          retval := dbms_pipe.receive_message( c_result_pipe, v_receive_wait_time );
          if retval != 0
          then
            raise recv_error;
          end if;

          /* Get the execution status */
          dbms_pipe.unpack_message( exec_status );
          if exec_status < 0
          then
            raise exec_error;
          end if;

          /* get the sql code */
          dbms_pipe.unpack_message( sqlcode );
          if sqlcode = 0
          then
            null;
          else
            raise sql_error;
          end if;
        end if;

        exit retry_loop; /* OK */
      exception
        when recv_error
        then
          dbms_output.put_line( 'recv_error' );
          dbms_pipe.purge( c_result_pipe );
        when send_error
        then
          dbms_output.put_line( 'send_error' );
          null;
        when sql_error
        then
          dbms_output.put_line( '(epc.request_perform_routine) ' ||
                                'sqlcode: ' || to_char(sqlcode) );
          raise exec_error;
      end;

      exit retry_loop; /* temporarily */
    end loop retry_loop;

  end request_perform_routine;

  function proc01(
    i_par1 in varchar2,
    io_par2 in out varchar2,
    o_par3 out varchar2 )
  return varchar2
  is
    result varchar2(2000);
  begin
    request_set_header( 'epctest', 'string proc01( [out] string i_par1, [out] string io_par2, [out] string o_par3 )', 0 );
    dbms_pipe.pack_message( i_par1 );
    dbms_pipe.pack_message( io_par2 );
/*
    request_perform_routine( 1 );
*/
    request_perform_routine( 0 );
    dbms_pipe.unpack_message( io_par2 );
    dbms_pipe.unpack_message( o_par3 );
    dbms_pipe.unpack_message( result );
    RETURN result;
  end proc01;

  function proc02(
    io_par1 in out binary_integer,
    o_par2 out binary_integer,
    i_par3 in binary_integer )
  return binary_integer
  is
    result BINARY_INTEGER;
  BEGIN
    request_set_header( 'epctest', 'int proc02( [out] int io_par1, [out] int o_par2, [out] int i_par3 )', 0 );
    dbms_pipe.pack_message( io_par1 );
    dbms_pipe.pack_message( i_par3 );
/*
    request_perform_routine( 1 );
*/
    request_perform_routine( 0 );
    dbms_pipe.unpack_message( io_par1 );
    dbms_pipe.unpack_message( o_par2 );
    dbms_pipe.unpack_message( result );
    RETURN result;
  end proc02;

  function proc03(
    o_par1 out double precision,
    i_par2 in double precision,
    io_par3 in out double precision )
  return double precision
  is
    result DOUBLE PRECISION;
  BEGIN
    request_set_header( 'epctest', 'double proc03( [out] double o_par1, [out] double i_par2, [out] double io_par3 )', 0 );
    dbms_pipe.pack_message( i_par2 );
    dbms_pipe.pack_message( io_par3 );
/*
    request_perform_routine( 1 );
*/
    request_perform_routine( 0 );
    dbms_pipe.unpack_message( o_par1 );
    dbms_pipe.unpack_message( io_par3 );
    dbms_pipe.unpack_message( result );
    RETURN result;
  end proc03;

  function proc04( 
    i_par1 in float,
    io_par2 in out float,
    o_par3 out float )
  return float
  is
    result FLOAT;
  BEGIN
    request_set_header( 'epctest', 'float proc04( [out] float i_par1, [out] float io_par2, [out] float o_par3 )', 0 );
    dbms_pipe.pack_message( i_par1 );
    dbms_pipe.pack_message( io_par2 );
/*
    request_perform_routine( 1 );
*/
    request_perform_routine( 0 );
    dbms_pipe.unpack_message( io_par2 );
    dbms_pipe.unpack_message( o_par3 );
    dbms_pipe.unpack_message( result );
    RETURN result;
  end proc04;

  procedure nothing1
  is
  begin
    request_set_header( 'epctest', 'void nothing1', 0 );
/*    request_perform_routine( 1 );*/
    request_perform_routine( 0 );
  end nothing1;

  procedure nothing2
  is
  begin
    request_set_header( 'epctest', 'oneway void nothing2', 1 );
    request_perform_routine( 1 );
  end nothing2;

begin
  for v_nr in 1..&&1
  loop
    if mod(v_nr, 1000) = 0 
    then
      dbms_output.put_line( 'round: ' || to_char(v_nr) );
    end if;

    declare
      v_par1  varchar2(100) := 'dhsd';
      v_par2  varchar2(200) := '1234';
      v_par3  varchar2(300);
      v_result varchar2(10);
    begin
      dbms_output.put_line( 'proc01' );
      dbms_output.put_line( 'i_par1: ' || v_par1 );
      dbms_output.put_line( 'io_par2: ' || v_par2 );

      v_result := proc01( v_par1, v_par2, v_par3 );
      dbms_output.put_line( 'result: ' || v_result );
      dbms_output.put_line( 'io_par2: ' || v_par2 );
      dbms_output.put_line( 'o_par3: ' || v_par3 );
    end;

    declare
      v_par1  integer := 99;
      v_par2  integer;
      v_par3  integer := 1;
      v_result integer;
    begin
      dbms_output.put_line( 'proc02' );
      dbms_output.put_line( 'io_par1: ' || v_par1 );
      dbms_output.put_line( 'i_par3: ' || v_par3 );

      v_result := proc02( v_par1, v_par2, v_par3 );
      dbms_output.put_line( 'result: ' || v_result );
      dbms_output.put_line( 'io_par1: ' || v_par1 );
      dbms_output.put_line( 'o_par2: ' || v_par2 );
    end;

    declare
      v_par1  number;
      v_par2  number := 10.111;
      v_par3  number := 99.09;
      v_result number;
    begin
      dbms_output.put_line( 'proc03' );
      dbms_output.put_line( 'i_par2: ' || v_par2 );
      dbms_output.put_line( 'io_par3: ' || v_par3 );

      v_result := proc03( v_par1, v_par2, v_par3 );
      dbms_output.put_line( 'result: ' || v_result );
      dbms_output.put_line( 'o_par1: ' || v_par1 );
      dbms_output.put_line( 'io_par3: ' || v_par3 );
    end;

    declare
      v_par1  number := 232367.44;
      v_par2  number := 1234.99999;
      v_par3  number;
      v_result number;
    begin
      dbms_output.put_line( 'proc04' );
      dbms_output.put_line( 'i_par1: ' || v_par1 );
      dbms_output.put_line( 'io_par2: ' || v_par2 );

      v_result := proc04( v_par1, v_par2, v_par3 );
      dbms_output.put_line( 'result: ' || v_result );
      dbms_output.put_line( 'io_par2: ' || v_par2 );
      dbms_output.put_line( 'o_par3: ' || v_par3 );
    end;

    nothing1;

    nothing2;
  end loop;

end;
/