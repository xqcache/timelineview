
if(NOT TIMELINE_JSON_EXTERNAL)
    add_subdirectory(json)
endif()

if(NOT TIMELINE_FFMPEG_EXTERNAL)
    add_subdirectory(ffmpeg)
endif()

if(NOT TIMELINE_FMT_EXTERNAL)
    add_subdirectory(fmt)
endif()

if(NOT TIMELINE_SPDLOG_EXTERNAL)
    add_subdirectory(spdlog)
endif()