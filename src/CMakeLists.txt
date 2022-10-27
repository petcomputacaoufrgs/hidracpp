# Collects source file names
aux_source_directory(./core core)
aux_source_directory(./gui gui)
aux_source_directory(./machines machines)

# Qt package location
# Core and Gui are included because Widgets depends on it
find_package(Qt5 COMPONENTS REQUIRED Widgets) # Qt COMPONENTS // Core and Gui are included becuase Widgets depends on it

add_executable(${PROJECT_NAME}
    WIN32 # If you need a terminal for debug, please comment this statement 
    MACOSX_BUNDLE
    main.cpp
    ${core}
    ${gui}
    ${machines} 
) 

# Link the Qt library
target_link_libraries(${PROJECT_NAME} PRIVATE 
    Qt5::Widgets # Widgets include Core and Gui
) # Qt5 Shared Library