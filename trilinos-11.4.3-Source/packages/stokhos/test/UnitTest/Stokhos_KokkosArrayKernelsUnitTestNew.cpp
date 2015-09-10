// @HEADER
// ***********************************************************************
// 
//                           Stokhos Package
//                 Copyright (2009) Sandia Corporation
// 
// Under terms of Contract DE-AC04-94AL85000, there is a non-exclusive
// license for use of this work by or on behalf of the U.S. Government.
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
// Questions? Contact Eric T. Phipps (etphipp@sandia.gov).
// 
// ***********************************************************************
// @HEADER

#include "Teuchos_UnitTestHarness.hpp"
#include "Teuchos_UnitTestRepository.hpp"
#include "Teuchos_GlobalMPISession.hpp"

#include "Stokhos_KokkosArrayKernelsUnitTestNew.hpp"

#include "Stokhos_Host_CrsMatrix.hpp"
#include "Stokhos_Host_BlockCrsMatrix.hpp"
#include "Stokhos_Host_StochasticProductTensor.hpp"
#include "Stokhos_Host_CrsProductTensor.hpp"
#include "Stokhos_Host_FlatSparse3Tensor.hpp"
#include "Stokhos_Host_FlatSparse3Tensor_kji.hpp"
#include "Stokhos_Host_TiledCrsProductTensor.hpp"

#include "Stokhos_LexicographicBlockSparse3Tensor.hpp"
#include "Stokhos_Host_LexicographicBlockSparse3Tensor.hpp"
#include "Stokhos_Host_LinearSparse3Tensor.hpp"

#include "KokkosArray_hwloc.hpp"
#include "KokkosArray_Cuda.hpp"

using namespace KokkosArrayKernelsUnitTest;

UnitTestSetup setup;

TEUCHOS_UNIT_TEST( Stokhos_KokkosArrayKernels, CrsMatrixFree_Host ) {
  typedef double Scalar;
  typedef KokkosArray::Host Device;
  typedef Stokhos::DefaultSparseMatOps SparseMatOps;
  bool test_block = false;

  success = test_crs_matrix_free<Scalar,Device,SparseMatOps>(
    setup, test_block, out);
}

#ifdef HAVE_STOKHOS_MKL

TEUCHOS_UNIT_TEST( Stokhos_KokkosArrayKernels, CrsMatrixFree_HostMKL ) {
  typedef double Scalar;
  typedef KokkosArray::Host Device;
  typedef Stokhos::MKLSparseMatOps SparseMatOps;
  bool test_block = true;

  success = test_crs_matrix_free<Scalar,Device,SparseMatOps>(
    setup, test_block, out);
}

#endif

TEUCHOS_UNIT_TEST( Stokhos_KokkosArrayKernels, CrsProductTensor_Host ) {
  typedef double Scalar;
  typedef KokkosArray::Host Device;
  typedef Stokhos::CrsProductTensor<Scalar,Device> Tensor;

  success = test_crs_product_tensor<Scalar,Tensor,Device>(setup, out);
}

TEUCHOS_UNIT_TEST( Stokhos_KokkosArrayKernels, TiledCrsProductTensor_Host ) {
  typedef double Scalar;
  typedef KokkosArray::Host Device;
  typedef Stokhos::TiledCrsProductTensor<Scalar,Device> Tensor;

  Teuchos::ParameterList params;
  params.set("Tile Size", 10);
  params.set("Max Tiles", 10000);
  success = test_crs_product_tensor<Scalar,Tensor,Device>(setup, out, params);
}

TEUCHOS_UNIT_TEST( Stokhos_KokkosArrayKernels, FlatSparse3Tensor_Host ) {
  typedef double Scalar;
  typedef KokkosArray::Host Device;
  typedef Stokhos::FlatSparse3Tensor<Scalar,Device> Tensor;

  success = test_crs_product_tensor<Scalar,Tensor,Device>(setup, out);
}

TEUCHOS_UNIT_TEST( Stokhos_KokkosArrayKernels, FlatSparse3Tensor_kji_Host ) {
  typedef double Scalar;
  typedef KokkosArray::Host Device;
  typedef Stokhos::FlatSparse3Tensor_kji<Scalar,Device> Tensor;

  success = test_crs_product_tensor<Scalar,Tensor,Device>(setup, out);
}

