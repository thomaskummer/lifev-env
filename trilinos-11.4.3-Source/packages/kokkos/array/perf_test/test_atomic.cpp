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

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <ctime>

#ifndef DEVICE
#define DEVICE 1
#endif
#if DEVICE==1
#include "KokkosArray_Host.hpp"
typedef KokkosArray::Host device_type;
#endif
#if DEVICE==2
#include "KokkosArray_Cuda.hpp"
typedef KokkosArray::Cuda device_type;
#endif

#include "Kokkos_Atomic.hpp"
#include "KokkosArray_ParallelFor.hpp"
#include "KokkosArray_Macros.hpp"

#define RESET		0
#define BRIGHT 		1
#define DIM		2
#define UNDERLINE 	3
#define BLINK		4
#define REVERSE		7
#define HIDDEN		8

#define BLACK 		0
#define RED		1
#define GREEN		2
#define YELLOW		3
#define BLUE		4
#define MAGENTA		5
#define CYAN		6
#define GREY		7
#define	WHITE		8

void textcolor(int attr, int fg, int bg)
{	char command[13];

	/* Command is the control command to the terminal */
	sprintf(command, "%c[%d;%d;%dm", 0x1B, attr, fg + 30, bg + 40);
	printf("%s", command);
}
void textcolor_standard() {textcolor(RESET, BLACK, WHITE);}


template<class T,class DEVICE_TYPE>
struct ZeroFunctor{
  typedef DEVICE_TYPE device_type;
  typedef typename KokkosArray::View<T,device_type> type;
  typedef typename KokkosArray::View<T,device_type>::HostMirror h_type;
  type data;
  KOKKOSARRAY_INLINE_FUNCTION
  void operator()(int i) const {
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
  void operator()(int i) const {
    KokkosArray::atomic_fetch_add(&data(),(T)1);
  }
};

template<class T>
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

template<class T,class DEVICE_TYPE>
struct AddNonAtomicFunctor{
  typedef DEVICE_TYPE device_type;
  typedef KokkosArray::View<T,device_type> type;
  type data;

  KOKKOSARRAY_INLINE_FUNCTION
  void operator()(int i) const {
    data()+=(T)1;
  }
};

template<class T>
T AddLoopNonAtomic(int loop) {
  struct ZeroFunctor<T,device_type> f_zero;
  typename ZeroFunctor<T,device_type>::type data("Data");
  typename ZeroFunctor<T,device_type>::h_type h_data("HData");

  f_zero.data = data;
  KokkosArray::parallel_for(1,f_zero);
  device_type::fence();

  struct AddNonAtomicFunctor<T,device_type> f_add;
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
  void operator()(int i) const {
	  T old = data();
	  T newval, assumed;
	  do {
	    assumed = old;
	    newval = assumed + (T)1;
	    old = KokkosArray::atomic_compare_exchange(&data(), assumed, newval);
	  }
	  while( old != assumed );
  }
};

template<class T>
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

template<class T,class DEVICE_TYPE>
struct CASNonAtomicFunctor{
  typedef DEVICE_TYPE device_type;
  typedef KokkosArray::View<T,device_type> type;
  type data;

  KOKKOSARRAY_INLINE_FUNCTION
  void operator()(int i) const {
	  volatile T assumed;
	  volatile T newval;
	  bool fail=1;
	  do {
	    assumed = data();
	    newval = assumed + (T)1;
	    if(data()==assumed) {
	    	data() = newval;
	    	fail = 0;
	    }
	  }
	  while(fail);
  }
};

template<class T>
T CASLoopNonAtomic(int loop) {
  struct ZeroFunctor<T,device_type> f_zero;
  typename ZeroFunctor<T,device_type>::type data("Data");
  typename ZeroFunctor<T,device_type>::h_type h_data("HData");
  f_zero.data = data;
  KokkosArray::parallel_for(1,f_zero);
  device_type::fence();

  struct CASNonAtomicFunctor<T,device_type> f_cas;
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
	T old = KokkosArray::atomic_exchange(&data(),(T)i);
    KokkosArray::atomic_fetch_add(&data2(),old);
  }
};

template<class T>
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

template<class T,class DEVICE_TYPE>
struct ExchNonAtomicFunctor{
  typedef DEVICE_TYPE device_type;
  typedef KokkosArray::View<T,device_type> type;
  type data, data2;

  KOKKOSARRAY_INLINE_FUNCTION
  void operator()(int i) const {
		T old = data();
		data()=(T) i;
		data2()+=old;
  }
};


