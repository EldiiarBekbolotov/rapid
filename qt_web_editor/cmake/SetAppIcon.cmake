# This function sets the application icon on different platforms
function(set_application_icon target icon_file)
    if(APPLE)
        # On macOS, we need to create an .icns file and add it to the bundle
        set(ICONSET_NAME "${CMAKE_CURRENT_BINARY_DIR}/${target}.iconset")
        
        # Create iconset directory
        file(MAKE_DIRECTORY ${ICONSET_NAME})
        
        # Generate different icon sizes for macOS
        set(ICON_SIZES 16 32 64 128 256 512 1024)
        
        foreach(SIZE ${ICON_SIZES})
            execute_process(
                COMMAND rsvg-convert -w ${SIZE} -h ${SIZE} -f png -o ${ICONSET_NAME}/icon_${SIZE}x${SIZE}.png ${icon_file}
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            )
            math(EXPR SIZE_X2 "${SIZE} * 2")
            if(SIZE_X2 LESS_EQUAL 1024)
                execute_process(
                    COMMAND rsvg-convert -w ${SIZE_X2} -h ${SIZE_X2} -f png -o ${ICONSET_NAME}/icon_${SIZE}x${SIZE}@2x.png ${icon_file}
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                )
            endif()
        endforeach()
        
        # Create .icns file
        execute_process(
            COMMAND iconutil -c icns -o ${CMAKE_CURRENT_BINARY_DIR}/${target}.icns ${ICONSET_NAME}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        )
        
        # Set the icon for the macOS bundle
        set_target_properties(${target} PROPERTIES
            MACOSX_BUNDLE_ICON_FILE ${target}.icns
            MACOSX_BUNDLE_BUNDLE_NAME "${target}"
            MACOSX_BUNDLE_DISPLAY_NAME "${target}"
            MACOSX_BUNDLE_BUNDLE_VERSION "1.0.0"
            MACOSX_BUNDLE_SHORT_VERSION_STRING "1.0.0"
            MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Info.plist.in"
        )
        
        # Copy the icon to the bundle
        add_custom_command(TARGET ${target} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_CURRENT_BINARY_DIR}/${target}.icns
            $<TARGET_BUNDLE_DIR:${target}>/Contents/Resources/
        )
        
    elseif(WIN32)
        # On Windows, we'll use a .ico file
        set(RESOURCE_FILE "${CMAKE_CURRENT_BINARY_DIR}/${target}.rc"
        )
        
        # Create a resource file
        file(WRITE ${RESOURCE_FILE} "IDI_ICON1 ICON DISCARDABLE \"${icon_file}\"\n")
        
        # Add the resource file to the target
        target_sources(${target} PRIVATE ${RESOURCE_FILE})
        
        # Set the icon for the Windows executable
        set_target_properties(${target} PROPERTIES
            WIN32_EXECUTABLE TRUE
            LINK_FLAGS "-mwindows"
        )
        
    elseif(UNIX AND NOT APPLE)
        # On Linux, we'll use a .desktop file
        set(DESKTOP_FILE "${CMAKE_CURRENT_BINARY_DIR}/${target}.desktop")
        
        # Create a .desktop file
        file(WRITE ${DESKTOP_FILE}
            "[Desktop Entry]\n"
            "Type=Application\n"
            "Name=${target}\n"
            "Comment=Qt Web Editor\n"
            "Exec=$<TARGET_FILE:${target}> %f\n"
            "Icon=${target}\n"
            "Categories=Development;IDE;\n"
            "Terminal=false\n"
        )
        
        # Install the .desktop file
        install(FILES ${DESKTOP_FILE} DESTINATION share/applications)
        
        # Install the icon
        install(FILES ${icon_file} DESTINATION share/icons/hicolor/scalable/apps RENAME ${target}.svg)
    endif()
endfunction()
