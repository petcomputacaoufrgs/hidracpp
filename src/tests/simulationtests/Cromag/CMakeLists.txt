find_package(Qt5Test REQUIRED)


add_executable(TestCromag 
tst_cromag.cpp
)

configure_file(TestCromag.mem TestCromag.mem COPYONLY)

target_link_libraries(TestCromag PRIVATE Qt5::Test)
target_link_libraries(TestCromag PRIVATE hidramachines) 

target_include_directories(TestCromag PUBLIC ../../gui PUBLIC ../../core PUBLIC ../../machines PUBLIC ../..)


add_test(NAME TestCromag COMMAND TestCromag)



