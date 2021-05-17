::获取当前目录 当前目录拷贝到z pause
set pa=%cd%
echo %pa%
xcopy %pa% Y:\test\RtspServer /Y/K/S
::pause