cmake_minimum_required(VERSION 3.5)
project(hidracpp VERSION 1.0 LANGUAGES CXX)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt required tools
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Set prefix for find_package
set(CMAKE_PREFIX_PATH "Path/to/Qt")

enable_testing(true)

add_subdirectory(src)
add_subdirectory(src/tests)