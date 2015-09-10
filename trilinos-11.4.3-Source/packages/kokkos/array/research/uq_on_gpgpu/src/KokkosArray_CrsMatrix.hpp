/*
//@HEADER
// ************************************************************************
// 
//    KokkosArray: Manycore Performance-Portable Multidimensional Arrays
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
// Questions? Contact H. Carter Edwards (hcedwar@sandia.gov)
// 
// ************************************************************************
//@HEADER
*/

#ifndef KOKKOSARRAY_CRSMATRIX_HPP
#define KOKKOSARRAY_CRSMATRIX_HPP

#include <KokkosArray_CrsArray.hpp>
#include <impl/KokkosArray_Multiply.hpp>

namespace KokkosArray {

/** \brief  CRS matrix.  */

template< typename ValueType , class Device >
class CrsMatrix {
public:
  typedef Device     device_type ;
  typedef ValueType  value_type ;

  View< value_type[] , device_type >   values ;
  CrsArray< int , device_type , void , int >  graph ;
};

template< typename MatrixValueType ,
          typename VectorValueType ,
          class Device >
void multiply( const CrsMatrix<MatrixValueType,Device> & A ,
               const View<VectorValueType[],Device>         & x ,
               const View<VectorValueType[],Device>         & y )
{
  typedef CrsMatrix<MatrixValueType,Device>  matrix_type ;
  typedef View<VectorValueType[],Device>     vector_type ;

  Impl::Multiply<matrix_type,vector_type,vector_type>::apply( A , x , y );
}

template< typename MatrixValueType ,
          typename VectorValueType ,
	  typename OrdinalType ,
          class Device >
void multiply( const CrsMatrix<MatrixValueType,Device> & A ,
	       const View<VectorValueType**, LayoutLeft, Device> & x ,
	       const View<VectorValueType**, LayoutLeft, Device> & y ,
	       const std::vector<OrdinalType>& col_indices, 
	       bool use_block_multiply = true)
{
  typedef CrsMatrix<MatrixValueType,Device>           matrix_type ;
  typedef View<VectorValueType[],Device>               vector_type ;
  typedef View<VectorValueType**, LayoutLeft, Device> multi_vector_type ;

  if (use_block_multiply)
    Impl::MMultiply<matrix_type,multi_vector_type,multi_vector_type>::apply( 
      A , x , y , col_indices );
  else {
    for (size_t i=0; i<col_indices.size(); ++i) {
      const vector_type x_view( x , col_indices[i] );
      const vector_type y_view( y , col_indices[i] );
      Impl::Multiply<matrix_type,vector_type,vector_type>::apply( 
	A , x_view , y_view );
    }
  }
}

template< typename MatrixValueType ,
          typename VectorValueType ,
	  class Device >
void multiply( const CrsMatrix<MatrixValueType,Device> & A ,
	       const std::vector< View<VectorValueType[], Device> > & x ,
	       const std::vector< View<VectorValueType[], Device> > & y ,
	       bool use_block_multiply = true)
{
  typedef CrsMatrix<MatrixValueType,Device>           matrix_type ;
  typedef View<VectorValueType[],Device>               vector_type ;

  if (use_block_multiply)
    Impl::MMultiply<matrix_type,vector_type,vector_type>::apply( 
      A , x , y  );
  else {
    for (size_t i=0; i<x.size(); ++i) {
      Impl::Multiply<matrix_type,vector_type,vector_type>::apply( 
	A , x[i] , y[i] );
    }
  }
}

template< typename MatrixValueType ,
          class Device >
void write_matrix_market(const CrsMatrix<MatrixValueType,Device> & A ,
			 const std::string& filename)
{
  Impl::MatrixMarketWriter<MatrixValueType,Device>::write(A, filename);
}

template< typename ValueType, 
	  typename VectorValueType ,
          class Device >
void update( const ValueType& alpha, 
	     const View<VectorValueType[],Device> & x ,
	     const ValueType& beta,
	     const View<VectorValueType[],Device> & y )
{
  typedef ValueType                          value_type ;
  typedef View<VectorValueType[],Device>     vector_type ;

  Impl::Update<value_type,vector_type>::apply( alpha , x , beta, y );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

} // namespace KokkosArray

#endif /* #ifndef KOKKOSARRAY_CRSMATRIX_HPP */

