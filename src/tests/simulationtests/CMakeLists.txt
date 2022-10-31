find_package(Qt5Test REQUIRED)


add_executable(TestNeander 
tst_neander.cpp
)

configure_file(TestNeander.mem TestNeander.mem COPYONLY)

target_link_libraries(TestNeander PRIVATE Qt5::Test)
target_link_libraries(TestNeander PRIVATE hidramachines) 

target_include_directories(TestNeander PUBLIC ../../gui PUBLIC ../../core PUBLIC ../../machines PUBLIC ../..)


add_test(NAME TestNeander COMMAND TestNeander)


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
