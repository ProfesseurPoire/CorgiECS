cmake_minimum_required(VERSION 3.13.0) # need 3.13 for target_sources

project(TestECS CXX)

add_executable(${PROJECT_NAME} "")

target_include_directories(${PROJECT_NAME} PRIVATE ./)

target_sources(${PROJECT_NAME} PRIVATE
    unit/TestECS.cpp 
    unit/TestEntity.cpp)

find_package(CorgiTest)

target_link_libraries(${PROJECT_NAME} PRIVATE CorgiECS CorgiTest)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)

add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})