@echo off
IF NOT EXIST ..\build mkdir ..\build
pushd ..\build
cl /MT /Zi /Od -nologo ../code/main.cpp /I D:\SDL2\SDL2-2.0.3\include /link /ENTRY:wmainCRTStartup /SUBSYSTEM:CONSOLE /LIBPATH:D:\SDL2\SDL2-2.0.3\lib\x86\ SDL2.lib SDL2main.lib
popd