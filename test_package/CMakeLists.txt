cmake_minimum_required(VERSION 3.16)
project(LassTest)

find_package(Lass REQUIRED)

add_executable(test_package main.cpp)

target_link_libraries(test_package
    PRIVATE
        Lass::lass_python  # also links to Python::Module
        Python::Python     # so we to add Python::Python for embedding
)
