#!/bin/bash

#-----------------------------------------------------------------------------
# Simple build script with options
#-----------------------------------------------------------------------------

# Directory for KokkosArray

KOKKOSARRAY="../.."

source ${KOKKOSARRAY}/src/build_common.sh

# Process command line options and set compilation variables
#
# INC_PATH     : required include paths
# CXX          : C++ compiler with compiler-specific options
# CXX_SOURCES  : C++ files for host
# NVCC         : Cuda compiler with compiler-specific options
# NVCC_SOURCES : Cuda sources

CXX_OPTIONS=""

#-----------------------------------------------------------------------------
# Add local source code:

EXECUTABLE="query_configuration.exe"

INC_PATH="${INC_PATH} -I. -I../common"

CXX_SOURCES="${CXX_SOURCES} query_configuration.cpp"

#-----------------------------------------------------------------------------

if [ -n "${NVCC}" ] ;
then
  ${NVCC} ${INC_PATH} ${NVCC_SOURCES}
fi

#-----------------------------------------------------------------------------

echo ${CXX} ${CXX_OPTIONS} ${INC_PATH} -o ${EXECUTABLE} ${CXX_SOURCES} ${LIB}

${CXX} ${CXX_OPTIONS} ${INC_PATH} -o ${EXECUTABLE} ${CXX_SOURCES} ${LIB}

rm -f *.o *.a

#-----------------------------------------------------------------------------

