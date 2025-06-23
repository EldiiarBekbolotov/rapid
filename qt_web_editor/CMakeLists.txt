cmake_minimum_required(VERSION 3.16)
project(Rapid VERSION 1.0.0 LANGUAGES CXX)

set(APP_NAME "Rapid")
set(APP_DESCRIPTION "A lightweight web development text editor")
set(APP_ORGANIZATION "Rapid")
set(APP_DOMAIN "eldiiarbekbolotov.github.io/rapid")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS 
    Core 
    Widgets 
    WebEngineWidgets 
    WebEngineCore 
    WebChannel 
    WebSockets 
    Network 
    Svg 
    SvgWidgets
)

message(STATUS "Found all required Qt6 components, including WebEngineWidgets")
add_definitions(-DHAVE_WEBENGINEWIDGETS)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
include(SetAppIcon)

set(SOURCES
    src/main.cpp
    src/core/application.cpp
    src/core/mainwindow.cpp
    src/core/editorwidget.cpp
    src/core/filebrowser.cpp
    src/core/settings.cpp
    src/utils/syntaxhighlighter.cpp
)

set(HEADERS
    src/core/application.h
    src/core/mainwindow.h
    src/core/editorwidget.h
    src/core/filebrowser.h
    src/core/settings.h
    src/utils/syntaxhighlighter.h
)

set(FORMS forms/mainwindow.ui)
set(RESOURCES resources/resources.qrc)

add_executable(${PROJECT_NAME} MACOSX_BUNDLE ${SOURCES} ${HEADERS} ${FORMS} ${RESOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::WebEngineWidgets
    Qt${QT_VERSION_MAJOR}::WebChannel
    Qt${QT_VERSION_MAJOR}::WebSockets
    Qt${QT_VERSION_MAJOR}::Network
    Qt${QT_VERSION_MAJOR}::Svg
    Qt${QT_VERSION_MAJOR}::SvgWidgets
)

# Find Threads package (pthread on Unix)
find_package(Threads REQUIRED)

if(NOT MSVC)
    # Link pthreads only on non-MSVC platforms
    target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES
    OUTPUT_NAME Rapid
    VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_BUNDLE_NAME ${APP_NAME}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION}
    MACOSX_BUNDLE_COPYRIGHT "Copyright © 2025 ${APP_ORGANIZATION}"
    MACOSX_BUNDLE_INFO_STRING "${APP_DESCRIPTION}"
    MACOSX_BUNDLE_GUI_IDENTIFIER "${APP_DOMAIN}.${APP_ORGANIZATION}"
)

# macOS icon generation
if(APPLE)
    set(ICON_SVG "${CMAKE_SOURCE_DIR}/resources/icons/app-icon.svg")
    set(ICON_ICNS "${CMAKE_CURRENT_BINARY_DIR}/app-icon.icns")
    find_program(RSVG_CONVERT_EXECUTABLE rsvg-convert)
    find_program(ICONUTIL_EXECUTABLE iconutil)

    if(RSVG_CONVERT_EXECUTABLE AND ICONUTIL_EXECUTABLE)
        set(ICONSET_DIR "${CMAKE_CURRENT_BINARY_DIR}/Rapid.iconset")
        file(MAKE_DIRECTORY ${ICONSET_DIR})
        foreach(SIZE 16 32 64 128 256 512 1024)
            set(OUT "${ICONSET_DIR}/icon_${SIZE}x${SIZE}.png")
            add_custom_command(OUTPUT ${OUT}
                COMMAND ${RSVG_CONVERT_EXECUTABLE} -w ${SIZE} -h ${SIZE} ${ICON_SVG} -o ${OUT}
                DEPENDS ${ICON_SVG}
            )
            list(APPEND PNGS ${OUT})
        endforeach()

        add_custom_command(OUTPUT ${ICON_ICNS}
            COMMAND ${ICONUTIL_EXECUTABLE} -c icns -o ${ICON_ICNS} ${ICONSET_DIR}
            DEPENDS ${PNGS}
        )
        set_source_files_properties(${ICON_ICNS} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
        set(RESOURCES ${RESOURCES} ${ICON_ICNS})
        set_target_properties(${PROJECT_NAME} PROPERTIES
            MACOSX_BUNDLE_ICON_FILE app-icon.icns
            RESOURCE "${ICON_ICNS}"
        )
    endif()
endif()

install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
    BUNDLE DESTINATION .
    LIBRARY DESTINATION lib
)

install(DIRECTORY resources/
    DESTINATION resources
    USE_SOURCE_PERMISSIONS
    FILES_MATCHING PATTERN "*" PATTERN "*.qrc" EXCLUDE
)

add_custom_target(run
    COMMAND $<TARGET_FILE:${PROJECT_NAME}>
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    DEPENDS ${PROJECT_NAME}
    COMMENT "Running ${PROJECT_NAME}..."
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/resources $<TARGET_FILE_DIR:${PROJECT_NAME}>/resources
)
