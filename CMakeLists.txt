cmake_minimum_required(VERSION 3.8)
project(j3dmaterializer)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(lib)

file(GLOB_RECURSE J3DMATERIALIZER_SRC
    # J3DMaterializer
    "src/*.cpp"
    "src/*.c"
    "include/*.hpp"
    "include/*.h"
    "res/*.hpp"
    "res/*.h"
    
    # ImGuiFileDialog
    "lib/ImGuiFileDialog/ImGuiFileDialog/*.cpp"
)

add_executable(j3dmaterializer ${J3DMATERIALIZER_SRC})
target_include_directories(j3dultra PUBLIC include res include/util lib/glfw/include lib/ImGuiFileDialog/ImGuiFileDialog/)
target_link_libraries(j3dmaterializer PUBLIC j3dultra imgui glfw)