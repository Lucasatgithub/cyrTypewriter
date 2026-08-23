cmake_minimum_required(VERSION 3.10)
project(CyrillicTypingApp)

set(CMAKE_CXX_STANDARD 17)
add_compile_options("/utf-8")

include_directories(
	${CMAKE_SOURCE_DIR}/include
	${CMAKE_SOURCE_DIR}/imgui
	${CMAKE_SOURCE_DIR}/imgui/backends
)

link_directories("${CMAKE_SOURCE_DIR}/lib")

set(APP_SOURCES
	imgui/imgui.cpp
	imgui/imgui_draw.cpp
	imgui/imgui_tables.cpp
	imgui/imgui_widgets.cpp
	imgui/backends/imgui_impl_glfw.cpp
	imgui/backends/imgui_impl_opengl3.cpp
)

set(GLAD_SOURCES
	src/glad.c
)

add_executable(cyrTypewriter 
	main.cpp 
	${APP_SOURCES} 
	${GLAD_SOURCES}
)

target_link_libraries(cyrTypewriter glfw3 OpenGL32)