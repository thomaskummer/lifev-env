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

#ifndef STOKHOS_LEXICOGRAPHIC_BLOCK_SPARSE_3_TENSOR_HPP
#define STOKHOS_LEXICOGRAPHIC_BLOCK_SPARSE_3_TENSOR_HPP

#include "KokkosArray_View.hpp"

#include "Stokhos_Multiply.hpp"
#include "Stokhos_ProductBasis.hpp"
#include "Stokhos_LTBSparse3Tensor.hpp"
#include "Teuchos_ParameterList.hpp"

#include <sstream>
#include <fstream>

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

namespace Stokhos {

/** \brief  Sparse product tensor with replicated entries
 *          to provide subsets with a given coordinate.
 */
template< typename ValueType , class DeviceType >
class LexicographicBlockSparse3Tensor {
public:

  typedef DeviceType                       device_type;
  typedef typename device_type::size_type  size_type;
  typedef ValueType                        value_type;

private:

  typedef KokkosArray::View< int[][7] , KokkosArray::LayoutRight, device_type >       coord_array_type;
  typedef KokkosArray::View< value_type[], device_type >    value_array_type;

  coord_array_type  m_coord;
  value_array_type  m_value;
  size_type         m_dimension;
  size_type         m_flops;
  bool              m_symmetric;

public:

  inline
  ~LexicographicBlockSparse3Tensor() {}

  inline
  LexicographicBlockSparse3Tensor() :
    m_coord(),
    m_value(),
    m_dimension(),
    m_flops(0),
    m_symmetric(false) {}

  inline
  LexicographicBlockSparse3Tensor(
    const LexicographicBlockSparse3Tensor & rhs) :
    m_coord(rhs.m_coord),
    m_value(rhs.m_value),
    m_dimension(rhs.m_dimension),
    m_flops(rhs.m_flops),
    m_symmetric(rhs.m_symmetric) {}

  inline
  LexicographicBlockSparse3Tensor &operator=(
    const LexicographicBlockSparse3Tensor & rhs)
  {
    m_coord = rhs.m_coord;
    m_value = rhs.m_value;
    m_dimension = rhs.m_dimension;
    m_flops = rhs.m_flops;
    m_symmetric = rhs.m_symmetric;
    return *this ;
  }

  /** \brief  Dimension of the tensor. */
  KOKKOSARRAY_INLINE_FUNCTION
  size_type dimension() const { return m_dimension; }

  /** \brief  Number of coordinates. */
  KOKKOSARRAY_INLINE_FUNCTION
  size_type num_coord() const { return m_coord.dimension_0(); }

  /** \brief  Number of values. */
  KOKKOSARRAY_INLINE_FUNCTION
  size_type num_value() const { return m_value.dimension_0(); }

  /** \brief   */
  KOKKOSARRAY_INLINE_FUNCTION
  int get_i_begin(const size_type entry) const {
    return m_coord(entry,0);
  }

  /** \brief   */
  KOKKOSARRAY_INLINE_FUNCTION
  int get_j_begin(const size_type entry) const {
    return m_coord(entry,1);
  }

  /** \brief   */
  KOKKOSARRAY_INLINE_FUNCTION
  int get_k_begin(const size_type entry) const {
    return m_coord(entry,2);
  }

  /** \brief   */
  KOKKOSARRAY_INLINE_FUNCTION
  int get_p_i(const size_type entry) const {
    return m_coord(entry,3);
  }

  /** \brief   */
  KOKKOSARRAY_INLINE_FUNCTION
  int get_p_j(const size_type entry) const {
    return m_coord(entry,4);
  }

  /** \brief   */
  KOKKOSARRAY_INLINE_FUNCTION
  int get_p_k(const size_type entry) const {
    return m_coord(entry,5);
  }

  /** \brief   */
  KOKKOSARRAY_INLINE_FUNCTION
  int get_j_eq_k(const size_type entry) const {
    return m_coord(entry,6);
  }

  /** \brief  Cijk for entry 'entry' */
  KOKKOSARRAY_INLINE_FUNCTION
  const value_type& value(const size_type entry) const
  { return m_value(entry); }

  /** \brief Number of non-zero's */
  KOKKOSARRAY_INLINE_FUNCTION
  size_type num_non_zeros() const { return m_value.dimension_0(); }

  /** \brief Number flop's per multiply-add */
  KOKKOSARRAY_INLINE_FUNCTION
  size_type num_flops() const { return m_flops; }

