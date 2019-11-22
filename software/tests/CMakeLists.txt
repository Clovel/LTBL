# 
#        Copyright (C) 2019 Clovis Durand <cd.clovel19²gmail.com>
# 
# -----------------------------------------------------------------------------

set(CMAKE_PROJECT_NAME ${PROJECT_NAME}_tests)
set(CMAKE_SYSTEM_NAME Linux)

set(CMAKE_PROJECT_BRIEF "LTBL tests")

#------------------------------------------------------------------------------
# Target configuration for tests
#------------------------------------------------------------------------------

# Header files --------------------------------------------
file(GLOB PUBLIC_HEADERS ${CMAKE_SOURCE_DIR}/software/inc/*.h ${CMAKE_SOURCE_DIR}/software/inc/*.hpp)
file(GLOB PRIVATE_HEADERS ${CMAKE_SOURCE_DIR}/software/src/*.h ${CMAKE_SOURCE_DIR}/software/src/*.hpp)

set(HEADERS 
    ${PUBLIC_HEADERS}
    ${PRIVATE_HEADERS}
)

include_directories(
    ${CMAKE_SOURCE_DIR}/software/tests
    ${CMAKE_SOURCE_DIR}/software/inc
    ${CMAKE_SOURCE_DIR}/software/src
)

# Source files --------------------------------------------
file(GLOB SOURCES ${CMAKE_SOURCE_DIR}/software/src/*.c ${CMAKE_SOURCE_DIR}/software/src/*.cpp)
file(GLOB TEST_SOURCES ${CMAKE_SOURCE_DIR}/software/tests/*.c ${CMAKE_SOURCE_DIR}/software/tests/*.cpp)

list(REMOVE_ITEM SOURCES ${CMAKE_SOURCE_DIR}/software/src/main.cpp)

# Target definition ---------------------------------------
add_definitions(-DTESTS)

# add_test(testname Exename arg1 arg2 ...)
add_test(test_default tests_${PROJECT_NAME} -1)
add_test(test_Relay_isOn tests_${PROJECT_NAME} 0)

add_executable(tests_${PROJECT_NAME}
    ${TEST_SOURCES}
    ${SOURCES}
)
