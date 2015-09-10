// @HEADER
// ************************************************************************
//
//                           Intrepid Package
//                 Copyright (2007) Sandia Corporation
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
// Questions: Alejandro Mota (amota@sandia.gov)
//
// ************************************************************************
// @HEADER

#if !defined(Intrepid_MiniTensor_Tensor3_t_h)
#define Intrepid_MiniTensor_Tensor3_t_h

namespace Intrepid {

//
// \return \f$ B = A \cdot u := B_{ij} = A_{ijp} u_p \f$
//
template<typename S, typename T>
Tensor<typename Promote<S, T>::type>
dot(Tensor3<T> const & A, Vector<S> const & u)
{
  Index const
  N = A.get_dimension();

  assert(u.get_dimension() == N);

  Tensor<typename Promote<S, T>::type>
  B(N);

  for (Index i = 0; i < N; ++i) {
    for (Index j = 0; j < N; ++j) {

      typename Promote<S, T>::type
      s = 0.0;

      for (Index p = 0; p < N; ++p) {
        s += A(i,j,p) * u(p);
      }
      B(i,j) = s;
    }
  }

  return B;
}

//
// \return \f$ B = u \cdot A := B_{ij} = u_p A{pij} \f$
//
template<typename S, typename T>
Tensor<typename Promote<S, T>::type>
dot(Vector<S> const & u, Tensor3<T> const & A)
{
  Index const
  N = A.get_dimension();

  assert(u.get_dimension() == N);

  Tensor<typename Promote<S, T>::type>
  B(N);

  for (Index i = 0; i < N; ++i) {
    for (Index j = 0; j < N; ++j) {

      typename Promote<S, T>::type
      s = 0.0;

      for (Index p = 0; p < N; ++p) {
        s += u(p) * A(p,i,j);
      }
      B(i,j) = s;
    }
  }

  return B;
}


//
// \return \f$ B = A \cdot u := B_{ij} = A_{ipj} u_p \f$
//
template<typename S, typename T>
Tensor<typename Promote<S, T>::type>
dot2(Tensor3<T> const & A, Vector<S> const & u)
{
  Index const
  N = A.get_dimension();

  assert(u.get_dimension() == N);

  Tensor<typename Promote<S, T>::type>
  B(N);

  for (Index i = 0; i < N; ++i) {
    for (Index j = 0; j < N; ++j) {

      typename Promote<S, T>::type
      s = 0.0;

      for (Index p = 0; p < N; ++p) {
        s += A(i,p,j) * u(p);
      }
      B(i,j) = s;
    }
  }

  return B;
}

//
// \return \f$ B = u \cdot A := B_{ij} = u_p A_{ipj} \f$
//
template<typename S, typename T>
Tensor<typename Promote<S, T>::type>
dot2(Vector<S> const & u, Tensor3<T> const & A)
{
  return dot2(A, u);
}

///
/// \return \f$ C = A \cdot B := C_{ijk} = A_{ijp} B_{pk} \f$
///
template<typename S, typename T>
Tensor3<typename Promote<S, T>::type>
dot(Tensor3<T> const & A, Tensor<S> const & B)
{
  Index const
  N = A.get_dimension();

  assert(B.get_dimension() == N);

  Tensor3<typename Promote<S, T>::type>
  C(N);

  for (Index i = 0; i < N; ++i) {
    for (Index k = 0; k < N; ++k) {
      for (Index j = 0; j < N; ++j) {

        typename Promote<S, T>::type
        s = 0.0;

        for (Index p = 0; p < N; ++p) {
          s += A(i,j,p) * B(p,k);
        }
        C(i,j,k) = s;
      }
    }
  }

  return C;
}

///
/// \return \f$ C = A \cdot B := C_{ijk} = A_{ip} B_{pjk} \f$
///
template<typename S, typename T>
Tensor3<typename Promote<S, T>::type>
dot(Tensor<S> const & A, Tensor3<T> const & B)
{
  Index const
  N = A.get_dimension();

  assert(B.get_dimension() == N);

  Tensor3<typename Promote<S, T>::type>
  C(N);

  for (Index i = 0; i < N; ++i) {
    for (Index k = 0; k < N; ++k) {
      for (Index j = 0; j < N; ++j) {

        typename Promote<S, T>::type
        s = 0.0;

        for (Index p = 0; p < N; ++p) {
          s += A(i,p) * B(p,j,k);
        }
        C(i,j,k) = s;
      }
    }
  }

  return C;
}

///
/// \return \f$ C = A \cdot B := C_{ijk} = A_{ipj} B_{pk} \f$
///
template<typename S, typename T>
Tensor3<typename Promote<S, T>::type>
dot2(Tensor3<T> const & A, Tensor<S> const & B)
{
  Index const
  N = A.get_dimension();

  assert(B.get_dimension() == N);

  Tensor3<typename Promote<S, T>::type>
  C(N);

  for (Index i = 0; i < N; ++i) {
    for (Index k = 0; k < N; ++k) {
      for (Index j = 0; j < N; ++j) {

        typename Promote<S, T>::type
        s = 0.0;

        for (Index p = 0; p < N; ++p) {
          s += A(i,p,j) * B(p,k);
        }
        C(i,j,k) = s;
      }
    }
  }

  return C;
}


///
/// \return \f$ C = A \cdot B := C_{ijk} = A_{ip} B_{jpk} \f$
///
template<typename S, typename T>
Tensor3<typename Promote<S, T>::type>
dot2(Tensor<S> const & A, Tensor3<T> const & B)
{
  Index const
  N = A.get_dimension();

  assert(B.get_dimension() == N);

  Tensor3<typename Promote<S, T>::type>
  C(N);

  for (Index i = 0; i < N; ++i) {
    for (Index k = 0; k < N; ++k) {
      for (Index j = 0; j < N; ++j) {

        typename Promote<S, T>::type
        s = 0.0;

        for (Index p = 0; p < N; ++p) {
          s += A(i,p) * B(j,p,k);
        }
        C(i,j,k) = s;
      }
    }
  }

  return C;
}


//
// 3rd-order tensor input
// \param A 3rd-order tensor
// \param is input stream
// \return is input stream
//
template<typename T>
std::istream &
operator>>(std::istream & is, Tensor3<T> & A)
{
  Index const
  N = A.get_dimension();

  for (Index i = 0; i < N; ++i) {
    for (Index j = 0; j < N; ++j) {
      for (Index k = 0; k < N; ++k) {
        is >> A(i,j,k);
      }
    }
  }

  return is;
}

//
// 3rd-order tensor output
// \param A 3rd-order tensor
// \param os output stream
// \return os output stream
//
template<typename T>
std::ostream &
operator<<(std::ostream & os, Tensor3<T> const & A)
{
  Index const
  N = A.get_dimension();

  if (N == 0) {
    return os;
  }

  for (Index i = 0; i < N; ++i) {

    for (Index j = 0; j < N; ++j) {

      os << std::scientific << A(i,j,0);

      for (Index k = 1; k < N; ++k) {
        os << std::scientific << "," << A(i,j,k);
      }

      os << std::endl;

    }

    os << std::endl;
    os << std::endl;

  }

  return os;
}

} // namespace Intrepid

#endif // Intrepid_MiniTensor_Tensor3_t_h
