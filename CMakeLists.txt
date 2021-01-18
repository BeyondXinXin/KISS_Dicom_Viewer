cmake_minimum_required(VERSION 3.1.0)
include (ExternalProject)
project(KissDicomViewer)

# Set self path
if(WIN32)
    set(VTK_DIR  "D:/lib/VTK-8.2.0/bulid")
    set(ITK_DIR  "D:/lib/InsightToolkit-4.13.2/bulid")
    set(OpenCV_DIR  "D:/lib/opencv-3.4.9/bulid")
    set(CGAL_DIR  "D:/lib/CGAL/bulid")
    set(DCMTK_DIR  "D:/lib/DCMTK3.6.5/bulid")
endif()


# Output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG
    ${PROJECT_SOURCE_DIR}/bin/Debug)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG
    ${PROJECT_SOURCE_DIR}/bin/Debug)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG
    ${PROJECT_SOURCE_DIR}/bin/Debug)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE
    ${PROJECT_SOURCE_DIR}/bin/Release)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE
    ${PROJECT_SOURCE_DIR}/bin/Release)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE
    ${PROJECT_SOURCE_DIR}/bin/Release)


# Link to external third-party libraries
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 14)
set(BUILD_SHARED_LIBS "" ON)
find_package(Qt5
    REQUIRED COMPONENTS Core Gui Widgets Sql Test Xml Network Concurrent)
find_package(Qt5LinguistTools)
add_definitions(-DQT_MESSAGELOGCONTEXT)

find_package(DCMTK REQUIRED ${DCMTK_FIND_PACKAGE_STATEGY})
include_directories(${DCMTK_INCLUDE_DIRS})


# LOCAL FILES
set(INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src)
set(KissDicomViewer_INCLUDE_DIRS "${INCLUDE_DIRS}")
set(KissDicomViewer_LIB_DIR      "${PROJECT_SOURCE_DIR}/bin/Release")
set(KissDicomViewer_CMAKE_DIR    "${PROJECT_BINARY_DIR}")

# Set Config
message(STATUS "Generating '${PROJECT_BINARY_DIR}/KissProjectConfig.cmake'")
configure_file(${PROJECT_SOURCE_DIR}/cmake/KissDicomViewerConfig.cmake.in
  "${PROJECT_BINARY_DIR}/KissDicomViewerConfig.cmake" @ONLY)

# Set up the compiled project
add_subdirectory(src)
add_subdirectory(apps)


# Clear
add_custom_target(clean-all
    COMMAND ${CMAKE_BUILD_TOOL} clean
    )

