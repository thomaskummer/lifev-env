/*
//@HEADER
// ************************************************************************
//
//                             KokkosArray
//         Manycore Performance-Portable Multidimensional Arrays
//
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
// Questions?  Contact  H. Carter Edwards (hcedwar@sandia.gov)
//
// ************************************************************************
//@HEADER
*/

#ifndef KOKKOSARRAY_LAYOUT_HPP
#define KOKKOSARRAY_LAYOUT_HPP

#include <impl/KokkosArray_ArrayTraits.hpp>

namespace KokkosArray {

/** \brief  Left-to-right striding of multi-indices (Fortran scheme). */
struct LayoutLeft { typedef LayoutLeft array_layout ; };

/** \brief  Right-to-left striding of multi-indices
 *         (C or lexigraphical scheme).
 */
struct LayoutRight { typedef LayoutRight array_layout ; };


/** \brief  Left-to-right striding of multi-indices (Fortran scheme) by tiles.
 */
template < unsigned ArgN0 , unsigned ArgN1 ,
           bool IsPowerOfTwo = ( Impl::is_power_of_two<ArgN0>::value &&
                                 Impl::is_power_of_two<ArgN1>::value )
         >
struct LayoutTileLeft {
  typedef LayoutTileLeft<ArgN0,ArgN1,IsPowerOfTwo> array_layout ;
  enum { N0 = ArgN0 };
  enum { N1 = ArgN1 };
};

} /* namespace KokkosArray */

#endif /* #ifndef KOKKOSARRAY_LAYOUT_HPP */

