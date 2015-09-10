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

#include <gtest/gtest.h>

#include <stdexcept>
#include <sstream>
#include <iostream>

/*--------------------------------------------------------------------------*/

namespace Test {

template< class T , class L , class D , class M , class S >
size_t allocation_count( const KokkosArray::View<T,L,D,M,S> & view )
{
  const size_t card  = KokkosArray::Impl::cardinality_count( view.shape() );
  const size_t alloc = view.capacity();

  return card <= alloc ? alloc : 0 ;
}

/*--------------------------------------------------------------------------*/

template< typename T, class DeviceType>
struct TestViewOperator
{
  typedef DeviceType  device_type ;

  static const unsigned N = 100 ;
  static const unsigned D = 3 ;

  typedef KokkosArray::View< T*[D] , device_type > view_type ;

  const view_type v1 ;
  const view_type v2 ;

  TestViewOperator()
    : v1( "v1" , N )
    , v2( "v2" , N )
    {}

  static void apply()
  {
    KokkosArray::parallel_for( N , TestViewOperator() );
  }

  KOKKOSARRAY_INLINE_FUNCTION
  void operator()( const unsigned i ) const
  {
    const unsigned X = 0 ;
    const unsigned Y = 1 ;
    const unsigned Z = 2 ;

    v2(i,X) = v1(i,X);
    v2(i,Y) = v1(i,Y);
    v2(i,Z) = v1(i,Z);
  }
};

/*--------------------------------------------------------------------------*/

template< class ViewType >
ViewType create_test_view( const typename ViewType::shape_type shape )
{
  const unsigned stride =
    KokkosArray::Impl::ShapeMap< typename ViewType::shape_type,
                                 typename ViewType::array_layout >
    ::template stride< typename ViewType::memory_space >( shape );

  return ViewType( (typename ViewType::scalar_type *) 0 , shape , stride );
}

template< class DataType >
struct rank {
private:
  typedef typename KokkosArray::Impl::AnalyzeShape<DataType>::shape shape ;
public:
  static const unsigned value = shape::rank ;
};

template< class DataType ,
          class DeviceType ,
          unsigned Rank = rank< DataType >::value >
struct TestViewOperator_LeftAndRight ;

template< class DataType , class DeviceType >
struct TestViewOperator_LeftAndRight< DataType , DeviceType , 8 >
{
  typedef DeviceType                          device_type ;
  typedef typename device_type::memory_space  memory_space ;
  typedef typename device_type::size_type     size_type ;

  typedef int value_type ;

  KOKKOSARRAY_INLINE_FUNCTION
  static void join( volatile value_type & update ,
                    const volatile value_type & input )
    { update |= input ; }

  KOKKOSARRAY_INLINE_FUNCTION
  static void init( value_type & update )
    { update = 0 ; }


  typedef KokkosArray::
    View< DataType, KokkosArray::LayoutLeft, device_type > left_view ;

  typedef KokkosArray::
    View< DataType, KokkosArray::LayoutRight, device_type > right_view ;

  typedef typename left_view ::shape_type  left_shape ;
  typedef typename right_view::shape_type  right_shape ;

  left_shape   lsh ;
  right_shape  rsh ;
  left_view    left ;
  right_view   right ;
  long         left_alloc ;
  long         right_alloc ;

  TestViewOperator_LeftAndRight()
    : lsh()
    , rsh()
    , left(  "left" )
    , right( "right" )
    , left_alloc( allocation_count( left ) )
    , right_alloc( allocation_count( right ) )
    {}

  static void apply()
  {
    TestViewOperator_LeftAndRight driver ;

    ASSERT_TRUE( (long) KokkosArray::Impl::cardinality_count( driver.lsh ) <= driver.left_alloc );
    ASSERT_TRUE( (long) KokkosArray::Impl::cardinality_count( driver.rsh ) <= driver.right_alloc );

    const int error_flag = KokkosArray::parallel_reduce( 1 , driver );

    ASSERT_EQ( error_flag , 0 );
  }

  KOKKOSARRAY_INLINE_FUNCTION
  void operator()( const size_type , value_type & update ) const
  {
    long offset ;

    offset = -1 ;
    for ( unsigned i7 = 0 ; i7 < lsh.N7 ; ++i7 )
    for ( unsigned i6 = 0 ; i6 < lsh.N6 ; ++i6 )
    for ( unsigned i5 = 0 ; i5 < lsh.N5 ; ++i5 )
    for ( unsigned i4 = 0 ; i4 < lsh.N4 ; ++i4 )
    for ( unsigned i3 = 0 ; i3 < lsh.N3 ; ++i3 )
    for ( unsigned i2 = 0 ; i2 < lsh.N2 ; ++i2 )
    for ( unsigned i1 = 0 ; i1 < lsh.N1 ; ++i1 )
    for ( unsigned i0 = 0 ; i0 < lsh.N0 ; ++i0 )
    {
      const long j = & left( i0, i1, i2, i3, i4, i5, i6, i7 ) -
                     & left(  0,  0,  0,  0,  0,  0,  0,  0 );
      if ( j <= offset || left_alloc <= j ) { update |= 1 ; }
      offset = j ;
    }

    offset = -1 ;
    for ( unsigned i0 = 0 ; i0 < rsh.N0 ; ++i0 )
    for ( unsigned i1 = 0 ; i1 < rsh.N1 ; ++i1 )
    for ( unsigned i2 = 0 ; i2 < rsh.N2 ; ++i2 )
    for ( unsigned i3 = 0 ; i3 < rsh.N3 ; ++i3 )
    for ( unsigned i4 = 0 ; i4 < rsh.N4 ; ++i4 )
    for ( unsigned i5 = 0 ; i5 < rsh.N5 ; ++i5 )
    for ( unsigned i6 = 0 ; i6 < rsh.N6 ; ++i6 )
    for ( unsigned i7 = 0 ; i7 < rsh.N7 ; ++i7 )
    {
      const long j = & right( i0, i1, i2, i3, i4, i5, i6, i7 ) -
                     & right(  0,  0,  0,  0,  0,  0,  0,  0 );
      if ( j <= offset || right_alloc <= j ) { update |= 2 ; }
      offset = j ;
    }
  }
};

template< class DataType , class DeviceType >
struct TestViewOperator_LeftAndRight< DataType , DeviceType , 7 >
{
  typedef DeviceType                          device_type ;
  typedef typename device_type::memory_space  memory_space ;
  typedef typename device_type::size_type     size_type ;

