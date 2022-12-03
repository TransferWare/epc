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
-- skip repeatables    : 0
-- interface           : pkg_ddl_util v5
-- transform params    : 
-- owner               : ORACLE_TOOLS
*/
-- pkg_ddl_util v5
call dbms_application_info.set_module('uninstall.sql', null);

/* SQL statement 7 (DROP;ORACLE_TOOLS;PACKAGE_BODY;EPC;;;;;;;;2) */
call dbms_application_info.set_action('SQL statement 7');
DROP PACKAGE BODY EPC;

/* SQL statement 8 (DROP;ORACLE_TOOLS;PACKAGE_BODY;EPC_CLNT;;;;;;;;2) */
call dbms_application_info.set_action('SQL statement 8');
DROP PACKAGE BODY EPC_CLNT;

/* SQL statement 9 (DROP;ORACLE_TOOLS;PACKAGE_BODY;EPC_SRVR;;;;;;;;2) */
call dbms_application_info.set_action('SQL statement 9');
DROP PACKAGE BODY EPC_SRVR;

/* SQL statement 11 (DROP;ORACLE_TOOLS;TYPE_BODY;EPC_CLNT_OBJECT;;;;;;;;2) */
call dbms_application_info.set_action('SQL statement 11');
DROP TYPE BODY EPC_CLNT_OBJECT;

/* SQL statement 12 (DROP;ORACLE_TOOLS;PACKAGE_SPEC;EPC_CLNT;;;;;;;;2) */
call dbms_application_info.set_action('SQL statement 12');
DROP PACKAGE EPC_CLNT;

/* SQL statement 13 (DROP;ORACLE_TOOLS;PACKAGE_SPEC;EPC_SRVR;;;;;;;;2) */
call dbms_application_info.set_action('SQL statement 13');
DROP PACKAGE EPC_SRVR;

/* SQL statement 14 (DROP;ORACLE_TOOLS;PACKAGE_SPEC;EPC;;;;;;;;2) */
call dbms_application_info.set_action('SQL statement 14');
DROP PACKAGE EPC;

/* SQL statement 18 (DROP;ORACLE_TOOLS;TYPE_SPEC;EPC_CLNT_OBJECT;;;;;;;;2) */
call dbms_application_info.set_action('SQL statement 18');
DROP TYPE EPC_CLNT_OBJECT;


