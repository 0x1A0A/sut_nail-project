add_executable(App main.cpp)

target_include_directories( App PRIVATE ${OpenCV_INCLUDE_DIRS} )
target_link_libraries( App ${OpenCV_LIBS} raylib )