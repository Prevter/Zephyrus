cmake_minimum_required(VERSION 3.15)

project(Zephyrus LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE ZEPHYRUS_HEADERS ${PROJECT_SOURCE_DIR}/include/*.hpp)
file(GLOB_RECURSE ZEPHYRUS_SOURCES
    ${PROJECT_SOURCE_DIR}/src/*.cpp
    ${PROJECT_SOURCE_DIR}/thirdparty/json.hpp
)

add_library(Zephyrus STATIC ${ZEPHYRUS_HEADERS} ${ZEPHYRUS_SOURCES})

target_include_directories(Zephyrus PUBLIC ${PROJECT_SOURCE_DIR}/include)
