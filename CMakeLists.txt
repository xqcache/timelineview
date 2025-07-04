cmake_minimum_required(VERSION 3.23.0 FATAL_ERROR)

project(timelineview VERSION 0.0.1 LANGUAGES CXX)

option(TIMELINE_BUILD_TESTS "Build test tools" OFF)
option(TIMELINE_JSON_EXTERNAL "Use external json library" OFF)
option(TIMELINE_BUILD_SHARED_LIBS "Build shared library" OFF)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/bin)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/source)

if(MSVC)
    add_compile_options(/utf-8)
endif()

add_subdirectory(source)
add_subdirectory(thirdparty)

if(TIMELINE_BUILD_TESTS)
    add_subdirectory(tests)
endif()
