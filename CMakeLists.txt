cmake_minimum_required(VERSION 3.23.0 FATAL_ERROR)

project(timelineview VERSION 0.0.1 LANGUAGES CXX)

option(TIMELINE_BUILD_TESTS "Build test tools" OFF)
option(TIMELINE_BUILD_SHARED_LIBS "Build shared library" OFF)

option(TIMELINE_FMT_EXTERNAL "Use external fmt library" OFF)
option(TIMELINE_SPDLOG_EXTERNAL "Use external spdlog library" OFF)
option(TIMELINE_JSON_EXTERNAL "Use external json library" OFF)
option(TIMELINE_FFMPEG_EXTERNAL "Use external ffmpeg library" OFF)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)

if (NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/bin)
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/source)

if(MSVC)
    add_compile_options(/utf-8)
endif()

add_subdirectory(thirdparty)
add_subdirectory(source)

if(TIMELINE_BUILD_TESTS)
    add_subdirectory(tests)
endif()
