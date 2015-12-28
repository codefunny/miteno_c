@echo off

color 3a
:start
cls
echo -------------------------编译Debug选择-------------------------

@echo off
echo 1-支持TMS 2-普通 3-普通和支持TMS的都编译 4-删除过程文件 5-退出
set input=
set /p input=请选择编译操作选项:

if %input%==1 goto tms
if %input%==2 goto normal
if %input%==3 goto normal
if %input%==4 goto del
if %input%==5 goto exit

:normal
echo 开始编译普通版本
gmake -f Makefile DEBUGFLAG=DEBUG
goto end

:tms
echo 开始编译支持TMS版本
gmake -f Makefile TMSFUNC=USETMS DEBUGFLAG=DEBUG
goto end

:end
set a=
set /p a=按确认键删除过程文件:
if "%a%"=="" goto del
goto exit

:del
echo 删除过程文件...
del obj_err\*.o
del obj_err\*.deps
del obj_err\*.err
echo 删除成功

:exit
set a=
set /p a=按确认键退出，其他键继续:
if "%a%"=="" exit
goto start
