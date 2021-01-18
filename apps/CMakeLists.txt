
macro(DCMTK_ADD_EXECUTABLE PROGRAM)
    include_directories(${KissDicomViewer_INCLUDE_DIRS})
    add_executable(${PROGRAM} ${PROGRAM}.cpp  ${KissDicomViewer_Resource_DIR})
    target_link_libraries(${PROGRAM} KissDicomViewer_sharde)
endmacro()

foreach(SUBDIR studyexplorer dicomviewer logviewer)
  DCMTK_ADD_EXECUTABLE(${SUBDIR})
endforeach()



