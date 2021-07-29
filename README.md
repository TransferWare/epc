# External Procedure Call toolkit

This is EPC, the External Procedure Call toolkit.

It is used to:
- provide an Oracle PL/SQL library as foundation for [PLSDBUG, a PL/SQL debugging library](https://github.com/TransferWare/plsdbug)
- invoke external procedures from Oracle PL/SQL

EPC itself consists of:
1. a PL/SQL library to be installed in the database
2. the C library (-lepc)
3. an IDL compiler
4. C headers

When you are interested in the first item only, just follow the instructions
in [DATABASE INSTALL](#database-install).

## DATABASE INSTALL

This section explains how to install just the PL/SQL library as a foundation for PLSDBUG.

There are two methods:
1. use the [Oracle Tools GUI](https://github.com/paulissoft/oracle-tools-gui)
with the pom.xml file from the project root and schema ORACLE_TOOLS as the owner
2. execute `src/sql/install.sql` connected as the owner using SQL*Plus, SQLcl or SQL Developer

The advantage of the first method is that you the installation is tracked and
that you can upgrade later on.

## INSTALL FROM SOURCE

Also called the MAINTAINER BUILD. You just need the sources either cloned from [EPC on GitHub](https://github.com/TransferWare/epc) or from a source archive.

You need a Unix shell which is available on Mac OS X, Linux and Unix of course.
On Windows you can use the Windows Subsystem for Linux (WSL), Cygwin or Git Bash.

You need the following programs:
- automake
- autoconf
- libtool (on a Mac OS X glibtool)

Next the following command will generate the Autotools `configure` script:

```
$ ./bootstrap
```

## INSTALL

The EPC toolkit depends on the package [DBUG, a C debugging
library](https://github.com/TransferWare/dbug). Both packages should be
installed into the same lib and bin directories (e.g. use the same prefix when
installing).

This section explains how to install the complete toolkit (including the PL/SQL library).

#### Preconditions

First install:
- Oracle SQL*Plus (executable name sqlplus)
- Oracle PRO*C (executable name proc)

You can use the [Oracle Instant Client Downloads](https://www.oracle.com/database/technologies/instant-client/downloads.html).

The correct procedure is to:
1. first download and install (unzip) the newest Basic (Light) Package
2. next download and install (unzip) the newest SQL*Plus Package
3. next download and install (unzip) the newest Instant Client Package - Precompiler

For the Mac OS X the precompiler package has not the same version as the other packages but that is not a problem. Just add the installation directories to your PATH.

The EPC has the following modes of communication:
- SOAP (via HTTP, i.e. package SYS.UTL_HTTP)
- XMLRPC (via TCP/IP, i.e. package SYS.UTL_HTTP)
- NATIVE (database pipes, i.e. package SYS.DBMS_PIPE)

You may need to grant (as SYS) those SYS packages to the owner.

The EPC can thus process XML messages and for that you need the Oracle XML C
SDK. Starting from Oracle 11 this is included in the database
installation. For 10 and earlier download this from OTN and install it into
`$ORACLE_HOME/xdk`.

### Configure

Here you need either a distribution archive with the `configure` script or you must have bootstrapped your environment.

In order to have an out-of-source build create a `build` directory first and configure from that directory:

```
$ mkdir build
$ cd build
$ ../configure
```

An important feature is enabling/disabling server interrupts. Because
the EPC server waits forever for a message using
`dbms_pipe.receive_message`, it may be difficult to stop the server
gracefully. The `--enable-server-interrupt` configure option enables the
EPC server to be killed by an interrupt (CTRL-C). The default is to
enable this, but this means that a second Oracle session is started
which sends an empty message into the request pipe so the EPC server can
exit. This can be disabled by `--disable-server-interrupt`.

### Build

See file `INSTALL` for further installation instructions. Do not forget to set environment variable USERID as the Oracle connect string.

## DOCUMENTATION

To generate the C and SQL documentation you need to install:
- Doxygen
- PLDoc

### Doxygen

On the Mac OS X:
```
$ brew install doxygen
```

### PLDoc

You should:
- download the ZIP bundle from [the PLDoc project](http://pldoc.sourceforge.net/maven-site/downloads.html)
- unzip the bundle to an installation directory
- add the installation directory to your PATH

The PLDoc documentation says that you need to set an environment variable ORACLE_HOME to point to an Oracle home having the Jar file `ojdbc6.jar`. Setting ORACLE_HOME can be tricky and that is why I propose to do it only when generating the documentation.

### Generate the documentation

Issue this to generate the documentation:

```
$ cd build
$ ORACLE_HOME='<directory containing ojdbc6.jar>'
$ ../configure # if you did (re-)install one of those two programs.
$ SCHEMA='<Oracle owner schema>' make doc # or make
```

In the build directory you will find these files now:
- [C documentation](doc/c/index.html)
- [EPC manual](doc/epcman.html)
- [SQL documentation](doc/sql/index.html)
- [How to empty database pipes](utils/empty_pipes.html)
