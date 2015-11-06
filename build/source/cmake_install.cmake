# Install script for directory: /Users/birdbrain/Documents/Research/Code/subsample/source

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/Users/birdbrain/Documents/Research/Code/subsample/bin/ComputeSubsample")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/Users/birdbrain/Documents/Research/Code/subsample/bin" TYPE EXECUTABLE FILES "/Users/birdbrain/Documents/Research/Code/subsample/build/bin/ComputeSubsample")
  if(EXISTS "$ENV{DESTDIR}/Users/birdbrain/Documents/Research/Code/subsample/bin/ComputeSubsample" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/Users/birdbrain/Documents/Research/Code/subsample/bin/ComputeSubsample")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/opt/local/lib"
      "$ENV{DESTDIR}/Users/birdbrain/Documents/Research/Code/subsample/bin/ComputeSubsample")
    execute_process(COMMAND /usr/bin/install_name_tool
      -add_rpath "/usr/local/lib"
      "$ENV{DESTDIR}/Users/birdbrain/Documents/Research/Code/subsample/bin/ComputeSubsample")
    execute_process(COMMAND /usr/bin/install_name_tool
      -add_rpath "/opt/local/lib"
      "$ENV{DESTDIR}/Users/birdbrain/Documents/Research/Code/subsample/bin/ComputeSubsample")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" "$ENV{DESTDIR}/Users/birdbrain/Documents/Research/Code/subsample/bin/ComputeSubsample")
    endif()
  endif()
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/Users/birdbrain/Documents/Research/Code/subsample/bin/ComputeDistances")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/Users/birdbrain/Documents/Research/Code/subsample/bin" TYPE EXECUTABLE FILES "/Users/birdbrain/Documents/Research/Code/subsample/build/bin/ComputeDistances")
  if(EXISTS "$ENV{DESTDIR}/Users/birdbrain/Documents/Research/Code/subsample/bin/ComputeDistances" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/Users/birdbrain/Documents/Research/Code/subsample/bin/ComputeDistances")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/opt/local/lib"
      "$ENV{DESTDIR}/Users/birdbrain/Documents/Research/Code/subsample/bin/ComputeDistances")
    execute_process(COMMAND /usr/bin/install_name_tool
      -add_rpath "/usr/local/lib"
      "$ENV{DESTDIR}/Users/birdbrain/Documents/Research/Code/subsample/bin/ComputeDistances")
    execute_process(COMMAND /usr/bin/install_name_tool
      -add_rpath "/opt/local/lib"
      "$ENV{DESTDIR}/Users/birdbrain/Documents/Research/Code/subsample/bin/ComputeDistances")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" "$ENV{DESTDIR}/Users/birdbrain/Documents/Research/Code/subsample/bin/ComputeDistances")
    endif()
  endif()
endif()

