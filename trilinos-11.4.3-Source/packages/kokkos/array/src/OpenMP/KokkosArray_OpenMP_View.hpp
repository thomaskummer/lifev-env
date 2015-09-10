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

#ifndef KOKKOSARRAY_OPENMP_VIEW_HPP
#define KOKKOSARRAY_OPENMP_VIEW_HPP

#include <KokkosArray_OpenMP.hpp>
#include <KokkosArray_View.hpp>

#include <OpenMP/KokkosArray_OpenMP_Parallel.hpp>

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

namespace KokkosArray {
namespace Impl {

template< class OutputView , unsigned Rank = OutputView::Rank >
struct OpenMPViewInit 
{
  explicit OpenMPViewInit( const OutputView & output )
  {
#pragma omp parallel
    {
      HostThread & thread = * OpenMP::get_host_thread();

      std::pair<OpenMP::size_type,OpenMP::size_type> range =
        thread.work_range( output.dimension_0() );

      for ( OpenMP::size_type i0 = range.first ; i0 < range.second ; ++i0 ) {
      for ( OpenMP::size_type i1 = 0 ; i1 < output.dimension_1() ; ++i1 ) {
      for ( OpenMP::size_type i2 = 0 ; i2 < output.dimension_2() ; ++i2 ) {
      for ( OpenMP::size_type i3 = 0 ; i3 < output.dimension_3() ; ++i3 ) {
      for ( OpenMP::size_type i4 = 0 ; i4 < output.dimension_4() ; ++i4 ) {
      for ( OpenMP::size_type i5 = 0 ; i5 < output.dimension_5() ; ++i5 ) {
      for ( OpenMP::size_type i6 = 0 ; i6 < output.dimension_6() ; ++i6 ) {
      for ( OpenMP::size_type i7 = 0 ; i7 < output.dimension_7() ; ++i7 ) {
        output(i0,i1,i2,i3,i4,i5,i6,i7) = 0 ;
      }}}}}}}}
    }
  }
};

template< class OutputView >
struct OpenMPViewInit< OutputView , 1 >
{
  explicit OpenMPViewInit( const OutputView & output )
  {
#pragma omp parallel
    {
      HostThread & thread = * OpenMP::get_host_thread();

      std::pair<OpenMP::size_type,OpenMP::size_type> range =
        thread.work_range( output.dimension_0() );

      for ( OpenMP::size_type i0 = range.first ; i0 < range.second ; ++i0 ) {
        output(i0) = 0 ;
      }
    }
  }
};

template< class OutputView >
struct OpenMPViewInit< OutputView , 0 >
{
  OpenMPViewInit( const OutputView & output )
    { *output = 0 ; }
};

template<>
struct ViewInitialize< OpenMP >
{
  template< class ViewType >
  inline explicit ViewInitialize( const ViewType & view )
  {
    OpenMPViewInit< ViewType > init( view );
  }
};

} // namespace Impl
} // namespace KokkosArray

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

