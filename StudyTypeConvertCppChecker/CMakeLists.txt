cmake_minimum_required(VERSION 3.5)

project(StudyTypeConvertCppChecker LANGUAGES C )

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(StudyTypeConvertCppChecker main.c )

install(TARGETS StudyTypeConvertCppChecker
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
