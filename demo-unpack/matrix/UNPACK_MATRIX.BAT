@ECHO OFF
powershell.exe -NoProfile -ExecutionPolicy Bypass -File "%~dp0UNPACK_MATRIX.ps1"
SET "HCL_RESULT=%ERRORLEVEL%"

ECHO.
IF NOT "%HCL_RESULT%"=="0" ECHO Extraction failed with exit code %HCL_RESULT%.
PAUSE
EXIT /B %HCL_RESULT%
