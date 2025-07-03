set(PRIVATE_SOURCES
    timelineitem.h
    timelineitem.cpp
    timelinearmitem.h
    timelinearmitem.cpp
    timelinetrackitem.h
    timelinetrackitem.cpp
)

target_sources(${TARGET_NAME} PRIVATE ${PRIVATE_SOURCES})