find_package(Qt5Test REQUIRED)


add_executable(TestPitagoras 
tst_pitagoras.cpp
)

configure_file(TestPitagoras.mem TestPitagoras.mem COPYONLY)

target_link_libraries(TestPitagoras PRIVATE Qt5::Test)
target_link_libraries(TestPitagoras PRIVATE hidramachines) 

target_include_directories(TestPitagoras PUBLIC ../../gui PUBLIC ../../core PUBLIC ../../machines PUBLIC ../..)


add_test(NAME TestPitagoras COMMAND TestPitagoras)



