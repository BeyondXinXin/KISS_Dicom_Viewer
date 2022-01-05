if(WIN32)
    macro(DCMTK_ADD_EXECUTABLE PROGRAM)
        include_directories(${KissDicomViewer_INCLUDE_DIRS})
        add_executable(${PROGRAM} ${PROGRAM}.cpp  ${KissDicomViewer_Resource_DIR})
        target_link_libraries(${PROGRAM} KissDicomViewer)
		if(MSVC)
		set_target_properties(${PROGRAM} PROPERTIES
			WIN32_EXECUTABLE YES
			LINK_FLAGS "/ENTRY:mainCRTStartup"
		)
		endif()

    endmacro()
else()
    macro(DCMTK_ADD_EXECUTABLE PROGRAM)
        include_directories(${KissDicomViewer_INCLUDE_DIRS})
        add_executable(${PROGRAM} ${PROGRAM}.cpp  ${KissDicomViewer_Resource_DIR})
        target_link_libraries(${PROGRAM} KissDicomViewer)
    endmacro()
endif()

foreach(SUBDIR studyexplorer dicomviewer logviewer)
    DCMTK_ADD_EXECUTABLE(${SUBDIR})
endforeach()

