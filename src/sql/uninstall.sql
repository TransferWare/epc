/* perl generate_ddl.pl (version 2022-12-02) --nodynamic-sql --force-view --group-constraints --skip-install-sql --source-schema=EPCAPP --strip-source-schema */

/*
-- JDBC url            : jdbc:oracle:thin:EPCAPP@//localhost:1521/orcl
-- source schema       : 
-- source database link: 
-- target schema       : EPCAPP
-- target database link: 
-- object type         : 
-- object names include: 1
-- object names        : EPC_CLNT_OBJECT,
EPC,
EPC_CLNT,
EPC_SRVR,
-- skip repeatables    : 0
-- interface           : pkg_ddl_util v5
-- transform params    : 
-- owner               : ORACLE_TOOLS
*/
-- pkg_ddl_util v5
call dbms_application_info.set_module('uninstall.sql', null);
/* SQL statement 1 (DROP;EPCAPP;PACKAGE_BODY;EPC_CLNT;;;;;;;;2) */
call dbms_application_info.set_action('SQL statement 1');
DROP PACKAGE BODY EPC_CLNT;

/* SQL statement 2 (DROP;EPCAPP;PACKAGE_BODY;EPC_SRVR;;;;;;;;2) */
call dbms_application_info.set_action('SQL statement 2');
DROP PACKAGE BODY EPC_SRVR;

/* SQL statement 3 (DROP;EPCAPP;TYPE_BODY;EPC_CLNT_OBJECT;;;;;;;;2) */
call dbms_application_info.set_action('SQL statement 3');
DROP TYPE BODY EPC_CLNT_OBJECT;

/* SQL statement 4 (DROP;EPCAPP;PACKAGE_SPEC;EPC_CLNT;;;;;;;;2) */
call dbms_application_info.set_action('SQL statement 4');
DROP PACKAGE EPC_CLNT;

/* SQL statement 5 (DROP;EPCAPP;PACKAGE_SPEC;EPC_SRVR;;;;;;;;2) */
call dbms_application_info.set_action('SQL statement 5');
DROP PACKAGE EPC_SRVR;

/* SQL statement 6 (DROP;EPCAPP;PACKAGE_SPEC;EPC;;;;;;;;2) */
call dbms_application_info.set_action('SQL statement 6');
DROP PACKAGE EPC;

/* SQL statement 7 (DROP;EPCAPP;TYPE_SPEC;EPC_CLNT_OBJECT;;;;;;;;2) */
call dbms_application_info.set_action('SQL statement 7');
DROP TYPE EPC_CLNT_OBJECT;

