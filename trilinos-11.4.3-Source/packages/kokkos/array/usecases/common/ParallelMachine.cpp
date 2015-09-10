/*
//@HEADER
// ************************************************************************
//
//   KokkosArray: Manycore Performance-Portable Multidimensional Arrays
//              Copyright (2012) Sandia Corporation
//
// Under the terms of Contract DE-AC04-94AL85000 with Sandia Corporation,
// the U.S. Government retains certain rights in this software.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// 1. Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the Corporation nor the names of the
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY SANDIA CORPORATION "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL SANDIA CORPORATION OR THE
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Questions? Contact  H. Carter Edwards (hcedwar@sandia.gov)
//
// ************************************************************************
//@HEADER
*/

#include <stdlib.h>
#include <string.h>

#include <ParallelMachine.hpp>

#include <KokkosArray_Cuda.hpp>
#include <KokkosArray_Host.hpp>
#include <KokkosArray_hwloc.hpp>

#if ! defined( KOKKOSARRAY_HAVE_MPI )
#define MPI_COMM_NULL 0
#endif

//------------------------------------------------------------------------

namespace Parallel {

Machine::Machine( int * argc , char *** argv )
  : m_mpi_comm( MPI_COMM_NULL )
  , m_mpi_size(0)
  , m_mpi_rank(0)
  , m_mpi_gpu(0)
{

#if defined( KOKKOSARRAY_HAVE_CUDA )
  //------------------------------------
  // Might be using a Cuda aware version of MPI.
  // Must select Cuda device before initializing MPI.
  {
    int i = 1 ;
    for ( ; i < *argc && strcmp((*argv)[i],"mpi_cuda") ; ++i );

    if ( i < *argc ) {
      // Determine, if possible, what will be the node-local
      // rank of the MPI process once MPI has been initialized.
      // This rank is needed to set the Cuda device before 'mvapich'
      // is initialized.

      const char * const mvapich_local_rank = getenv("MV2_COMM_WORLD_LOCAL_RANK");
      const char * const slurm_local_rank   = getenv("SLURM_LOCALID");

      const int pre_mpi_local_rank =
        0 != mvapich_local_rank ? atoi( mvapich_local_rank ) : (
        0 != slurm_local_rank   ? atoi( slurm_local_rank ) : (
        -1 ) );

      if ( 0 <= pre_mpi_local_rank ) {

        const int ngpu = KokkosArray::Cuda::detect_device_count();

        const int cuda_device_rank = pre_mpi_local_rank % ngpu ;

        KokkosArray::Cuda::initialize( KokkosArray::Cuda::SelectDevice( cuda_device_rank ) );

        m_mpi_gpu = 1 ;
      }
    }
  }
#endif

  //------------------------------------

#if defined( KOKKOSARRAY_HAVE_MPI )
  MPI_Init( argc , argv );
  m_mpi_comm = MPI_COMM_WORLD ;
  MPI_Comm_size( m_mpi_comm , & m_mpi_size );
  MPI_Comm_rank( m_mpi_comm , & m_mpi_rank );
#endif

  // Query hwloc after MPI initialization to allow MPI binding:
  //------------------------------------
  // Request to use host device:
  {
    int i = 1 ;
    for ( ; i < *argc && strcmp((*argv)[i],"host") ; ++i );

    if ( i < *argc ) {

      const std::pair<unsigned,unsigned> core_top  = KokkosArray::hwloc::get_core_topology();
      const unsigned                     core_size = KokkosArray::hwloc::get_core_capacity();

      std::pair<unsigned,unsigned> host_gang_top(core_top.first,core_top.second * core_size);

      if ( i + 2 < *argc ) {
        host_gang_top.first  = atoi( (*argv)[i+1] );
        host_gang_top.second = atoi( (*argv)[i+2] );
      }

      KokkosArray::Host::initialize( host_gang_top , core_top );
    }
  }

#if defined( KOKKOSARRAY_HAVE_CUDA )
  //------------------------------------
  // Request to use Cuda device and not already initialized.
  if ( ! m_mpi_gpu ) {
    int i = 1 ;
    for ( ; i < *argc && strcmp((*argv)[i],"mpi_cuda") && strcmp((*argv)[i],"cuda") ; ++i );

    if ( i < *argc ) {

      const int ngpu = KokkosArray::Cuda::detect_device_count();

      const int cuda_device_rank = m_mpi_rank % ngpu ;

      KokkosArray::Cuda::initialize( KokkosArray::Cuda::SelectDevice( cuda_device_rank ) );
    }
  }
#endif

}

Machine::~Machine()
{
  KokkosArray::Host::finalize();
#if defined( KOKKOSARRAY_HAVE_CUDA )
  KokkosArray::Cuda::finalize();
#endif
#if defined( KOKKOSARRAY_HAVE_MPI )
  MPI_Finalize();
#endif
}

void Machine::print_configuration( std::ostream & msg ) const
{
  msg << "MPI [ " << m_mpi_rank << " / " << m_mpi_size << " ]" << std::endl ;
  KokkosArray::Host::print_configuration( msg );
#if defined( KOKKOSARRAY_HAVE_CUDA )
  KokkosArray::Cuda::print_configuration( msg );
#endif
}

}

