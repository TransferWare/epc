set serveroutput on size 1000000
set feedback off
set trimspool on

declare
	v_par1	varchar2(100) := 'dhsd';
	v_par2	varchar2(200) := '1234';
	v_par3	varchar2(300);
	v_result varchar2(10);
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
	v_par1	integer := 99;
	v_par2	integer;
	v_par3	integer := 1;
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
	v_par1	number;
	v_par2	number := 10.111;
	v_par3	number := 99.09;
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
	v_par1	number := 232367.44;
	v_par2	number := 1234.99999;
	v_par3	number;
	v_result number;
begin
	dbms_output.put_line( 'proc04' );
	dbms_output.put_line( 'i_par1: ' || v_par1 );
	dbms_output.put_line( 'io_par2: ' || v_par2 );

	v_result := epctest.proc04( v_par1, v_par2, v_par3 );
	dbms_output.put_line( 'result: ' || v_result );
	dbms_output.put_line( 'io_par2: ' || v_par2 );
	dbms_output.put_line( 'o_par3: ' || v_par3 );
end;
/

prompt Performance test doing a null block

set timing on
BEGIN
	NULL;
END;
/

prompt Performance test doing N number of calls doing nothing
BEGIN
	FOR	v_nr IN 1..&1
	LOOP
		epctest.nothing;
	END LOOP;
END;
/

