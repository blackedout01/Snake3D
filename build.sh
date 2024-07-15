

conf="-g -O0"
#conf=

glfw_src_shared="context.c init.c input.c monitor.c platform.c vulkan.c window.c egl_context.c osmesa_context.c null_init.c null_monitor.c null_window.c null_joystick.c"

glfw_src_apple="cocoa_time.c posix_module.c posix_thread.c"
glfw_src_other="posix_time.c posix_module.c posix_thread.c"

glfw_src_cocoa="cocoa_init.m cocoa_joystick.m cocoa_monitor.m cocoa_window.m nsgl_context.m"

cd glfw/src
echo Compiling GLFW
clang -c $conf -D_GLFW_COCOA $glfw_src_shared $glfw_src_apple $glfw_src_cocoa
cd ~-


echo Compiling and creating Snake3D
clang++ $conf -std=c++11 -Iglfw/include -Iglm -D_DEBUG -DGL_SILENCE_DEPRECATION main.cpp glfw/src/*.o -framework Cocoa -framework IOKit -framework OpenGL

rm glfw/src/*.o
