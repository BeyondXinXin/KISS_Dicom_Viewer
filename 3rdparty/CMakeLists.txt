if(WIN32)
    add_subdirectory(quazip-0.8.1-win)
    include_directories(quazip-0.8.1-win/quazip)
    set(3rdpartyPath
        ${PROJECT_SOURCE_DIR}/3rdparty/quazip-0.8.1-win/quazip
        )
    set(3rdparty_Path ${3rdpartyPath} CACHE INTERNAL "mailiu 3rdpartyPath")
else()
    add_subdirectory(quazip-0.8.1)
    include_directories(quazip-0.8.1/quazip)
    set(3rdpartyPath
        ${PROJECT_SOURCE_DIR}/3rdparty/quazip-0.8.1/quazip
        )
    set(3rdparty_Path ${3rdpartyPath} CACHE INTERNAL "mailiu 3rdpartyPath")
endif()


