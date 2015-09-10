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

#ifndef KOKKOSARRAY_VIEW_HPP
#define KOKKOSARRAY_VIEW_HPP

#include <string>
#include <KokkosArray_Macros.hpp>
#include <KokkosArray_HostSpace.hpp>

#include <KokkosArray_MemoryTraits.hpp>

#include <impl/KokkosArray_StaticAssert.hpp>
#include <impl/KokkosArray_ArrayTraits.hpp>
#include <impl/KokkosArray_Shape.hpp>
#include <impl/KokkosArray_AnalyzeShape.hpp>

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

namespace KokkosArray {
namespace Impl {

/** \brief  View specialization mapping of view traits to a specialization tag */
template< typename ScalarType , class ValueType ,
          class ArrayLayout , class uRank , class uRankDynamic ,
          class MemorySpace , class MemoryTraits >
struct ViewSpecialize ;

}

/** \brief  ViewTraits
 *
 *  Template argument permutations:
 *
 *    View< DataType , Device , void         , void >
 *    View< DataType , Device , MemoryTraits , void >
 *    View< DataType , Device , void         , MemoryTraits >
 *    View< DataType , ArrayLayout , Device  , void >
 *    View< DataType , ArrayLayout , Device  , MemoryTraits >
 */

template< class DataType ,
          class ArrayLayout ,
          class DeviceType ,
          class MemoryTraits >
class ViewTraits {
private:

  typedef Impl::AnalyzeShape<DataType> analysis ;

public:

  //------------------------------------
  // Data type traits:

  typedef DataType                            data_type ;
  typedef typename analysis::const_type       const_data_type ;
  typedef typename analysis::non_const_type   non_const_data_type ;

  //------------------------------------
  // Scalar type traits:

  typedef typename analysis::scalar_type            scalar_type ;
  typedef typename analysis::const_scalar_type      const_scalar_type ;
  typedef typename analysis::non_const_scalar_type  non_const_scalar_type ;

  //------------------------------------
  // Value type traits:

  typedef typename analysis::value_type            value_type ;
  typedef typename analysis::const_value_type      const_value_type ;
  typedef typename analysis::non_const_value_type  non_const_value_type ;

  //------------------------------------
  // Layout and shape traits:

  typedef typename Impl::StaticAssertSame< ArrayLayout , typename ArrayLayout ::array_layout >::type  array_layout ;

  typedef typename analysis::shape   shape_type ;

  enum { rank         = shape_type::rank };
  enum { rank_dynamic = shape_type::rank_dynamic };

  //------------------------------------
  // Device and memory space traits:

  typedef typename Impl::StaticAssertSame< DeviceType   , typename DeviceType  ::device_type   >::type  device_type ;
  typedef typename Impl::StaticAssertSame< MemoryTraits , typename MemoryTraits::memory_traits >::type  memory_traits ;

  typedef typename device_type::memory_space  memory_space ;
  typedef typename device_type::size_type     size_type ;

  enum { is_hostspace = Impl::is_same< memory_space , HostSpace >::value };
  enum { is_managed   = memory_traits::Unmanaged == 0 };

