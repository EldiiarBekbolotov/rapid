cmake_minimum_required(VERSION 3.16)
project(QtWebEditor VERSION 1.0.0 LANGUAGES CXX)

set(APP_NAME "Rapid")
set(APP_DESCRIPTION "A lightweight web development text editor")
set(APP_ORGANIZATION "Rapid")
set(APP_DOMAIN "eldiiarbekbolotov.github.io.rapid")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Force QT_VERSION_MAJOR (6)
string(SUBSTRING "${Qt6_VERSION}" 0 1 QT_VERSION_MAJOR)

# Find Qt packages explicitly with exact names and REQUIRED
find_package(Qt6 COMPONENTS Widgets WebEngineWidgets REQUIRED)

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

set(FORMS
    forms/mainwindow.ui
)

set(RESOURCES
    resources/resources.qrc
)

# This creates a macOS .app bundle automatically when APPLE is detected
add_executable(${PROJECT_NAME} MACOSX_BUNDLE ${SOURCES} ${HEADERS} ${FORMS} ${RESOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Widgets
    Qt6::WebEngineWidgets
)

# Set macOS bundle properties if on Apple
if(APPLE)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        MACOSX_BUNDLE_BUNDLE_NAME ${APP_NAME}
        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION}
        MACOSX_BUNDLE_COPYRIGHT "Copyright © 2025 ${APP_ORGANIZATION}"
        MACOSX_BUNDLE_INFO_STRING "${APP_DESCRIPTION}"
        MACOSX_BUNDLE_GUI_IDENTIFIER "${APP_DOMAIN}.${APP_ORGANIZATION}"
        MACOSX_BUNDLE_ICON_FILE app-icon.icns
    )

    # Set the icon (assumes you generate app-icon.icns and put in build dir)
    set(ICON_SVG "${CMAKE_SOURCE_DIR}/resources/icons/app-icon.svg")
    set(ICON_ICNS "${CMAKE_CURRENT_BINARY_DIR}/app-icon.icns")

    # Simple icon generation if rsvg-convert and iconutil are available
    find_program(RSVG_CONVERT_EXECUTABLE rsvg-convert)
    find_program(ICONUTIL_EXECUTABLE iconutil)

    if(RSVG_CONVERT_EXECUTABLE AND ICONUTIL_EXECUTABLE)
        file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.iconset")

        foreach(size 16 32 64 128 256 512 1024)
            add_custom_command(
                OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.iconset/icon_${size}x${size}.png"
                COMMAND ${RSVG_CONVERT_EXECUTABLE} -w ${size} -h ${size} ${ICON_SVG} -o "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.iconset/icon_${size}x${size}.png"
                DEPENDS ${ICON_SVG}
            )
        endforeach()

        add_custom_command(
            OUTPUT ${ICON_ICNS}
            COMMAND ${ICONUTIL_EXECUTABLE} -c icns -o ${ICON_ICNS} "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.iconset"
            DEPENDS
                "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.iconset/icon_16x16.png"
                "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.iconset/icon_32x32.png"
                "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.iconset/icon_64x64.png"
                "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.iconset/icon_128x128.png"
                "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.iconset/icon_256x256.png"
                "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.iconset/icon_512x512.png"
                "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.iconset/icon_1024x1024.png"
        )
        add_custom_target(GenerateIcons ALL DEPENDS ${ICON_ICNS})
        add_dependencies(${PROJECT_NAME} GenerateIcons)
        set_source_files_properties(${ICON_ICNS} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
        target_sources(${PROJECT_NAME} PRIVATE ${ICON_ICNS})
    endif()
endif()

# Copy resource directory after build
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/resources $<TARGET_FILE_DIR:${PROJECT_NAME}>/resources
)

# Install rules
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
    BUNDLE DESTINATION .
    LIBRARY DESTINATION lib
)

install(DIRECTORY resources/
    DESTINATION resources
    FILES_MATCHING PATTERN "*"
    PATTERN "*.qrc" EXCLUDE
)
