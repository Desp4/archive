cmake_minimum_required(VERSION 3.8)

project(kultivator VERSION 0.6.0)

set(KULT_PPAF_SOURCE "" CACHE PATH "ppaf source directory")
set(KULT_TAGLIB_LIB "" CACHE PATH "TagLib lib directory.")
set(KULT_TAGLIB_INC "" CACHE PATH "TagLib include directory.")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC OFF)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_subdirectory(${KULT_PPAF_SOURCE} ${CMAKE_BINARY_DIR}/ppa_build)

find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/mainwindow.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/playerq.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/playerwidget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/playlistwidget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/util/hoverfilter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/util/strutil.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utilwidgets/multistatebutton.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utilwidgets/rangeslider.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utilwidgets/timelabel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utilwidgets/playlistview.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/tag/tag.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/itemmodel/playlistmodel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/playlist/audiolib.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/playlist/playlistfilter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/framelesswindow/framelesswindow.cpp)

set(HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/mainwindow.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/playerq.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/playerwidget.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/playlistwidget.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/util/hoverfilter.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/util/nmath.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/util/strutil.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/util/sliderstyle.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utilwidgets/multistatebutton.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utilwidgets/rangeslider.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utilwidgets/timelabel.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utilwidgets/playlistview.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/tag/tag.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/itemmodel/playlistmodel.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/playlist/audiolib.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/playlist/playlistfilter.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/framelesswindow/framelesswindow.h)

set(UI
    ${CMAKE_CURRENT_SOURCE_DIR}/src/forms/mainwindow.ui
    ${CMAKE_CURRENT_SOURCE_DIR}/src/forms/playerwidget.ui
    ${CMAKE_CURRENT_SOURCE_DIR}/src/forms/playlistwidget.ui)

set(RESOUCES
    ${CMAKE_CURRENT_SOURCE_DIR}/res/icons.qrc)

qt5_wrap_ui(SOURCES ${UI})

find_library(KULT_TAGLIB NAMES tag HINTS ${KULT_TAGLIB_INC} REQUIRED)
add_executable(kult WIN32 ${SOURCES} ${HEADERS} ${UI} ${RESOUCES})

target_include_directories(kult PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utilwidgets
    ${KULT_TAGLIB_INC})
target_link_libraries(kult PRIVATE ppaf ${KULT_TAGLIB} Qt5::Core Qt5::Gui Qt5::Widgets)
