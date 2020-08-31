cmake_minimum_required(VERSION 3.13.0)

project(
    CorgiECS
    DESCRIPTION "Library for ECS things "
    LANGUAGES CXX
    VERSION 1.0.0
    HOMEPAGE_URL "http://github.com/ProfesseurPoire/CorgiECS")

if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

set(CMAKE_DEBUG_POSTFIX d)

# We'll install the library in lib64 if we're in x64, in lib otherwise
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(arch_suffix "64")
endif()

add_library(${PROJECT_NAME} "")

set_property(GLOBAL PROPERTY FIND_LIBRARY_USE_LIB64_PATHS TRUE)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)

add_subdirectory(src)

target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:../>)

find_package(CorgiContainers)
find_package(CorgiLogger)
find_package(CorgiMemory)

target_link_libraries(${PROJECT_NAME} CorgiContainers CorgiLogger CorgiMemory)

## TESTS

add_subdirectory(tests)