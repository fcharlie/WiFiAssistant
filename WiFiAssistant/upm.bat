@echo off 
SET mvr=1 
SET mir=0  
SET par=0  
SET bdt=20  
echo ^/^*Defined PreEdit Version^*/ >upver.h 
echo // Please #include "upver.h" >>upver.h 
echo #define BUILD_TIME     %bdt% >>upver.h 
echo #define PATCH_TIME     %par% >>upver.h 
echo #define MINJOR_VERSION %mir% >>upver.h 
echo #define MAJOR_VERSION  %mvr% >>upver.h 
SET /a bdt+=1 
call  %~dp0static.bat       %bdt% 
goto :EOF 
