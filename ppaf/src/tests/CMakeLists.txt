add_executable(playback ${CMAKE_CURRENT_SOURCE_DIR}/playbacktest.cpp)
target_link_libraries(playback PRIVATE ppaf)