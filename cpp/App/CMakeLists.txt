file(
	GLOB_RECURSE APP_SRC
	"${CMAKE_CURRENT_LIST_DIR}/src/*.cpp"
)

add_executable(App ${APP_SRC})

target_include_directories( App PUBLIC ${OpenCV_INCLUDE_DIRS} )
target_link_libraries( App ${OpenCV_LIBS} raylib raygui )