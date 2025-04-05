@echo off
set SERVNAME=LhwmLoader
reg add "HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Services\EventLog\Application\%SERVNAME%" /v "EventMessageFile" /t REG_SZ /d "%~dp0\EventLogMessages.dll"
reg add "HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Services\EventLog\Application\%SERVNAME%" /v "TypesSupported" /t REG_DWORD /d 7
sc create "%SERVNAME%" binPath= "%~dp0\%SERVNAME%Service.exe" start= delayed-auto
sc start "%SERVNAME%"
pause
