cmake_minimum_required(VERSION 3.21)

project(RayTracer VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_COMPILER "clang++")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -Wshadow")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

add_subdirectory("third_party")

add_executable(raytracer)

target_sources(raytracer
PUBLIC
	src/main.cpp

	includes/ray.hpp
	includes/utils.hpp
	includes/shapes.hpp
	includes/hit_record.hpp
	includes/scene.hpp
	includes/camera.hpp
	includes/material.hpp
)

target_compile_options(raytracer PRIVATE -Wall -Wextra -Wpedantic)

target_include_directories(raytracer PUBLIC includes/)
target_link_libraries(raytracer OpenGL::GL glfw glad imgui)