  typedef int value_type ;

  KOKKOSARRAY_INLINE_FUNCTION
  static void join( volatile value_type & update ,
                    const volatile value_type & input )
    { update |= input ; }

  KOKKOSARRAY_INLINE_FUNCTION
  static void init( value_type & update )
    { update = 0 ; }


  typedef KokkosArray::
    View< DataType, KokkosArray::LayoutLeft, device_type > left_view ;

  typedef KokkosArray::
    View< DataType, KokkosArray::LayoutRight, device_type > right_view ;

  typedef typename left_view ::shape_type  left_shape ;
  typedef typename right_view::shape_type  right_shape ;

  left_shape   lsh ;
  right_shape  rsh ;
  left_view    left ;
  right_view   right ;
  long         left_alloc ;
  long         right_alloc ;

  TestViewOperator_LeftAndRight()
    : lsh()
    , rsh()
    , left(  "left" )
    , right( "right" )
    , left_alloc( allocation_count( left ) )
    , right_alloc( allocation_count( right ) )
    {}

  static void apply()
  {
    TestViewOperator_LeftAndRight driver ;

    ASSERT_TRUE( (long) KokkosArray::Impl::cardinality_count( driver.lsh ) <= driver.left_alloc );
    ASSERT_TRUE( (long) KokkosArray::Impl::cardinality_count( driver.rsh ) <= driver.right_alloc );

    const int error_flag = KokkosArray::parallel_reduce( 1 , driver );

    ASSERT_EQ( error_flag , 0 );
  }

  KOKKOSARRAY_INLINE_FUNCTION
  void operator()( const size_type , value_type & update ) const
  {
    long offset ;

    offset = -1 ;
    for ( unsigned i6 = 0 ; i6 < lsh.N6 ; ++i6 )
    for ( unsigned i5 = 0 ; i5 < lsh.N5 ; ++i5 )
    for ( unsigned i4 = 0 ; i4 < lsh.N4 ; ++i4 )
    for ( unsigned i3 = 0 ; i3 < lsh.N3 ; ++i3 )
    for ( unsigned i2 = 0 ; i2 < lsh.N2 ; ++i2 )
    for ( unsigned i1 = 0 ; i1 < lsh.N1 ; ++i1 )
    for ( unsigned i0 = 0 ; i0 < lsh.N0 ; ++i0 )
    {
      const long j = & left( i0, i1, i2, i3, i4, i5, i6 ) -
                     & left(  0,  0,  0,  0,  0,  0,  0 );
      if ( j <= offset || left_alloc <= j ) { update |= 1 ; }
      offset = j ;
    }

    offset = -1 ;
    for ( unsigned i0 = 0 ; i0 < rsh.N0 ; ++i0 )
    for ( unsigned i1 = 0 ; i1 < rsh.N1 ; ++i1 )
    for ( unsigned i2 = 0 ; i2 < rsh.N2 ; ++i2 )
    for ( unsigned i3 = 0 ; i3 < rsh.N3 ; ++i3 )
    for ( unsigned i4 = 0 ; i4 < rsh.N4 ; ++i4 )
    for ( unsigned i5 = 0 ; i5 < rsh.N5 ; ++i5 )
    for ( unsigned i6 = 0 ; i6 < rsh.N6 ; ++i6 )
    {
      const long j = & right( i0, i1, i2, i3, i4, i5, i6 ) -
                     & right(  0,  0,  0,  0,  0,  0,  0 );
      if ( j <= offset || right_alloc <= j ) { update |= 2 ; }
      offset = j ;
    }
  }
};

template< class DataType , class DeviceType >
struct TestViewOperator_LeftAndRight< DataType , DeviceType , 6 >
{
  typedef DeviceType                          device_type ;
  typedef typename device_type::memory_space  memory_space ;
  typedef typename device_type::size_type     size_type ;

  typedef int value_type ;

  KOKKOSARRAY_INLINE_FUNCTION
  static void join( volatile value_type & update ,
                    const volatile value_type & input )
    { update |= input ; }

  KOKKOSARRAY_INLINE_FUNCTION
  static void init( value_type & update )
    { update = 0 ; }


  typedef KokkosArray::
    View< DataType, KokkosArray::LayoutLeft, device_type > left_view ;

  typedef KokkosArray::
    View< DataType, KokkosArray::LayoutRight, device_type > right_view ;

