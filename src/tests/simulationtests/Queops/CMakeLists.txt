find_package(Qt5Test REQUIRED)


add_executable(TestQueops 
tst_queops.cpp
)

configure_file(TestQueops.mem TestQueops.mem COPYONLY)

target_link_libraries(TestQueops PRIVATE Qt5::Test)
target_link_libraries(TestQueops PRIVATE hidramachines) 

target_include_directories(TestQueops PUBLIC ../../gui PUBLIC ../../core PUBLIC ../../machines PUBLIC ../..)


add_test(NAME TestQueops COMMAND TestQueops)


#add_subdirectory(Header)
#add_subdirectory(External/glfw)
#target_include_directories(${PROJECT_NAME} 
#    PUBLIC Header 
#    PUBLIC External/glfw/include
#)

#target_link_directories(${PROJECT_NAME} 
#    PUBLIC Header
#    PRIVATE External/glfw/src 
#)
