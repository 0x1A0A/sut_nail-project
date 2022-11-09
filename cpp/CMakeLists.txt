cmake_minimum_required(VERSION 3.2)

project( raycv VERSION 0.1 )

set(CMAKE_BUILD_TYPE RELEASE)
set(OpenGL_GL_PREFERENCE LEGACY)

add_subdirectory(vendors/raylib-4.2.0)
add_subdirectory(vendors/raygui-3.2/projects/CMake)

find_package( OpenCV REQUIRED )

add_subdirectory(App)