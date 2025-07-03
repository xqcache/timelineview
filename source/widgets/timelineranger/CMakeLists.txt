set(PRIVATE_SOURCES
    timelineranger.cpp
    timelineranger.h
    timelineranger.ui
    timelinerangeslider.cpp
    timelinerangeslider.h
)

target_sources(${TARGET_NAME} PRIVATE
    ${PRIVATE_SOURCES}
)
