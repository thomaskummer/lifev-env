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

#ifndef SPARSELINEARSYSTEM_HOST_HPP
#define SPARSELINEARSYSTEM_HOST_HPP

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

#if defined( __INTEL_COMPILER )

namespace KokkosArray {
namespace Impl {

template< typename AScalarType ,
          typename VScalarType ,
          class LayoutType >
struct Multiply< CrsMatrix<AScalarType,Host> ,
                 View<VScalarType*,LayoutType,Host > ,
                 View<VScalarType*,LayoutType,Host > >
  : public HostThreadWorker
{
  typedef Host                             device_type ;
  typedef typename device_type::size_type  size_type ;

  typedef View<       VScalarType*, LayoutType, device_type, MemoryUnmanaged >  vector_type ;
  typedef View< const VScalarType*, LayoutType, device_type, MemoryUnmanaged >  vector_const_type ;

  typedef CrsMatrix< AScalarType , device_type >    matrix_type ;

private:

  matrix_type        m_A ;
  vector_const_type  m_x ;
  vector_type        m_y ;

public:

  //--------------------------------------------------------------------------

  inline
  void execute_on_thread( HostThread & this_thread ) const
  {
    const std::pair<size_type,size_type> range =
      this_thread.work_range( m_y.dimension_0() );

    size_type iEntryBegin = m_A.graph.row_map[range.first];

    for ( size_type iRow = range.first ; iRow < range.second ; ++iRow ) {

      const size_type iEntryEnd = m_A.graph.row_map[iRow+1];

      double sum = 0 ;

#pragma simd reduction(+:sum)
#pragma ivdep
      for ( size_type iEntry = iEntryBegin ; iEntry < iEntryEnd ; ++iEntry ) {
        sum += m_A.coefficients(iEntry) * m_x( m_A.graph.entries(iEntry) );
      }

      m_y(iRow) = sum ;

      iEntryBegin = iEntryEnd ;
    }
  }

  //--------------------------------------------------------------------------

  Multiply( const matrix_type & A ,
            const size_type nrow ,
            const size_type , // ncol ,
            const vector_type & x ,
            const vector_type & y )
    : m_A( A ), m_x( x ), m_y( y )
  {
    HostThreadWorker::execute();
  }
};

//----------------------------------------------------------------------------

} // namespace Impl
} // namespace KokkosArray

#endif /* #if defined( __INTEL_COMPILER ) */

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

#endif /* #ifndef SPARSELINEARSYSTEM_HPP */

