#glad
add_library(glad "glad/src/glad.c")
target_include_directories(glad SYSTEM PUBLIC "glad/include")

#imgui
add_library(imgui)

target_sources(imgui
    PRIVATE imgui/imgui_demo.cpp
            imgui/imgui_draw.cpp
            imgui/imgui_tables.cpp
            imgui/imgui_widgets.cpp
            imgui/imgui.cpp
            imgui/backends/imgui_impl_opengl3.cpp
            imgui/backends/imgui_impl_glfw.cpp)

target_include_directories(imgui SYSTEM PUBLIC imgui imgui/backends "glad/include")
target_link_libraries(imgui glfw OpenGL::GL)
