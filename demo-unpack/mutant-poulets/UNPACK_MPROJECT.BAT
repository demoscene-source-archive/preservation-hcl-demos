@ECHO OFF
"%~dp0hcl_unpack.exe" "%~dp0MPROJECT.DAT" "%~dp0MPROJECT"
SET "HCL_RESULT=%ERRORLEVEL%"

ECHO.
IF NOT "%HCL_RESULT%"=="0" ECHO Extraction failed with exit code %HCL_RESULT%.
PAUSE
EXIT /B %HCL_RESULT%
