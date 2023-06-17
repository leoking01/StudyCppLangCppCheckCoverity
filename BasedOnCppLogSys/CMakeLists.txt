cmake_minimum_required(VERSION 3.5)

project(BasedOnCppLogSys LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories( "D:\\win10\\aspDep14\\zlib\\zlib_1.2.11.0_MSVC_19.0.24210.0_install\\include" )
link_directories( "D:\\win10\\aspDep14\\zlib\\zlib_1.2.11.0_MSVC_19.0.24210.0_install\\lib" )
add_executable(BasedOnCppLogSys main.cpp  logfile.h logfile.cpp )
target_link_libraries( BasedOnCppLogSys  zlib  )

install(TARGETS BasedOnCppLogSys
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