  //------------------------------------
  // Specialization:
  typedef typename
    Impl::ViewSpecialize< scalar_type ,
                          value_type ,
                          array_layout ,
                          Impl::unsigned_<rank> ,
                          Impl::unsigned_<rank_dynamic> ,
                          memory_space ,
                          memory_traits
                        >::type specialize ;
};

/** \brief  Traits for View<DataType,DeviceType,void,void> */

template< class DataType ,
          class DeviceType >
class ViewTraits<DataType,DeviceType,void,void>
  : public ViewTraits< DataType , typename DeviceType::array_layout , DeviceType , MemoryManaged > {};

/** \brief  Traits for View<DataType,DeviceType,void,MemoryTraits> */

template< class DataType ,
          class DeviceType ,
          class MemoryTraits >
class ViewTraits<DataType,DeviceType,void,MemoryTraits>
  : public ViewTraits< DataType , typename DeviceType::array_layout , DeviceType , MemoryTraits > {};

/** \brief  Traits for View<DataType,DeviceType,MemoryTraits,void> */

template< class DataType , class DeviceType , class MemoryTraits >
class ViewTraits< DataType , DeviceType , MemoryTraits , 
  typename Impl::enable_if< (
    Impl::is_same< DeviceType   , typename DeviceType  ::device_type   >::value &&
    Impl::is_same< MemoryTraits , typename MemoryTraits::memory_traits >::value
  ) >::type >
  : public ViewTraits< DataType , typename DeviceType::array_layout , DeviceType , MemoryTraits > {};

/** \brief  Traits for View<DataType,ArrayLayout,DeviceType,void> */

template< class DataType , class ArrayLayout , class DeviceType >
class ViewTraits< DataType , ArrayLayout , DeviceType ,
  typename Impl::enable_if< (
    Impl::is_same< ArrayLayout , typename ArrayLayout::array_layout >::value &&
    Impl::is_same< DeviceType  , typename DeviceType ::device_type  >::value
  ) >::type >
  : public ViewTraits< DataType , ArrayLayout , DeviceType , MemoryManaged > {};

//----------------------------------------------------------------------------

/** \brief  View to array of data.
 *
 *  Options for template arguments:
 *
 *    View< DataType , Device >
 *    View< DataType , Device ,        MemoryTraits >
 *    View< DataType , Device , void , MemoryTraits >
 *
 *    View< DataType , Layout , Device >
 *    View< DataType , Layout , Device , MemoryTraits >
 */

template< class DataType ,
          class Arg1Type ,        /* ArrayLayout or DeviceType */
          class Arg2Type = void , /* DeviceType or MemoryTraits */
          class Arg3Type = void , /* MemoryTraits */
          class Specialize =
            typename ViewTraits<DataType,Arg1Type,Arg2Type,Arg3Type>::specialize >
class View ;

//----------------------------------------------------------------------------

template< class LT , class LL , class LD , class LM , class LS ,
          class RT , class RL , class RD , class RM , class RS >
KOKKOSARRAY_INLINE_FUNCTION
typename Impl::enable_if<( Impl::is_same< LS , RS >::value ), bool >::type
operator == ( const View<LT,LL,LD,LM,LS> & lhs ,
              const View<RT,RL,RD,RM,RS> & rhs )
{
  // Same data, layout, dimensions
  typedef ViewTraits<LT,LL,LD,LM> lhs_traits ;
  typedef ViewTraits<RT,RL,RD,RM> rhs_traits ;

  return
    Impl::is_same< typename lhs_traits::const_data_type ,
                   typename rhs_traits::const_data_type >::value &&
    Impl::is_same< typename lhs_traits::array_layout ,
                   typename rhs_traits::array_layout >::value &&
    Impl::is_same< typename lhs_traits::memory_space ,
                   typename rhs_traits::memory_space >::value &&
    Impl::is_same< typename lhs_traits::specialize ,
                   typename rhs_traits::specialize >::value &&
    lhs.ptr_on_device() == rhs.ptr_on_device() &&
    lhs.shape()         == rhs.shape() ;
}

template< class LT , class LL , class LD , class LM , class LS ,
          class RT , class RL , class RD , class RM , class RS >
KOKKOSARRAY_INLINE_FUNCTION
bool operator != ( const View<LT,LL,LD,LM,LS> & lhs ,
                   const View<RT,RL,RD,RM,RS> & rhs )
{
  return ! operator==( lhs , rhs );
}

//----------------------------------------------------------------------------

} // namespace KokkosArray

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

namespace KokkosArray {
namespace Impl {

template< class DstViewSpecialize , class SrcViewSpecialize = void , class Enable = void >
struct ViewAssignment ;

template< class Device >
struct ViewInitialize
{
  template< class ViewType >
  inline explicit ViewInitialize( const ViewType & ) {}
};

} // namespace Impl
} // namespace KokkosArray

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

