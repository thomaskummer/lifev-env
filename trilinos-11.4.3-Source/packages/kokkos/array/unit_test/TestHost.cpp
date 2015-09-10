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

#include <KokkosArray_Host.hpp>
#include <KokkosArray_hwloc.hpp>

#include <KokkosArray_View.hpp>

#include <KokkosArray_CrsArray.hpp>

//----------------------------------------------------------------------------

#include <TestViewImpl.hpp>

#include <TestMemoryTracking.hpp>
#include <TestViewAPI.hpp>
#include <TestAtomic.hpp>

#include <TestCrsArray.hpp>
#include <TestReduce.hpp>
#include <TestMultiReduce.hpp>

namespace Test {

class host : public ::testing::Test {
protected:
  static void SetUpTestCase()
  {
    // Finalize without initialize is a no-op:
    KokkosArray::Host::finalize();

    // Initialize and finalize with no threads:
    KokkosArray::Host::initialize( 1 , 1 );
    KokkosArray::Host::finalize();

    const std::pair<unsigned,unsigned> core_top =
      KokkosArray::hwloc::get_core_topology();

    const unsigned core_size =
      KokkosArray::hwloc::get_core_capacity();

    const unsigned gang_count        = core_top.first ;
    const unsigned gang_worker_count = ( core_top.second * core_size ) / 2 ;

    // Quick attempt to verify thread start/terminate don't have race condition:
    for ( unsigned i = 0 ; i < 10 ; ++i ) {
      KokkosArray::Host::initialize( gang_count , gang_worker_count );
      KokkosArray::Host::finalize();
    }

    KokkosArray::Host::initialize( gang_count , gang_worker_count );
    KokkosArray::Host::print_configuration( std::cout );
  }

