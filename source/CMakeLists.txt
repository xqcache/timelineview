set(TARGET_NAME ${PROJECT_NAME})

find_package(QT NAMES Qt6 CONFIG REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} CONFIG REQUIRED COMPONENTS Widgets)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

if(TIMELINE_BUILD_SHARED OR BUILD_SHARED_LIBS)
    add_library(${TARGET_NAME} SHARED)
else()
    add_library(${TARGET_NAME} STATIC)
    target_compile_definitions(${TARGET_NAME} PUBLIC TIMELINE_BUILD_STATIC)
endif()

target_link_libraries(${TARGET_NAME} PUBLIC Qt${QT_VERSION_MAJOR}::Widgets nlohmann_json)
target_compile_definitions(${TARGET_NAME} PRIVATE TIMELINE_COMPILE_LIB)
target_include_directories(${TARGET_NAME} INTERFACE "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}")
target_sources(${TARGET_NAME} PRIVATE
    timelinelibexport.h
    timelinedef.h
    timelineaxis.h
    timelineaxis.cpp
    timelineview.h
    timelineview.cpp
    timelinemodel.h
    timelinemodel.cpp
    timelinescene.h
    timelinescene.cpp
    timelineitemfactory.h
    timelineitemfactory.cpp
)

add_subdirectory(item)
add_subdirectory(itemview)