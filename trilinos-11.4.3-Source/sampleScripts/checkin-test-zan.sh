#!/bin/bash

#
# This is the script that I used to checkin to Trilinos on gabriel.sandia.gov.
# You can copy this script and adapt it to your own machine.
#
# If you want to automatically do the remote pull/test/push on godel, you can
# use the arguments:
#
#    "--execute-on-ready-to-push=\"ssh -q godel /home/rabartl/PROJECTS/Trilinos.base.checkin/BUILDS/CHECKIN/checkin-test-godel-remote-test-push-gabriel-remote-driver.sh &\""
#
# NOTE: You will need the funny quotes when passing through a shell script.
#

#
# Allow command-line arguments to pass through to cmake configure!
#

EXTRA_ARGS=$@

#
# Set up configuration files
#

echo "
 -D Trilinos_ENABLE_CXX11:BOOL=ON
 -D CMAKE_CXX_FLAGS='-std=c++0x'
 -D CMAKE_C_FLAGS=''
 -D CMAKE_Fortran_FLAGS=''
 -D Tpetra_ENABLE_RTI=ON
 -D TPL_ENABLE_QD:BOOL=ON
 -D QD_INCLUDE_DIRS=/usr/local/qd/include
 -D QD_LIBRARY_DIRS=/usr/local/qd/lib
 -D TPL_ENABLE_TBB:BOOL=ON
 -D TBB_LIBRARY_DIRS=/Users/ogb/sw_builds/TBB22/ia32/cc4.0.1_os10.4.9/lib
 -D TBB_INCLUDE_DIRS=/Users/ogb/sw_builds/TBB22/include
 -D TPL_ENABLE_CUDA:BOOL=OFF
 -D TPL_ENABLE_Thrust:BOOL=OFF
 -D Trilinos_ENABLE_OpenMP:BOOL=ON 
" > COMMON.config

echo "
 -D CMAKE_C_COMPILER=/usr/bin/gcc
 -D CMAKE_CXX_COMPILER=/usr/bin/g++
 -D Trilinos_ENABLE_Fortran:BOOL=OFF 
 -D Trilinos_ENABLE_CXX11:BOOL=OFF
 -D Tpetra_ENABLE_RTI=OFF
 -D TPL_ENABLE_CUDA:BOOL=ON
 -D TPL_ENABLE_Thrust:BOOL=ON
 -D Thrust_INCLUDE_DIRS=/usr/local/cuda/include
" > SERIAL_RELEASE_CUDA.config

echo "
-DCMAKE_CXX_COMPILER=/opt/gcc451/bin/g++
-DCMAKE_C_COMPILER=/opt/gcc451/bin/gcc
-DCMAKE_Fortran_COMPILER=/opt/gcc451/bin/gfortran
" > SERIAL_RELEASE.config

echo "
-DMPI_BASE_DIR:PATH=/opt/openmpi143
" > MPI_DEBUG.config

#
# Run the standard checkin testing script with my specializations
#

../../Trilinos/checkin-test.py \
--send-email-to=bakercg@ornl.gov \
--make-options="-j2" \
--ctest-options="-j1" \
--ctest-timeout=180 \
--enable-all-packages=off --no-enable-fwd-packages \
--extra-builds=SERIAL_RELEASE_CUDA \
$EXTRA_ARGS  

# Options to run with:
#
#  --extra-builds=SERIAL_DEBUG_BOOST_TRACE,SERIAL_DEBUG_TRACE,SERIAL_DEBUG_BOOST

#
# NOTES:
#
# (*) Enabling shared libaries makes relinks go *much* faster and massively
# speeds up the checkin testing process when rebulding!
#
# (*) Be sure to set --make-options="-jN" to speed up building.  It makes a
# *big* difference.
#
# (*) Passing -jN to ctest with --ctest-optioins="-jN" can speed up running
# the tests but I have not seen very good speedup in general and some people
# have reported no speedup at all.  You should experiment with ctest -jN to
# see what number N works well on your machine.
#
