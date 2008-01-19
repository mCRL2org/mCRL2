echo off
echo Configuring source tree...

copy config.jam ..\config.jam
copy setup.h ..\..\libraries\utilities\include\mcrl2\setup.h

echo done!
echo Bootstrapping bjam (as build\bin\bjam)
..\..\3rd-party\boost\tools\jam\build.bat msvc

pause