namespace KokkosArray {

//----------------------------------------------------------------------------
/** \brief  A deep copy between views of the same specialization, compatible type,
 *          same rank, same layout are handled by that specialization.
 */

template< class DT , class DL , class DD , class DM , class DS ,
          class ST , class SL , class SD , class SM , class SS >
inline
void deep_copy( const View<DT,DL,DD,DM,DS> & dst ,
                const View<ST,SL,SD,SM,SS> & src ,
                typename Impl::enable_if<(
                  Impl::is_same< typename ViewTraits<DT,DL,DD,DM>::scalar_type ,
                                 typename ViewTraits<ST,SL,SD,SM>::non_const_scalar_type >::value
                  &&
                  Impl::is_same< typename ViewTraits<DT,DL,DD,DM>::array_layout ,
                                 typename ViewTraits<ST,SL,SD,SM>::array_layout >::value
                  &&
                  ( unsigned(ViewTraits<DT,DL,DD,DM>::rank) == unsigned(ViewTraits<ST,SL,SD,SM>::rank) )
                )>::type * = 0 )
{ Impl::ViewAssignment<DS,SS>::deep_copy( dst , src ); }

//----------------------------------------------------------------------------

template< class DstViewType ,
          class T , class L , class D , class M , class S ,
          class ArgType0 >
KOKKOSARRAY_INLINE_FUNCTION
View< typename DstViewType::data_type ,
      typename DstViewType::array_layout ,
      typename DstViewType::device_type ,
      MemoryUnmanaged >
subview( const View<T,L,D,M,S> & src ,
         const ArgType0 & arg0 )
{
  typedef View< typename DstViewType::data_type ,
                typename DstViewType::array_layout ,
                typename DstViewType::device_type ,
                MemoryUnmanaged > dst_type ;

  dst_type dst ;

  Impl::ViewAssignment<typename dst_type::specialize,S>( dst , src , arg0 );

  return dst ;
}

template< class DstViewType ,
          class T , class L , class D , class M , class S ,
          class ArgType0 , class ArgType1 >
KOKKOSARRAY_INLINE_FUNCTION
View< typename DstViewType::data_type ,
      typename DstViewType::array_layout ,
      typename DstViewType::device_type ,
      MemoryUnmanaged >
subview( const View<T,L,D,M,S> & src ,
         const ArgType0 & arg0 ,
         const ArgType1 & arg1 )
{
  typedef View< typename DstViewType::data_type ,
                typename DstViewType::array_layout ,
                typename DstViewType::device_type ,
                MemoryUnmanaged > dst_type ;

  dst_type dst ;

  Impl::ViewAssignment<typename dst_type::specialize,S>( dst, src, arg0, arg1 );

  return dst ;
}

template< class DstViewType ,
          class T , class L , class D , class M , class S ,
          class ArgType0 , class ArgType1 , class ArgType2 >
KOKKOSARRAY_INLINE_FUNCTION
View< typename DstViewType::data_type ,
      typename DstViewType::array_layout ,
      typename DstViewType::device_type ,
      MemoryUnmanaged >
subview( const View<T,L,D,M,S> & src ,
         const ArgType0 & arg0 ,
         const ArgType1 & arg1 ,
         const ArgType2 & arg2 )
{
  typedef View< typename DstViewType::data_type ,
                typename DstViewType::array_layout ,
                typename DstViewType::device_type ,
                MemoryUnmanaged > dst_type ;

  dst_type dst ;

  Impl::ViewAssignment<typename dst_type::specialize,S>( dst, src, arg0, arg1, arg2 );

  return dst ;
}

template< class DstViewType ,
          class T , class L , class D , class M , class S ,
          class ArgType0 , class ArgType1 , class ArgType2 , class ArgType3 >
KOKKOSARRAY_INLINE_FUNCTION
View< typename DstViewType::data_type ,
      typename DstViewType::array_layout ,
      typename DstViewType::device_type ,
      MemoryUnmanaged >
subview( const View<T,L,D,M,S> & src ,
         const ArgType0 & arg0 ,
         const ArgType1 & arg1 ,
         const ArgType2 & arg2 ,
         const ArgType3 & arg3 )
{
  typedef View< typename DstViewType::data_type ,
                typename DstViewType::array_layout ,
                typename DstViewType::device_type ,
                MemoryUnmanaged > dst_type ;

  dst_type dst ;

  Impl::ViewAssignment<typename dst_type::specialize,S>( dst, src, arg0, arg1, arg2, arg3 );

  return dst ;
}

template< class DstViewType ,
          class T , class L , class D , class M , class S ,
          class ArgType0 , class ArgType1 , class ArgType2 , class ArgType3 ,
          class ArgType4 >
KOKKOSARRAY_INLINE_FUNCTION
View< typename DstViewType::data_type ,
      typename DstViewType::array_layout ,
      typename DstViewType::device_type ,
      MemoryUnmanaged >
subview( const View<T,L,D,M,S> & src ,
         const ArgType0 & arg0 ,
         const ArgType1 & arg1 ,
         const ArgType2 & arg2 ,
         const ArgType3 & arg3 ,
         const ArgType4 & arg4 )
{
  typedef View< typename DstViewType::data_type ,
                typename DstViewType::array_layout ,
                typename DstViewType::device_type ,
                MemoryUnmanaged > dst_type ;

  dst_type dst ;

  Impl::ViewAssignment<typename dst_type::specialize,S>( dst, src, arg0, arg1, arg2, arg3, arg4 );

  return dst ;
}

template< class DstViewType ,
          class T , class L , class D , class M , class S ,
          class ArgType0 , class ArgType1 , class ArgType2 , class ArgType3 ,
          class ArgType4 , class ArgType5 >
KOKKOSARRAY_INLINE_FUNCTION
View< typename DstViewType::data_type ,
      typename DstViewType::array_layout ,
      typename DstViewType::device_type ,
      MemoryUnmanaged >
subview( const View<T,L,D,M,S> & src ,
         const ArgType0 & arg0 ,
         const ArgType1 & arg1 ,
         const ArgType2 & arg2 ,
         const ArgType3 & arg3 ,
         const ArgType4 & arg4 ,
         const ArgType5 & arg5 )
{
  typedef View< typename DstViewType::data_type ,
                typename DstViewType::array_layout ,
                typename DstViewType::device_type ,
                MemoryUnmanaged > dst_type ;

  dst_type dst ;

  Impl::ViewAssignment<typename dst_type::specialize,S>( dst, src, arg0, arg1, arg2, arg3, arg4, arg5 );

  return dst ;
}

template< class DstViewType ,
          class T , class L , class D , class M , class S ,
          class ArgType0 , class ArgType1 , class ArgType2 , class ArgType3 ,
          class ArgType4 , class ArgType5 , class ArgType6 >
KOKKOSARRAY_INLINE_FUNCTION
View< typename DstViewType::data_type ,
      typename DstViewType::array_layout ,
      typename DstViewType::device_type ,
      MemoryUnmanaged >
subview( const View<T,L,D,M,S> & src ,
         const ArgType0 & arg0 ,
         const ArgType1 & arg1 ,
         const ArgType2 & arg2 ,
         const ArgType3 & arg3 ,
         const ArgType4 & arg4 ,
         const ArgType5 & arg5 ,
         const ArgType6 & arg6 )
{
  typedef View< typename DstViewType::data_type ,
                typename DstViewType::array_layout ,
                typename DstViewType::device_type ,
                MemoryUnmanaged > dst_type ;

  dst_type dst ;

  Impl::ViewAssignment<typename dst_type::specialize,S>( dst, src, arg0, arg1, arg2, arg3, arg4, arg5, arg6 );

  return dst ;
}

template< class DstViewType ,
          class T , class L , class D , class M , class S ,
          class ArgType0 , class ArgType1 , class ArgType2 , class ArgType3 ,
          class ArgType4 , class ArgType5 , class ArgType6 , class ArgType7 >
KOKKOSARRAY_INLINE_FUNCTION
View< typename DstViewType::data_type ,
      typename DstViewType::array_layout ,
      typename DstViewType::device_type ,
      MemoryUnmanaged >
subview( const View<T,L,D,M,S> & src ,
         const ArgType0 & arg0 ,
         const ArgType1 & arg1 ,
         const ArgType2 & arg2 ,
         const ArgType3 & arg3 ,
         const ArgType4 & arg4 ,
         const ArgType5 & arg5 ,
         const ArgType6 & arg6 ,
         const ArgType7 & arg7 )
{
  typedef View< typename DstViewType::data_type ,
                typename DstViewType::array_layout ,
                typename DstViewType::device_type ,
                MemoryUnmanaged > dst_type ;

  dst_type dst ;

  Impl::ViewAssignment<typename dst_type::specialize,S>( dst, src, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7 );

  return dst ;
}

//----------------------------------------------------------------------------


template< class T , class L , class D , class M , class S >
typename View<T,L,D,M,S>::HostMirror
create_mirror_view( const View<T,L,D,M,S> & view ,
                    typename Impl::enable_if<
                      Impl::is_same< typename ViewTraits<T,L,D,M>::memory_space ,
                                     HostSpace >::value
                    >::type * = 0 )
{
  return view ;
}

template< class T , class L , class D , class M , class S >
typename View<T,L,D,M,S>::HostMirror
create_mirror_view( const View<T,L,D,M,S> & view ,
                    typename Impl::enable_if<
                      ! Impl::is_same< typename ViewTraits<T,L,D,M>::memory_space ,
                                       HostSpace >::value
                    >::type * = 0 )
{
  typedef typename View<T,L,D,M>::HostMirror host_view ;
  host_view tmp ;
  Impl::ViewAssignment< S >( tmp , view );
  return tmp ;
}

template< class T , class L , class D , class M , class S >
typename View<T,L,D,M,S>::HostMirror
create_mirror( const View<T,L,D,M,S> & view )
{
#if KOKKOSARRAY_MIRROR_VIEW_OPTIMIZE
  return create_mirror_view( view );
#else
  typedef typename View<T,L,D,M,S>::HostMirror host_view ;
  host_view tmp ;
  Impl::ViewAssignment< S >( tmp , view );
  return tmp ;
#endif
}

} // namespace KokkosArray

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

#include <impl/KokkosArray_ViewDefault.hpp>
#include <impl/KokkosArray_ViewScalar.hpp>
#include <impl/KokkosArray_ViewTileLeft.hpp>

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

#endif

