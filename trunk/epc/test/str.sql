SET DEFINE ON

PROMPT 1 - Number of times for a strcpy
PROMPT &&1

DEFINE NR = &&1

SET FEEDBACK OFF VERIFY OFF 

SET SERVEROUTPUT ON

VAR str10 VARCHAR2(10);
VAR str20 VARCHAR2(20);
VAR str30 VARCHAR2(30);
VAR str100 VARCHAR2(100);
VAR num NUMBER;

BEGIN
  :str10 := '0123456789';
  :str20 := '0123456789ABCDEFGHIJ';
  :str30 := '987654321098765432109876543210';
  :str100 := '';
END;
/

EXECUTE :num := str.strlen( :str10 );

PROMPT Test 1
PRINT num

EXECUTE :num := str.strlen( :str20 );

PROMPT Test 2
PRINT num

REM Should raise an exception (epc.exec_error)
BEGIN
  :num := str.strlen( :str30 );
EXCEPTION
  WHEN epc.exec_error
  THEN
    NULL;
END;
/

PROMPT Test 3
PRINT num

EXECUTE str.strcpy1( :str30, :str10 );

PROMPT Test 4
PRINT str30

PROMPT Test 5
PRINT str10

REM Should raise an exception (epc.exec_error)
BEGIN
  str.strcpy1( :str10, :str30 );
EXCEPTION
  WHEN epc.exec_error
  THEN
    NULL;
END;
/

PROMPT Test 6
PRINT str10

REM Should raise an exception (epc.exec_error) since '' is equal to NULL
BEGIN
  str.strcat( :str100, :str10 );
EXCEPTION
  WHEN epc.illegal_null_value
  THEN
    NULL;
END;
/

PROMPT Test 7
PRINT str100

REM Should raise an exception (epc.exec_error) since '' is equal to NULL
BEGIN
  :str100 := ' ';
  str.strcat( :str100, :str20 );
END;
/

PROMPT Test 8
PRINT str100
PRINT str20

SET TIMING ON

UNDEFINE N

PROMPT Performance test doing &&NR number of strcpy1 calls
BEGIN
        FOR     v_nr IN 1..&&NR
        LOOP
                str.strcpy1( :str30, :str10 );
        END LOOP;
END;
/

PROMPT Performance test doing &&NR number of strcpy2 calls
BEGIN
        FOR     v_nr IN 1..&&NR
        LOOP
                str.strcpy2( :str30, :str10 );
        END LOOP;
END;
/

SET TIMING OFF

UNDEFINE 1
UNDEFINE NR
SET FEEDBACK ON VERIFY ON
