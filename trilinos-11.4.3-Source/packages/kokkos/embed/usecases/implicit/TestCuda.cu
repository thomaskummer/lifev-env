
#include <KokkosArray_Host.hpp>
#include <KokkosArray_Cuda.hpp>

#include <ParallelComm.hpp>
#include <TestImplicit.hpp>

namespace Test {

int test_cuda( comm::Machine machine , std::istream & input )
{
  const unsigned parallel_rank = comm::rank( machine );
  const unsigned device_count  = KokkosArray::Cuda::detect_device_count();

  unsigned device = 0 ;
  unsigned elem_beg = 3 ;
  unsigned elem_end = 4 ;
  unsigned run = 1 ;

  while ( ! input.eof() ) {
    std::string which ;

    input >> which ;

    if ( which == std::string("device") ) {
      input >> device ;
    }
    else if ( which == std::string("implicit") ) {
      input >> elem_beg ;
      input >> elem_end ;
      input >> run ;
    }
    else {
      std::cerr << "Expected \"device #Device\" OR \"implicit #ElemBeg #ElemEnd #Run\""
                << std::endl ;
      return -1 ;
    }
  }

  device += parallel_rank % device_count ;

  KokkosArray::Cuda::initialize( KokkosArray::Cuda::SelectDevice( device ) );

  {
    std::ostringstream label ;

    label << "Scalar, CudaArch[" << KokkosArray::Cuda::detect_device_arch()[device] << "]" ;

    implicit_driver<double,KokkosArray::Cuda>(
      label.str().c_str() , machine , 1 , elem_beg , elem_end , run );
  }

  {
    std::ostringstream label ;

    label << "Ensemble[32], CudaArch[" << KokkosArray::Cuda::detect_device_arch()[device] << "]" ;

    implicit_driver< KokkosArray::Array<double,32> , KokkosArray::Cuda>(
      label.str().c_str() , machine , 1 , elem_beg , elem_end , run );
  }

  KokkosArray::Cuda::finalize();

  return 0 ;
}

}

