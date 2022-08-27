@echo on

setlocal
cd /d %~dp0

set TEMP_FILE=temp_build_version.h
set BUILD_VERSION=build_version.h

REM set /p >%~dp0/%BUILD_VERSION%


 call  git name-rev --name-only HEAD >%~dp0/%TEMP_FILE%
 
 for /f "tokens=1" %%i in (%TEMP_FILE%) do (
 echo %%i 
 set /p="static const char * BUILD_GIT_BRANCH_NAME = "%%i" "<nul>%~dp0/%BUILD_VERSION%
 )
 
   echo ;>>%~dp0/%BUILD_VERSION%
 
 REM echo ;>>%~dp0/%BUILD_VERSION%
 
 call  git log --oneline | wc -l  >%~dp0/%TEMP_FILE%
 
 for /f "tokens=1" %%i in (%TEMP_FILE%) do (
 echo %%i 
 set /p="static const int BUILD_GIT_REVERSION = %%i   "<nul>>%~dp0/%BUILD_VERSION%
 )
 

REM git log --abbrev-commit --pretty=oneline 

 call git rev-parse HEAD >%~dp0/%TEMP_FILE%
 
 for /f "tokens=1" %%i in (%TEMP_FILE%) do (
 echo %%i 
 echo ;>>%~dp0/%BUILD_VERSION%
 set /p="static const char * BUILD_GIT_HASH = "%%i";   "<nul>>%~dp0/%BUILD_VERSION%
 )
  
set d=%date:~0,2%
set mo=%date:~3,2%
set y=%date:~6,4%
set h=%time:~0,2%
set mi=%time:~3,2%
set second=%time:~6,2%
REM echo %y%_%mo%_%d%_%h%_%mi%

echo ;>>%~dp0/%BUILD_VERSION%
REM 2022/03/18-19:15:35
set /p="static const char * BUILD_TIME = "%y%-%d%-%mo% %h%:%mi%:%second%";   "<nul>>%~dp0/%BUILD_VERSION%

  
 REM  set LOG_TIME=%date:0,4%-%date:5,2%-%date:8,2%-%time:0,2%-%time:3,2%-%time:6,2%
  
  
   REM %LOG_TIME%;
  REM  echo ;>%~dp0/%BUILD_VERSION%

   del  %TEMP_FILE%

REM pause
REM  set /p="static const int BUILD_GIT_REVERSION = 6;"<nul>%~dp0/build_version.h
REM call  git log --oneline | wc -l  >>%~dp0/build_version.h


REM echo static const int BUILD_GIT_REVERSION = >%~dp0/build_version.h 
REM 
REM call git log --oneline | wc -l set/p >>%~dp0/build_version.h
REM 
REM echo ;>>%~dp0/build_version.h

rEM >>abcd.txt set/p=abcd
REM >>abcd.txt set/p=abcd

REM set /p="abc"<nul>a.txt
REM set /p="abc"<nul>>a.txt



