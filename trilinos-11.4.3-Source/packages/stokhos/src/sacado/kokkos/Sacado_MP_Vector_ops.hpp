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

#include "Sacado_cmath.hpp"
#include <ostream>	// for std::ostream

/*
namespace Sacado {
  namespace MP {

    template <typename T, typename N>
    class LogOp :
      public Expr< LogOp< T,N >,N > {
    public:

      typedef typename T::value_type value_type;
      typedef typename T::storage_type storage_type;

      KOKKOSARRAY_INLINE_FUNCTION
      LogOp(const T& expr_) : expr(expr_)  {}

      KOKKOSARRAY_INLINE_FUNCTION
      std::string name() const {
	return std::string("log") + expr.name();
      }

      KOKKOSARRAY_INLINE_FUNCTION
      int size() const {
	return expr.size();
      }

      KOKKOSARRAY_INLINE_FUNCTION
      bool hasFastAccess(int sz) const {
	return expr.hasFastAccess(sz);
      }

      KOKKOSARRAY_INLINE_FUNCTION
      value_type val() const {
	return std::log(expr.val());
      }

      KOKKOSARRAY_INLINE_FUNCTION
      value_type coeff(int i) const {
	return std::log(expr.coeff(i));
      }

      KOKKOSARRAY_INLINE_FUNCTION
      value_type fastAccessCoeff(int i) const {
	return std::log(expr.fastAccessCoeff(i));
      }

    protected:

      const T& expr;

    };

    template <typename T, typename N>
    KOKKOSARRAY_INLINE_FUNCTION
    LogOp< T,N >
    log (const Expr<T,N>& expr)
    {
      typedef LogOp< typename Expr<T,N>::derived_type, N > expr_t;
 
      return expr_t(expr.derived());
    }
  }
}
*/

