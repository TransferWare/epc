# Changelog

Copyright (C) 1997-2022 G.J. Paulissen 


All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

Please see the [EPC issue queue](https://github.com/TransferWare/epc/issues) for issues.

Types of changes:
- *Added* for new features.
- *Changed* for changes in existing functionality.
- *Deprecated* for soon-to-be removed features.
- *Removed* for now removed features.
- *Fixed* for any bug fixes.
- *Security* in case of vulnerabilities.

[Unreleased]

## [7.0.0] - 2023-04-27

### Added

- [It must be possible to create a private request pipe with a custom maximum pipe size.](https://github.com/TransferWare/epc/issues/10)
- [Add the ability to only output C or PL/SQL code.](https://github.com/TransferWare/epc/issues/9)
- [Add debugging to EPC_CLNT when no message can be sent for a database pipe.](https://github.com/TransferWare/epc/issues/8)

## [6.1.0] - 2021-08-11

### Fixed

- [On Mac OS X you should disable shared libraries.](https://github.com/TransferWare/epc/issues/2)
- [On Mac OS X libraries or executables with @rpath pose problems while testing.](https://github.com/TransferWare/epc/issues/3)
- [The configure script must set ORACLE_LIBS and ORACLE_LDFLAGS instead of LIBS and LDFLAGS.](https://github.com/TransferWare/epc/issues/4)
- [When PostgreSQL sqlca is installed in /usr/local/include it overrides Oracle's sqlca.](https://github.com/TransferWare/epc/issues/5)

## [6.0.0] - 2021-08-02

A new version on GitHub.

### Added

- README.md describes up to date installation instructions
- CHANGELOG.md describes releases found on [EPC releases](https://sourceforge.net/projects/transferware/files/epc/) and in files Changelog and NEWS

### Changed

- README now refers to README.md
- ChangeLog now refers to CHANGELOG.md
- NEWS now refers to CHANGELOG.md
- Removed RCS keywords like $Revision$, $HeadURL$, $Header$, $Id$ and $RCSfile$ from source files
- Corrected small errors in CHANGELOG.md like Copyright
- src/sql SQL scripts have been created using the DDL generation of [Oracle Tools](https://github.com/paulissoft/oracle-tools)
- Build on Mac OS X works now
- When no XML communication is used, the respective C sources are not processed
- [On Mac OS X you should disable shared libraries](https://github.com/TransferWare/epc/issues/2)
- [The configure script sets ORACLE_LIBS and ORACLE_LDFLAGS instead of LIBS and LDFLAGS](https://github.com/TransferWare/epc/issues/4)
- [On Mac OS X libraries or executables with @rpath pose problems while testing](https://github.com/TransferWare/epc/issues/3)
- Word files in doc folder converted to HTML
- make distcheck works now

## [5.2.0] -  2018-08-19

### Changed

- Improved distribution support

## [5.1.2] - 2014-08-11

### Added

- Added support for Cygwin 1.7.x

### Changed

- The configure script adapted for AIX builds
- src/lib/epc_lib.c and src/lib/epc_xml.c adapted for AIX

## [5.1.1] -  2008-07-28

### Added

- Added epc.debug method to debug the EPC and related software

### Changed

- Renamed src/sql/install.sql into src/sql/epc_install.sql to avoid naming conflicts with PLSDBUG
- Renamed src/sql/uninstall.sql into src/sql/epc_uninstall.sql to avoid naming conflicts with PLSDBUG
- Renamed src/sql/verify.sql into src/sql/epc_verify.sql to avoid naming conflicts with PLSDBUG

## [5.1.0] -  2008-06-20

### Added

- Added support for stateless applications like Apex (Oracle Application Express). See src/sql/std_object_mgr.pks

## [5.0.0] - 2007-08-21

### Added

- XMLRPC protocol
- native dbms_pipe protocol without the overhead of XMLRPC and SOAP
- The epc_clnt package has a new procedure shutdown for cleaning up resources (response database pipes)
- The feature server-interrupt has been added to configure. See the README
- The manual describes how to recover from EPC communication errors (epc.e_comm_error)

### Changed

- The procedure empty_pipes (util/empty_pipes.prc) now removes pipes after emptying them
- The IDL compiler now generates both C and PRO*C files with the same functionality. Only one of them is needed to compile your application
- The installation directories from the DBUG package are used
- On Solaris the math library (-lm) is added to $LIBS (if unset)
- Package dbug can be used or not (--without-dbug)
- Backwards compatibility for subtypes and exceptions which are part of the EPC package specification
- The EPC communication error message can be retrieved via SQLERRM instead of DBMS_OUTPUT.GET_LINE(S)
- Several portability issues solved for Solaris (rpt_malloc, strtof)
- The first (shared) library to try for xmlinit is now -lnmemso, which solved linking problems on the Mac OS X, because libxml10.a is a static library
- The EPC library has been checked for allocation errors using dmalloc (http://dmalloc.com/)
- Generated PL/SQL packages now have bounds checking enabled for strings
- EPC exceptions have been mapped to error numbers

## [4.5.0]

### Added

- Added support for Oracle 10g
- Documentation is generated using pldoc and doxygen

### Changed

- Testing enhanced

## [4.4.0]

### Added

- Amazon Web services example
- XML IDL type added

### Changed

- Documentation updated

## [4.3.0]

### Added

- Web services implemented
- Support for HTTP added

### Changed

- Documentation updated

## [4.2.0]

### Added 

- SOAP message layout implemented
- Support for TCP/IP added

### Changed

- epc_* functions renamed to epc__*
- Support enhanced for several interfaces running at the same time using a different server

## [4.0.0] -  2004-03-15

### Changed

- Solved request id 849475: configure script problem
- Solved request id 901781: CTRL-C kills EPC listener but not the session
- Solved request id 915081: The EPC idl compiler fails during build of the demo
- LGPL license applied instead of GPL license

### Removed

- imake support obsolete

## [3.0.0] - 2003-08-21

### Added

- GNU Programming Standards enforced (ChangeLog, NEWS, etc.)
- Implement GNU build system

### Changed

- Build EPC in separate build environment
- make dist for imake and Autoconf separated
- Solved request id 699638: Empty connect string dumps core
- Solved request id 692220: public synonyms ts_dbug_send/ts_dbug_recv missing

## [1.0.0] - 1997-06-25

For historical reasons, this is the date of the first EPC release.

