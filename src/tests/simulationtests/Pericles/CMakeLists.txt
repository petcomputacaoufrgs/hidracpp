find_package(Qt5Test REQUIRED)


add_executable(TestPericles 
tst_pericles.cpp
)

configure_file(TestPericles.mem TestPericles.mem COPYONLY)

target_link_libraries(TestPericles PRIVATE Qt5::Test)
target_link_libraries(TestPericlesPRIVATE hidramachines) 

target_include_directories(TestPericles PUBLIC ../../gui PUBLIC ../../core PUBLIC ../../machines PUBLIC ../..)


add_test(NAME TestPericles COMMAND TestPericles