  typedef typename left_view ::shape_type  left_shape ;
  typedef typename right_view::shape_type  right_shape ;

  left_shape   lsh ;
  right_shape  rsh ;
  left_view    left ;
  right_view   right ;
  long         left_alloc ;
  long         right_alloc ;

  TestViewOperator_LeftAndRight()
    : lsh()
    , rsh()
    , left(  "left" )
    , right( "right" )
    , left_alloc( allocation_count( left ) )
    , right_alloc( allocation_count( right ) )
    {}

  static void apply()
  {
    TestViewOperator_LeftAndRight driver ;

    ASSERT_TRUE( (long) KokkosArray::Impl::cardinality_count( driver.lsh ) <= driver.left_alloc );
    ASSERT_TRUE( (long) KokkosArray::Impl::cardinality_count( driver.rsh ) <= driver.right_alloc );

    const int error_flag = KokkosArray::parallel_reduce( 1 , driver );

    ASSERT_EQ( error_flag , 0 );
  }

  KOKKOSARRAY_INLINE_FUNCTION
  void operator()( const size_type , value_type & update ) const
  {
    long offset ;

    offset = -1 ;
    for ( unsigned i5 = 0 ; i5 < lsh.N5 ; ++i5 )
    for ( unsigned i4 = 0 ; i4 < lsh.N4 ; ++i4 )
    for ( unsigned i3 = 0 ; i3 < lsh.N3 ; ++i3 )
    for ( unsigned i2 = 0 ; i2 < lsh.N2 ; ++i2 )
    for ( unsigned i1 = 0 ; i1 < lsh.N1 ; ++i1 )
    for ( unsigned i0 = 0 ; i0 < lsh.N0 ; ++i0 )
    {
      const long j = & left( i0, i1, i2, i3, i4, i5 ) -
                     & left(  0,  0,  0,  0,  0,  0 );
      if ( j <= offset || left_alloc <= j ) { update |= 1 ; }
      offset = j ;
    }

    offset = -1 ;
    for ( unsigned i0 = 0 ; i0 < rsh.N0 ; ++i0 )
    for ( unsigned i1 = 0 ; i1 < rsh.N1 ; ++i1 )
    for ( unsigned i2 = 0 ; i2 < rsh.N2 ; ++i2 )
    for ( unsigned i3 = 0 ; i3 < rsh.N3 ; ++i3 )
    for ( unsigned i4 = 0 ; i4 < rsh.N4 ; ++i4 )
    for ( unsigned i5 = 0 ; i5 < rsh.N5 ; ++i5 )
    {
      const long j = & right( i0, i1, i2, i3, i4, i5 ) -
                     & right(  0,  0,  0,  0,  0,  0 );
      if ( j <= offset || right_alloc <= j ) { update |= 2 ; }
      offset = j ;
    }
  }
};

template< class DataType , class DeviceType >
struct TestViewOperator_LeftAndRight< DataType , DeviceType , 5 >
{
  typedef DeviceType                          device_type ;
  typedef typename device_type::memory_space  memory_space ;
  typedef typename device_type::size_type     size_type ;

  typedef int value_type ;

  KOKKOSARRAY_INLINE_FUNCTION
  static void join( volatile value_type & update ,
                    const volatile value_type & input )
    { update |= input ; }

  KOKKOSARRAY_INLINE_FUNCTION
  static void init( value_type & update )
    { update = 0 ; }


  typedef KokkosArray::
    View< DataType, KokkosArray::LayoutLeft, device_type > left_view ;

  typedef KokkosArray::
    View< DataType, KokkosArray::LayoutRight, device_type > right_view ;

  typedef typename left_view ::shape_type  left_shape ;
  typedef typename right_view::shape_type  right_shape ;

  left_shape   lsh ;
  right_shape  rsh ;
  left_view    left ;
  right_view   right ;
  long         left_alloc ;
  long         right_alloc ;

  TestViewOperator_LeftAndRight()
    : lsh()
    , rsh()
    , left(  "left" )
    , right( "right" )
    , left_alloc( allocation_count( left ) )
    , right_alloc( allocation_count( right ) )
    {}

  static void apply()
  {
    TestViewOperator_LeftAndRight driver ;

    ASSERT_TRUE( (long) KokkosArray::Impl::cardinality_count( driver.lsh ) <= driver.left_alloc );
    ASSERT_TRUE( (long) KokkosArray::Impl::cardinality_count( driver.rsh ) <= driver.right_alloc );

    const int error_flag = KokkosArray::parallel_reduce( 1 , driver );

    ASSERT_EQ( error_flag , 0 );
  }

