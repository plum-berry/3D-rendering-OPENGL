cmake_minimum_required(VERSION 3.10)
project(OpenGLApp C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)

# Include and library directories
include_directories(${CMAKE_SOURCE_DIR}/src/include)
link_directories(${CMAKE_SOURCE_DIR}/src/lib)

# Source files
set(SOURCES
    main.cpp
    src/include/imgui/imgui.cpp
    src/include/imgui/imgui_draw.cpp
    src/include/imgui/imgui_impl_glfw.cpp
    src/include/imgui/imgui_impl_opengl3.cpp
    src/include/imgui/imgui_tables.cpp
    src/include/imgui/imgui_widgets.cpp
    glad.c
    stb_image.cpp
)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Executable target
add_executable(main ${SOURCES})

# Link libraries (Windows specific)
target_link_libraries(main
    opengl32
    glfw3
    glfw3dll
    gdi32
    user32
    kernel32
)

