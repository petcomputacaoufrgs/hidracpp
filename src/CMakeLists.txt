
set(EXECUTABLE_NAME ${PROJECT_NAME})

# Qt package location
# Core and Gui are included because Widgets depends on it
find_package(Qt5 COMPONENTS REQUIRED Widgets) # Qt COMPONENTS // Core and Gui are included becuase Widgets depends on it


# Collects source file names

aux_source_directory(./core core)
aux_source_directory(./gui gui)
aux_source_directory(./machines machines)

# Includes relevant files

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/gui/fonts
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/gui)

add_library(hidracore STATIC ${core})
add_library(hidramachines STATIC ${machines})
add_library(hidragui STATIC ${gui})

# hidracore depends on widgets
target_link_libraries(hidracore Qt5::Widgets)
# hidramachines depends on hidracore and widgets
target_link_libraries(hidramachines hidracore)
# hidragui depends on hidramachines and hidracore and widgets
target_link_libraries(hidragui hidramachines)


add_executable(${EXECUTABLE_NAME}
    WIN32 # If you need a terminal for debug, please comment this statement 
    MACOSX_BUNDLE
    main.cpp
)

target_link_libraries(${EXECUTABLE_NAME} hidragui)