  KOKKOSARRAY_INLINE_FUNCTION
  void operator()( const size_type , value_type & update ) const
  {
    long offset ;

    offset = -1 ;
    for ( unsigned i4 = 0 ; i4 < lsh.N4 ; ++i4 )
    for ( unsigned i3 = 0 ; i3 < lsh.N3 ; ++i3 )
    for ( unsigned i2 = 0 ; i2 < lsh.N2 ; ++i2 )
    for ( unsigned i1 = 0 ; i1 < lsh.N1 ; ++i1 )
    for ( unsigned i0 = 0 ; i0 < lsh.N0 ; ++i0 )
    {
      const long j = & left( i0, i1, i2, i3, i4 ) -
                     & left(  0,  0,  0,  0,  0 );
      if ( j <= offset || left_alloc <= j ) { update |= 1 ; }
      offset = j ;
    }

    offset = -1 ;
    for ( unsigned i0 = 0 ; i0 < rsh.N0 ; ++i0 )
    for ( unsigned i1 = 0 ; i1 < rsh.N1 ; ++i1 )
    for ( unsigned i2 = 0 ; i2 < rsh.N2 ; ++i2 )
    for ( unsigned i3 = 0 ; i3 < rsh.N3 ; ++i3 )
    for ( unsigned i4 = 0 ; i4 < rsh.N4 ; ++i4 )
    {
      const long j = & right( i0, i1, i2, i3, i4 ) -
                     & right(  0,  0,  0,  0,  0 );
      if ( j <= offset || right_alloc <= j ) { update |= 2 ; }
      offset = j ;
    }
  }
};

template< class DataType , class DeviceType >
struct TestViewOperator_LeftAndRight< DataType , DeviceType , 4 >
{
  typedef DeviceType                          device_type ;
  typedef typename device_type::memory_space  memory_space ;
  typedef typename device_type::size_type     size_type ;

  typedef int value_type ;

  KOKKOSARRAY_INLINE_FUNCTION
  static void join( volatile value_type & update ,
                    const volatile value_type & input )
    { update |= input ; }

  KOKKOSARRAY_INLINE_FUNCTION
  static void init( value_type & update )
    { update = 0 ; }


  typedef KokkosArray::
    View< DataType, KokkosArray::LayoutLeft, device_type > left_view ;

  typedef KokkosArray::
    View< DataType, KokkosArray::LayoutRight, device_type > right_view ;

  typedef typename left_view ::shape_type  left_shape ;
  typedef typename right_view::shape_type  right_shape ;

  left_shape   lsh ;
  right_shape  rsh ;
  left_view    left ;
  right_view   right ;
  long         left_alloc ;
  long         right_alloc ;

  TestViewOperator_LeftAndRight()
    : lsh()
    , rsh()
    , left(  "left" )
    , right( "right" )
    , left_alloc( allocation_count( left ) )
    , right_alloc( allocation_count( right ) )
    {}

  static void apply()
  {
    TestViewOperator_LeftAndRight driver ;

    ASSERT_TRUE( (long) KokkosArray::Impl::cardinality_count( driver.lsh ) <= driver.left_alloc );
    ASSERT_TRUE( (long) KokkosArray::Impl::cardinality_count( driver.rsh ) <= driver.right_alloc );

    const int error_flag = KokkosArray::parallel_reduce( 1 , driver );

    ASSERT_EQ( error_flag , 0 );
  }

  KOKKOSARRAY_INLINE_FUNCTION
  void operator()( const size_type , value_type & update ) const
  {
    long offset ;

    offset = -1 ;
    for ( unsigned i3 = 0 ; i3 < lsh.N3 ; ++i3 )
    for ( unsigned i2 = 0 ; i2 < lsh.N2 ; ++i2 )
    for ( unsigned i1 = 0 ; i1 < lsh.N1 ; ++i1 )
    for ( unsigned i0 = 0 ; i0 < lsh.N0 ; ++i0 )
    {
      const long j = & left( i0, i1, i2, i3 ) -
                     & left(  0,  0,  0,  0 );
      if ( j <= offset || left_alloc <= j ) { update |= 1 ; }
      offset = j ;
    }

    offset = -1 ;
    for ( unsigned i0 = 0 ; i0 < rsh.N0 ; ++i0 )
    for ( unsigned i1 = 0 ; i1 < rsh.N1 ; ++i1 )
    for ( unsigned i2 = 0 ; i2 < rsh.N2 ; ++i2 )
    for ( unsigned i3 = 0 ; i3 < rsh.N3 ; ++i3 )
    {
      const long j = & right( i0, i1, i2, i3 ) -
                     & right(  0,  0,  0,  0 );
      if ( j <= offset || right_alloc <= j ) { update |= 2 ; }
      offset = j ;
    }
  }
};

template< class DataType , class DeviceType >
struct TestViewOperator_LeftAndRight< DataType , DeviceType , 3 >
{
  typedef DeviceType                          device_type ;
  typedef typename device_type::memory_space  memory_space ;
  typedef typename device_type::size_type     size_type ;

  typedef int value_type ;

  KOKKOSARRAY_INLINE_FUNCTION
  static void join( volatile value_type & update ,
                    const volatile value_type & input )
    { update |= input ; }

  KOKKOSARRAY_INLINE_FUNCTION
  static void init( value_type & update )
    { update = 0 ; }


  typedef KokkosArray::
    View< DataType, KokkosArray::LayoutLeft, device_type > left_view ;

  typedef KokkosArray::
    View< DataType, KokkosArray::LayoutRight, device_type > right_view ;

  typedef typename left_view ::shape_type  left_shape ;
  typedef typename right_view::shape_type  right_shape ;

  left_shape   lsh ;
  right_shape  rsh ;
  left_view    left ;
  right_view   right ;
  long         left_alloc ;
  long         right_alloc ;

  TestViewOperator_LeftAndRight()
    : lsh()
    , rsh()
    , left(  "left" )
    , right( "right" )
    , left_alloc( allocation_count( left ) )
    , right_alloc( allocation_count( right ) )
    {}