  static void TearDownTestCase()
  {
    KokkosArray::Host::finalize();
  }
};

TEST_F( host, memory_tracking) {
  TestMemoryTracking();
}

TEST_F( host, view_impl) {
  test_view_impl< KokkosArray::Host >();
}

TEST_F( host, view_api) {
  TestViewAPI< double , KokkosArray::Host >();
}


TEST_F( host, crsarray) {
  TestCrsArray< KokkosArray::Host >();
}

TEST_F( host, long_reduce) {
  TestReduce< long ,   KokkosArray::Host >( 1000000 );
}

TEST_F( host, double_reduce) {
  TestReduce< double ,   KokkosArray::Host >( 1000000 );
}

TEST_F( host, long_reduce_dynamic ) {
  TestReduceDynamic< long ,   KokkosArray::Host >( 1000000 );
}

TEST_F( host, double_reduce_dynamic ) {
  TestReduceDynamic< double ,   KokkosArray::Host >( 1000000 );
}

TEST_F( host, long_reduce_dynamic_view ) {
  TestReduceDynamicView< long ,   KokkosArray::Host >( 1000000 );
}

TEST_F( host, long_multi_reduce) {
  TestReduceMulti< long , KokkosArray::Host >( 1000000 , 7 );
}

TEST_F( host , view_remap )
{
  enum { N0 = 3 , N1 = 2 , N2 = 8 , N3 = 9 };

  typedef KokkosArray::View< double*[N1][N2][N3] ,
                             KokkosArray::LayoutRight ,
                             KokkosArray::Host > output_type ;

  typedef KokkosArray::View< int**[N2][N3] ,
                             KokkosArray::LayoutLeft ,
                             KokkosArray::Host > input_type ;

  typedef KokkosArray::View< int*[N0][N2][N3] ,
                             KokkosArray::LayoutLeft ,
                             KokkosArray::Host > diff_type ;

  output_type output( "output" , N0 );
  input_type  input ( "input" , N0 , N1 );
  diff_type   diff  ( "diff" , N0 );

  int value = 0 ;
  for ( size_t i3 = 0 ; i3 < N3 ; ++i3 ) {
  for ( size_t i2 = 0 ; i2 < N2 ; ++i2 ) {
  for ( size_t i1 = 0 ; i1 < N1 ; ++i1 ) {
  for ( size_t i0 = 0 ; i0 < N0 ; ++i0 ) {
    input(i0,i1,i2,i3) = ++value ;
  }}}}

  // KokkosArray::deep_copy( diff , input ); // throw with incompatible shape
  KokkosArray::deep_copy( output , input );

  value = 0 ;
  for ( size_t i3 = 0 ; i3 < N3 ; ++i3 ) {
  for ( size_t i2 = 0 ; i2 < N2 ; ++i2 ) {
  for ( size_t i1 = 0 ; i1 < N1 ; ++i1 ) {
  for ( size_t i0 = 0 ; i0 < N0 ; ++i0 ) {
    ++value ;
    ASSERT_EQ( value , ((int) output(i0,i1,i2,i3) ) );
  }}}}
}

//----------------------------------------------------------------------------

struct HostFunctor
  : public KokkosArray::Impl::HostThreadWorker
{
  struct Finalize {

    typedef int value_type ;

    volatile int & flag ;

    void operator()( const value_type & value ) const
      { flag += value + 1 ; }

    Finalize( int & f ) : flag(f) {}
  };

  struct Reduce {

    typedef int value_type ;

    static void init( int & update ) { update = 0 ; }

    static void join( volatile int & update , const volatile int & input )
      { update += input ; }
  };

  typedef KokkosArray::Impl::ReduceOperator< Reduce , Finalize > reduce_type ;

  typedef int value_type ;

  const reduce_type m_reduce ;

  HostFunctor( int & f ) : m_reduce(f)
    { KokkosArray::Impl::HostThreadWorker::execute(); }

  void execute_on_thread( KokkosArray::Impl::HostThread & thread ) const
    {
      m_reduce.init( thread.reduce_data() );

      thread.barrier();
      thread.barrier();

      // Reduce to master thread:
      thread.reduce( m_reduce );
      if ( 0 == thread.rank() ) m_reduce.finalize( thread.reduce_data() );

      // Reduce to master thread:
      thread.reduce( m_reduce );
      if ( 0 == thread.rank() ) m_reduce.finalize( thread.reduce_data() );

      thread.end_barrier();
    }
};

TEST_F( host , host_thread )
{
  const int N = 1000 ;
  int flag = 0 ;

  for ( int i = 0 ; i < N ; ++i ) {
    HostFunctor tmp( flag );
  }

  ASSERT_EQ( flag , N * 2 );

  for ( int i = 0 ; i < 10 ; ++i ) {
    KokkosArray::Host::sleep();
    KokkosArray::Host::wake();
  }

}

//----------------------------------------------------------------------------

TEST_F( host , atomics )
{
  const int loop_count = 1e6 ;

  ASSERT_TRUE( ( TestAtomic::Loop<int,KokkosArray::Host>(loop_count,1) ) );
  ASSERT_TRUE( ( TestAtomic::Loop<int,KokkosArray::Host>(loop_count,2) ) );
  ASSERT_TRUE( ( TestAtomic::Loop<int,KokkosArray::Host>(loop_count,3) ) );

  ASSERT_TRUE( ( TestAtomic::Loop<unsigned int,KokkosArray::Host>(loop_count,1) ) );
  ASSERT_TRUE( ( TestAtomic::Loop<unsigned int,KokkosArray::Host>(loop_count,2) ) );
  ASSERT_TRUE( ( TestAtomic::Loop<unsigned int,KokkosArray::Host>(loop_count,3) ) );

  ASSERT_TRUE( ( TestAtomic::Loop<long int,KokkosArray::Host>(loop_count,1) ) );
  ASSERT_TRUE( ( TestAtomic::Loop<long int,KokkosArray::Host>(loop_count,2) ) );
  ASSERT_TRUE( ( TestAtomic::Loop<long int,KokkosArray::Host>(loop_count,3) ) );

  ASSERT_TRUE( ( TestAtomic::Loop<unsigned long int,KokkosArray::Host>(loop_count,1) ) );
  ASSERT_TRUE( ( TestAtomic::Loop<unsigned long int,KokkosArray::Host>(loop_count,2) ) );
  ASSERT_TRUE( ( TestAtomic::Loop<unsigned long int,KokkosArray::Host>(loop_count,3) ) );

  ASSERT_TRUE( ( TestAtomic::Loop<long long int,KokkosArray::Host>(loop_count,1) ) );
  ASSERT_TRUE( ( TestAtomic::Loop<long long int,KokkosArray::Host>(loop_count,2) ) );
  ASSERT_TRUE( ( TestAtomic::Loop<long long int,KokkosArray::Host>(loop_count,3) ) );

  ASSERT_TRUE( ( TestAtomic::Loop<double,KokkosArray::Host>(loop_count,1) ) );
  ASSERT_TRUE( ( TestAtomic::Loop<double,KokkosArray::Host>(loop_count,2) ) );
  ASSERT_TRUE( ( TestAtomic::Loop<double,KokkosArray::Host>(loop_count,3) ) );

  ASSERT_TRUE( ( TestAtomic::Loop<float,KokkosArray::Host>(100,1) ) );
  ASSERT_TRUE( ( TestAtomic::Loop<float,KokkosArray::Host>(100,2) ) );
  ASSERT_TRUE( ( TestAtomic::Loop<float,KokkosArray::Host>(100,3) ) );
}

//----------------------------------------------------------------------------

} // namespace test

