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

EXEC :num := str.strlen( :str10 );

PRINT num

EXEC :num := str.strlen( :str20 );

PRINT num

REM Should raise an exception
EXEC :num := str.strlen( :str30 );

PRINT num

EXEC str.strcpy1( :str30, :str10 );

PRINT str30

PRINT str10

REM Should raise an exception
EXEC str.strcpy1( :str10, :str30 );

PRINT str10

REM Should raise an exception since '' is equal to NULL
EXEC str.strcat( :str100, :str10 ); 

PRINT str100

EXEC :str100 := ' '; str.strcat( :str100, :str20 );

PRINT str100

SET TIMING ON

UNDEFINE N

PROMPT Performance test doing &&N number of strcpy1 calls
BEGIN
        FOR     v_nr IN 1..&&N
	LOOP
		str.strcpy1( :str30, :str10 );
	END LOOP;
END;
/

PROMPT Performance test doing &&N number of strcpy2 calls
BEGIN
        FOR     v_nr IN 1..&&N
	LOOP
		str.strcpy2( :str30, :str10 );
	END LOOP;
END;
/