  static void apply()
  {
    TestViewOperator_LeftAndRight driver ;

    ASSERT_TRUE( (long) KokkosArray::Impl::cardinality_count( driver.lsh ) <= driver.left_alloc );
    ASSERT_TRUE( (long) KokkosArray::Impl::cardinality_count( driver.rsh ) <= driver.right_alloc );

    const int error_flag = KokkosArray::parallel_reduce( 1 , driver );

    ASSERT_EQ( error_flag , 0 );
  }

  KOKKOSARRAY_INLINE_FUNCTION
  void operator()( const size_type , value_type & update ) const
  {
    long offset ;

    offset = -1 ;
    for ( unsigned i2 = 0 ; i2 < lsh.N2 ; ++i2 )
    for ( unsigned i1 = 0 ; i1 < lsh.N1 ; ++i1 )
    for ( unsigned i0 = 0 ; i0 < lsh.N0 ; ++i0 )
    {
      const long j = & left( i0, i1, i2 ) -
                     & left(  0,  0,  0 );
      if ( j <= offset || left_alloc <= j ) { update |= 1 ; }
      offset = j ;
    }

    offset = -1 ;
    for ( unsigned i0 = 0 ; i0 < rsh.N0 ; ++i0 )
    for ( unsigned i1 = 0 ; i1 < rsh.N1 ; ++i1 )
    for ( unsigned i2 = 0 ; i2 < rsh.N2 ; ++i2 )
    {
      const long j = & right( i0, i1, i2 ) -
                     & right(  0,  0,  0 );
      if ( j <= offset || right_alloc <= j ) { update |= 2 ; }
      offset = j ;
    }

    for ( unsigned i0 = 0 ; i0 < lsh.N0 ; ++i0 )
    for ( unsigned i1 = 0 ; i1 < lsh.N1 ; ++i1 )
    for ( unsigned i2 = 0 ; i2 < lsh.N2 ; ++i2 )
    {
      if ( & left(i0,i1,i2)  != & left(i0,i1,i2,0) )  { update |= 3 ; }
      if ( & left(i0,i1,i2)  != & left(i0,i1,i2,0,0) )  { update |= 3 ; }
      if ( & right(i0,i1,i2) != & right(i0,i1,i2,0) ) { update |= 3 ; }
      if ( & right(i0,i1,i2) != & right(i0,i1,i2,0,0) ) { update |= 3 ; }
    }
  }
};

template< class DataType , class DeviceType >
struct TestViewOperator_LeftAndRight< DataType , DeviceType , 2 >
{
  typedef DeviceType                          device_type ;
  typedef typename device_type::memory_space  memory_space ;
  typedef typename device_type::size_type     size_type ;

  typedef int value_type ;

  KOKKOSARRAY_INLINE_FUNCTION
  static void join( volatile value_type & update ,
                    const volatile value_type & input )
    { update |= input ; }

  KOKKOSARRAY_INLINE_FUNCTION
  static void init( value_type & update )
    { update = 0 ; }


  typedef KokkosArray::
    View< DataType, KokkosArray::LayoutLeft, device_type > left_view ;

  typedef KokkosArray::
    View< DataType, KokkosArray::LayoutRight, device_type > right_view ;

  typedef typename left_view ::shape_type  left_shape ;
  typedef typename right_view::shape_type  right_shape ;

  left_shape   lsh ;
  right_shape  rsh ;
  left_view    left ;
  right_view   right ;
  long         left_alloc ;
  long         right_alloc ;

  TestViewOperator_LeftAndRight()
    : lsh()
    , rsh()
    , left(  "left" )
    , right( "right" )
    , left_alloc( allocation_count( left ) )
    , right_alloc( allocation_count( right ) )
    {}

  static void apply()
  {
    TestViewOperator_LeftAndRight driver ;

    ASSERT_TRUE( (long) KokkosArray::Impl::cardinality_count( driver.lsh ) <= driver.left_alloc );
    ASSERT_TRUE( (long) KokkosArray::Impl::cardinality_count( driver.rsh ) <= driver.right_alloc );

    const int error_flag = KokkosArray::parallel_reduce( 1 , driver );

    ASSERT_EQ( error_flag , 0 );
  }

  KOKKOSARRAY_INLINE_FUNCTION
  void operator()( const size_type , value_type & update ) const
  {
    long offset ;

    offset = -1 ;
    for ( unsigned i1 = 0 ; i1 < lsh.N1 ; ++i1 )
    for ( unsigned i0 = 0 ; i0 < lsh.N0 ; ++i0 )
    {
      const long j = & left( i0, i1 ) -
                     & left(  0,  0 );
      if ( j <= offset || left_alloc <= j ) { update |= 1 ; }
      offset = j ;
    }

    offset = -1 ;
    for ( unsigned i0 = 0 ; i0 < rsh.N0 ; ++i0 )
    for ( unsigned i1 = 0 ; i1 < rsh.N1 ; ++i1 )
    {
      const long j = & right( i0, i1 ) -
                     & right(  0,  0 );
      if ( j <= offset || right_alloc <= j ) { update |= 2 ; }
      offset = j ;
    }

    for ( unsigned i0 = 0 ; i0 < lsh.N0 ; ++i0 )
    for ( unsigned i1 = 0 ; i1 < lsh.N1 ; ++i1 )
    {
      if ( & left(i0,i1)  != & left(i0,i1,0) )  { update |= 3 ; }
      if ( & left(i0,i1)  != & left(i0,i1,0,0) )  { update |= 3 ; }
      if ( & right(i0,i1) != & right(i0,i1,0) ) { update |= 3 ; }
      if ( & right(i0,i1) != & right(i0,i1,0,0) ) { update |= 3 ; }
    }
  }
};

template< class DataType , class DeviceType >
struct TestViewOperator_LeftAndRight< DataType , DeviceType , 1 >
{
  typedef DeviceType                          device_type ;
  typedef typename device_type::memory_space  memory_space ;
  typedef typename device_type::size_type     size_type ;

