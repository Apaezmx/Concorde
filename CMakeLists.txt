#
#  CMakeLists.txt
#  Concorde
#
#  Created by Andres Paez Martinez on 3/14/19.
#  Copyright © 2019 Concorde. All rights reserved.
#

# Compiler flags.
set(CMAKE_CXX_FLAGS -pthread)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
cmake_minimum_required (VERSION 2.6)
project (Concorde)
# The version number.
set (Concorde_VERSION_MAJOR 0)
set (Concorde_VERSION_MINOR 1)

add_subdirectory(curlpp)

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
find_package( Boost COMPONENTS program_options REQUIRED )
include_directories( ${Boost_INCLUDE_DIR} )

# add the binary tree to the search path for include files
# so that we will find concorde.h
include_directories("${PROJECT_BINARY_DIR}")
 
# For adding other libs
#  include_directories ("${PROJECT_SOURCE_DIR}/MathFunctions")
#  add_subdirectory (MathFunctions)
#  set (EXTRA_LIBS ${EXTRA_LIBS} MathFunctions)

add_library(ConcordeLib src/concorde.cc)
set (EXTRA_LIBS ${EXTRA_LIBS} ConcordeLib)


# add the executable
add_executable(Concorde src/main.cc)
target_link_libraries(Concorde ${EXTRA_LIBS})
target_link_libraries(Concorde LINK_PUBLIC ${Boost_LIBRARIES} )

# add the install targets
install (TARGETS Concorde DESTINATION bin)
install (FILES "${PROJECT_BINARY_DIR}/src/concorde.h" DESTINATION include)
INSTALL ( DIRECTORY src/ DESTINATION include FILES_MATCHING PATTERN "*.h")

################################
# GTest
################################
add_SUBDIRECTORY (googletest)
enable_testing()
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
include_directories(${curlpp_SOURCE_DIR}/include ${curlpp_SOURCE_DIR})

set (EXTRA_TEST_LIBS ${EXTRA_TEST_LIBS} curlpp)


################################
# Unit Tests
################################
# Add test cpp file
add_executable(tests src/concorde_test.cc )
# Link test executable against gtest & gtest_main
target_link_libraries(tests gtest gtest_main)
target_link_libraries(tests ${EXTRA_LIBS})
target_link_libraries(tests ${EXTRA_TEST_LIBS})
add_test(tests tests)


