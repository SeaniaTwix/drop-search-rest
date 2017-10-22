cmake_minimum_required(VERSION 3.8)
project(maple_search_rest)

include_directories(/usr/local/include)
link_directories("/usr/local/lib"
        "/usr/local/Cellar/mariadb-connector-c/2.2.2/lib/mariadb")
link_libraries("libre2.a"
        "libserved.dylib"
        "libboost_system.dylib"
        "libboost_filesystem.dylib"
        "libpugixml.a"
        "libmariadb.dylib")

set(CMAKE_CXX_STANDARD 17)

set(SOURCE_FILES main.cpp Server.cpp Server.hpp INIReader.h)
add_executable(maple_search_rest ${SOURCE_FILES})