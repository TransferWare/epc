# External Procedure Call toolkit

This is 'EPC', the External Procedure Call toolkit.

It is used to:
1. provide a PL/SQL library as foundation for [PLSDBUG, a PL/SQL debugging library](https://github.com/TransferWare/plsdbug)
2. invoke external procedures from Oracle PL/SQL

When you are interested in the first usage only, just follow the instructions
in the [first section of INSTALLATION](#installation).

EPC itself consists of:
- the library, libepc.la
- an IDL compiler
- the headers installed in '/usr/local/include'
- a PL/SQL library to be installed in the database

The EPC toolkit depends on the package
[DBUG, a C debugging library](https://github.com/TransferWare/dbug). This should be installed into
the same lib and bin directories as EPC (e.g. use the same prefix when
installing).

## DOCUMENTATION

- doc/c/index.html
- doc/epcman.doc
- doc/sql/index.html
- utils/empty_pipes.html

## INSTALLATION

### Just the PL/SQL library as foundation for PLSDBUG

This section explains how to install just the PL/SQL library as a foundation for PLSDBUG.

There are two methods:
1. use the [Oracle Tools GUI](https://github.com/paulissoft/oracle-tools-gui)
with the pom.xml file from the project root and schema ORACLE_TOOLS as the owner
2. execute src/sql/install.sql connected as the owner using SQL*Plus, SQLcl or SQL Developer

The advantage of the first method is that you the installation is tracked and
that you can upgrade later on.

### Everything

This section explains how to install the complete toolkit (including the PL/SQL library).

### Oracle stuff

First install:
- Oracle SQL*Plus (executable name sqlplus)
- Oracle PRO*C (executable name proc)

The most easy way to do this is to install the Oracle database (for
instance an XE version) and then the corresponding PRO*C compiler.

The following command will give you the Oracle version: sqlplus -V

This version number can be used to find the [precompiler download](https://www.oracle.com/database/technologies/instant-client/precompiler-112010-downloads.html).

Now install the proc precompiler in the same directory as the sqlplus
executable. Ensure the headers are installed in ../include.

The EPC has the following modes of communication:
- SOAP (via HTTP, i.e. SYS.UTL_HTTP)
- XMLRPC (via TCP/IP, i.e. SYS.UTL_HTTP)
- NATIVE (database pipes, I.E. SYS.DBMS_PIPE)

You may need to grant (as SYS) those packages to the owner.

The EPC can thus process XML messages and for that you need the Oracle XML C
SDK. Starting from Oracle 11 this is included in the database
installation. For 10 and earlier download this from OTN and install it into
$ORACLE_HOME/xdk.

An important feature is enabling/disabling server interrupts. Because
the EPC server waits forever for a message using
dbms_pipe.receive_message, it may be difficult to stop the server
gracefully. The --enable-server-interrupt configure option enables the
EPC server to be killed by an interrupt (CTRL-C). The default is to
enable this, but this means that a second Oracle session is started
which sends an empty message into the request pipe so the EPC server can
exit. This can be disabled by --disable-server-interrupt.

### GNU stuff

Installation using the GNU build tools is described in INSTALL. 

#### Windows specific

For Windows platforms the Cygwin suite with GCC and the MinGW extension
can be used. There is one caveat: the libraries should not depend on the
Cygwin DLL, but on the Microsoft run-time DLLs instead. See the file
BUGS in the DBUG distribution for more information.

#### Solaris specific

On SUN Solaris version 9 or higher the library libm has to be added to
the list of libraries, because the Oracle XML library may need
it. 

The configure script will try to set the environment correctly. In case
that fails the following configure command may be tried:

```
$ ./configure LIBS="-lm"
```

## BUILD

First follow the BUILD instructions in the DBUG README.

Next (creating a build directory to separate generated and source files):

```shell
$ mkdir build
$ cd build
$ ../configure
$ make
```

## DIST

Follow instructions from [BUILD](#build).

Next:

```shell
$ make distcheck
$ make dist
```

