find_package(Qt5Test REQUIRED)


add_executable(TestReg 
tst_reg.cpp
)

configure_file(TestReg.mem TestReg.mem COPYONLY)

target_link_libraries(TestReg PRIVATE Qt5::Test)
target_link_libraries(TestReg PRIVATE hidramachines) 

target_include_directories(TestReg PUBLIC ../../gui PUBLIC ../../core PUBLIC ../../machines PUBLIC ../..)


add_test(NAME TestReg COMMAND TestReg)