  typedef int value_type ;

  KOKKOSARRAY_INLINE_FUNCTION
  static void join( volatile value_type & update ,
                    const volatile value_type & input )
    { update |= input ; }

  KOKKOSARRAY_INLINE_FUNCTION
  static void init( value_type & update )
    { update = 0 ; }


  typedef KokkosArray::
    View< DataType, KokkosArray::LayoutLeft, device_type > left_view ;

  typedef KokkosArray::
    View< DataType, KokkosArray::LayoutRight, device_type > right_view ;

  typedef typename left_view ::shape_type  left_shape ;
  typedef typename right_view::shape_type  right_shape ;

  left_shape   lsh ;
  right_shape  rsh ;
  left_view    left ;
  right_view   right ;
  long         left_alloc ;
  long         right_alloc ;

  TestViewOperator_LeftAndRight()
    : lsh()
    , rsh()
    , left(  "left" )
    , right( "right" )
    , left_alloc( allocation_count( left ) )
    , right_alloc( allocation_count( right ) )
    {}

  static void apply()
  {
    TestViewOperator_LeftAndRight driver ;

    ASSERT_TRUE( (long) KokkosArray::Impl::cardinality_count( driver.lsh ) <= driver.left_alloc );
    ASSERT_TRUE( (long) KokkosArray::Impl::cardinality_count( driver.rsh ) <= driver.right_alloc );

    const int error_flag = KokkosArray::parallel_reduce( 1 , driver );

    ASSERT_EQ( error_flag , 0 );
  }

  KOKKOSARRAY_INLINE_FUNCTION
  void operator()( const size_type , value_type & update ) const
  {
    for ( unsigned i0 = 0 ; i0 < lsh.N0 ; ++i0 )
    {
      if ( & left(i0)  != & left(i0,0) )  { update |= 3 ; }
      if ( & left(i0)  != & left(i0,0,0) )  { update |= 3 ; }
      if ( & right(i0) != & right(i0,0) ) { update |= 3 ; }
      if ( & right(i0) != & right(i0,0,0) ) { update |= 3 ; }
    }
  }
};

/*--------------------------------------------------------------------------*/

template< typename T, class DeviceType >
class TestViewAPI
{
public:
  typedef DeviceType        device ;
  typedef KokkosArray::Host host ;

  TestViewAPI()
  {
    run_test_mirror();
    run_test();
    run_test_scalar();
    run_test_const();
    run_test_subview();
    run_test_vector();

    TestViewOperator< T , device >::apply();
    TestViewOperator_LeftAndRight< int[2][3][4][2][3][4][2][3] , device >::apply();
    TestViewOperator_LeftAndRight< int[2][3][4][2][3][4][2] , device >::apply();
    TestViewOperator_LeftAndRight< int[2][3][4][2][3][4] , device >::apply();
    TestViewOperator_LeftAndRight< int[2][3][4][2][3] , device >::apply();
    TestViewOperator_LeftAndRight< int[2][3][4][2] , device >::apply();
    TestViewOperator_LeftAndRight< int[2][3][4] , device >::apply();
    TestViewOperator_LeftAndRight< int[2][3] , device >::apply();
    TestViewOperator_LeftAndRight< int[2] , device >::apply();
  }

  enum { N0 = 1000 ,
         N1 = 3 ,
         N2 = 5 ,
         N3 = 7 };

  typedef KokkosArray::View< T , device > dView0 ;
  typedef KokkosArray::View< T* , device > dView1 ;
  typedef KokkosArray::View< T*[N1] , device > dView2 ;
  typedef KokkosArray::View< T*[N1][N2] , device > dView3 ;
  typedef KokkosArray::View< T*[N1][N2][N3] , device > dView4 ;
  typedef KokkosArray::View< const T*[N1][N2][N3] , device > const_dView4 ;

  typedef KokkosArray::View< T*[N1][N2][N3], device, KokkosArray::MemoryUnmanaged > dView4_unmanaged ;

  static void run_test_mirror()
  {
    typedef KokkosArray::View< int , host > view_type ;
    typedef typename view_type::HostMirror mirror_type ;
    view_type a("a");
    mirror_type am = KokkosArray::create_mirror_view(a);
    mirror_type ax = KokkosArray::create_mirror(a);
    ASSERT_EQ( & a() , & am() );
  }

  static void run_test_scalar()
  {
    typedef typename dView0::HostMirror  hView0 ;

    dView0 dx , dy ;
    hView0 hx , hy ;

    dx = dView0( "dx" );
    dy = dView0( "dy" );

    hx = KokkosArray::create_mirror( dx );
    hy = KokkosArray::create_mirror( dy );

    hx = 1 ;

    KokkosArray::deep_copy( dx , hx );
    KokkosArray::deep_copy( dy , dx );
    KokkosArray::deep_copy( hy , dy );

    ASSERT_EQ( hx(), hy() );
  }

