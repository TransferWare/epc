/* perl generate_ddl.pl (version 2021-08-27) --nodynamic-sql --force-view --skip-install-sql --nostrip-source-schema */

/*
-- JDBC url            : jdbc:oracle:thin:ORACLE_TOOLS@//localhost:1521/orcl
-- source schema       : 
-- source database link: 
-- target schema       : ORACLE_TOOLS
-- target database link: 
-- object type         : 
-- object names include: 1
-- object names        : EPC,
EPC_CLNT,
EPC_CLNT_OBJECT,
EPC_SRVR,
STD_OBJECT,
STD_OBJECT_MGR,
STD_OBJECTS
-- skip repeatables    : 0
-- interface           : pkg_ddl_util v5
-- transform params    : 
-- owner               : ORACLE_TOOLS
*/
-- pkg_ddl_util v5
call dbms_application_info.set_module('uninstall.sql', null);
/* SQL statement 1 (ALTER;ORACLE_TOOLS;CONSTRAINT;STD_OBJECTS_CHK4;ORACLE_TOOLS;TABLE;STD_OBJECTS;;;;;2) */
call dbms_application_info.set_action('SQL statement 1');
ALTER TABLE "ORACLE_TOOLS"."STD_OBJECTS" DROP CONSTRAINT STD_OBJECTS_CHK4;

/* SQL statement 2 (ALTER;ORACLE_TOOLS;CONSTRAINT;STD_OBJECTS_CHK3;ORACLE_TOOLS;TABLE;STD_OBJECTS;;;;;2) */
call dbms_application_info.set_action('SQL statement 2');
ALTER TABLE "ORACLE_TOOLS"."STD_OBJECTS" DROP CONSTRAINT STD_OBJECTS_CHK3;

/* SQL statement 3 (ALTER;ORACLE_TOOLS;CONSTRAINT;STD_OBJECTS_CHK2;ORACLE_TOOLS;TABLE;STD_OBJECTS;;;;;2) */
call dbms_application_info.set_action('SQL statement 3');
ALTER TABLE "ORACLE_TOOLS"."STD_OBJECTS" DROP CONSTRAINT STD_OBJECTS_CHK2;

/* SQL statement 4 (ALTER;ORACLE_TOOLS;CONSTRAINT;STD_OBJECTS_CHK1;ORACLE_TOOLS;TABLE;STD_OBJECTS;;;;;2) */
call dbms_application_info.set_action('SQL statement 4');
ALTER TABLE "ORACLE_TOOLS"."STD_OBJECTS" DROP CONSTRAINT STD_OBJECTS_CHK1;

/* SQL statement 5 (ALTER;ORACLE_TOOLS;CONSTRAINT;STD_OBJECTS_PK;ORACLE_TOOLS;TABLE;STD_OBJECTS;;;;;2) */
call dbms_application_info.set_action('SQL statement 5');
ALTER TABLE "ORACLE_TOOLS"."STD_OBJECTS" DROP PRIMARY KEY KEEP INDEX;

/* SQL statement 6 (DROP;ORACLE_TOOLS;INDEX;STD_OBJECTS_PK;ORACLE_TOOLS;TABLE;STD_OBJECTS;;;;;2) */
call dbms_application_info.set_action('SQL statement 6');
DROP INDEX ORACLE_TOOLS.STD_OBJECTS_PK;

/* SQL statement 7 (DROP;ORACLE_TOOLS;PACKAGE_BODY;EPC;;;;;;;;2) */
call dbms_application_info.set_action('SQL statement 7');
DROP PACKAGE BODY ORACLE_TOOLS.EPC;

/* SQL statement 8 (DROP;ORACLE_TOOLS;PACKAGE_BODY;EPC_CLNT;;;;;;;;2) */
call dbms_application_info.set_action('SQL statement 8');
DROP PACKAGE BODY ORACLE_TOOLS.EPC_CLNT;

/* SQL statement 9 (DROP;ORACLE_TOOLS;PACKAGE_BODY;EPC_SRVR;;;;;;;;2) */
call dbms_application_info.set_action('SQL statement 9');
DROP PACKAGE BODY ORACLE_TOOLS.EPC_SRVR;

/* SQL statement 10 (DROP;ORACLE_TOOLS;PACKAGE_BODY;STD_OBJECT_MGR;;;;;;;;2) */
call dbms_application_info.set_action('SQL statement 10');
DROP PACKAGE BODY ORACLE_TOOLS.STD_OBJECT_MGR;

/* SQL statement 11 (DROP;ORACLE_TOOLS;TYPE_BODY;EPC_CLNT_OBJECT;;;;;;;;2) */
call dbms_application_info.set_action('SQL statement 11');
DROP TYPE BODY ORACLE_TOOLS.EPC_CLNT_OBJECT;

/* SQL statement 12 (DROP;ORACLE_TOOLS;PACKAGE_SPEC;EPC_CLNT;;;;;;;;2) */
call dbms_application_info.set_action('SQL statement 12');
DROP PACKAGE ORACLE_TOOLS.EPC_CLNT;

/* SQL statement 13 (DROP;ORACLE_TOOLS;PACKAGE_SPEC;EPC_SRVR;;;;;;;;2) */
call dbms_application_info.set_action('SQL statement 13');
DROP PACKAGE ORACLE_TOOLS.EPC_SRVR;

/* SQL statement 14 (DROP;ORACLE_TOOLS;PACKAGE_SPEC;EPC;;;;;;;;2) */
call dbms_application_info.set_action('SQL statement 14');
DROP PACKAGE ORACLE_TOOLS.EPC;

/* SQL statement 15 (DROP;ORACLE_TOOLS;TYPE_BODY;STD_OBJECT;;;;;;;;2) */
call dbms_application_info.set_action('SQL statement 15');
DROP TYPE BODY ORACLE_TOOLS.STD_OBJECT;

/* SQL statement 16 (DROP;ORACLE_TOOLS;PACKAGE_SPEC;STD_OBJECT_MGR;;;;;;;;2) */
call dbms_application_info.set_action('SQL statement 16');
DROP PACKAGE ORACLE_TOOLS.STD_OBJECT_MGR;

/* SQL statement 17 (DROP;ORACLE_TOOLS;TABLE;STD_OBJECTS;;;;;;;;2) */
call dbms_application_info.set_action('SQL statement 17');
DROP TABLE ORACLE_TOOLS.STD_OBJECTS PURGE;

/* SQL statement 18 (DROP;ORACLE_TOOLS;TYPE_SPEC;EPC_CLNT_OBJECT;;;;;;;;2) */
call dbms_application_info.set_action('SQL statement 18');
DROP TYPE ORACLE_TOOLS.EPC_CLNT_OBJECT;

/* SQL statement 19 (DROP;ORACLE_TOOLS;TYPE_SPEC;STD_OBJECT;;;;;;;;2) */
call dbms_application_info.set_action('SQL statement 19');
DROP TYPE ORACLE_TOOLS.STD_OBJECT;

