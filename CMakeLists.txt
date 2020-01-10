# 
#          Copyright (C) 2019 Clovis Durand <cd.clovel19@gmail.com>
# 
# -----------------------------------------------------------------------------

# CMake version required ----------------------------------
cmake_minimum_required(VERSION 3.0)
project(LTBL)

if(NOT DEFINED PRJ_VERSION)
    set(PRJ_VERSION "1.0.0")
endif(NOT DEFINED PRJ_VERSION)

# Build type
if(NOT CMAKE_BUILD_TYPE)
set(CMAKE_BUILD_TYPE Debug CACHE STRING
    "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
    FORCE)
endif(NOT CMAKE_BUILD_TYPE)

# CASE OF C PROJECT
set(CMAKE_C_STANDARD 99)
if(NOT CMAKE_C_FLAGS)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wformat-security" CACHE STRING "C99 compilation flags" FORCE)
endif(NOT CMAKE_C_FLAGS)

# CASE OF CPP PROJECT
set(CMAKE_CXX_STANDARD 17)
if(NOT CMAKE_CXX_FLAGS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wformat-security" CACHE STRING "C++11 compilation flags" FORCE)
endif(NOT CMAKE_CXX_FLAGS)

#------------------------------------------------------------------------------
# Project definition, variable and dependencies 
#------------------------------------------------------------------------------

set(CMAKE_PROJECT_NAME ${PROJECT_NAME})
set(CMAKE_SYSTEM_NAME ESP8266)

set(CMAKE_PROJECT_BRIEF "LTBL")

# Allow CTest
enable_testing()

find_package(Doxygen)
option(ENABLE_DOCS "Build API documentation" ${DOXYGEN_FOUND})

#------------------------------------------------------------------------------
# Project version
#------------------------------------------------------------------------------

# Get the latest abbreviated commit hash of the working branch
execute_process(
    COMMAND git log -1 --format=%h
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_COMMIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
  
string(REGEX REPLACE
    "^([\\.0-9]+).*"
    "\\1"
    tmp_VERSION
    "${PRJ_VERSION}"
)
  
string(REGEX REPLACE
    "^([0-9]+)\\..*"
    "\\1"
    tmp_SOVERSION
    "${tmp_VERSION}"
)

# Major version is for product/marketing purpose
# Minor version is incremented everytime it gets public
set(MAJ_MIN_VERSION ${tmp_VERSION} CACHE STRING "Major and minor version" FORCE)

# Release version is incremented everytime it gets use inside the company
set(RELEASE_VERSION ${tmp_SOVERSION} CACHE STRING "Release version" FORCE)

# Build version is generated using the hash of the current commit build with
set(BUILD_VERSION ${GIT_COMMIT_HASH} CACHE STRING "Build version" FORCE)

#------------------------------------------------------------------------------
# pkgconfig
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
# Project configuration 
#------------------------------------------------------------------------------

add_definitions(-DUNIX)

#------------------------------------------------------------------------------
# Sub-directories 
#------------------------------------------------------------------------------

# Main build
# add_subdirectory(src) # TODO : Build main project w/ CMake

add_subdirectory(software/tests)
add_subdirectory(software/docs)
add_subdirectory(examples)
