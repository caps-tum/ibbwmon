cmake_minimum_required(VERSION 3.1)

########
# If the user specifies -DCMAKE_BUILD_TYPE on the command line, take their definition
# and dump it in the cache along with proper documentation, otherwise set CMAKE_BUILD_TYPE
# to Debug prior to calling PROJECT()
IF(DEFINED CMAKE_BUILD_TYPE)
   SET(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING "Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel.")
ELSE()
   SET(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel.")
ENDIF()
string( TOLOWER "${CMAKE_BUILD_TYPE}" CMAKE_BUILD_TYPE )
########

########
# General project settings
project(ibbwmon)
# Enable support for external projects
include(ExternalProject)
########

########
# Compiler/linker options based on the different compilers
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  # Using Clang
  if ("${CMAKE_BUILD_TYPE}" STREQUAL "release")
    ADD_DEFINITIONS(-g -Ofast -march=native)
  endif()

  if ("${CMAKE_BUILD_TYPE}" STREQUAL "debug")
    ADD_DEFINITIONS(-g3 -O0 -Weverything)

    ADD_DEFINITIONS(-Wno-c++98-compat -Wno-shadow -Wno-c++98-compat-pedantic -Wno-padded -Wno-weak-vtables -Wno-extra-semi -Wno-exit-time-destructors -Wno-global-constructors)
  endif()



elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  # Using GCC
  if ("${CMAKE_BUILD_TYPE}" STREQUAL "release")
    ADD_DEFINITIONS(-g -O2 -march=native)
  endif()

  if ("${CMAKE_BUILD_TYPE}" STREQUAL "debug")
    ADD_DEFINITIONS(-g3 -O0 -Wall -Wextra -Wunused)
  endif()

elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
  # Using Intel C++
  if ("${CMAKE_BUILD_TYPE}" STREQUAL "release")
    ADD_DEFINITIONS(-g -fast)
  endif()

  if ("${CMAKE_BUILD_TYPE}" STREQUAL "debug")
    ADD_DEFINITIONS(-g3 -O0 -Wall -Wextra)
  endif()

endif()
########

########
# Threads
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
########

########
# MPI
find_package(MPI REQUIRED)
include_directories(${MPI_INCLUDE_PATH})
########

include_directories(include)

########
# Compiling and linking
add_executable(ibbwmon src/ibbwmon.cpp)
target_link_libraries(ibbwmon ${MPI_C_LIBRARIES})
set_property(TARGET ibbwmon PROPERTY CXX_STANDARD 14)
########
