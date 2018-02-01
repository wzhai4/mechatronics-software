# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.7

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
CMAKE_COMMAND = /home/cos/bin/clion-2017.1/bin/cmake/bin/cmake

# The command to remove a file.
RM = /home/cos/bin/clion-2017.1/bin/cmake/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/cos/pmd-dvrk-nodocker/src/cisst-saw/sawRobotIO1394/components/code/Amp1394

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/cos/pmd-dvrk-nodocker/src/cisst-saw/sawRobotIO1394/components/code/Amp1394/cmake-build-debug

# Include any dependencies generated for this target.
include tests/CMakeFiles/qladisp.dir/depend.make

# Include the progress variables for this target.
include tests/CMakeFiles/qladisp.dir/progress.make

# Include the compile flags for this target's objects.
include tests/CMakeFiles/qladisp.dir/flags.make

tests/CMakeFiles/qladisp.dir/qladisp.cpp.o: tests/CMakeFiles/qladisp.dir/flags.make
tests/CMakeFiles/qladisp.dir/qladisp.cpp.o: ../tests/qladisp.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/cos/pmd-dvrk-nodocker/src/cisst-saw/sawRobotIO1394/components/code/Amp1394/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object tests/CMakeFiles/qladisp.dir/qladisp.cpp.o"
	cd /home/cos/pmd-dvrk-nodocker/src/cisst-saw/sawRobotIO1394/components/code/Amp1394/cmake-build-debug/tests && /usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/qladisp.dir/qladisp.cpp.o -c /home/cos/pmd-dvrk-nodocker/src/cisst-saw/sawRobotIO1394/components/code/Amp1394/tests/qladisp.cpp

tests/CMakeFiles/qladisp.dir/qladisp.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/qladisp.dir/qladisp.cpp.i"
	cd /home/cos/pmd-dvrk-nodocker/src/cisst-saw/sawRobotIO1394/components/code/Amp1394/cmake-build-debug/tests && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/cos/pmd-dvrk-nodocker/src/cisst-saw/sawRobotIO1394/components/code/Amp1394/tests/qladisp.cpp > CMakeFiles/qladisp.dir/qladisp.cpp.i

tests/CMakeFiles/qladisp.dir/qladisp.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/qladisp.dir/qladisp.cpp.s"
	cd /home/cos/pmd-dvrk-nodocker/src/cisst-saw/sawRobotIO1394/components/code/Amp1394/cmake-build-debug/tests && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/cos/pmd-dvrk-nodocker/src/cisst-saw/sawRobotIO1394/components/code/Amp1394/tests/qladisp.cpp -o CMakeFiles/qladisp.dir/qladisp.cpp.s

tests/CMakeFiles/qladisp.dir/qladisp.cpp.o.requires:

.PHONY : tests/CMakeFiles/qladisp.dir/qladisp.cpp.o.requires

tests/CMakeFiles/qladisp.dir/qladisp.cpp.o.provides: tests/CMakeFiles/qladisp.dir/qladisp.cpp.o.requires
	$(MAKE) -f tests/CMakeFiles/qladisp.dir/build.make tests/CMakeFiles/qladisp.dir/qladisp.cpp.o.provides.build
.PHONY : tests/CMakeFiles/qladisp.dir/qladisp.cpp.o.provides

tests/CMakeFiles/qladisp.dir/qladisp.cpp.o.provides.build: tests/CMakeFiles/qladisp.dir/qladisp.cpp.o


# Object files for target qladisp
qladisp_OBJECTS = \
"CMakeFiles/qladisp.dir/qladisp.cpp.o"

# External object files for target qladisp
qladisp_EXTERNAL_OBJECTS =

tests/qladisp: tests/CMakeFiles/qladisp.dir/qladisp.cpp.o
tests/qladisp: tests/CMakeFiles/qladisp.dir/build.make
tests/qladisp: lib/libAmp1394.a
tests/qladisp: /usr/lib/x86_64-linux-gnu/libcurses.so
tests/qladisp: /usr/lib/x86_64-linux-gnu/libform.so
tests/qladisp: tests/CMakeFiles/qladisp.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/cos/pmd-dvrk-nodocker/src/cisst-saw/sawRobotIO1394/components/code/Amp1394/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable qladisp"
	cd /home/cos/pmd-dvrk-nodocker/src/cisst-saw/sawRobotIO1394/components/code/Amp1394/cmake-build-debug/tests && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/qladisp.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
tests/CMakeFiles/qladisp.dir/build: tests/qladisp

.PHONY : tests/CMakeFiles/qladisp.dir/build

tests/CMakeFiles/qladisp.dir/requires: tests/CMakeFiles/qladisp.dir/qladisp.cpp.o.requires

.PHONY : tests/CMakeFiles/qladisp.dir/requires

tests/CMakeFiles/qladisp.dir/clean:
	cd /home/cos/pmd-dvrk-nodocker/src/cisst-saw/sawRobotIO1394/components/code/Amp1394/cmake-build-debug/tests && $(CMAKE_COMMAND) -P CMakeFiles/qladisp.dir/cmake_clean.cmake
.PHONY : tests/CMakeFiles/qladisp.dir/clean

tests/CMakeFiles/qladisp.dir/depend:
	cd /home/cos/pmd-dvrk-nodocker/src/cisst-saw/sawRobotIO1394/components/code/Amp1394/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/cos/pmd-dvrk-nodocker/src/cisst-saw/sawRobotIO1394/components/code/Amp1394 /home/cos/pmd-dvrk-nodocker/src/cisst-saw/sawRobotIO1394/components/code/Amp1394/tests /home/cos/pmd-dvrk-nodocker/src/cisst-saw/sawRobotIO1394/components/code/Amp1394/cmake-build-debug /home/cos/pmd-dvrk-nodocker/src/cisst-saw/sawRobotIO1394/components/code/Amp1394/cmake-build-debug/tests /home/cos/pmd-dvrk-nodocker/src/cisst-saw/sawRobotIO1394/components/code/Amp1394/cmake-build-debug/tests/CMakeFiles/qladisp.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : tests/CMakeFiles/qladisp.dir/depend