TEUCHOS_UNIT_TEST( Stokhos_KokkosArrayKernels, ProductTensorCijk ) {
  success = true;

  typedef double value_type;
  typedef KokkosArray::Host Device;
  typedef Stokhos::CrsProductTensor< value_type , Device > tensor_type ;

  tensor_type tensor =
   Stokhos::create_product_tensor<Device>( *setup.basis, *setup.Cijk );

  for (int i=0; i<setup.stoch_length; ++i) {
    const size_t iEntryBeg = tensor.entry_begin(i);
    const size_t iEntryEnd = tensor.entry_end(i);
    for (size_t iEntry = iEntryBeg ; iEntry < iEntryEnd ; ++iEntry ) {
      const size_t kj = tensor.coord( iEntry );
      const size_t j  = kj & 0x0ffff;
      const size_t k  = kj >> 16;
      // const size_t j = tensor.coord(iEntry,0);
      // const size_t k = tensor.coord(iEntry,1);
      double c2 = tensor.value(iEntry);
      if (j == k) c2 *= 2.0;

      int ii = setup.inv_perm[i];
      int jj = setup.inv_perm[j];
      int kk = setup.inv_perm[k];
      double c = setup.Cijk->getValue(ii,jj,kk);

      if (std::abs(c-c2) > std::abs(c)*setup.rel_tol + setup.abs_tol) {
        out << "(" << ii << "," << jj << "," << kk << "):  " << c
            << " == " << c2 << " failed!" << std::endl;
        success = false;
      }
    }
  }
}

TEUCHOS_UNIT_TEST( Stokhos_KokkosArrayKernels, TiledProductTensorCijk ) {
  success = true;

  typedef double value_type;
  typedef KokkosArray::Host Device;
  typedef Stokhos::TiledCrsProductTensor< value_type , Device > tensor_type ;

  Teuchos::ParameterList params;
  params.set("Tile Size",10);
  params.set("Max Tiles",10000);

  tensor_type tensor =
    Stokhos::create_tiled_product_tensor<Device>( *setup.basis, *setup.Cijk,
                                                  params );

  // This is a valid test only with no symmetry
  // TEUCHOS_TEST_EQUALITY( tensor.entry_count(), setup.Cijk->num_entries(),
  //                        out, success );

  const size_t n_tile = tensor.num_tiles();
  for ( size_t tile = 0 ; tile < n_tile ; ++tile ) {
    const size_t i_offset = tensor.offset(tile, 0);
    const size_t j_offset = tensor.offset(tile, 1);
    const size_t k_offset = tensor.offset(tile, 2);
    const size_t n_row = tensor.num_rows(tile);

    for (size_t i=0; i<n_row; ++i) {
      const size_t iEntryBeg = tensor.entry_begin(tile,i);
      const size_t iEntryEnd = tensor.entry_end(tile,i);
      for (size_t iEntry = iEntryBeg ; iEntry < iEntryEnd ; ++iEntry ) {
        const size_t j = tensor.coord(iEntry,0);
        const size_t k = tensor.coord(iEntry,1);
        double c2 = tensor.value(iEntry);
        int ii = i + i_offset;
        int jj = j + j_offset;
        int kk = k + k_offset;
        if (jj == kk)
          c2 *= 2.0;
        double c = setup.Cijk->getValue(ii,jj,kk);

        if (std::abs(c-c2) > std::abs(c)*setup.rel_tol + setup.abs_tol) {
          out << "(" << ii << "," << jj << "," << kk << "):  " << c
              << " == " << c2 << " failed!" << std::endl;
          success = false;
        }
      }
    }
  }
}

