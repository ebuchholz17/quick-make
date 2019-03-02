@echo off
call "%EMSCRIPTEN_PATH%\emsdk_env.bat"
call "%VC_VARS_PATH%\vcvarsall.bat" x64
set path=%CD%\misc;%path%
