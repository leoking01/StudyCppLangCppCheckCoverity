cmake_minimum_required(VERSION 3.5)

project(StudyCLang LANGUAGES C)

add_executable(StudyCLang main.c)

install(TARGETS StudyCLang
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
