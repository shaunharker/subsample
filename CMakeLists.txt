
cmake_minimum_required (VERSION 2.6) 
project (Subsample) 

#########
# RPATH #
#########

set(CMAKE_MACOSX_RPATH 1)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

#########
# C++11 #
#########

if( CMAKE_COMPILER_IS_GNUCXX )
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O3 -DNDEBUG")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -stdlib=libc++ -O3 -DNDEBUG")
endif()

MESSAGE( STATUS "Chose the following prefix: " ${MYPREFIX} )

############
# Packages #
############

find_package(Boost 1.58.0 REQUIRED COMPONENTS system thread serialization chrono )
find_package(MPI REQUIRED)

#######################
# Include Directories #
#######################

include_directories ( 
  ./include 
  /usr/local/include 
  /opt/local/include 
  ${Boost_INCLUDE_DIRS} 
  ${MPI_INCLUDE_PATH}
  ${USER_INCLUDE_PATH} )

#######################
# Library Directories #
#######################

link_directories ( 
  ${USER_LIBRARY_PATH} 
  /usr/local/lib 
  /opt/local/lib )

###########
# Recurse #
###########

add_subdirectory (source)