  static void run_test()
  {
    typedef typename dView0::HostMirror  hView0 ;
    typedef typename dView1::HostMirror  hView1 ;
    typedef typename dView2::HostMirror  hView2 ;
    typedef typename dView3::HostMirror  hView3 ;
    typedef typename dView4::HostMirror  hView4 ;

    dView4 dx , dy , dz ;
    hView4 hx , hy , hz ;

    ASSERT_TRUE( dx.is_null() );
    ASSERT_TRUE( dy.is_null() );
    ASSERT_TRUE( dz.is_null() );
    ASSERT_TRUE( hx.is_null() );
    ASSERT_TRUE( hy.is_null() );
    ASSERT_TRUE( hz.is_null() );
    ASSERT_EQ( dx.dimension_0() , 0u );
    ASSERT_EQ( dy.dimension_0() , 0u );
    ASSERT_EQ( dz.dimension_0() , 0u );
    ASSERT_EQ( hx.dimension_0() , 0u );
    ASSERT_EQ( hy.dimension_0() , 0u );
    ASSERT_EQ( hz.dimension_0() , 0u );
    ASSERT_EQ( dx.dimension_1() , unsigned(N1) );
    ASSERT_EQ( dy.dimension_1() , unsigned(N1) );
    ASSERT_EQ( dz.dimension_1() , unsigned(N1) );
    ASSERT_EQ( hx.dimension_1() , unsigned(N1) );
    ASSERT_EQ( hy.dimension_1() , unsigned(N1) );
    ASSERT_EQ( hz.dimension_1() , unsigned(N1) );

    dx = dView4( "dx" , N0 );
    dy = dView4( "dy" , N0 );



    dView4_unmanaged unmanaged_dx = dx;
    dView4_unmanaged unmanaged_from_ptr_dx = dView4_unmanaged(dx.ptr_on_device(),dx.dimension_0());
    const_dView4 const_dx = dx ;


    ASSERT_FALSE( dx.is_null() );
    ASSERT_FALSE( const_dx.is_null() );
    ASSERT_FALSE( unmanaged_dx.is_null() );
    ASSERT_FALSE( unmanaged_from_ptr_dx.is_null() );
    ASSERT_FALSE( dy.is_null() );
    ASSERT_NE( dx , dy );

    ASSERT_EQ( dx.dimension_0() , unsigned(N0) );
    ASSERT_EQ( dx.dimension_1() , unsigned(N1) );
    ASSERT_EQ( dx.dimension_2() , unsigned(N2) );
    ASSERT_EQ( dx.dimension_3() , unsigned(N3) );

    ASSERT_EQ( dy.dimension_0() , unsigned(N0) );
    ASSERT_EQ( dy.dimension_1() , unsigned(N1) );
    ASSERT_EQ( dy.dimension_2() , unsigned(N2) );
    ASSERT_EQ( dy.dimension_3() , unsigned(N3) );

    ASSERT_EQ( unmanaged_from_ptr_dx.capacity(),unsigned(N0)*unsigned(N1)*unsigned(N2)*unsigned(N3) );

    hx = KokkosArray::create_mirror( dx );
    hy = KokkosArray::create_mirror( dy );

    size_t count = 0 ;
    for ( size_t ip = 0 ; ip < N0 ; ++ip ) {
    for ( size_t i1 = 0 ; i1 < hx.dimension_1() ; ++i1 ) {
    for ( size_t i2 = 0 ; i2 < hx.dimension_2() ; ++i2 ) {
    for ( size_t i3 = 0 ; i3 < hx.dimension_3() ; ++i3 ) {
      hx(ip,i1,i2,i3) = ++count ;
    }}}}

    KokkosArray::deep_copy( dx , hx );
    KokkosArray::deep_copy( dy , dx );
    KokkosArray::deep_copy( hy , dy );

    for ( size_t ip = 0 ; ip < N0 ; ++ip ) {
    for ( size_t i1 = 0 ; i1 < N1 ; ++i1 ) {
    for ( size_t i2 = 0 ; i2 < N2 ; ++i2 ) {
    for ( size_t i3 = 0 ; i3 < N3 ; ++i3 ) {
      { ASSERT_EQ( hx(ip,i1,i2,i3) , hy(ip,i1,i2,i3) ); }
    }}}}

    dz = dx ; ASSERT_EQ( dx, dz); ASSERT_NE( dy, dz);
    dz = dy ; ASSERT_EQ( dy, dz); ASSERT_NE( dx, dz);

    dx = dView4();
    ASSERT_TRUE( dx.is_null() );
    ASSERT_FALSE( dy.is_null() );
    ASSERT_FALSE( dz.is_null() );
    dy = dView4();
    ASSERT_TRUE( dx.is_null() );
    ASSERT_TRUE( dy.is_null() );
    ASSERT_FALSE( dz.is_null() );
    dz = dView4();
    ASSERT_TRUE( dx.is_null() );
    ASSERT_TRUE( dy.is_null() );
    ASSERT_TRUE( dz.is_null() );
  }

  typedef T DataType[2] ;

  static void
  check_auto_conversion_to_const(
     const KokkosArray::View< const DataType , device > & arg_const ,
     const KokkosArray::View< DataType , device > & arg )
  {
    ASSERT_TRUE( arg_const == arg );
  }

  static void run_test_const()
  {
    typedef KokkosArray::View< DataType , device > typeX ;
    typedef KokkosArray::View< const DataType , device > const_typeX ;
    typedef KokkosArray::View< const DataType , device , KokkosArray::MemoryRandomRead > const_typeR ;
    typeX x( "X" );
    const_typeX xc = x ;
    const_typeR xr = x ;

    ASSERT_TRUE( xc == x );
    ASSERT_TRUE( x == xc );
    ASSERT_TRUE( x.ptr_on_device() == xr.ptr_on_device() );

    // typeX xf = xc ; // setting non-const from const must not compile

    check_auto_conversion_to_const( x , x );
  }

