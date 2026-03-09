cmake_minimum_required(VERSION 3.30)

project(argman
    VERSION 1.0.0
    DESCRIPTION "A modern C++ argument parsing library with subcommand support"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(ARGMAN_BUILD_EXAMPLE "Build example executable" OFF)

add_library(${PROJECT_NAME} STATIC)

target_sources(${PROJECT_NAME}
    PUBLIC FILE_SET CXX_MODULES FILES
        src/argman.cppm
        src/argman/option.cppm
        src/argman/command.cppm
        src/argman/parser.cppm
)

target_compile_options(${PROJECT_NAME} PRIVATE
    -Wall -Wextra -Wpedantic -Wconversion -Wshadow
)

include(CheckIPOSupported)
check_ipo_supported(RESULT ipo_supported)
if(ipo_supported)
    set_target_properties(${PROJECT_NAME} PROPERTIES INTERPROCEDURAL_OPTIMIZATION_RELEASE ON)
endif()

if(ARGMAN_BUILD_EXAMPLE)
    add_executable(${PROJECT_NAME}_example test/main.cpp)
    target_link_libraries(${PROJECT_NAME}_example PRIVATE ${PROJECT_NAME})

    set_target_properties(${PROJECT_NAME}_example PROPERTIES
        OUTPUT_NAME "argman_example"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    )

    target_compile_options(${PROJECT_NAME}_example PRIVATE
        -Wall -Wextra -Wpedantic -Wconversion -Wshadow
    )

    if(ipo_supported)
        set_target_properties(${PROJECT_NAME}_example PROPERTIES INTERPROCEDURAL_OPTIMIZATION_RELEASE ON)
    endif()
endif()
