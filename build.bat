@echo off

set conf=/DEBUG /Z7 /Od
set conf=/O2

set glfw_src=context.c init.c input.c monitor.c platform.c vulkan.c window.c egl_context.c osmesa_context.c null_init.c null_monitor.c null_window.c null_joystick.c win32_time.c win32_module.c win32_thread.c win32_init.c win32_joystick.c win32_monitor.c win32_window.c wgl_context.c

pushd .
cd glfw\src
echo Compiling GLFW
cl /c /nologo %conf% /D_GLFW_WIN32 %glfw_src%
popd

echo Compiling and creating Snake3D
cl /nologo %conf% /Iglfw\include main.cpp /Iglm glfw\src\*.obj User32.lib Gdi32.lib Shell32.lib Opengl32.lib /link /OUT:Snake3D.exe
del glfw\src\*.obj
del *.obj