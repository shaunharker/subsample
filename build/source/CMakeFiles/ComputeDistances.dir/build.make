# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.3

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
CMAKE_COMMAND = /usr/local/Cellar/cmake/3.3.2/bin/cmake

# The command to remove a file.
RM = /usr/local/Cellar/cmake/3.3.2/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/birdbrain/Documents/Research/Code/clean/subsample

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/birdbrain/Documents/Research/Code/clean/subsample/build

# Include any dependencies generated for this target.
include source/CMakeFiles/ComputeDistances.dir/depend.make

# Include the progress variables for this target.
include source/CMakeFiles/ComputeDistances.dir/progress.make

# Include the compile flags for this target's objects.
include source/CMakeFiles/ComputeDistances.dir/flags.make

source/CMakeFiles/ComputeDistances.dir/ComputeDistances.cpp.o: source/CMakeFiles/ComputeDistances.dir/flags.make
source/CMakeFiles/ComputeDistances.dir/ComputeDistances.cpp.o: ../source/ComputeDistances.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/birdbrain/Documents/Research/Code/clean/subsample/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object source/CMakeFiles/ComputeDistances.dir/ComputeDistances.cpp.o"
	cd /Users/birdbrain/Documents/Research/Code/clean/subsample/build/source && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/ComputeDistances.dir/ComputeDistances.cpp.o -c /Users/birdbrain/Documents/Research/Code/clean/subsample/source/ComputeDistances.cpp

source/CMakeFiles/ComputeDistances.dir/ComputeDistances.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/ComputeDistances.dir/ComputeDistances.cpp.i"
	cd /Users/birdbrain/Documents/Research/Code/clean/subsample/build/source && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /Users/birdbrain/Documents/Research/Code/clean/subsample/source/ComputeDistances.cpp > CMakeFiles/ComputeDistances.dir/ComputeDistances.cpp.i

source/CMakeFiles/ComputeDistances.dir/ComputeDistances.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/ComputeDistances.dir/ComputeDistances.cpp.s"
	cd /Users/birdbrain/Documents/Research/Code/clean/subsample/build/source && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /Users/birdbrain/Documents/Research/Code/clean/subsample/source/ComputeDistances.cpp -o CMakeFiles/ComputeDistances.dir/ComputeDistances.cpp.s

source/CMakeFiles/ComputeDistances.dir/ComputeDistances.cpp.o.requires:

.PHONY : source/CMakeFiles/ComputeDistances.dir/ComputeDistances.cpp.o.requires

source/CMakeFiles/ComputeDistances.dir/ComputeDistances.cpp.o.provides: source/CMakeFiles/ComputeDistances.dir/ComputeDistances.cpp.o.requires
	$(MAKE) -f source/CMakeFiles/ComputeDistances.dir/build.make source/CMakeFiles/ComputeDistances.dir/ComputeDistances.cpp.o.provides.build
.PHONY : source/CMakeFiles/ComputeDistances.dir/ComputeDistances.cpp.o.provides

source/CMakeFiles/ComputeDistances.dir/ComputeDistances.cpp.o.provides.build: source/CMakeFiles/ComputeDistances.dir/ComputeDistances.cpp.o


# Object files for target ComputeDistances
ComputeDistances_OBJECTS = \
"CMakeFiles/ComputeDistances.dir/ComputeDistances.cpp.o"

# External object files for target ComputeDistances
ComputeDistances_EXTERNAL_OBJECTS =

bin/ComputeDistances: source/CMakeFiles/ComputeDistances.dir/ComputeDistances.cpp.o
bin/ComputeDistances: source/CMakeFiles/ComputeDistances.dir/build.make
bin/ComputeDistances: /usr/local/lib/libboost_system-mt.dylib
bin/ComputeDistances: /usr/local/lib/libboost_thread-mt.dylib
bin/ComputeDistances: /usr/local/lib/libboost_serialization-mt.dylib
bin/ComputeDistances: /usr/local/lib/libboost_chrono-mt.dylib
bin/ComputeDistances: /usr/local/Cellar/open-mpi/1.10.0/lib/libmpi_cxx.dylib
bin/ComputeDistances: /usr/local/Cellar/open-mpi/1.10.0/lib/libmpi.dylib
bin/ComputeDistances: lib/libbottleneck.a
bin/ComputeDistances: lib/libANN.a
bin/ComputeDistances: source/CMakeFiles/ComputeDistances.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/birdbrain/Documents/Research/Code/clean/subsample/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable ../bin/ComputeDistances"
	cd /Users/birdbrain/Documents/Research/Code/clean/subsample/build/source && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/ComputeDistances.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
source/CMakeFiles/ComputeDistances.dir/build: bin/ComputeDistances

.PHONY : source/CMakeFiles/ComputeDistances.dir/build

source/CMakeFiles/ComputeDistances.dir/requires: source/CMakeFiles/ComputeDistances.dir/ComputeDistances.cpp.o.requires

.PHONY : source/CMakeFiles/ComputeDistances.dir/requires

source/CMakeFiles/ComputeDistances.dir/clean:
	cd /Users/birdbrain/Documents/Research/Code/clean/subsample/build/source && $(CMAKE_COMMAND) -P CMakeFiles/ComputeDistances.dir/cmake_clean.cmake
.PHONY : source/CMakeFiles/ComputeDistances.dir/clean

source/CMakeFiles/ComputeDistances.dir/depend:
	cd /Users/birdbrain/Documents/Research/Code/clean/subsample/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/birdbrain/Documents/Research/Code/clean/subsample /Users/birdbrain/Documents/Research/Code/clean/subsample/source /Users/birdbrain/Documents/Research/Code/clean/subsample/build /Users/birdbrain/Documents/Research/Code/clean/subsample/build/source /Users/birdbrain/Documents/Research/Code/clean/subsample/build/source/CMakeFiles/ComputeDistances.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : source/CMakeFiles/ComputeDistances.dir/depend

