@echo off
echo Configuring source tree...

copy config.jam ..\config.jam

echo done!
echo Bootstrapping bjam (as build\bin\bjam)
cd ..\..\3rd-party\boost\tools\jam\
call build.bat msvc
cd ..\..\..\..\build\msvc

pause
