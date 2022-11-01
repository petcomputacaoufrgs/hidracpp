find_package(Qt5Test REQUIRED)


add_executable(TestAhmes 
tst_ahmes.cpp
../simulationtestcase.h
)

target_link_libraries(TestAhmes PRIVATE Qt5::Test)
target_link_libraries(TestAhmes PRIVATE hidramachines) 

target_include_directories(
    TestAhmes 
    PUBLIC ../../../gui 
    PUBLIC ../../../core 
    PUBLIC ../../../machines 
    PUBLIC ../../..
    PUBLIC ..
    )

add_test(NAME TestAhmes COMMAND TestAhmes)


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