#define MP_UNARYOP_MACRO(OPNAME,OP,OPER)				\
namespace Sacado {							\
  namespace MP {							\
									\
									\
    template <typename T, typename N>					\
    class OP :								\
      public Expr< OP< T,N >,N > {					\
    public:								\
      									\
      typedef typename T::value_type value_type;			\
      typedef typename T::storage_type storage_type;			\
      									\
      KOKKOSARRAY_INLINE_FUNCTION					\
      OP(const T& expr_) : expr(expr_)  {}				\
      									\
      KOKKOSARRAY_INLINE_FUNCTION					\
      std::string name() const {					\
	return std::string(#OPER) + expr.name();			\
      }									\
      									\
      KOKKOSARRAY_INLINE_FUNCTION					\
      int size() const {						\
	return expr.size();						\
      }									\
      									\
      KOKKOSARRAY_INLINE_FUNCTION					\
      bool hasFastAccess(int sz) const {				\
	return expr.hasFastAccess(sz);					\
      }									\
      									\
      KOKKOSARRAY_INLINE_FUNCTION					\
	value_type val() const {					\
	return OPER(expr.val());					\
      }									\
      									\
      KOKKOSARRAY_INLINE_FUNCTION					\
      value_type coeff(int i) const {					\
	return OPER(expr.coeff(i));					\
      }									\
									\
      KOKKOSARRAY_INLINE_FUNCTION					\
      value_type fastAccessCoeff(int i) const {				\
	return OPER(expr.fastAccessCoeff(i));				\
      }									\
									\
      template <int i>							\
      KOKKOSARRAY_INLINE_FUNCTION					\
      value_type getCoeff() const {					\
	return OPER(expr.template getCoeff<i>());			\
      }									\
									\
    protected:								\
									\
      typename const_expr_ref<T>::type expr;				\
									\
    };									\
									\
    template <typename T, typename N>					\
    KOKKOSARRAY_INLINE_FUNCTION						\
    OP< T,N >								\
    OPNAME (const Expr<T,N>& expr)					\
    {									\
      typedef OP< typename Expr<T,N>::derived_type,			\
		  N > expr_t;						\
      									\
      return expr_t(expr.derived());					\
    }									\
  }									\
}

MP_UNARYOP_MACRO(operator+, UnaryPlusOp, +)
MP_UNARYOP_MACRO(operator-, UnaryMinusOp, -)
MP_UNARYOP_MACRO(exp, ExpOp, std::exp)
MP_UNARYOP_MACRO(log, LogOp, std::log)
MP_UNARYOP_MACRO(log10, Log10Op, std::log10)
MP_UNARYOP_MACRO(sqrt, SqrtOp, std::sqrt)
MP_UNARYOP_MACRO(cos, CosOp, std::cos)
MP_UNARYOP_MACRO(sin, SinOp, std::sin)
MP_UNARYOP_MACRO(tan, TanOp, std::tan)
MP_UNARYOP_MACRO(acos, ACosOp, std::acos)
MP_UNARYOP_MACRO(asin, ASinOp, std::asin)
MP_UNARYOP_MACRO(atan, ATanOp, std::atan)
MP_UNARYOP_MACRO(cosh, CoshOp, std::cosh)
MP_UNARYOP_MACRO(sinh, SinhOp, std::sinh)
MP_UNARYOP_MACRO(tanh, TanhOp, std::tanh)
MP_UNARYOP_MACRO(acosh, ACoshOp, std::acosh)
MP_UNARYOP_MACRO(asinh, ASinhOp, std::asinh)
MP_UNARYOP_MACRO(atanh, ATanhOp, std::atanh)
MP_UNARYOP_MACRO(abs, AbsOp, std::abs)
MP_UNARYOP_MACRO(fabs, FAbsOp, std::fabs)

#undef MP_UNARYOP_MACRO

#define MP_BINARYOP_MACRO(OPNAME,OP,OPER)				\
namespace Sacado {							\
  namespace MP {							\
									\
    template <typename T1, typename T2, typename N>			\
    class OP :								\
      public Expr< OP< T1, T2, N>, N > {				\
									\
    public:								\
									\
      typedef typename T1::value_type value_type_1;			\
      typedef typename T2::value_type value_type_2;			\
      typedef typename Sacado::Promote<value_type_1,			\
				       value_type_2>::type value_type;  \
									\
      typedef typename T1::storage_type storage_type;			\
									\
									\
      KOKKOSARRAY_INLINE_FUNCTION					\
      OP(const T1& expr1_, const T2& expr2_) :				\
	expr1(expr1_), expr2(expr2_) {}					\
      									\
      KOKKOSARRAY_INLINE_FUNCTION					\
      std::string name() const {					\
	return expr1.name() + std::string(#OPER) + expr2.name();	\
      }									\
									\
      KOKKOSARRAY_INLINE_FUNCTION					\
      int size() const {						\
	int sz1 = expr1.size(), sz2 = expr2.size();			\
	return sz1 > sz2 ? sz1 : sz2;					\
      }									\
									\
      KOKKOSARRAY_INLINE_FUNCTION					\
      bool hasFastAccess(int sz) const {				\
	return expr1.hasFastAccess(sz) && expr2.hasFastAccess(sz);	\
      }									\
									\
      KOKKOSARRAY_INLINE_FUNCTION					\
      value_type val() const {						\
	return (expr1.val() OPER expr2.val());				\
      }									\
      									\
      KOKKOSARRAY_INLINE_FUNCTION					\
      value_type coeff(int i) const {					\
	return (expr1.coeff(i) OPER expr2.coeff(i));			\
      }									\
									\
      KOKKOSARRAY_INLINE_FUNCTION					\
      value_type fastAccessCoeff(int i) const {				\
	return (expr1.fastAccessCoeff(i) OPER expr2.fastAccessCoeff(i)); \
      }									\
      									\
      template <int i>							\
	KOKKOSARRAY_INLINE_FUNCTION					\
      value_type getCoeff() const {					\
	return expr1.template getCoeff<i>() OPER expr2.template getCoeff<i>(); \
      }									\
									\
    protected:								\
									\
      typename const_expr_ref<T1>::type expr1;				\
      typename const_expr_ref<T2>::type expr2;				\
									\
    };									\
    									\
    template <typename T1, typename N>					\
    class OP< T1, typename T1::value_type, N > :			\
      public Expr< OP< T1, typename T1::value_type, N >, N > {		\
									\
    public:								\
									\
      typedef typename T1::value_type value_type;			\
      typedef typename T1::value_type ConstT;				\
									\
      typedef typename T1::storage_type storage_type;			\
									\
      KOKKOSARRAY_INLINE_FUNCTION					\
      OP(const T1& expr1_, const ConstT& c_) :				\
	expr1(expr1_), c(c_) {}						\
									\
      KOKKOSARRAY_INLINE_FUNCTION					\
      std::string name() const {					\
	return expr1.name() + std::string(#OPER) + std::string("c");	\
      }									\
									\
      KOKKOSARRAY_INLINE_FUNCTION					\
      int size() const {						\
	return expr1.size();						\
      }									\
									\
      KOKKOSARRAY_INLINE_FUNCTION					\
      bool hasFastAccess(int sz) const {				\
	return expr1.hasFastAccess(sz);					\
      }									\
									\
      KOKKOSARRAY_INLINE_FUNCTION					\
      value_type val() const {						\
	return (expr1.val() OPER c);					\
      }									\
      									\
      KOKKOSARRAY_INLINE_FUNCTION					\
      value_type coeff(int i) const {					\
	return (expr1.coeff(i) OPER c);					\
      }									\
									\
      KOKKOSARRAY_INLINE_FUNCTION					\
      value_type fastAccessCoeff(int i) const {				\
	return (expr1.fastAccessCoeff(i) OPER c);			\
      }									\
									\
      template <int i>							\
	KOKKOSARRAY_INLINE_FUNCTION					\
      value_type getCoeff() const {					\
	return expr1.template getCoeff<i>() OPER c;			\
      }									\
									\
    protected:								\
									\
      typename const_expr_ref<T1>::type expr1;				\
      const ConstT& c;							\
    };									\
									\
    template <typename T2, typename N>					\
    class OP< typename T2::value_type, T2, N > :			\
      public Expr< OP< typename T2::value_type, T2, N >, N > {		\
									\
    public:								\
									\
      typedef typename T2::value_type value_type;			\
      typedef typename T2::value_type ConstT;				\
									\
      typedef typename T2::storage_type storage_type;			\
									\
      KOKKOSARRAY_INLINE_FUNCTION					\
      OP(const ConstT& c_, const T2& expr2_) :				\
	c(c_), expr2(expr2_) {}						\
									\
      KOKKOSARRAY_INLINE_FUNCTION					\
      std::string name() const {					\
	return std::string("c") + std::string(#OPER) + expr2.name();	\
      }									\
									\
      KOKKOSARRAY_INLINE_FUNCTION					\
      int size() const { return expr2.size(); }				\
									\
      KOKKOSARRAY_INLINE_FUNCTION					\
      bool hasFastAccess(int sz) const {				\
	return expr2.hasFastAccess(sz);					\
      }									\
									\
      KOKKOSARRAY_INLINE_FUNCTION					\
      value_type val() const {						\
	return (c OPER expr2.val());					\
      }									\
      									\
      KOKKOSARRAY_INLINE_FUNCTION					\
      value_type coeff(int i) const {					\
	return (c OPER expr2.coeff(i));					\
      }									\
									\
      KOKKOSARRAY_INLINE_FUNCTION					\
      value_type fastAccessCoeff(int i) const {				\
	return (c OPER expr2.fastAccessCoeff(i));			\
      }									\
									\
      template <int i>							\
      KOKKOSARRAY_INLINE_FUNCTION					\
      value_type getCoeff() const {					\
	return c OPER expr2.template getCoeff<i>();			\
      }									\
      									\
    protected:								\
									\
      const ConstT& c;							\
      typename const_expr_ref<T2>::type expr2;				\
    };									\
									\
    template <typename T1, typename T2, typename N>			\
    KOKKOSARRAY_INLINE_FUNCTION						\
    OP< T1, T2, N >							\
    OPNAME (const Expr<T1,N>& expr1,					\
	    const Expr<T2,N>& expr2)					\
    {									\
      typedef OP< typename Expr<T1,N>::derived_type,			\
		  typename Expr<T2,N>::derived_type,			\
		  N > expr_t;						\
      									\
      return expr_t(expr1.derived(), expr2.derived());			\
    }									\
    									\
    template <typename T, typename N>					\
    KOKKOSARRAY_INLINE_FUNCTION						\
    OP< typename T::value_type, T, N >					\
    OPNAME (const typename T::value_type& c,				\
	    const Expr<T,N>& expr)					\
    {									\
      typedef typename T::value_type ConstT;				\
      typedef OP< ConstT,						\
		  typename Expr<T,N>::derived_type,			\
		  N > expr_t;						\
									\
      return expr_t(c, expr.derived());					\
    }									\
									\
    template <typename T, typename N>					\
    KOKKOSARRAY_INLINE_FUNCTION						\
    OP< T, typename T::value_type,N >					\
    OPNAME (const Expr<T,N>& expr,					\
	    const typename T::value_type& c)				\
    {									\
      typedef typename T::value_type ConstT;				\
      typedef OP< typename Expr<T,N>::derived_type,			\
		  ConstT, N > expr_t;					\
      									\
      return expr_t(expr.derived(), c);					\
    }									\
  }									\
}

MP_BINARYOP_MACRO(operator+, AdditionOp, +)
MP_BINARYOP_MACRO(operator-, SubtractionOp, -)
MP_BINARYOP_MACRO(operator*, MultiplicationOp, *)
MP_BINARYOP_MACRO(operator/, DivisionOp, /)

#undef MP_BINARYOP_MACRO

#define MP_BINARYOP_MACRO(OPNAME,OP,OPER)				\
namespace Sacado {							\
  namespace MP {							\
									\
    template <typename T1, typename T2, typename N>			\
    class OP :								\
      public Expr< OP< T1, T2, N >, N > {				\
      									\
    public:								\
									\
      typedef typename T1::value_type value_type_1;			\
      typedef typename T2::value_type value_type_2;			\
      typedef typename Sacado::Promote<value_type_1,			\
				       value_type_2>::type value_type;  \
									\
      typedef typename T1::storage_type storage_type;			\
									\
									\
      KOKKOSARRAY_INLINE_FUNCTION					\
      OP(const T1& expr1_, const T2& expr2_) :				\
	expr1(expr1_), expr2(expr2_) {}					\
      									\
      KOKKOSARRAY_INLINE_FUNCTION					\
      std::string name() const {					\
	return expr1.name() + std::string(#OPER) + expr2.name();	\
      }									\
									\
      KOKKOSARRAY_INLINE_FUNCTION					\
      int size() const {						\
	int sz1 = expr1.size(), sz2 = expr2.size();			\
	return sz1 > sz2 ? sz1 : sz2;					\
      }									\
									\
      KOKKOSARRAY_INLINE_FUNCTION					\
      bool hasFastAccess(int sz) const {				\
	return expr1.hasFastAccess(sz) && expr2.hasFastAccess(sz);	\
      }									\
									\
      KOKKOSARRAY_INLINE_FUNCTION					\
      value_type val() const {						\
	return OPER(expr1.val(), expr2.val());				\
      }									\
      									\
      KOKKOSARRAY_INLINE_FUNCTION					\
      value_type coeff(int i) const {					\
	return OPER(expr1.coeff(i), expr2.coeff(i));			\
      }									\
									\
      KOKKOSARRAY_INLINE_FUNCTION					\
      value_type fastAccessCoeff(int i) const {				\
	return OPER(expr1.fastAccessCoeff(i), expr2.fastAccessCoeff(i)); \
      }									\
									\
      template <int i>							\
      KOKKOSARRAY_INLINE_FUNCTION					\
      value_type getCoeff() const {					\
	return OPER(expr1.template getCoeff<i>(),			\
		    expr2.template getCoeff<i>());			\
      }									\
      									\
    protected:								\
									\
      typename const_expr_ref<T1>::type expr1;				\
      typename const_expr_ref<T2>::type expr2;				\
									\
    };									\
									\
    template <typename T1, typename N>					\
    class OP< T1, typename T1::value_type, N > :			\
      public Expr< OP< T1, typename T1::value_type, N >, N > {		\
									\
    public:								\
									\
      typedef typename T1::value_type value_type;			\
      typedef typename T1::value_type ConstT;				\
									\
      typedef typename T1::storage_type storage_type;			\
									\
      KOKKOSARRAY_INLINE_FUNCTION					\
      OP(const T1& expr1_, const ConstT& c_) :				\
	expr1(expr1_), c(c_) {}						\
									\
      KOKKOSARRAY_INLINE_FUNCTION					\
      std::string name() const {					\
	return expr1.name() + std::string(#OPER) + std::string("c");	\
      }									\
									\
      KOKKOSARRAY_INLINE_FUNCTION					\
      int size() const { return expr1.size(); }				\
									\
      KOKKOSARRAY_INLINE_FUNCTION					\
      bool hasFastAccess(int sz) const {				\
	return expr1.hasFastAccess(sz);					\
      }									\
									\
      KOKKOSARRAY_INLINE_FUNCTION					\
      value_type val() const {						\
	return OPER(expr1.val(), c);					\
      }									\
      									\
      KOKKOSARRAY_INLINE_FUNCTION					\
      value_type coeff(int i) const {					\
	return OPER(expr1.coeff(i), c);					\
      }									\
									\
      KOKKOSARRAY_INLINE_FUNCTION					\
      value_type fastAccessCoeff(int i) const {				\
	return OPER(expr1.fastAccessCoeff(i), c);			\
      }									\
									\
      template <int i>							\
	KOKKOSARRAY_INLINE_FUNCTION					\
      value_type getCoeff() const {					\
	return OPER(expr1.template getCoeff<i>(), c);			\
      }									\
									\
    protected:								\
									\
      typename const_expr_ref<T1>::type expr1;				\
      const ConstT& c;							\
    };									\
									\
    template <typename T2, typename N>					\
    class OP< typename T2::value_type, T2, N > :			\
      public Expr< OP< typename T2::value_type, T2, N >, N > {		\
									\
    public:								\
									\
      typedef typename T2::value_type value_type;			\
      typedef typename T2::value_type ConstT;				\
									\
      typedef typename T2::storage_type storage_type;			\
									\
      KOKKOSARRAY_INLINE_FUNCTION					\
      OP(const ConstT& c_, const T2& expr2_) :				\
	c(c_), expr2(expr2_) {}						\
									\
      KOKKOSARRAY_INLINE_FUNCTION					\
      std::string name() const {					\
	return std::string("c") + std::string(#OPER) + expr2.name();	\
      }									\
									\
      KOKKOSARRAY_INLINE_FUNCTION					\
      int size() const { return expr2.size(); }				\
									\
      KOKKOSARRAY_INLINE_FUNCTION					\
      bool hasFastAccess(int sz) const {				\
	return expr2.hasFastAccess(sz);					\
      }									\
									\
      KOKKOSARRAY_INLINE_FUNCTION					\
      value_type val() const {						\
	return OPER(c, expr2.val());					\
      }									\
      									\
      KOKKOSARRAY_INLINE_FUNCTION					\
      value_type coeff(int i) const {					\
	return OPER(c, expr2.coeff(i));					\
      }									\
									\
      KOKKOSARRAY_INLINE_FUNCTION					\
      value_type fastAccessCoeff(int i) const {				\
	return OPER(c, expr2.fastAccessCoeff(i));			\
      }									\
									\
      template <int i>							\
      KOKKOSARRAY_INLINE_FUNCTION					\
      value_type getCoeff() const {					\
	return OPER(c, expr2.template getCoeff<i>());			\
      }									\
      									\
    protected:								\
									\
      const ConstT& c;							\
      typename const_expr_ref<T2>::type expr2;				\
    };									\
									\
    template <typename T1, typename T2, typename N>			\
    KOKKOSARRAY_INLINE_FUNCTION						\
    OP< T1, T2, N >							\
    OPNAME (const Expr<T1,N>& expr1,					\
	    const Expr<T2,N>& expr2)					\
    {									\
      typedef OP< typename Expr<T1,N>::derived_type,			\
		  typename Expr<T2,N>::derived_type,			\
		  N > expr_t;						\
    									\
      return expr_t(expr1.derived(), expr2.derived());			\
    }									\
									\
    template <typename T, typename N>					\
    KOKKOSARRAY_INLINE_FUNCTION						\
    OP< typename T::value_type, T, N >					\
    OPNAME (const typename T::value_type& c,				\
	    const Expr<T,N>& expr)					\
    {									\
      typedef typename T::value_type ConstT;				\
      typedef OP< ConstT,						\
		  typename Expr<T,N>::derived_type,			\
		  N > expr_t;						\
									\
      return expr_t(c, expr.derived());					\
    }									\
    									\
    template <typename T, typename N>					\
    KOKKOSARRAY_INLINE_FUNCTION						\
    OP< T, typename T::value_type, N >					\
    OPNAME (const Expr<T,N>& expr,					\
	    const typename T::value_type& c)				\
    {									\
      typedef typename T::value_type ConstT;				\
      typedef OP< typename Expr<T,N>::derived_type,			\
		  ConstT, N > expr_t;					\
      									\
      return expr_t(expr.derived(), c);					\
    }									\
  }									\
}

MP_BINARYOP_MACRO(atan2, Atan2Op, std::atan2)
MP_BINARYOP_MACRO(pow, PowerOp, std::pow)
MP_BINARYOP_MACRO(max, MaxOp, std::max)
MP_BINARYOP_MACRO(min, MinOp, std::min)

#undef MP_BINARYOP_MACRO

//-------------------------- Relational Operators -----------------------

#define MP_RELOP_MACRO(OP)						\
namespace Sacado {							\
  namespace MP {							\
									\
    template <typename T1, typename T2, typename N>			\
    KOKKOSARRAY_INLINE_FUNCTION						\
    bool								\
    operator OP (const Expr<T1,N>& expr1,				\
		 const Expr<T2,N>& expr2)				\
    {									\
      return expr1.derived().val() OP expr2.derived().val();		\
    }									\
									\
    template <typename T2, typename N>					\
    KOKKOSARRAY_INLINE_FUNCTION						\
    bool								\
    operator OP (const typename T2::value_type& a,			\
		 const Expr<T2,N>& expr2)				\
    {									\
      return a OP expr2.derived().val();				\
    }									\
									\
    template <typename T1, typename N>					\
    KOKKOSARRAY_INLINE_FUNCTION						\
    bool								\
    operator OP (const Expr<T1,N>& expr1,				\
		 const typename T1::value_type& b)			\
    {									\
      return expr1.derived().val() OP b;				\
    }									\
  }									\
}

MP_RELOP_MACRO(==)
MP_RELOP_MACRO(!=)
MP_RELOP_MACRO(<)
MP_RELOP_MACRO(>)
MP_RELOP_MACRO(<=)
MP_RELOP_MACRO(>=)
MP_RELOP_MACRO(<<=)
MP_RELOP_MACRO(>>=)
MP_RELOP_MACRO(&)
MP_RELOP_MACRO(|)

#undef MP_RELOP_MACRO

namespace Sacado {

  namespace MP {

    template <typename T, typename N>
    KOKKOSARRAY_INLINE_FUNCTION
    bool operator ! (const Expr<T,N>& expr) 
    {
      return ! expr.derived().val();
    }

  } // namespace MP

} // namespace Sacado


//-------------------------- Boolean Operators -----------------------
namespace Sacado {

  namespace MP {

    template <typename T, typename N>
    KOKKOSARRAY_INLINE_FUNCTION
    bool toBool(const Expr<T,N>& xx) {
      const typename Expr<T,N>::derived_type& x = 
	xx.derived();
      bool is_zero = true;
      for (int i=0; i<x.size(); i++)
	is_zero = is_zero && (x.coeff(i) == 0.0);
      return !is_zero;
    }

  } // namespace MP

} // namespace Sacado

#define PCE_BOOL_MACRO(OP)						\
namespace Sacado {							\
  namespace MP {							\
									\
    template <typename T1, typename T2, typename N>			\
    KOKKOSARRAY_INLINE_FUNCTION						\
    bool								\
    operator OP (const Expr<T1,N>& expr1,				\
		 const Expr<T2,N>& expr2)				\
    {									\
      return toBool(expr1) OP toBool(expr2);				\
    }									\
									\
    template <typename T2, typename N>					\
    KOKKOSARRAY_INLINE_FUNCTION						\
    bool								\
    operator OP (const typename T2::value_type& a,			\
		 const Expr<T2,N>& expr2)				\
    {									\
      return a OP toBool(expr2);					\
    }									\
									\
    template <typename T1, typename N>					\
    KOKKOSARRAY_INLINE_FUNCTION						\
    bool								\
    operator OP (const Expr<T1,N>& expr1,				\
		 const typename T1::value_type& b)			\
    {									\
      return toBool(expr1) OP b;					\
    }									\
  }									\
}

PCE_BOOL_MACRO(&&)
PCE_BOOL_MACRO(||)

#undef PCE_BOOL_MACRO


//-------------------------- I/O Operators -----------------------

namespace Sacado {

  namespace MP {

    template <typename T, typename N>
    KOKKOSARRAY_INLINE_FUNCTION
    std::ostream& operator << (std::ostream& os, 
			       const Expr<T,N>& x) {
      typedef typename T::value_type value_type;
      typedef typename T::storage_type storage_type;
      Vector<value_type, storage_type> a(x);
      os << a;
      return os;
    }

  } // namespace MP

} // namespace Sacado
