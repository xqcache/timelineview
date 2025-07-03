set(PRIVATE_SOURCES
    timelineserializable.h
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
)

target_sources(${TARGET_NAME} PRIVATE
    ${PRIVATE_SOURCES}
)

add_subdirectory(item)
add_subdirectory(itemview)

target_include_directories(${TARGET_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})