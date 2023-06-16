cmake_minimum_required(VERSION 3.5)

project(StudyStrArrayAndAcces LANGUAGES C)

add_executable(StudyStrArrayAndAcces main.c)

install(TARGETS StudyStrArrayAndAcces
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
