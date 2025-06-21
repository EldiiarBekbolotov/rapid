cmake_minimum_required(VERSION 3.16)
project(QtWebEditor VERSION 1.0.0 LANGUAGES CXX)

# Set application propserties
set(APP_NAME "Rapid")
set(APP_DESCRIPTION "A lightweight web development text editor")
set(APP_ORGANIZATION "Rapid")
set(APP_DOMAIN "eldiiarbekbolotov.github.io/rapid")

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find Qt packages
find_package(Qt6 REQUIRED COMPONENTS Core Widgets WebEngineCore WebChannel WebSockets Network Svg SvgWidgets)

# Make WebEngineWidgets optional
find_package(Qt6 COMPONENTS WebEngineWidgets QUIET)
if (Qt6WebEngineWidgets_FOUND)
    message(STATUS "Found Qt6WebEngineWidgets")
    add_definitions(-DHAVE_WEBENGINEWIDGETS)
else()
    message(STATUS "Qt6WebEngineWidgets not found - some features may be disabled")
endif()

if (NOT Qt6_FOUND)
    message(FATAL_ERROR "Qt6 not found")
endif()

# Include our CMake modules
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
include(SetAppIcon)

# Set source files
set(SOURCES
    src/main.cpp
    src/core/application.cpp
    src/core/mainwindow.cpp
    src/core/editorwidget.cpp
    src/core/filebrowser.cpp
    src/core/settings.cpp
    src/utils/syntaxhighlighter.cpp
)

# Set header files
set(HEADERS
    src/core/application.h
    src/core/mainwindow.h
    src/core/editorwidget.h
    src/core/filebrowser.h
    src/core/settings.h
    src/utils/syntaxhighlighter.h
)

# Set UI files
set(FORMS
    forms/mainwindow.ui
)

# Set resource files
set(RESOURCES
    resources/resources.qrc
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS} ${FORMS} ${RESOURCES})

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::WebEngineWidgets
)

# Set application properties
set_target_properties(${PROJECT_NAME} PROPERTIES
    OUTPUT_NAME ${PROJECT_NAME}
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
    MACOSX_BUNDLE_BUNDLE_NAME ${APP_NAME}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION}
    MACOSX_BUNDLE_COPYRIGHT "Copyright © 2025 ${APP_ORGANIZATION}"
    MACOSX_BUNDLE_INFO_STRING "${APP_DESCRIPTION}"
    MACOSX_BUNDLE_GUI_IDENTIFIER "${APP_DOMAIN}.${APP_ORGANIZATION}"
)

# Function to generate .icns from SVG
function(generate_icns_from_svg SVG_FILE ICNS_FILE)
    # Check if we have rsvg-convert and iconutil available
    find_program(RSVG_CONVERT_EXECUTABLE rsvg-convert)
    find_program(ICONUTIL_EXECUTABLE iconutil)
    
    if(RSVG_CONVERT_EXECUTABLE AND ICONUTIL_EXECUTABLE)
        # Create iconset directory
        set(ICONSET_DIR "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.iconset")
        file(MAKE_DIRECTORY ${ICONSET_DIR})
        
        # Define icon sizes
        set(ICON_SIZES "16 32 64 128 256 512 1024")
        
        # Generate PNGs for each size
        foreach(SIZE ${ICON_SIZES})
            set(OUTPUT_FILE "${ICONSET_DIR}/icon_${SIZE}x${SIZE}.png")
            add_custom_command(
                OUTPUT ${OUTPUT_FILE}
                COMMAND ${RSVG_CONVERT_EXECUTABLE} -w ${SIZE} -h ${SIZE} ${SVG_FILE} -o ${OUTPUT_FILE}
                DEPENDS ${SVG_FILE}
                COMMENT "Generating ${SIZE}x${SIZE} icon"
            )
            list(APPEND GENERATED_PNGS ${OUTPUT_FILE})
        endforeach()
        
        # Create .icns file
        add_custom_command(
            OUTPUT ${ICNS_FILE}
            COMMAND ${ICONUTIL_EXECUTABLE} -c icns -o ${ICNS_FILE} ${ICONSET_DIR}
            DEPENDS ${GENERATED_PNGS}
            COMMENT "Generating .icns file"
        )
    else()
        message(WARNING "rsvg-convert or iconutil not found. Using default icon.")
    endif()
endfunction()

# Set application icon
if(APPLE)
    set(ICON_SVG "${CMAKE_SOURCE_DIR}/resources/icons/app-icon.svg")
    set(ICON_ICNS "${CMAKE_CURRENT_BINARY_DIR}/app-icon.icns")
    
    # Generate .icns from .svg
    generate_icns_from_svg(${ICON_SVG} ${ICON_ICNS})
    
    # Set icon properties
    set_source_files_properties(${ICON_ICNS} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
    set(RESOURCES ${RESOURCES} ${ICON_ICNS})
    set_target_properties(${PROJECT_NAME} PROPERTIES
        MACOSX_BUNDLE_ICON_FILE app-icon.icns
        RESOURCE "${ICON_ICNS}"
    )
endif()

# Installation
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
    BUNDLE DESTINATION .
    LIBRARY DESTINATION lib
)

# Install resources
install(DIRECTORY resources/
    DESTINATION resources
    USE_SOURCE_PERMISSIONS
    FILES_MATCHING
    PATTERN "*"
    PATTERN "*.qrc" EXCLUDE
)

# Create a run target for convenience
add_custom_target(run
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}$<$<CONFIG:Debug>:_debug>$<$<CONFIG:RelWithDebInfo>:_release>$<$<CONFIG:Release>:_release>$<$<CONFIG:MinSizeRel>:_release>$
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    DEPENDS ${PROJECT_NAME}
    COMMENT "Running ${PROJECT_NAME}..."
)

# Copy resources on build
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/resources $<TARGET_FILE_DIR:${PROJECT_NAME}>/resources
)
