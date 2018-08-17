@echo off
::Globa SET
if /i "%1"=="" ( 
set BDT=1 
) else (
SET BDT=%1
)
SET MAX=1
SET MIN=0
SET PATCH=0

::Write upm.bat
echo ^@echo off >upm.bat
echo SET mvr=%MAX% >>upm.bat
echo SET mir=%MIN%  >>upm.bat
echo SET par=%PATCH%  >>upm.bat
echo SET bdt=%BDT%  >>upm.bat
echo echo ^^/^^*Defined PreEdit Version^^*^/ ^>upver.h >>upm.bat
echo echo ^/^/ Please #include ^"upver.h^" ^>^>upver.h >>upm.bat
echo echo #define BUILD_TIME     %%bdt%% ^>^>upver.h >>upm.bat
echo echo #define PATCH_TIME     %%par%% ^>^>upver.h >>upm.bat
echo echo #define MINJOR_VERSION %%mir%% ^>^>upver.h >>upm.bat
echo echo #define MAJOR_VERSION  %%mvr%% ^>^>upver.h >>upm.bat
echo SET /a bdt+=1 >>upm.bat
echo call  %%~dp0static.bat       %%bdt%% >>upm.bat
echo goto :EOF >>upm.bat