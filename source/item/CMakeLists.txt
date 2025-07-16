set(PRIVATE_SOURCES
    timelineitem.h
    timelineitem.cpp
    timelinearmitem.h
    timelinearmitem.cpp
    timelinetrackitem.h
    timelinetrackitem.cpp
    timelineaimitem.h
    timelineaimitem.cpp
    timelinevideoitem.h
    timelinevideoitem.cpp
    timelinefocusitem.h
    timelinefocusitem.cpp
    timelinezoomitem.h
    timelinezoomitem.cpp
)

target_sources(${TARGET_NAME} PRIVATE ${PRIVATE_SOURCES})