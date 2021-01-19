cmake_minimum_required(VERSION 3.5)

project(KissDicomViewer VERSION "0.0.0.0")

configure_file(
    "${PROJECT_SOURCE_DIR}/KissDicomViewConfig.h.in"
    "${PROJECT_SOURCE_DIR}/global/KissDicomViewConfig.h"
    )

file(GLOB_RECURSE SOURCES "*.cpp" "*.cxx" "*.cc")
file(GLOB_RECURSE RESOURCES "*.qrc")
file(GLOB_RECURSE HEADERS "*.h")
file(GLOB_RECURSE FORMS *.ui)

set(SRCS ${SOURCES} ${HEADERS} ${RESOURCES} ${FORMS})
set(KissDicomViewer_Resource_DIR ${RESOURCES} CACHE PATH "KissDicomViewer Resource")


if(WIN32)
    add_library(KissDicomViewer_static STATIC ${SRCS})
    target_link_libraries(
        KissDicomViewer_static
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
        Qt5::Sql
        Qt5::Xml
        Qt5::Network
        Qt5::Concurrent
        ${DCMTK_LIBRARIES}
        )
else()
    add_library(KissDicomViewer_sharde SHARED ${SRCS})
    target_link_libraries(
        KissDicomViewer_sharde
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
        Qt5::Sql
        Qt5::Xml
        Qt5::Network
        Qt5::Concurrent
        ${DCMTK_LIBRARIES}
        )
endif()





