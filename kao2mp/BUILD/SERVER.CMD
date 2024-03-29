@ECHO OFF

ECHO ****************
ECHO * SERVER.EXE   *
ECHO ****************

:: Assert output directories exist

IF NOT EXIST OUT\ MD OUT
IF NOT EXIST OUT\OBJ\ MD OUT\OBJ
IF NOT EXIST OUT\OBJ\SERVER MD OUT\OBJ\SERVER

:: Compile "SERVER\SERVER.C"

CL.EXE /nologo ^
    /W4 /WX /GS- /Oi %_COMPILER_FLAGS% ^
    ^
    /I"SOURCE" ^
    /c /TC "SOURCE\SERVER\SERVER.C" ^
    /Fo:"OUT\OBJ\SERVER\SERVER.OBJ"

IF ERRORLEVEL 1 EXIT /B 1

:: Compile "SERVER\SERVER2.C"

CL.EXE /nologo ^
    /W4 /WX /GS- /Oi %_COMPILER_FLAGS% ^
    ^
    /I"SOURCE" ^
    /c /TC "SOURCE\SERVER\SERVER2.C" ^
    /Fo:"OUT\OBJ\SERVER\SERVER2.OBJ"

IF ERRORLEVEL 1 EXIT /B 1

:: Translate "SERVER.RC"

RC.EXE /nologo ^
    /fo "OUT\OBJ\SERVER\SERVER.RES" ^
    "RESOURCE\SERVER.RC"

IF ERRORLEVEL 1 EXIT /B 1

:: Link "SERVER.EXE"

LINK.EXE /NOLOGO ^
    /EMITTOOLVERSIONINFO:NO /EMITPOGOPHASEINFO /MANIFEST:NO ^
    /STUB:"OUT\STUBS\MZ16STUB.EXE" ^
    /SUBSYSTEM:CONSOLE %_LINKER_FLAGS% ^
    /NODEFAULTLIB /ENTRY:EntryPoint ^
    "KERNEL32.LIB" "USER32.LIB" "WS2_32.LIB" ^
    ^
    "OUT\OBJ\SERVER\SERVER.OBJ" ^
    "OUT\OBJ\SERVER\SERVER2.OBJ" ^
    "OUT\OBJ\SERVER\SERVER.RES" ^
    ^
    /OUT:"OUT\SERVER.EXE"

IF ERRORLEVEL 1 EXIT /B 1

EXIT /B 0
