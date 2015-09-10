# @HEADER
# ************************************************************************
#
#            TriBITS: Tribial Build, Integrate, and Test System
#                    Copyright 2013 Sandia Corporation
#
#
# Under the terms of Contract DE-AC04-94AL85000 with Sandia Corporation,
# the U.S. Government retains certain rights in this software.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are
# met:
#
# 1. Redistributions of source code must retain the above copyright
# notice, this list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright
# notice, this list of conditions and the following disclaimer in the
# documentation and/or other materials provided with the distribution.
#
# 3. Neither the name of the Corporation nor the names of the
# contributors may be used to endorse or promote products derived from
# this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY SANDIA CORPORATION "AS IS" AND ANY
# EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
# PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL SANDIA CORPORATION OR THE
# CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
# EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
# PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
# PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
# LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
# NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# ************************************************************************
# @HEADER

##############################################################################
#
# CMake variable for use by Trilinos/Galeri clients. 
#
# Do not edit: This file was generated automatically by CMake.
#
##############################################################################

#
# Make sure CMAKE_CURRENT_LIST_DIR is usable
#

IF (NOT DEFINED CMAKE_CURRENT_LIST_DIR)
  GET_FILENAME_COMPONENT(_THIS_SCRIPT_PATH ${CMAKE_CURRENT_LIST_FILE} PATH)
  SET(CMAKE_CURRENT_LIST_DIR ${_THIS_SCRIPT_PATH})
ENDIF()


## ---------------------------------------------------------------------------
## Compilers used by Trilinos/Galeri build
## ---------------------------------------------------------------------------

SET(Galeri_CXX_COMPILER "/usr/lib64/openmpi/bin/mpicxx")

SET(Galeri_C_COMPILER "/usr/lib64/openmpi/bin/mpicc")

SET(Galeri_FORTRAN_COMPILER "/usr/lib64/openmpi/bin/mpif90")


## ---------------------------------------------------------------------------
## Compiler flags used by Trilinos/Galeri build
## ---------------------------------------------------------------------------

## Set compiler flags, including those determined by build type
SET(Galeri_CXX_FLAGS " -O3")

SET(Galeri_C_FLAGS " -O3")

SET(Galeri_FORTRAN_FLAGS " -O3")

## Extra link flags (e.g., specification of fortran libraries)
SET(Galeri_EXTRA_LD_FLAGS "-lrt")

## This is the command-line entry used for setting rpaths. In a build
## with static libraries it will be empty. 
SET(Galeri_SHARED_LIB_RPATH_COMMAND "")
SET(Galeri_BUILD_SHARED_LIBS "OFF")

SET(Galeri_LINKER /usr/bin/ld)
SET(Galeri_AR /usr/bin/ar)

## ---------------------------------------------------------------------------
## Set library specifications and paths 
## ---------------------------------------------------------------------------

## List of package include dirs
SET(Galeri_INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}/../../../include")

## List of package library paths
SET(Galeri_LIBRARY_DIRS "${CMAKE_CURRENT_LIST_DIR}/../../../lib")

## List of package libraries
SET(Galeri_LIBRARIES "galeri-xpetra;galeri;xpetra-sup;xpetra-ext;xpetra;epetraext;tpetraext;tpetrainout;tpetra;triutils;epetra;kokkosdisttsqr;kokkosnodetsqr;kokkoslinalg;kokkosnodeapi;kokkos;tpi;teuchosremainder;teuchosnumerics;teuchoscomm;teuchosparameterlist;teuchoscore")

## Specification of directories for TPL headers
SET(Galeri_TPL_INCLUDE_DIRS "/usr/include;/usr/include/suitesparse;/usr/include/suitesparse;/usr/include")

## Specification of directories for TPL libraries
SET(Galeri_TPL_LIBRARY_DIRS "")

## List of required TPLs
SET(Galeri_TPL_LIBRARIES "/usr/lib64/libhdf5.so;/usr/lib64/libz.so;/usr/lib64/libamd.so;/usr/lib64/libumfpack.so;/usr/lib64/libamd.so;/usr/lib64/liblapack.so;/usr/lib64/libblas.so")

## ---------------------------------------------------------------------------
## MPI specific variables
##   These variables are provided to make it easier to get the mpi libraries
##   and includes on systems that do not use the mpi wrappers for compiling
## ---------------------------------------------------------------------------

SET(Galeri_MPI_LIBRARIES "")
SET(Galeri_MPI_LIBRARY_DIRS "")
SET(Galeri_MPI_INCLUDE_DIRS "")
SET(Galeri_MPI_EXEC "/usr/bin/mpiexec")
SET(Galeri_MPI_EXEC_MAX_NUMPROCS "4")
SET(Galeri_MPI_EXEC_NUMPROCS_FLAG "-np")

## ---------------------------------------------------------------------------
## Set useful general variables 
## ---------------------------------------------------------------------------

## The packages enabled for this project
SET(Galeri_PACKAGE_LIST "Galeri;Xpetra;EpetraExt;Tpetra;Triutils;Epetra;KokkosArray;KokkosClassic;ThreadPool;Teuchos;TeuchosRemainder;TeuchosNumerics;TeuchosComm;TeuchosParameterList;TeuchosCore")

## The TPLs enabled for this project
SET(Galeri_TPL_LIST "HDF5;AMD;UMFPACK;Boost;LAPACK;BLAS;MPI;Pthread")


# Import Trilinos targets
IF(NOT Trilinos_TARGETS_IMPORTED)
  SET(Trilinos_TARGETS_IMPORTED 1)
  INCLUDE("${CMAKE_CURRENT_LIST_DIR}/../Trilinos/TrilinosTargets.cmake")
ENDIF()

