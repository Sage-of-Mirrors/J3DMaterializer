cmake_minimum_required(VERSION 3.8)
project(j3dmaterializer)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(lib)

file(GLOB J3DMATERIALIZER_SRC
    # J3DMaterializer
    "src/*.cpp"
    "src/*.c"
    "include/*.hpp"
    "include/*.h"
    
    # ImGuiFileDialog
    "lib/ImGuiFileDialog/*.h"
    "lib/ImGuiFileDialog/*.cpp"
)

add_executable(j3dmaterializer ${J3DMATERIALIZER_SRC})
target_include_directories(j3dultra PUBLIC include lib/glfw/include)
target_link_libraries(j3dmaterializer PUBLIC j3dultra imgui glfw)