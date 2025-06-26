set(PRIVATE_SOURCES
    timelineitemview.h
    timelineitemview.cpp
    timelinearmitemview.h
    timelinearmitemview.cpp
    timelineitemconnview.h
    timelineitemconnview.cpp
)

target_sources(${TARGET_NAME} PRIVATE ${PRIVATE_SOURCES})