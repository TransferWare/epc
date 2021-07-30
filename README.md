# External Procedure Call toolkit

This is EPC, the External Procedure Call toolkit.

It is used to:
- provide an Oracle PL/SQL library as foundation for [PLSDBUG, a PL/SQL debugging library](https://github.com/TransferWare/plsdbug)
- invoke external procedures written in C from Oracle PL/SQL

EPC itself consists of:
1. a PL/SQL library to be installed in the database
2. the C library (-lepc) and headers
3. an interface definition language (IDL) compiler to parse interface descriptions

Follow these installation steps:

| Step | When |
| :--- | :--- |
| [DATABASE INSTALL](#database-install) | Always |
| [INSTALL FROM SOURCE](#install-from-source) | When you want the install the rest (not the database) from source |
| [INSTALL](#install) | When you want the install the rest and you have a `configure` script |

## DATABASE INSTALL

This section explains how to install the PL/SQL library as a foundation for PLSDBUG.

### Preconditions

You need Oracle SQL*Plus (executable name sqlplus). Test whether it exist by showing the version:

```
$ sqlplus -V
```

You can download and install the SQL*Plus package from the [Oracle Instant Client Downloads web site](https://www.oracle.com/database/technologies/instant-client/downloads.html).

For the Mac OS X use DMG files if available otherwise you will have to use the Security & Privacy panel in the System Preferences to accept opening executables and shared libraries which is quite annoying.

The correct procedure is to download and install (unzip) the newest Basic (Light) Package and SQL*Plus Package into the same instantclient directory. The versions should be the same.

In the end add the instantclient directory `<instantclient directory>` to your executable path environment variable PATH and shared library path environment variable (DYLD_LIBRARY_PATH on Mac OS X, LD_LIBRARY_PATH for the rest).

Test whether sqlplus exists now.

### Installation

There are two methods:
1. use the [Oracle Tools GUI](https://github.com/paulissoft/oracle-tools-gui)
with the pom.xml file from the project root and schema ORACLE_TOOLS as the EPC owner
2. execute `src/sql/install.sql` connected as the EPC owner using SQL*Plus, SQLcl or SQL Developer

The advantage of the first method is that you the installation is tracked and
that you can upgrade later on.

The username (Oracle account) must have been granted the following 
system privileges:
- create session
- create table
- create procedure
- create type
 
The username (Oracle account) must have been granted the following
object privileges:
- execute on sys.dbms_pipe
- execute on sys.utl_http
- execute on sys.utl_tcp

The username (Oracle account) must have acces to a data tablespace, for instance USERS.

An example:
SQL> alter user <EPC owner> default tablespace users;
SQL> alter user <EPC owner> quota unlimited on users;

## INSTALL FROM SOURCE

The Autotools community calls this a MAINTAINER BUILD. You just need the sources either cloned from
[EPC on GitHub](https://github.com/TransferWare/epc) or from a source archive.

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

This section explains how to install everything except the database.

### Preconditions

You need Oracle PRO*C (executable name proc). Test whether it exist by:

```
$ proc
```

You can download and install it from the [Oracle Instant Client Downloads web site](https://www.oracle.com/database/technologies/instant-client/downloads.html).

For the Mac OS X use DMG files if available.

The correct procedure is to download and install (unzip) the newest Instant Client Package - Precompiler.

For the Mac OS X the precompiler package has not the same version as the SQL*Plus packages but that is not a problem **AS LONG YOU USE THE SAME INSTANTCLIENT DIRECTORY** for all instantclient packages.

In the end add the instantclient directory `<instantclient directory>/sdk` to your PATH. Test whether proc exists now.

### Configure

Here you need either a distribution archive with the `configure` script or you must have bootstrapped your environment.

An important feature is enabling/disabling server interrupts. Because
the EPC server waits forever for a message using
`dbms_pipe.receive_message`, it may be difficult to stop the server
gracefully. The `--enable-server-interrupt` configure option enables the
EPC server to be killed by an interrupt (CTRL-C). The default is to
enable this, but this means that a second Oracle session is started
which sends an empty message into the request pipe so the EPC server can
exit. This can be disabled by `--disable-server-interrupt`.

On the Mac OS X there are problems with creating shared libraries so you
should disable it by setting the `--disable-shared` configure line option.

To get help:

```
$ ./configure --help
```

### Build

See file `INSTALL` for further installation instructions. Do not forget to set USERID (a connect string) as specified in that file.

You can set USERID for connecting to your local orcl database by:

```
$ export USERID=<EPC owner>/<EPC password>@//localhost:1521/orcl
```

or using TNS:

```
$ export USERID=<EPC owner>/<EPC password>@orcl
```

## DOCUMENTATION

To generate the C and SQL documentation you need to install:
1. Doxygen
2. PLDoc

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

Issue this to generate the documentation (you need to set variable SCHEMA as well but you can do that in front of make):

```
$ ORACLE_HOME='<directory containing ojdbc6.jar>'
$ ./configure # if you did (re-)install one of those two programs.
$ SCHEMA='<EPC owner>' make doc
```

You will find these files now:
- [doc/epcman.html](doc/epcman.html), the EPC manual 
- [doc/EPC-optimize.html](doc/EPC-optimize.html), an article in the dutch magazine Optimize about the EPC
- [doc/c/index.html](doc/c/index.html), the C reference documentation
- [doc/sql/index.html](doc/sql/index.html), the SQL reference documentation
- [utils/empty_pipes.html](utils/empty_pipes.html), an HOWTO about how to empty database pipes

You can also have a look at [the EPC GitHub Pages](https://TransferWare.github.io/epc/).
