echo off
echo Configuring source tree...

copy config.jam ..\config.jam

echo done!
echo Bootstrapping bjam (as build\bin\bjam)
..\..\3rd-party\boost\tools\jam\build.bat msvc

pause
