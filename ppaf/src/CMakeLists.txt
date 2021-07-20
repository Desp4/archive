set(PPAF_INCLUDES
    ${CMAKE_CURRENT_SOURCE_DIR}/decoder.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/decstream.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/player.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/iocontext.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/utils.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/export.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/buffer/audiobuffer.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/buffer/ringbuffer.hpp)

set(PPAF_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/decoder.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/decstream.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/player.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/iocontext.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/buffer/audiobuffer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/buffer/ringbuffer.cpp)

add_library(ppaf STATIC ${PPAF_SOURCES} ${PPAF_INCLUDES})

if(PPAF_PA_FROM_SOURCE)
    add_subdirectory(${PPAF_PA_PATH} ${PROJECT_BINARY_DIR}/pabuild)
    target_link_libraries(ppaf PUBLIC portaudio_static)
else()
    find_library(PPAF_PA NAMES portaudio_static HINTS ${PPAF_PA_LIB} REQUIRED)
    target_link_libraries(ppaf PUBLIC ${PPAF_PA})
    target_include_directories(ppaf PUBLIC ${PPAF_PA_INC})
endif()

find_library(PPAF_AVFORMAT NAMES avformat HINTS ${PPAF_FFMPEG_LIB} REQUIRED)
find_library(PPAF_AVCODEC NAMES avcodec HINTS ${PPAF_FFMPEG_LIB} REQUIRED)
find_library(PPAF_AVUTIL NAMES avutil HINTS ${PPAF_FFMPEG_LIB} REQUIRED)
find_library(PPAF_SWRESAMPLE NAMES swresample HINTS ${PPAF_FFMPEG_LIB} REQUIRED)

target_include_directories(ppaf PUBLIC ${PPAF_FFMPEG_INC})
target_include_directories(ppaf PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include) # for convenience
target_link_libraries(ppaf PUBLIC 
    ${PPAF_AVFORMAT} 
    ${PPAF_AVCODEC} 
    ${PPAF_AVUTIL} 
    ${PPAF_SWRESAMPLE})