cmake_minimum_required (VERSION 3.8)

project(ppafl VERSION 2.1.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

option(PPAF_PA_FROM_SOURCE "Build PortAudio from source or else link to static library(portaudio_static)." ON)
option(PPAF_BUILD_TESTS "Build tests." ON)

if(PPAF_PA_FROM_SOURCE)
    set(PPAF_PA_PATH "" CACHE PATH "PortAudio source directory.")
else()
    set(PPAF_PA_LIB "" CACHE PATH "PortAudio library directory.")
    set(PPAF_PA_INC "" CACHE PATH "PortAudio include directory.")
endif()

set(PPAF_FFMPEG_LIB "" CACHE PATH "Ffmpeg library directory.")
set(PPAF_FFMPEG_INC "" CACHE PATH "Ffmpeg include directory.")

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src ${PROJECT_BINARY_DIR}/lib)
if(PPAF_BUILD_TESTS)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/tests ${PROJECT_BINARY_DIR}/tests_build)
endif()