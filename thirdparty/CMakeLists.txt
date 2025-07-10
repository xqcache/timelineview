
if(NOT TIMELINE_JSON_EXTERNAL)
    add_subdirectory(json)
endif()

if (NOT TIMELINE_FFMPEG_EXTERNAL)
    add_subdirectory(ffmpeg)
endif()