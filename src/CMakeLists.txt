cmake_minimum_required(VERSION 3.5)

project(KissDicomViewer VERSION "0.0.0.0")

configure_file(
    "${PROJECT_SOURCE_DIR}/KissDicomViewConfig.h.in"
    "${PROJECT_SOURCE_DIR}/global/KissDicomViewConfig.h"
    )

include_directories(${3rdparty_Path})

file(GLOB_RECURSE SOURCES "*.cpp" "*.cxx" "*.cc")
file(GLOB_RECURSE RESOURCES "*.qrc")
file(GLOB_RECURSE HEADERS "*.h")
file(GLOB_RECURSE FORMS *.ui)

set(SRCS ${SOURCES} ${HEADERS} ${RESOURCES} ${FORMS})
set(KissDicomViewer_Resource_DIR ${RESOURCES} CACHE PATH "KissDicomViewer Resource")


add_library(KissDicomViewer STATIC ${SRCS})
target_link_libraries(
    KissDicomViewer
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::Sql
    Qt5::Xml
    Qt5::Network
    Qt5::Concurrent
    ${DCMTK_LIBRARIES}
    quazip5
    ${QUAZIP_LIBRARIES}
    )





