# CMake generated Testfile for 
# Source directory: /Users/paezand/Concorde
# Build directory: /Users/paezand/Concorde
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(tests "tests")
set_tests_properties(tests PROPERTIES  _BACKTRACE_TRIPLES "/Users/paezand/Concorde/CMakeLists.txt;69;add_test;/Users/paezand/Concorde/CMakeLists.txt;0;")
subdirs("curlpp")
subdirs("googletest")