  static void run_test_subview()
  {
    typedef KokkosArray::View< const T , device > sView ;

    dView0 d0( "d0" );
    dView1 d1( "d1" , N0 );
    dView2 d2( "d2" , N0 );
    dView3 d3( "d3" , N0 );
    dView4 d4( "d4" , N0 );

    sView s0 = d0 ;
    sView s1 = KokkosArray::subview< sView >( d1 , 1 );
    sView s2 = KokkosArray::subview< sView >( d2 , 1 , 1 );
    sView s3 = KokkosArray::subview< sView >( d3 , 1 , 1 , 1 );
    sView s4 = KokkosArray::subview< sView >( d4 , 1 , 1 , 1 , 1 );
  }

  static void run_test_vector()
  {
    static const unsigned Length = 1000 , Count = 8 ;

    typedef KokkosArray::View< T* ,  KokkosArray::LayoutLeft , host > vector_type ;
    typedef KokkosArray::View< T** , KokkosArray::LayoutLeft , host > multivector_type ;

    typedef KokkosArray::View< T* ,  KokkosArray::LayoutRight , host > vector_right_type ;
    typedef KokkosArray::View< T** , KokkosArray::LayoutRight , host > multivector_right_type ;

    typedef KokkosArray::View< const T* , KokkosArray::LayoutRight, host > const_vector_right_type ;
    typedef KokkosArray::View< const T* , KokkosArray::LayoutLeft , host > const_vector_type ;
    typedef KokkosArray::View< const T** , KokkosArray::LayoutLeft , host > const_multivector_type ;

    multivector_type mv = multivector_type( "mv" , Length , Count );
    multivector_right_type mv_right = multivector_right_type( "mv" , Length , Count );

    vector_type v1 = KokkosArray::subview< vector_type >( mv , 0 );
    vector_type v2 = KokkosArray::subview< vector_type >( mv , 1 );
    vector_type v3 = KokkosArray::subview< vector_type >( mv , 2 );

    multivector_type mv1 = KokkosArray::subview< multivector_type >( mv , std::make_pair( 1 , 998 ) ,
                                                                          std::make_pair( 2 , 5 ) );

    multivector_right_type mvr1 =
      KokkosArray::subview< multivector_right_type >( mv_right ,
                                                      std::make_pair( 1 , 998 ) ,
                                                      std::make_pair( 2 , 5 ) );

    const_vector_type cv1 = KokkosArray::subview< const_vector_type >( mv , 0 );
    const_vector_type cv2 = KokkosArray::subview< const_vector_type >( mv , 1 );
    const_vector_type cv3 = KokkosArray::subview< const_vector_type >( mv , 2 );

    vector_right_type vr1 = KokkosArray::subview< vector_right_type >( mv , 0 );
    vector_right_type vr2 = KokkosArray::subview< vector_right_type >( mv , 1 );
    vector_right_type vr3 = KokkosArray::subview< vector_right_type >( mv , 2 );

    const_vector_right_type cvr1 = KokkosArray::subview< const_vector_right_type >( mv , 0 );
    const_vector_right_type cvr2 = KokkosArray::subview< const_vector_right_type >( mv , 1 );
    const_vector_right_type cvr3 = KokkosArray::subview< const_vector_right_type >( mv , 2 );

    ASSERT_TRUE( & v1[0] == & mv(0,0) );
    ASSERT_TRUE( & v2[0] == & mv(0,1) );
    ASSERT_TRUE( & v3[0] == & mv(0,2) );

    ASSERT_TRUE( & cv1[0] == & mv(0,0) );
    ASSERT_TRUE( & cv2[0] == & mv(0,1) );
    ASSERT_TRUE( & cv3[0] == & mv(0,2) );

    ASSERT_TRUE( & vr1[0] == & mv(0,0) );
    ASSERT_TRUE( & vr2[0] == & mv(0,1) );
    ASSERT_TRUE( & vr3[0] == & mv(0,2) );

    ASSERT_TRUE( & cvr1[0] == & mv(0,0) );
    ASSERT_TRUE( & cvr2[0] == & mv(0,1) );
    ASSERT_TRUE( & cvr3[0] == & mv(0,2) );

    ASSERT_TRUE( & mv1(0,0) == & mv( 1 , 2 ) );
    ASSERT_TRUE( & mv1(1,1) == & mv( 2 , 3 ) );
    ASSERT_TRUE( & mv1(3,2) == & mv( 4 , 4 ) );
    ASSERT_TRUE( & mvr1(0,0) == & mv_right( 1 , 2 ) );
    ASSERT_TRUE( & mvr1(1,1) == & mv_right( 2 , 3 ) );
    ASSERT_TRUE( & mvr1(3,2) == & mv_right( 4 , 4 ) );

    const_vector_type c_cv1( v1 );
    typename vector_type::const_type c_cv2( v2 );
    typename const_vector_type::const_type c_ccv2( v2 );

    const_multivector_type cmv( mv );
    typename multivector_type::const_type cmvX( cmv );
    typename const_multivector_type::const_type ccmvX( cmv );
  }
};

} // namespace Test

/*--------------------------------------------------------------------------*/

