cmake_minimum_required(VERSION 3.20)

project(argman 
    VERSION 1.0.0
    DESCRIPTION "A modern C++ argument parsing library with subcommand support"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(ARGMAN_BUILD_EXAMPLE "Build example executable" OFF)

add_library(${PROJECT_NAME} INTERFACE)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} 
    INTERFACE 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_20)

if(ARGMAN_BUILD_EXAMPLE)
    add_executable(${PROJECT_NAME}_example test/main.cpp)
    target_link_libraries(${PROJECT_NAME}_example PRIVATE ${PROJECT_NAME})
    
    set_target_properties(${PROJECT_NAME}_example PROPERTIES
        OUTPUT_NAME "argman_example"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    )
    
    target_compile_options(${PROJECT_NAME}_example PRIVATE 
        -Wall -Wextra -Wpedantic
    )
endif()

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
    add_compile_options(-fdiagnostics-color=always)
endif()
