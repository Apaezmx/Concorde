# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.12

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /usr/local/google/home/paezmartinez/prototypes/c++/Concorde

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /usr/local/google/home/paezmartinez/prototypes/c++/Concorde

# Include any dependencies generated for this target.
include CMakeFiles/ConcordeLib.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/ConcordeLib.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/ConcordeLib.dir/flags.make

CMakeFiles/ConcordeLib.dir/src/concorde.cc.o: CMakeFiles/ConcordeLib.dir/flags.make
CMakeFiles/ConcordeLib.dir/src/concorde.cc.o: src/concorde.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/usr/local/google/home/paezmartinez/prototypes/c++/Concorde/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/ConcordeLib.dir/src/concorde.cc.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/ConcordeLib.dir/src/concorde.cc.o -c /usr/local/google/home/paezmartinez/prototypes/c++/Concorde/src/concorde.cc

CMakeFiles/ConcordeLib.dir/src/concorde.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/ConcordeLib.dir/src/concorde.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /usr/local/google/home/paezmartinez/prototypes/c++/Concorde/src/concorde.cc > CMakeFiles/ConcordeLib.dir/src/concorde.cc.i

CMakeFiles/ConcordeLib.dir/src/concorde.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/ConcordeLib.dir/src/concorde.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /usr/local/google/home/paezmartinez/prototypes/c++/Concorde/src/concorde.cc -o CMakeFiles/ConcordeLib.dir/src/concorde.cc.s

# Object files for target ConcordeLib
ConcordeLib_OBJECTS = \
"CMakeFiles/ConcordeLib.dir/src/concorde.cc.o"

# External object files for target ConcordeLib
ConcordeLib_EXTERNAL_OBJECTS =

libConcordeLib.a: CMakeFiles/ConcordeLib.dir/src/concorde.cc.o
libConcordeLib.a: CMakeFiles/ConcordeLib.dir/build.make
libConcordeLib.a: CMakeFiles/ConcordeLib.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/usr/local/google/home/paezmartinez/prototypes/c++/Concorde/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX static library libConcordeLib.a"
	$(CMAKE_COMMAND) -P CMakeFiles/ConcordeLib.dir/cmake_clean_target.cmake
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/ConcordeLib.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/ConcordeLib.dir/build: libConcordeLib.a

.PHONY : CMakeFiles/ConcordeLib.dir/build

CMakeFiles/ConcordeLib.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/ConcordeLib.dir/cmake_clean.cmake
.PHONY : CMakeFiles/ConcordeLib.dir/clean

CMakeFiles/ConcordeLib.dir/depend:
	cd /usr/local/google/home/paezmartinez/prototypes/c++/Concorde && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /usr/local/google/home/paezmartinez/prototypes/c++/Concorde /usr/local/google/home/paezmartinez/prototypes/c++/Concorde /usr/local/google/home/paezmartinez/prototypes/c++/Concorde /usr/local/google/home/paezmartinez/prototypes/c++/Concorde /usr/local/google/home/paezmartinez/prototypes/c++/Concorde/CMakeFiles/ConcordeLib.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/ConcordeLib.dir/depend

