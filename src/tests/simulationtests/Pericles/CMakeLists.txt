find_package(Qt5Test REQUIRED)


add_executable(TestPericles 
tst_pericles.cpp
../simulationtestcase.h
)

target_link_libraries(TestPericles PRIVATE Qt5::Test)
target_link_libraries(TestPericles PRIVATE hidramachines) 

target_include_directories(
    TestPericles 
    PUBLIC ../../../gui 
    PUBLIC ../../../core 
    PUBLIC ../../../machines 
    PUBLIC ../../..
    PUBLIC ..
    )


add_test(NAME TestPericles COMMAND TestPericles)




