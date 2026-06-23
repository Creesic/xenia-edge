@ECHO OFF
SETLOCAL

SET "ROOT=%~dp0"
IF "%ROOT:~-1%"=="\" SET "ROOT=%ROOT:~0,-1%"
CD /D "%ROOT%"
SET "BUILD_DIR=%ROOT%\build-cl"
SET "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
SET "VCVARSALL="

CALL :check_python
IF %_RESULT% NEQ 0 (
  ECHO.
  ECHO Python 3.9+ must be installed and on PATH:
  ECHO https://www.python.org/
  CALL :build_fail 1
)

CALL :find_vcvarsall
IF "%VCVARSALL%"=="" (
  ECHO ERROR: Unable to find Visual Studio Build Tools with vcvarsall.bat.
  CALL :build_fail 1
)

ECHO [1/4] Downloading Slang if needed...
CALL "%ROOT%\xb.bat" slang
IF ERRORLEVEL 1 CALL :build_fail %ERRORLEVEL%

ECHO [2/4] Configuring MSVC build...
CALL "%VCVARSALL%" x64
IF ERRORLEVEL 1 CALL :build_fail %ERRORLEVEL%

cmake -S "%ROOT%" -B "%BUILD_DIR%" -G "Ninja Multi-Config" -DCMAKE_C_COMPILER=cl -DCMAKE_CXX_COMPILER=cl -DXENIA_BUILD_TESTS=OFF
IF ERRORLEVEL 1 CALL :build_fail %ERRORLEVEL%

ECHO [3/4] Generating version header...
"%PYTHON_EXE%" -c "import runpy; m=runpy.run_path(r'%ROOT%\xenia-build.py'); m['generate_version_h'](r'%BUILD_DIR%')"
IF ERRORLEVEL 1 CALL :build_fail %ERRORLEVEL%

ECHO [4/4] Building Release executable...
cmake --build "%BUILD_DIR%" --config Release
IF ERRORLEVEL 1 CALL :build_fail %ERRORLEVEL%

ECHO.
ECHO Build complete:
ECHO %BUILD_DIR%\bin\Windows\Release\xenia_edge.exe
EXIT /b 0

:find_vcvarsall
IF EXIST "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvarsall.bat" (
  SET "VCVARSALL=C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvarsall.bat"
  GOTO :eof
)
IF EXIST "C:\Program Files\Microsoft Visual Studio\18\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" (
  SET "VCVARSALL=C:\Program Files\Microsoft Visual Studio\18\BuildTools\VC\Auxiliary\Build\vcvarsall.bat"
  GOTO :eof
)
IF EXIST "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" (
  SET "VCVARSALL=C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat"
  GOTO :eof
)
IF EXIST "C:\Program Files\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" (
  SET "VCVARSALL=C:\Program Files\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvarsall.bat"
  GOTO :eof
)

IF EXIST "%VSWHERE%" (
  FOR /F "usebackq delims=" %%I IN (`"%VSWHERE%" -version [18.0^,19.0^) -latest -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -find VC\Auxiliary\Build\vcvarsall.bat`) DO (
    SET "VCVARSALL=%%I"
  )
)
GOTO :eof

:check_python
SETLOCAL ENABLEDELAYEDEXPANSION

SET FOUND_PATH=""
SET "CANDIDATE_PATHS[0]=%WINDIR%\py.exe"
SET OUTPUT_INDEX=1

FOR /F "usebackq delims=" %%L IN (`2^>NUL where python`) DO (
  IF %%~zL NEQ 0 (
    SET "CANDIDATE_PATHS[!OUTPUT_INDEX!]=%%L"
    SET /A OUTPUT_INDEX+=1
  )
)
FOR /F "usebackq delims=" %%L IN (`2^>NUL where python3`) DO (
  IF %%~zL NEQ 0 (
    SET "CANDIDATE_PATHS[!OUTPUT_INDEX!]=%%L"
    SET /A OUTPUT_INDEX+=1
  )
)

SET CANDIDATE_INDEX=0
:check_candidate_loop
IF NOT DEFINED CANDIDATE_PATHS[%CANDIDATE_INDEX%] (
  GOTO :found_python
)
CALL SET CANDIDATE_PATH=%%CANDIDATE_PATHS[%CANDIDATE_INDEX%]%%
IF NOT EXIST "%CANDIDATE_PATH%" (
  SET /A CANDIDATE_INDEX+=1
  GOTO :check_candidate_loop
)
SET "FOUND_PATH=%CANDIDATE_PATH%"

:found_python
IF "%FOUND_PATH%"=="" (
  ENDLOCAL & SET _RESULT=1
  GOTO :eof
)

ENDLOCAL & (
  SET _RESULT=0
  SET "PYTHON_EXE=%FOUND_PATH%"
)
GOTO :eof

:build_fail
ECHO.
ECHO *** BUILD FAILED (exit code %~1) ***
PAUSE
EXIT /b %~1
