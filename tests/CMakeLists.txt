set(TARGET_NAME timelineedit_test)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(QT NAMES Qt6 Qt5 CONFIG REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} CONFIG REQUIRED COMPONENTS Widgets)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

add_executable(${TARGET_NAME} main.cpp)
target_link_libraries(${TARGET_NAME} PRIVATE timelineview)

add_executable(test_audio_waveform test_audio_waveform.cpp)
set(FFMPEG_LIBS ffmpeg::avformat ffmpeg::swscale)
target_link_libraries(test_audio_waveform PRIVATE ${FFMPEG_LIBS})
target_link_libraries(test_audio_waveform PRIVATE timelineview)