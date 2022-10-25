find_package(Qt5Test REQUIRED)

add_executable(TestPointConversor 
tst_pointconversortest.cpp
../../core/pointconversor.cpp 
../../core/invalidconversorinput.cpp   
)

target_include_directories(TestPointConversor PRIVATE ../../core)
target_link_libraries(TestPointConversor PRIVATE Qt5::Test)


add_test(NAME TestPointConversor COMMAND TestPointConversor)