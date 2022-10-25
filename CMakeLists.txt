cmake_minimum_required(VERSION 3.5)
project(hidracpp VERSION 1.0 LANGUAGES CXX)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt required tools
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Can be set through the command line using -DQT_PATH={your path to qt}
if (NOT DEFINED QT_PATH)
    # Default Qt path
    set(QT_PATH "C:\\Qt\\5.15.2\\mingw81_64")
endif()
message(STATUS "Qt Path is set to " ${QT_PATH})


# Set prefix for find_package
set(CMAKE_PREFIX_PATH ${QT_PATH})

enable_testing(true)

add_subdirectory(src)
add_subdirectory(src/tests)