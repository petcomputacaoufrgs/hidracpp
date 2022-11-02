find_package(Qt5Test REQUIRED)


add_executable(TestNeander 
tst_neander.cpp
../simulationtestcase.h
)

target_link_libraries(TestNeander PRIVATE Qt5::Test)
target_link_libraries(TestNeander PRIVATE hidramachines) 

target_include_directories(
    TestNeander 
    PUBLIC ../../../gui 
    PUBLIC ../../../core 
    PUBLIC ../../../machines 
    PUBLIC ../../..
    PUBLIC ..
    )


add_test(NAME TestNeander COMMAND TestNeander)



