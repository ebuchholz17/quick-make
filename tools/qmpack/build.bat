@echo off
IF NOT EXIST .\build mkdir .\build
pushd .\build

cl -nologo -Od -W4 -Z7 ..\qmpack.cpp /Feqmpack.exe /link /OPT:REF

popd

