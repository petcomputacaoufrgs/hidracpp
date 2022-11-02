find_package(Qt5Test REQUIRED)


add_executable(TestRamses 
tst_ramses.cpp
../simulationtestcase.h
)

target_link_libraries(TestRamses PRIVATE Qt5::Test)
target_link_libraries(TestRamses PRIVATE hidramachines) 

target_include_directories(
    TestRamses
    PUBLIC ../../../gui 
    PUBLIC ../../../core 
    PUBLIC ../../../machines 
    PUBLIC ../../..
    PUBLIC ..
    )

add_test(NAME TestRamses COMMAND TestRamses)