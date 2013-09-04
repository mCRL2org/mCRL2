SET PATH=%PATH%;C:\Program Files (x86)\Java\jre7\bin;C:\Python27;C:\Qt\4.8.4\bin
SET SVNVERSION=C:\Program Files\SlikSvn\bin\svnversion
call "C:\Program Files\Microsoft SDKs\Windows\v7.1\Bin\SetEnv.Cmd" /Debug
setlocal enabledelayedexpansion
E:
cd \scratch
"C:\Program Files (x86)\GnuWin32\bin\wget.exe" http://mcrl2build1.win.tue.nl:8080/jnlpJars/slave.jar -Oslave.jar
FOR /F "usebackq" %%i IN (`hostname`) DO SET HOSTNAME=%%i
java -jar slave.jar -jnlpUrl http://mcrl2build1.win.tue.nl:8080/computer/%HOSTNAME%/slave-agent.jnlp -secret 62ddc1653e94de2cfead6568f5d33a394c3e2fdc8f40954db5474c72464e9e02