TEUCHOS_UNIT_TEST( Stokhos_KokkosArrayKernels, FlatSparseCijk ) {
  success = true;

  typedef double value_type;
  typedef KokkosArray::Host Device;
  typedef Stokhos::FlatSparse3Tensor< value_type , Device > tensor_type ;
  typedef size_t size_type;

  tensor_type tensor =
   Stokhos::create_flat_sparse_3_tensor<Device>( *setup.basis, *setup.Cijk );

  for (int i=0; i<setup.stoch_length; ++i) {
    const size_type nk = tensor.num_k(i);
    const size_type kBeg = tensor.k_begin(i);
    const size_type kEnd = kBeg + nk;
    for (size_type kEntry = kBeg; kEntry < kEnd; ++kEntry) {
      const size_type k = tensor.k_coord(kEntry);
      const size_type nj = tensor.num_j(kEntry);
      const size_type jBeg = tensor.j_begin(kEntry);
      const size_type jEnd = jBeg + nj;
      for (size_type jEntry = jBeg; jEntry < jEnd; ++jEntry) {
        const size_type j = tensor.j_coord(jEntry);
        double c2 = tensor.value(jEntry);
        if (j == k) c2 *= 2.0;
        double c = setup.Cijk->getValue(i,j,k);
        if (std::abs(c-c2) > std::abs(c)*setup.rel_tol + setup.abs_tol) {
          out << "(" << i << "," << j << "," << k << "):  " << c
              << " == " << c2 << " failed!" << std::endl;
          success = false;
        }
      }
    }
  }
}

TEUCHOS_UNIT_TEST( Stokhos_KokkosArrayKernels, FlatSparseCijk_kji ) {
  success = true;

  typedef double value_type;
  typedef KokkosArray::Host Device;
  typedef Stokhos::FlatSparse3Tensor_kji< value_type , Device > tensor_type ;
  typedef size_t size_type;

  tensor_type tensor =
   Stokhos::create_flat_sparse_3_tensor_kji<Device>(*setup.basis, *setup.Cijk);
  const size_type nk = tensor.num_k();

  for ( size_type k = 0; k < nk; ++k) {
    const size_type nj = tensor.num_j(k);
    const size_type jBeg = tensor.j_begin(k);
    const size_type jEnd = jBeg + nj;
    for (size_type jEntry = jBeg; jEntry < jEnd; ++jEntry) {
      const size_type j = tensor.j_coord(jEntry);
      const size_type ni = tensor.num_i(jEntry);
      const size_type iBeg = tensor.i_begin(jEntry);
      const size_type iEnd = iBeg + ni;
      for (size_type iEntry = iBeg; iEntry < iEnd; ++iEntry) {
        const size_type i = tensor.i_coord(iEntry);
        double c2 = tensor.value(iEntry);
        if (j == k) c2 *= 2.0;
        double c = setup.Cijk->getValue(i,j,k);
        if (std::abs(c-c2) > std::abs(c)*setup.rel_tol + setup.abs_tol) {
          out << "(" << i << "," << j << "," << k << "):  " << c
              << " == " << c2 << " failed!" << std::endl;
          success = false;
        }
      }
    }
  }
}

namespace KokkosArrayKernelsUnitTest {

