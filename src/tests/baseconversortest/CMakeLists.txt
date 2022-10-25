find_package(Qt5Test REQUIRED)

add_executable(TestBaseConversor 
tst_baseconversortest.cpp
../../core/baseconversor.cpp 
../../core/invalidconversorinput.cpp     
)

target_include_directories(TestBaseConversor PRIVATE ../../core)
target_link_libraries(TestBaseConversor PRIVATE Qt5::Test)


add_test(NAME TestBaseConversor COMMAND TestBaseConversor)
