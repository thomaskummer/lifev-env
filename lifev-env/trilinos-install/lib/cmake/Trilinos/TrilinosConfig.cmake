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
# CMake variable for use by Trilinos clients. 
#
# Do not edit: This file was generated automatically by CMake.
#
##############################################################################

#
# Ensure CMAKE_CURRENT_LIST_DIR is usable.
#

IF (NOT DEFINED CMAKE_CURRENT_LIST_DIR)
  GET_FILENAME_COMPONENT(_THIS_SCRIPT_PATH ${CMAKE_CURRENT_LIST_FILE} PATH)
  SET(CMAKE_CURRENT_LIST_DIR ${_THIS_SCRIPT_PATH})
ENDIF()


## ---------------------------------------------------------------------------
## Compilers used by Trilinos build
## ---------------------------------------------------------------------------

SET(Trilinos_CXX_COMPILER "/usr/lib64/openmpi/bin/mpicxx")

SET(Trilinos_C_COMPILER "/usr/lib64/openmpi/bin/mpicc")

SET(Trilinos_Fortran_COMPILER "/usr/lib64/openmpi/bin/mpif90")

## ---------------------------------------------------------------------------
## Compiler flags used by Trilinos build
## ---------------------------------------------------------------------------

SET(Trilinos_CXX_COMPILER_FLAGS " -O3")

SET(Trilinos_C_COMPILER_FLAGS " -O3")

SET(Trilinos_Fortran_COMPILER_FLAGS " -O3")

## Extra link flags (e.g., specification of fortran libraries)
SET(Trilinos_EXTRA_LD_FLAGS "-lrt")

## This is the command-line entry used for setting rpaths. In a build
## with static libraries it will be empty. 
SET(Trilinos_SHARED_LIB_RPATH_COMMAND "")
SET(Trilinos_BUILD_SHARED_LIBS "OFF")

SET(Trilinos_LINKER /usr/bin/ld)
SET(Trilinos_AR /usr/bin/ar)


## ---------------------------------------------------------------------------
## Set library specifications and paths 
## ---------------------------------------------------------------------------

## The project version number
SET(Trilinos_VERSION "11.4.3")

## The project include file directories.
SET(Trilinos_INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}/../../../include")

## The project library directories.
SET(Trilinos_LIBRARY_DIRS "${CMAKE_CURRENT_LIST_DIR}/../../../lib")

## The project libraries.
SET(Trilinos_LIBRARIES "rythmos;locathyra;locaepetra;localapack;loca;noxepetra;noxlapack;nox;teko;stratimikos;stratimikosbelos;stratimikosaztecoo;stratimikosamesos;stratimikosml;stratimikosifpack;shylu;anasazitpetra;ModeLaplace;anasaziepetra;anasazi;belostpetra;belosepetra;belos;ml;ifpack;amesos;galeri-xpetra;galeri;aztecoo;isorropia;thyratpetra;thyraepetraext;thyraepetra;thyracore;thyratpetra;thyraepetraext;thyraepetra;thyracore;xpetra-sup;xpetra-ext;xpetra;epetraext;tpetraext;tpetrainout;tpetra;triutils;zoltan;epetra;kokkosdisttsqr;kokkosnodetsqr;kokkoslinalg;kokkosnodeapi;kokkos;kokkosdisttsqr;kokkosnodetsqr;kokkoslinalg;kokkosnodeapi;kokkos;rtop;sacado;tpi;teuchosremainder;teuchosnumerics;teuchoscomm;teuchosparameterlist;teuchoscore;teuchosremainder;teuchosnumerics;teuchoscomm;teuchosparameterlist;teuchoscore")

## The project tpl include paths
SET(Trilinos_TPL_INCLUDE_DIRS "/usr/include;/usr/include/suitesparse;/local/kummerth/lifev-env/ParMetis-3.2.0")

## The project tpl library paths
SET(Trilinos_TPL_LIBRARY_DIRS "")

