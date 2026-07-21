cmake_minimum_required(VERSION 3.20)

project(
    seraphic_phoenix
    VERSION 0.0.0 
    DESCRIPTION "Swiss army knife for C"
    LANGUAGES C
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(CTest)
include(CMakePackageConfigHelpers)

# Gestione della libreria principale
add_subdirectory(src)
add_subdirectory(test)