  /** \brief Is PDF symmetric */
  KOKKOSARRAY_INLINE_FUNCTION
  bool symmetric() const { return m_symmetric; }

  template <typename OrdinalType>
  static LexicographicBlockSparse3Tensor
  create(const Stokhos::ProductBasis<OrdinalType,ValueType>& basis,
         const Stokhos::LTBSparse3Tensor<OrdinalType,ValueType>& Cijk,
         const Teuchos::ParameterList& params = Teuchos::ParameterList())
  {
    using Teuchos::Array;
    using Teuchos::RCP;

    // Allocate tensor data
    LexicographicBlockSparse3Tensor tensor ;
    tensor.m_dimension = basis.size();
    tensor.m_symmetric = Cijk.symmetric();
    tensor.m_coord = coord_array_type( "coord" , Cijk.num_leafs() );
    tensor.m_value = value_array_type( "value" , Cijk.num_entries() );

    // Create mirror, is a view if is host memory
    typename coord_array_type::HostMirror host_coord =
      KokkosArray::create_mirror_view( tensor.m_coord );
    typename value_array_type::HostMirror host_value =
      KokkosArray::create_mirror_view( tensor.m_value );

    // Fill flat 3 tensor
    typedef Stokhos::LTBSparse3Tensor<OrdinalType,ValueType> Cijk_type;
    typedef typename Cijk_type::CijkNode node_type;
    Array< RCP<const node_type> > node_stack;
    Array< OrdinalType > index_stack;
    node_stack.push_back(Cijk.getHeadNode());
    index_stack.push_back(0);
    RCP<const node_type> node;
    OrdinalType child_index;
    OrdinalType coord_index = 0;
    OrdinalType value_index = 0;
    tensor.m_flops = 0;
    while (node_stack.size() > 0) {
      node = node_stack.back();
      child_index = index_stack.back();

      // Leaf
      if (node->is_leaf) {
        host_coord(coord_index, 0) = node->i_begin;
        host_coord(coord_index, 1) = node->j_begin;
        host_coord(coord_index, 2) = node->k_begin;
        host_coord(coord_index, 3) = node->p_i;
        host_coord(coord_index, 4) = node->p_j;
        host_coord(coord_index, 5) = node->p_k;
        host_coord(coord_index, 6) = node->parent_j_equals_k;
        ++coord_index;
        for (OrdinalType i=0; i<node->my_num_entries; ++i)
          host_value(value_index++) = node->values[i];
        tensor.m_flops += 5*node->my_num_entries + node->i_size;
        node_stack.pop_back();
        index_stack.pop_back();
      }

      // More children to process -- process them first
      else if (child_index < node->children.size()) {
        ++index_stack.back();
        node = node->children[child_index];
        node_stack.push_back(node);
        index_stack.push_back(0);
      }

      // No more children
      else {
        node_stack.pop_back();
        index_stack.pop_back();
      }

    }
    TEUCHOS_ASSERT(coord_index == Cijk.num_leafs());
    TEUCHOS_ASSERT(value_index == Cijk.num_entries());

    /*
    // Save block volumes to file
    static int index = 0;
    std::stringstream file_name;
    file_name << "cijk_vol_" << index << ".txt";
    std::ofstream file(file_name.str().c_str());
    for (size_type i=0; i<coord_index; ++i) {
      int vol = host_coord(i,3) * host_coord(i,4) * host_coord(i,5);
      file << vol << std::endl;
    }
    file.close();
    index++;
    */

    // Copy data to device if necessary
    KokkosArray::deep_copy( tensor.m_coord , host_coord );
    KokkosArray::deep_copy( tensor.m_value , host_value );

    return tensor ;
  }
};

template< class Device , typename OrdinalType , typename ValueType >
LexicographicBlockSparse3Tensor<ValueType, Device>
create_lexicographic_block_sparse_3_tensor(
  const Stokhos::ProductBasis<OrdinalType,ValueType>& basis,
  const Stokhos::LTBSparse3Tensor<OrdinalType,ValueType>& Cijk,
  const Teuchos::ParameterList& params = Teuchos::ParameterList())
{
  return LexicographicBlockSparse3Tensor<ValueType, Device>::create(
    basis, Cijk, params);
}

} /* namespace Stokhos */

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

#endif /* #ifndef STOKHOS_LEXICOGRAPHIC_BLOCK_SPARSE_3_TENSOR_HPP */
