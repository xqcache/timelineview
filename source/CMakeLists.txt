set(TARGET_NAME ${PROJECT_NAME})

find_package(QT NAMES Qt6 CONFIG REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} CONFIG REQUIRED COMPONENTS Widgets)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

if(TIMELINE_BUILD_SHARED_LIBS OR BUILD_SHARED_LIBS)
    add_library(${TARGET_NAME} SHARED)
else()
    add_library(${TARGET_NAME} STATIC)
    target_compile_definitions(${TARGET_NAME} PUBLIC TIMELINE_BUILD_STATIC)
endif()

target_link_libraries(${TARGET_NAME} PUBLIC Qt${QT_VERSION_MAJOR}::Widgets nlohmann_json spdlog::spdlog)
target_compile_definitions(${TARGET_NAME} PRIVATE TIMELINE_COMPILE_LIB)
target_include_directories(${TARGET_NAME} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}> $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>)

set(PRIVATE_SOURCES
    timelinelibexport.h
    timelineserializable.h
    timelineutil.cpp
    timelineutil.h
    timelinescene.cpp
    timelinescene.h
    timelineview.cpp
    timelineview.h
    timelineaxis.cpp
    timelineaxis.h
    timelinemodel.cpp
    timelinemodel.h
    timelineitemfactory.cpp
    timelineitemfactory.h
    timelineranger.cpp
    timelineranger.h
    timelineranger.ui
    timelinerangeslider.cpp
    timelinerangeslider.h
    timelinemediautil.h
    timelinemediautil.cpp
)

target_sources(${TARGET_NAME} PRIVATE
    ${PRIVATE_SOURCES}
)

set(FFMPEG_LIBS ffmpeg::avformat ffmpeg::swscale)
target_link_libraries(${TARGET_NAME} PRIVATE ${FFMPEG_LIBS})

add_subdirectory(item)
add_subdirectory(itemview)

# 处理ffmpeg的依赖dll
include(${CMAKE_SOURCE_DIR}/cmake/ParseDependTargets.cmake)

set(ffmpeg_targets "")
ParseDependTargets(ffmpeg_targets TARGETS ${FFMPEG_LIBS} )

foreach(_dll ${ffmpeg_targets})
    add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${_dll}" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
endforeach()

# 安装目标
if(TIMELINE_BUILD_SHARED_LIBS OR BUILD_SHARED_LIBS)
    install(TARGETS ${TARGET_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR})
else()
    install(TARGETS ${TARGET_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()
install(FILES ${ffmpeg_targets} DESTINATION ${CMAKE_INSTALL_BINDIR})
