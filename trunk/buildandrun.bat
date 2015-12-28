@echo OFF

make
if %ERRORLEVEL% NEQ 0 GOTO BAD
falcon
GOTO DONE
:BAD
pause
:DONE