namespace KokkosArray {
namespace Impl {

template< class OutputView , class InputView , unsigned Rank = OutputView::Rank >
struct OpenMPViewRemap
{
  OpenMPViewRemap( const OutputView & output , const InputView & input )
  {
    const OpenMP::size_type n0 = std::min( output.dimension_0() , input.dimension_0() );
    const OpenMP::size_type n1 = std::min( output.dimension_1() , input.dimension_1() );
    const OpenMP::size_type n2 = std::min( output.dimension_2() , input.dimension_2() );
    const OpenMP::size_type n3 = std::min( output.dimension_3() , input.dimension_3() );
    const OpenMP::size_type n4 = std::min( output.dimension_4() , input.dimension_4() );
    const OpenMP::size_type n5 = std::min( output.dimension_5() , input.dimension_5() );
    const OpenMP::size_type n6 = std::min( output.dimension_6() , input.dimension_6() );
    const OpenMP::size_type n7 = std::min( output.dimension_7() , input.dimension_7() );

#pragma omp parallel
    {
      HostThread & thread = * OpenMP::get_host_thread();

      std::pair<OpenMP::size_type,OpenMP::size_type> range = thread.work_range( n0 );

      for ( OpenMP::size_type i0 = range.first ; i0 < range.second ; ++i0 ) {
      for ( OpenMP::size_type i1 = 0 ; i1 < n1 ; ++i1 ) {
      for ( OpenMP::size_type i2 = 0 ; i2 < n2 ; ++i2 ) {
      for ( OpenMP::size_type i3 = 0 ; i3 < n3 ; ++i3 ) {
      for ( OpenMP::size_type i4 = 0 ; i4 < n4 ; ++i4 ) {
      for ( OpenMP::size_type i5 = 0 ; i5 < n5 ; ++i5 ) {
      for ( OpenMP::size_type i6 = 0 ; i6 < n6 ; ++i6 ) {
      for ( OpenMP::size_type i7 = 0 ; i7 < n7 ; ++i7 ) {
        output(i0,i1,i2,i3,i4,i5,i6,i7) = input(i0,i1,i2,i3,i4,i5,i6,i7);
      }}}}}}}}
    }
  }
};

template< class OutputView , class InputView >
struct OpenMPViewRemap< OutputView , InputView , 1 >
{
  OpenMPViewRemap( const OutputView & output , const InputView & input )
  {
#pragma omp parallel
    {
      HostThread & thread = * OpenMP::get_host_thread();

      std::pair<OpenMP::size_type,OpenMP::size_type> range =
        thread.work_range( std::min( output.dimension_0() , input.dimension_0() ) );

      for ( OpenMP::size_type i0 = range.first ; i0 < range.second ; ++i0 ) {
        output(i0) = input(i0);
      }
    }
  }
};

template< class OutputView , class InputView >
struct OpenMPViewRemap< OutputView , InputView , 0 >
{
  OpenMPViewRemap( const OutputView & output , const InputView & input )
    { *output = *input ; }
};

} // namespace Impl
} // namespace KokkosArray

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

namespace KokkosArray {

/** \brief Deep copy equal dimension arrays in the host space which
 *         have different layouts or specializations.
 */

template< class DT , class DL , class DM , class DS ,
          class ST , class SL , class SM , class SS >
inline
void deep_copy( const View< DT, DL, OpenMP, DM, DS> & dst ,
                const View< ST, SL, OpenMP, SM, SS> & src ,
                const typename Impl::enable_if<(
                  // Destination is not constant:
                  Impl::is_same< typename ViewTraits<DT,DL,OpenMP,DM>::value_type ,
                                 typename ViewTraits<DT,DL,OpenMP,DM>::non_const_value_type >::value
                  &&
                  // Same rank
                  ( unsigned( ViewTraits<DT,DL,OpenMP,DM>::rank ) ==
                    unsigned( ViewTraits<ST,SL,OpenMP,SM>::rank ) )
                  &&
                  // Different layout or different specialization:
                  ( ( ! Impl::is_same< typename DL::array_layout ,
                                       typename SL::array_layout >::value )
                    ||
                    ( ! Impl::is_same< DS , SS >::value )
                  )
                )>::type * = 0 )
{
  typedef View< DT, DL, OpenMP, DM, DS> dst_type ;
  typedef View< ST, SL, OpenMP, SM, SS> src_type ;

  assert_shapes_equal_dimension( dst.shape() , src.shape() );

  Impl::OpenMPViewRemap< dst_type , src_type , dst_type::rank >( dst , src );
}

/** \brief  Deep copy of scalar value */

template< typename ValueType , class LayoutSrc , class MemoryTraits >
inline
void deep_copy( ValueType & dst ,
                const View< ValueType , LayoutSrc , OpenMP , MemoryTraits , Impl::LayoutScalar > & src )
{ dst = src ; }

template< typename ValueType , class LayoutDst , class MemoryTraits >
inline
void deep_copy( const View< ValueType , LayoutDst , OpenMP , MemoryTraits , Impl::LayoutScalar > & dst ,
                const ValueType & src )
{ dst = src ; }

} // namespace KokkosArray

#endif /* #ifndef KOKKOSARRAY_OPENMP_VIEW_HPP */


