#
#  CMakeLists.txt
#  Concorde
#
#  Created by Andres Paez Martinez on 3/14/19.
#  Copyright © 2019 Concorde. All rights reserved.
#

# Compiler flags.
set(CMAKE_CXX_FLAGS -pthread)

cmake_minimum_required (VERSION 2.6)
project (Concorde)
# The version number.
set (Concorde_VERSION_MAJOR 0)
set (Concorde_VERSION_MINOR 1)
 
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

# add the install targets
install (TARGETS Concorde DESTINATION bin)
install (FILES "${PROJECT_BINARY_DIR}/src/concorde.h" DESTINATION include)
INSTALL ( DIRECTORY src/ DESTINATION include FILES_MATCHING PATTERN "*.h")

################################
# GTest
################################
ADD_SUBDIRECTORY (googletest-master)
enable_testing()
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

################################
# Unit Tests
################################
# Add test cpp file
#add_executable( runUnitTests tutorial_test.cc )
# Link test executable against gtest & gtest_main
#target_link_libraries(runUnitTests gtest gtest_main)
#target_link_libraries(runUnitTests ${EXTRA_LIBS})
#add_test( runUnitTests runUnitTests )


