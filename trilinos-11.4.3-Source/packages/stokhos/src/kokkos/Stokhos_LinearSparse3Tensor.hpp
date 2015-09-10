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

#ifndef STOKHOS_LINEAR_SPARSE_3_TENSOR_HPP
#define STOKHOS_LINEAR_SPARSE_3_TENSOR_HPP

#include "KokkosArray_View.hpp"

#include "Stokhos_Multiply.hpp"
#include "Stokhos_ProductBasis.hpp"
#include "Stokhos_Sparse3Tensor.hpp"
#include "Teuchos_ParameterList.hpp"

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

namespace Stokhos {

/** \brief  Sparse product tensor with replicated entries
 *          to provide subsets with a given coordinate.
 */
template< typename ValueType , class DeviceType , int BlockSize >
class LinearSparse3Tensor {
public:

  typedef DeviceType                       device_type ;
  typedef typename device_type::size_type  size_type ;
  typedef ValueType                        value_type ;

  static const int block_size = BlockSize;

private:

  typedef KokkosArray::View< value_type[], device_type > value_array_type ;

  value_array_type   m_value ;
  size_type          m_dim ;
  size_type          m_aligned_dim ;
  size_type          m_nnz ;
  size_type          m_flops ;
  bool               m_symmetric ;

public:

  inline
  ~LinearSparse3Tensor() {}

  inline
  LinearSparse3Tensor() :
    m_value() ,
    m_dim() ,
    m_aligned_dim(),
    m_nnz(0) ,
    m_flops(0) ,
    m_symmetric(false) {}

  inline
  LinearSparse3Tensor( const LinearSparse3Tensor & rhs ) :
    m_value( rhs.m_value ) ,
    m_dim( rhs.m_dim ),
    m_aligned_dim( rhs.m_aligned_dim ),
    m_nnz( rhs.m_nnz ) ,
    m_flops( rhs.m_flops ) ,
    m_symmetric( rhs.m_symmetric ) {}

  inline
  LinearSparse3Tensor & operator = ( const LinearSparse3Tensor & rhs )
  {
    m_value = rhs.m_value ;
    m_dim = rhs.m_dim ;
    m_aligned_dim = rhs.m_aligned_dim;
    m_nnz = rhs.m_nnz;
    m_flops = rhs.m_flops;
    m_symmetric = rhs.m_symmetric;
    return *this ;
  }

  /** \brief  Dimension of the tensor. */
  KOKKOSARRAY_INLINE_FUNCTION
  size_type dimension() const { return m_dim ; }

  /** \brief  Dimension of the tensor. */
  KOKKOSARRAY_INLINE_FUNCTION
  size_type aligned_dimension() const { return m_aligned_dim ; }

  /** \brief  Number of sparse entries. */
  KOKKOSARRAY_INLINE_FUNCTION
  size_type entry_count() const
  { return m_value.dimension_0(); }

  /** \brief Is tensor built from symmetric PDFs. */
   KOKKOSARRAY_INLINE_FUNCTION
   bool symmetric() const
   { return m_symmetric; }

  /** \brief  Value for entry 'entry' */
  KOKKOSARRAY_INLINE_FUNCTION
  const value_type & value( const size_type entry ) const
  { return m_value( entry ); }

  /** \brief Number of non-zero's */
  KOKKOSARRAY_INLINE_FUNCTION
  size_type num_non_zeros() const
  { return m_nnz; }

  /** \brief Number flop's per multiply-add */
  KOKKOSARRAY_INLINE_FUNCTION
  size_type num_flops() const
  { return m_flops; }

  template <typename OrdinalType>
  static LinearSparse3Tensor
  create( const Stokhos::ProductBasis<OrdinalType,ValueType>& basis,
          const Stokhos::Sparse3Tensor<OrdinalType,ValueType>& Cijk,
          const Teuchos::ParameterList& params)
  {
    const bool symmetric = params.get<bool>("Symmetric");

    // Allocate tensor data -- currently assuming isotropic
    const size_type dim = basis.size();
    LinearSparse3Tensor tensor ;
    tensor.m_dim = dim;
    tensor.m_aligned_dim = dim;
    if (tensor.m_aligned_dim % block_size)
      tensor.m_aligned_dim += block_size - tensor.m_aligned_dim % block_size;
    tensor.m_symmetric = symmetric;
    tensor.m_nnz = symmetric ? 2 : 3 ;
    tensor.m_value = value_array_type( "value" , tensor.m_nnz );

    // Create mirror, is a view if is host memory
    typename value_array_type::HostMirror
      host_value = KokkosArray::create_mirror_view( tensor.m_value );

    // Get Cijk values
    Teuchos::Array< Teuchos::RCP<const Stokhos::OneDOrthogPolyBasis<OrdinalType,ValueType> > > bases = basis.getCoordinateBases();
    Teuchos::RCP< Stokhos::Dense3Tensor<OrdinalType,ValueType> > cijk =
      bases[0]->computeTripleProductTensor();
    // For non-isotropic, need to take products of these over basis components
    host_value(0) = (*cijk)(0,0,0);
    host_value(1) = (*cijk)(0,1,1);
    if (!symmetric)
      host_value(2) = (*cijk)(1,1,1);

    // Copy data to device if necessary
    KokkosArray::deep_copy( tensor.m_value , host_value );

    tensor.m_flops = 8*dim;
    if (!symmetric)
      tensor.m_flops += 2*dim ;

    return tensor ;
  }
};

template< class Device , typename OrdinalType , typename ValueType , int BlockSize >
LinearSparse3Tensor<ValueType, Device,BlockSize>
create_linear_sparse_3_tensor(
  const Stokhos::ProductBasis<OrdinalType,ValueType>& basis,
  const Stokhos::Sparse3Tensor<OrdinalType,ValueType>& Cijk,
  const Teuchos::ParameterList& params)
{
  return LinearSparse3Tensor<ValueType, Device, BlockSize>::create(
    basis, Cijk, params );
}

} /* namespace Stokhos */

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

#endif /* #ifndef STOKHOS_LINEAR_SPARSE_3_TENSOR_HPP */
