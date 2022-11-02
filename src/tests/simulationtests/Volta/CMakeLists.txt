find_package(Qt5Test REQUIRED)


add_executable(TestVolta 
tst_volta.cpp
)

configure_file(TestVolta.mem TestVolta.mem COPYONLY)

target_link_libraries(TestVolta PRIVATE Qt5::Test)
target_link_libraries(TestVolta PRIVATE hidramachines) 

target_include_directories(TestVolta PUBLIC ../../gui PUBLIC ../../core PUBLIC ../../machines PUBLIC ../..)


add_test(NAME TestVolta COMMAND TestVolta)



