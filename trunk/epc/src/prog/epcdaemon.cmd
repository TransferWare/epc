@echo off
set perl_script=%~d0%~p0%~n0.pl
perl %perl_script% %*
