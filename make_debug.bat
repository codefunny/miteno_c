@echo off

color 3a
:start
cls
echo -------------------------����Debugѡ��-------------------------

@echo off
echo 1-֧��TMS 2-��ͨ 3-��ͨ��֧��TMS�Ķ����� 4-ɾ�������ļ� 5-�˳�
set input=
set /p input=��ѡ��������ѡ��:

if %input%==1 goto tms
if %input%==2 goto normal
if %input%==3 goto normal
if %input%==4 goto del
if %input%==5 goto exit

:normal
echo ��ʼ������ͨ�汾
gmake -f Makefile DEBUGFLAG=DEBUG
goto end

:tms
echo ��ʼ����֧��TMS�汾
gmake -f Makefile TMSFUNC=USETMS DEBUGFLAG=DEBUG
goto end

:end
set a=
set /p a=��ȷ�ϼ�ɾ�������ļ�:
if "%a%"=="" goto del
goto exit

:del
echo ɾ�������ļ�...
del obj_err\*.o
del obj_err\*.deps
del obj_err\*.err
echo ɾ���ɹ�

:exit
set a=
set /p a=��ȷ�ϼ��˳�������������:
if "%a%"=="" exit
goto start
