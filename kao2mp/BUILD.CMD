@ECHO OFF
CLS
PROMPT $P$_$G$S

:: Load "x64_x86 Cross Tools Command Prompt for VS 2022"
::
:: Microsoft (R) C/C++ Optimizing Compiler Version 19.38.33135 for x86
:: Microsoft (R) Incremental Linker Version 14.38.33135.0

WHERE /Q CL.EXE || (
    CALL "%ProgramFiles%\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsamd64_x86.bat"
)

:: Set build flags

IF /I "%~1"=="/R" (
    SET "_BUILD_NAME=^(RELEASE^)"
    SET "_COMPILER_FLAGS=/O2 "
    SET "_LINKER_FLAGS=/OPT:REF /DEBUG:NONE"
) ELSE IF /I "%~1"=="/D" (
    SET "_BUILD_NAME=^(DEBUG^)  "
    SET "_COMPILER_FLAGS=/Od /Zi /D_DEBUG"
    SET "_LINKER_FLAGS=/DEBUG:FULL /MAP"
) ELSE (
    ECHO * Please specify build type:
    ECHO  ^> BUILD.CMD /R  :: RELEASE
    ECHO  ^> BUILD.CMD /D  :: DEBUG
    EXIT /B 1
)

ECHO ************************************************
ECHO * kao2mp: BUILD STARTED %_BUILD_NAME%              *
ECHO * %DATE%, %TIME%                      *
ECHO ************************************************

:: Build all projects

CALL "BUILD\HOTLOAD.CMD"  || EXIT /B 1
CALL "BUILD\KAO2PLUS.CMD" || EXIT /B 1
CALL "BUILD\SERVER.CMD"   || EXIT /B 1

:: Success!

ECHO ********************************
ECHO * kao2mp: BUILD OK             *
ECHO * %DATE%, %TIME%      *
ECHO ********************************
