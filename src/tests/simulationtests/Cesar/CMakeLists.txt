find_package(Qt5Test REQUIRED)


add_executable(TestCesar 
tst_Cesar.cpp
../simulationtestcase.h
)

target_link_libraries(TestCesar PRIVATE Qt5::Test)
target_link_libraries(TestCesar PRIVATE hidramachines) 

target_include_directories(
    TestCesar 
    PUBLIC ../../../gui 
    PUBLIC ../../../core 
    PUBLIC ../../../machines 
    PUBLIC ../../..
    PUBLIC ..
    )


add_test(NAME TestCesar COMMAND TestCesar)