## The project tpl libraries
SET(Trilinos_TPL_LIBRARIES "/usr/lib64/libhdf5.so;/usr/lib64/libz.so;/usr/lib64/libamd.so;/usr/lib64/libumfpack.so;/usr/lib64/libamd.so;/usr/lib64/libz.so;/local/kummerth/lifev-env/ParMetis-3.2.0/libparmetis.a;/local/kummerth/lifev-env/ParMetis-3.2.0/libmetis.a;/usr/lib64/liblapack.so;/usr/lib64/libblas.so")

## ---------------------------------------------------------------------------
## MPI specific variables
##   These variables are provided to make it easier to get the mpi libraries
##   and includes on systems that do not use the mpi wrappers for compiling
## ---------------------------------------------------------------------------

SET(Trilinos_MPI_LIBRARIES "")
SET(Trilinos_MPI_LIBRARY_DIRS "")
SET(Trilinos_MPI_INCLUDE_DIRS "")
SET(Trilinos_MPI_EXEC "/usr/bin/mpiexec")
SET(Trilinos_MPI_EXEC_MAX_NUMPROCS "4")
SET(Trilinos_MPI_EXEC_NUMPROCS_FLAG "-np")

## ---------------------------------------------------------------------------
## Set useful general variables 
## ---------------------------------------------------------------------------

## The packages enabled for this project
SET(Trilinos_PACKAGE_LIST "Rythmos;NOX;Teko;Stratimikos;ShyLU;Anasazi;Belos;ML;Ifpack;Amesos;Galeri;AztecOO;Isorropia;Thyra;ThyraTpetraAdapters;ThyraEpetraExtAdapters;ThyraEpetraAdapters;ThyraCore;Xpetra;EpetraExt;Tpetra;Triutils;Zoltan;Epetra;Kokkos;KokkosClassic;RTOp;Sacado;ThreadPool;Teuchos;TeuchosRemainder;TeuchosNumerics;TeuchosComm;TeuchosParameterList;TeuchosCore")

## The TPLs enabled for this project
SET(Trilinos_TPL_LIST "HDF5;AMD;UMFPACK;Zlib;ParMETIS;Boost;LAPACK;BLAS;MPI;Pthread")


# Import Trilinos targets
IF(NOT Trilinos_TARGETS_IMPORTED)
  SET(Trilinos_TARGETS_IMPORTED 1)
  INCLUDE("${CMAKE_CURRENT_LIST_DIR}/TrilinosTargets.cmake")
ENDIF()

# Load configurations from enabled packages
include("${CMAKE_CURRENT_LIST_DIR}/../Rythmos/RythmosConfig.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/../NOX/NOXConfig.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/../Teko/TekoConfig.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/../Stratimikos/StratimikosConfig.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/../ShyLU/ShyLUConfig.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/../Anasazi/AnasaziConfig.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/../Belos/BelosConfig.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/../ML/MLConfig.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/../Ifpack/IfpackConfig.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/../Amesos/AmesosConfig.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/../Galeri/GaleriConfig.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/../AztecOO/AztecOOConfig.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/../Isorropia/IsorropiaConfig.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/../Thyra/ThyraConfig.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/../ThyraTpetraAdapters/ThyraTpetraAdaptersConfig.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/../ThyraEpetraExtAdapters/ThyraEpetraExtAdaptersConfig.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/../ThyraEpetraAdapters/ThyraEpetraAdaptersConfig.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/../ThyraCore/ThyraCoreConfig.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/../Xpetra/XpetraConfig.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/../EpetraExt/EpetraExtConfig.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/../Tpetra/TpetraConfig.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/../Triutils/TriutilsConfig.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/../Zoltan/ZoltanConfig.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/../Epetra/EpetraConfig.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/../Kokkos/KokkosConfig.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/../KokkosClassic/KokkosClassicConfig.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/../RTOp/RTOpConfig.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/../Sacado/SacadoConfig.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/../ThreadPool/ThreadPoolConfig.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/../Teuchos/TeuchosConfig.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/../TeuchosRemainder/TeuchosRemainderConfig.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/../TeuchosNumerics/TeuchosNumericsConfig.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/../TeuchosComm/TeuchosCommConfig.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/../TeuchosParameterList/TeuchosParameterListConfig.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/../TeuchosCore/TeuchosCoreConfig.cmake")

