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
// Questions?  Contact  H. Carter Edwards (hcedwar@sandia.gov)
//
// ************************************************************************
//@HEADER
*/

#include <Kokkos_Atomic.hpp>

namespace TestAtomic {

template<class T,class DEVICE_TYPE>
struct ZeroFunctor {
  typedef DEVICE_TYPE device_type;
  typedef typename KokkosArray::View<T,device_type> type;
  typedef typename KokkosArray::View<T,device_type>::HostMirror h_type;
  type data;
  KOKKOSARRAY_INLINE_FUNCTION
  void operator()(int) const {
    data() = 0;
  }
};

//---------------------------------------------------
//--------------atomic_fetch_add---------------------
//---------------------------------------------------

template<class T,class DEVICE_TYPE>
struct AddFunctor{
  typedef DEVICE_TYPE device_type;
  typedef KokkosArray::View<T,device_type> type;
  type data;

  KOKKOSARRAY_INLINE_FUNCTION
  void operator()(int) const {
    Kokkos::atomic_fetch_add(&data(),(T)1);
  }
};

template<class T, class device_type >
T AddLoop(int loop) {
  struct ZeroFunctor<T,device_type> f_zero;
  typename ZeroFunctor<T,device_type>::type data("Data");
  typename ZeroFunctor<T,device_type>::h_type h_data("HData");
  f_zero.data = data;
  KokkosArray::parallel_for(1,f_zero);
  device_type::fence();

  struct AddFunctor<T,device_type> f_add;
  f_add.data = data;
  KokkosArray::parallel_for(loop,f_add);
  device_type::fence();

  KokkosArray::deep_copy(h_data,data);
  T val = h_data();
  return val;
}

template<class T>
T AddLoopSerial(int loop) {
  T* data = new T[1];
  data[0] = 0;

  for(int i=0;i<loop;i++)
  *data+=(T)1;

  T val = *data;
  delete data;
  return val;
}

template<class T,class DEVICE_TYPE>
struct CASFunctor{
  typedef DEVICE_TYPE device_type;
  typedef KokkosArray::View<T,device_type> type;
  type data;

  KOKKOSARRAY_INLINE_FUNCTION
  void operator()(int) const {
	  T old = data();
	  T newval, assumed;
	  do {
	    assumed = old;
	    newval = assumed + (T)1;
	    old = Kokkos::atomic_compare_exchange(&data(), assumed, newval);
	  }
	  while( old != assumed );
  }
};

template<class T, class device_type >
T CASLoop(int loop) {
  struct ZeroFunctor<T,device_type> f_zero;
  typename ZeroFunctor<T,device_type>::type data("Data");
  typename ZeroFunctor<T,device_type>::h_type h_data("HData");
  f_zero.data = data;
  KokkosArray::parallel_for(1,f_zero);
  device_type::fence();

  struct CASFunctor<T,device_type> f_cas;
  f_cas.data = data;
  KokkosArray::parallel_for(loop,f_cas);
  device_type::fence();

  KokkosArray::deep_copy(h_data,data);
  T val = h_data();

  return val;
}

template<class T>
T CASLoopSerial(int loop) {
  T* data = new T[1];
  data[0] = 0;

  for(int i=0;i<loop;i++) {
	  T assumed;
	  T newval;
	  T old;
	  do {
	    assumed = *data;
	    newval = assumed + (T)1;
	    old = *data;
	    *data = newval;
	  }
	  while(!(assumed==old));
  }

  T val = *data;
  delete data;
  return val;
}

template<class T,class DEVICE_TYPE>
struct ExchFunctor{
  typedef DEVICE_TYPE device_type;
  typedef KokkosArray::View<T,device_type> type;
  type data, data2;

  KOKKOSARRAY_INLINE_FUNCTION
  void operator()(int i) const {
    T old = Kokkos::atomic_exchange(&data(),(T)i);
    Kokkos::atomic_fetch_add(&data2(),old);
  }
};

template<class T, class device_type >
T ExchLoop(int loop) {
  struct ZeroFunctor<T,device_type> f_zero;
  typename ZeroFunctor<T,device_type>::type data("Data");
  typename ZeroFunctor<T,device_type>::h_type h_data("HData");
  f_zero.data = data;
  KokkosArray::parallel_for(1,f_zero);
  device_type::fence();

  typename ZeroFunctor<T,device_type>::type data2("Data");
  typename ZeroFunctor<T,device_type>::h_type h_data2("HData");
  f_zero.data = data2;
  KokkosArray::parallel_for(1,f_zero);
  device_type::fence();

  struct ExchFunctor<T,device_type> f_exch;
  f_exch.data = data;
  f_exch.data2 = data2;
  KokkosArray::parallel_for(loop,f_exch);
  device_type::fence();

  KokkosArray::deep_copy(h_data,data);
  KokkosArray::deep_copy(h_data2,data2);
  T val = h_data() + h_data2();

  return val;
}

template<class T>
T ExchLoopSerial(int loop) {
  T* data = new T[1];
  T* data2 = new T[1];
  data[0] = 0;
  data2[0] = 0;
  for(int i=0;i<loop;i++) {
	T old = *data;
	*data=(T) i;
	*data2+=old;
  }

  T val = *data2 + *data;
  delete data;
  delete data2;
  return val;
}

template<class T, class DeviceType >
T LoopVariant(int loop, int test) {
  switch (test) {
    case 1: return AddLoop<T,DeviceType>(loop);
    case 2: return CASLoop<T,DeviceType>(loop);
    case 3: return ExchLoop<T,DeviceType>(loop);
  }
  return 0;
}

template<class T>
T LoopVariantSerial(int loop, int test) {
  switch (test) {
    case 1: return AddLoopSerial<T>(loop);
    case 2: return CASLoopSerial<T>(loop);
    case 3: return ExchLoopSerial<T>(loop);
  }
  return 0;
}

template<class T,class DeviceType>
bool Loop(int loop, int test)
{
  T res       = LoopVariant<T,DeviceType>(loop,test);
  T resSerial = LoopVariantSerial<T>(loop,test);

  bool passed = true;

  if ( resSerial != res ) {
    passed = false;

    std::cout << "Loop<"
              << typeid(T).name()
              << ">( test = "
              << test << " FAILED : "
              << resSerial << " != " << res
              << std::endl ;
  }


  return passed ;
}

}

