# 
#        Copyright (C) 2019 Clovis Durand <cd.clovel19²gmail.com>
# 
# -----------------------------------------------------------------------------

set(CMAKE_PROJECT_NAME logger-example)
set(CMAKE_SYSTEM_NAME Linux)

set(CMAKE_PROJECT_BRIEF "Logger class example")

#------------------------------------------------------------------------------
# Target configuration for tests
#------------------------------------------------------------------------------

# Header files --------------------------------------------
include_directories(
    ${CMAKE_SOURCE_DIR}/software/inc
    ${CMAKE_SOURCE_DIR}/examples/logger_example
)

file(GLOB EXAMPLE_HEADERS
    ${CMAKE_SOURCE_DIR}/examples/logger_example/*.h
    ${CMAKE_SOURCE_DIR}/examples/logger_example/*.hpp
)

set(HEADERS
    ${CMAKE_SOURCE_DIR}/software/inc/Logger.hpp
    ${CMAKE_SOURCE_DIR}/software/inc/def.h
    ${EXAMPLE_HEADERS}
)

# Source files --------------------------------------------
file(GLOB EXAMPLE_SOURCES
    ${CMAKE_SOURCE_DIR}/examples/logger_example/*.c
    ${CMAKE_SOURCE_DIR}/examples/logger_example/*.cpp
)

set(SOURCES
    ${CMAKE_SOURCE_DIR}/software/src/Logger.cpp
    ${EXAMPLE_SOURCES}
)

# Target definition ---------------------------------------
add_definitions(-DTESTS)

add_executable(logger-example
    ${SOURCES}
)
