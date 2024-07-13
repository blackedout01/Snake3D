@echo off

set conf=/DEBUG /Z7 /Od
set conf=/O2

set glfw_3_2_1_src=context.c init.c input.c monitor.c vulkan.c window.c win32_init.c win32_joystick.c win32_monitor.c win32_time.c win32_tls.c win32_window.c wgl_context.c egl_context.c

pushd .
cd glfw\src
echo Compiling GLFW
cl /c /nologo %conf% /D_GLFW_WIN32 %glfw_3_2_1_src%
popd

echo Compiling and creating Snake3D
cl /nologo %conf% /Iglfw\include main.cpp /Iglm glfw\src\*.obj User32.lib Gdi32.lib Shell32.lib Opengl32.lib /link /OUT:Snake3D.exe
del glfw\src\*.obj
del *.obj