template<class T>
T ExchLoopNonAtomic(int loop) {
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

  struct ExchNonAtomicFunctor<T,device_type> f_exch;
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

template<class T>
T LoopVariant(int loop, int test) {
  switch (test) {
    case 1: return AddLoop<T>(loop);
    case 2: return CASLoop<T>(loop);
    case 3: return ExchLoop<T>(loop);
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

template<class T>
T LoopVariantNonAtomic(int loop, int test) {
  switch (test) {
    case 1: return AddLoopNonAtomic<T>(loop);
    case 2: return CASLoopNonAtomic<T>(loop);
    case 3: return ExchLoopNonAtomic<T>(loop);
  }
  return 0;
}

template<class T>
void Loop(int loop, int test, const char* type_name) {
  timespec starttime,endtime;
  LoopVariant<T>(loop,test);

  clock_gettime(CLOCK_REALTIME,&starttime);
  T res = LoopVariant<T>(loop,test);
  clock_gettime(CLOCK_REALTIME,&endtime);
  double time1 = endtime.tv_sec - starttime.tv_sec + 1.0 * (endtime.tv_nsec - starttime.tv_nsec) / 1000000000;

  clock_gettime(CLOCK_REALTIME,&starttime);
  T resNonAtomic = LoopVariantNonAtomic<T>(loop,test);
  clock_gettime(CLOCK_REALTIME,&endtime);
  double time2 = endtime.tv_sec - starttime.tv_sec + 1.0 * (endtime.tv_nsec - starttime.tv_nsec) / 1000000000;

  clock_gettime(CLOCK_REALTIME,&starttime);
  T resSerial = LoopVariantSerial<T>(loop,test);
  clock_gettime(CLOCK_REALTIME,&endtime);
  double time3 = endtime.tv_sec - starttime.tv_sec + 1.0 * (endtime.tv_nsec - starttime.tv_nsec) / 1000000000;

  time1*=1e6/loop;
  time2*=1e6/loop;
  time3*=1e6/loop;
  textcolor_standard();
  bool passed = true;
  if(resSerial!=res) passed = false;
  if(!passed) textcolor(RESET,BLACK,YELLOW);
  printf("%s Test %i %s  --- Loop: %i Value (S,A,NA): %e %e %e Time: %7.4lf %7.4lf %7.4lf Size of Type %i)",type_name,test,passed?"PASSED":"FAILED",loop,1.0*resSerial,1.0*res,1.0*resNonAtomic,time1,time2,time3,(int)sizeof(T));
  if(!passed) textcolor_standard();
  printf("\n");
}


template<class T>
void Test(int loop, int test, const char* type_name) {
  if(test==-1) {
    Loop<T>(loop,1,type_name);
    Loop<T>(loop,2,type_name);
    Loop<T>(loop,3,type_name);

  }
  else
    Loop<T>(loop,test,type_name);
}

int main(int argc, char* argv[])
{
  int type = -1;
  int loop = 1000000;
  int test = -1;
  int numa = 1;
  int threads = 8;
  int device = 0;

  for(int i=0;i<argc;i++)
  {
     if((strcmp(argv[i],"-t")==0)||(strcmp(argv[i],"--threads")==0)) {threads=atoi(argv[++i]); continue;}
     if((strcmp(argv[i],"--numa")==0)) {numa=atoi(argv[++i]); continue;}
     if((strcmp(argv[i],"--test")==0)) {test=atoi(argv[++i]); continue;}
     if((strcmp(argv[i],"--type")==0)) {type=atoi(argv[++i]); continue;}
     if((strcmp(argv[i],"-l")==0)||(strcmp(argv[i],"--loop")==0)) {loop=atoi(argv[++i]); continue;}
     if((strcmp(argv[i], "-d") == 0) || (strcmp(argv[i], "--device") == 0)) {
       device = atoi(argv[++i]);
       continue;
     }
  }

#if DEVICE==1
  KokkosArray::Host::initialize(numa,threads);
#endif

#if DEVICE==2
  KokkosArray::Cuda::SelectDevice select_device(device);
  KokkosArray::Cuda::initialize(select_device);
#endif

  printf("Using %i numa regions with %i threads\n",numa,threads);
  printf("Using %s\n",KokkosArray::atomic_query_version());
  bool all_tests = false;
  if(type==-1) all_tests = true;
  while(type<100) {
    if(type==1) {
     Test<int>(loop,test,"int                    ");
    }
    if(type==2) {
     Test<long int>(loop,test,"long int               ");
    }
    if(type==3) {
     Test<long long int>(loop,test,"long long int          ");
    }
    if(type==4) {
     Test<unsigned int>(loop,test,"unsigned int           ");
    }
    if(type==5) {
     Test<unsigned long int>(loop,test,"unsigned long int      ");
    }
    if(type==6) {
     Test<unsigned long long int>(loop,test,"unsigned long long int ");
    }
    if(type==10) {
     Test<float>(loop,test,"float                  ");
    }
    if(type==11) {
     Test<double>(loop,test,"double                 ");
    }
    if(!all_tests) type=100;
    else type++;
  }

#if DEVICE==1
  KokkosArray::Host::finalize();
#endif
#if DEVICE==2
  KokkosArray::Cuda::finalize();
#endif


}