  template< typename ScalarType , class Device >
  bool test_lexo_block_tensor(const UnitTestSetup& setup,
                              Teuchos::FancyOStream& out) {
    typedef ScalarType value_type ;
    typedef int ordinal_type;
    typedef KokkosArray::View< value_type** , KokkosArray::LayoutLeft ,
                               Device > block_vector_type ;
    typedef Stokhos::LexicographicBlockSparse3Tensor<value_type,Device> TensorType;
    typedef Stokhos::StochasticProductTensor< value_type , TensorType , Device > tensor_type ;

    typedef Stokhos::BlockCrsMatrix< tensor_type , value_type , Device > matrix_type ;
    typedef typename matrix_type::graph_type graph_type ;

    //------------------------------
    // Generate input multivector:

    block_vector_type x =
      block_vector_type( "x" , setup.stoch_length , setup.fem_length );
    block_vector_type y =
      block_vector_type( "y" , setup.stoch_length , setup.fem_length );

    typename block_vector_type::HostMirror hx =
      KokkosArray::create_mirror( x );

    for ( int iColFEM = 0 ;   iColFEM < setup.fem_length ;   ++iColFEM ) {
      for ( int iColStoch = 0 ; iColStoch < setup.stoch_length ; ++iColStoch ) {
        hx(iColStoch,iColFEM) =
          generate_vector_coefficient<ScalarType>(
            setup.fem_length , setup.stoch_length , iColFEM , iColStoch );
      }
    }

    KokkosArray::deep_copy( x , hx );

    //------------------------------

    matrix_type matrix ;

    /*
    typedef UnitTestSetup::abstract_basis_type abstract_basis_type;
    typedef UnitTestSetup::basis_type basis_type;
    typedef Stokhos::LexographicLess< Stokhos::MultiIndex<int> > less_type;
    typedef Stokhos::TotalOrderBasis<ordinal_type,value_type,less_type> product_basis_type;
    Teuchos::Array< Teuchos::RCP<const abstract_basis_type> > bases(setup.d);
    for (int i=0; i<setup.d; i++)
      bases[i] = rcp(new basis_type(setup.p,true));
    product_basis_type basis(bases, 1e-12);
    */
    const bool symmetric = true;
    Teuchos::RCP< Stokhos::LTBSparse3Tensor<ordinal_type, value_type> > Cijk =
      Stokhos::computeTripleProductTensorLTBBlockLeaf(*setup.basis, symmetric);

    matrix.block =
      Stokhos::create_stochastic_product_tensor< TensorType >( *setup.basis,
                                                               *Cijk );

    matrix.graph = KokkosArray::create_crsarray<graph_type>(
      std::string("test crs graph") , setup.fem_graph );

    matrix.values = block_vector_type(
      "matrix" , setup.stoch_length , setup.fem_graph_length );

    typename block_vector_type::HostMirror hM =
      KokkosArray::create_mirror( matrix.values );

    for ( int iRowFEM = 0 , iEntryFEM = 0 ; iRowFEM < setup.fem_length ; ++iRowFEM ) {
      for ( size_t iRowEntryFEM = 0 ; iRowEntryFEM < setup.fem_graph[iRowFEM].size() ; ++iRowEntryFEM , ++iEntryFEM ) {
        const int iColFEM = setup.fem_graph[iRowFEM][iRowEntryFEM] ;

        for ( int k = 0 ; k < setup.stoch_length ; ++k ) {
          hM(k,iEntryFEM) =
            generate_matrix_coefficient<ScalarType>(
              setup.fem_length , setup.stoch_length , iRowFEM , iColFEM , k );
          //hM(k,iEntryFEM) = 1.0;
        }
      }
    }

    KokkosArray::deep_copy( matrix.values , hM );

    //------------------------------

    Stokhos::multiply( matrix , x , y );

    typename block_vector_type::HostMirror hy = KokkosArray::create_mirror( y );
    KokkosArray::deep_copy( hy , y );

    bool success = setup.test_commuted_no_perm(hy, out);
    //bool success = true;
    return success;
  }

}

TEUCHOS_UNIT_TEST( Stokhos_KokkosArrayKernels, LexoBlockTensor_Host ) {
  typedef double Scalar;
  typedef KokkosArray::Host Device;

  success = test_lexo_block_tensor<Scalar,Device>(setup, out);
}

TEUCHOS_UNIT_TEST( Stokhos_KokkosArrayKernels, LinearTensorSymmetric_Host ) {
  typedef double Scalar;
  typedef KokkosArray::Host Device;
  const bool symmetric = true;

  UnitTestSetup s;
  s.setup(1, 10);
  success = test_linear_tensor<Scalar,Device,4>(s, out, symmetric);
}

TEUCHOS_UNIT_TEST( Stokhos_KokkosArrayKernels, LinearTensorAsymmetric_Host ) {
  typedef double Scalar;
  typedef KokkosArray::Host Device;
  const bool symmetric = false;

  UnitTestSetup s;
  s.setup(1, 10);
  success = test_linear_tensor<Scalar,Device,4>(s, out, symmetric);
}

int main( int argc, char* argv[] ) {
  Teuchos::GlobalMPISession mpiSession(&argc, &argv);
  setup.setup();

  // Initialize host
  const std::pair<unsigned,unsigned> core_topo =
    KokkosArray::hwloc::get_core_topology();
  const size_t core_capacity = KokkosArray::hwloc::get_core_capacity();
  const size_t gang_count = core_topo.first;
  const size_t gang_worker_count = core_topo.second * core_capacity;
  // const size_t gang_count = 1 ;
  // const size_t gang_worker_count = 1;
  KokkosArray::Host::initialize( gang_count , gang_worker_count );

#ifdef KOKKOSARRAY_HAVE_CUDA
  // Initialize Cuda
  KokkosArray::Cuda::initialize( KokkosArray::Cuda::SelectDevice(0) );
#endif

  // Run tests
  int ret = Teuchos::UnitTestRepository::runUnitTestsFromMain(argc, argv);

  // Finish up
  KokkosArray::Host::finalize();
#ifdef KOKKOSARRAY_HAVE_CUDA
  KokkosArray::Cuda::finalize();
#endif

  return ret;
}
