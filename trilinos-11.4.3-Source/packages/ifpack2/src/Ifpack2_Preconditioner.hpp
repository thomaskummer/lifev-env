/*@HEADER
// ***********************************************************************
//
//       Ifpack2: Tempated Object-Oriented Algebraic Preconditioner Package
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
// Questions? Contact Michael A. Heroux (maherou@sandia.gov)
//
// ***********************************************************************
//@HEADER
*/

#ifndef IFPACK2_PRECONDITIONER_HPP
#define IFPACK2_PRECONDITIONER_HPP

#include "Ifpack2_ConfigDefs.hpp"
#include "Ifpack2_CondestType.hpp"
#include "Kokkos_DefaultNode.hpp"
#include "Tpetra_Operator.hpp"
#include "Tpetra_RowMatrix.hpp"
#include "Teuchos_ParameterList.hpp"
#include "Teuchos_ScalarTraits.hpp"
#include <iostream>

namespace Ifpack2 {

//! Base class for all Ifpack2 preconditioners.

/*!
  Ifpack2::Preconditioner is a pure virtual class, and it defines
  the structure of all Ifpack2 preconditioners.

  This class is a simple extension to Tpetra::Operator. It provides 
  the following additional methods:
  - initialize() performs all operations based on the graph
    of the matrix (without considering the numerical values);
  - isInitialized() returns true if the preconditioner
    has been successfully initialized;
  - compute() computes everything required to apply the
    preconditioner, using matrix values  (and assuming that the
    sparsity of the matrix has not been changed);
  - isComputed() should return true if the preconditioner
    has been successfully computed, false otherwise.
  - computeCondest() returns an estimation of the condition number, or -1.0
    if not available
  - getMatrix() returns a reference to the matrix to be preconditioned.

It is required that compute() internally call initialize() if isInitialized()
returns false. The preconditioner is applied by apply()
(which returns if isComputed() is false). Every time that initialize()
is called, the object destroys all the previously allocated 
information, and re-initializes the preconditioner. Every time
compute() is called, the object re-computes the actual values of
the preconditioner.

<b>Estimating Preconditioner Condition Numbers</b>

The condition of a matrix \f$B\f$, called \f$cond_p(B)\f$, is defined as
\f$cond_p(B) = \|B\|_p\|B^{-1}\|_p\f$ in some appropriate norm \f$p\f$.  \f$cond_p(B)\f$
gives some indication of how many accurate floating point
digits can be expected from operations involving the matrix and its
inverse.  A condition number approaching the accuracy of a given
floating point number system, about 15 decimal digits in IEEE double
precision, means that any results involving \f$B\f$ or \f$B^{-1}\f$ may be
meaningless.

Method compute() can be used to estimate of the condition number.
compute() requires one parameter, of type Ifpack2::CondestType
(default value is Ifpack2::Cheap; other valid choices are Ifpack2::CG and
Ifpack2::GMRES).

While Ifpack2::CG and Ifpack2::GMRES construct a solver, and
use methods AZ_cg_condnum and AZ_gmres_condnum to evaluate an
accurate (but very expensive) estimate of the condition number, 
Ifpack2::Cheap computes \f$\|(P)^{-1}e\|_\infty\f$, which is
only a very crude estimation of the actual condition number. Note that
this estimated number can be less than 1.0. 
However, this approach has the following advantages:
- since finding \f$z\f$ such that \f$P z = y\f$
is a basic kernel for applying the preconditioner, computing this
estimate of \f$cond_\infty(P^{-1})\f$ is performed by setting \f$y = e\f$, calling
the solve kernel to compute \f$z\f$ and then
computing \f$\|z\|_\infty\f$;
- the only cost is one application of the preconditioner.

If this estimate is very large, the application of the computed 
preconditioner may generate large numerical errors. Hence, the user
may check this number, and decide to recompute the preconditioner is
the computed estimate is larger than a given threshold. This is particularly useful in ICT and RILUK factorizations, as for 
ill-conditioned matrices, we often have difficulty computing usable incomplete
factorizations.  The most common source of problems is that the factorization may encounter a small or zero pivot,
in which case the factorization can fail, or even if the factorization
succeeds, the factors may be so poorly conditioned that use of them in
the iterative phase produces meaningless results.  Before we can fix
this problem, we must be able to detect it.  

*/

template<class Scalar, class LocalOrdinal = int, class GlobalOrdinal = LocalOrdinal, class Node = Kokkos::DefaultNode::DefaultNodeType>
class Preconditioner : virtual public Tpetra::Operator<Scalar,LocalOrdinal,GlobalOrdinal,Node> {

  public:
    typedef typename Teuchos::ScalarTraits<Scalar>::magnitudeType magnitudeType;

    //! Destructor.
    virtual ~Preconditioner(){}

    /** \name Methods implementing Tpetra::Operator. */
    //@{

    //! Returns the Map associated with the domain of this operator, which must be compatible with X.getMap().
    virtual const Teuchos::RCP<const Tpetra::Map<LocalOrdinal,GlobalOrdinal,Node> > & getDomainMap() const = 0;

    //! Returns the Map associated with the range of this operator, which must be compatible with Y.getMap().
    virtual const Teuchos::RCP<const Tpetra::Map<LocalOrdinal,GlobalOrdinal,Node> > & getRangeMap() const = 0;

    //! Applies the effect of the preconditioner.
    virtual void apply(const Tpetra::MultiVector<Scalar,LocalOrdinal,GlobalOrdinal,Node> &X, 
                             Tpetra::MultiVector<Scalar,LocalOrdinal,GlobalOrdinal,Node> &Y, 
                       Teuchos::ETransp mode = Teuchos::NO_TRANS,
               Scalar alpha = Teuchos::ScalarTraits<Scalar>::one(),
               Scalar beta = Teuchos::ScalarTraits<Scalar>::zero()) const = 0;

    //@}

    //! Sets all parameters for the preconditioner.
    virtual void setParameters(const Teuchos::ParameterList& List) = 0;

    //! Computes all (graph-related) data necessary to initialize the preconditioner.
    virtual void initialize() = 0;

    //! Returns true if the  preconditioner has been successfully initialized, false otherwise.
    virtual bool isInitialized() const = 0;

    //! Computes all (coefficient) data necessary to apply the preconditioner.
    virtual void compute() = 0;

    //! Returns true if the  preconditioner has been successfully computed, false otherwise.
    virtual bool isComputed() const = 0;

    //! Computes the condition number estimate and returns its value.
    virtual magnitudeType computeCondEst(CondestType CT = Ifpack2::Cheap,
                                         LocalOrdinal MaxIters = 1550,
                                         magnitudeType Tol = 1e-9,
                                         const Teuchos::Ptr<const Tpetra::RowMatrix<Scalar,LocalOrdinal,GlobalOrdinal,Node> > &Matrix = Teuchos::null) = 0;

    //! Returns the computed condition number estimate, or -1.0 if not computed.
    virtual magnitudeType getCondEst() const = 0;

    //! Returns a pointer to the input matrix.
    virtual Teuchos::RCP<const Tpetra::RowMatrix<Scalar,LocalOrdinal,GlobalOrdinal,Node> > getMatrix() const = 0;

    //! Returns the number of calls to initialize().
    virtual int getNumInitialize() const = 0;

    //! Returns the number of calls to compute().
    virtual int getNumCompute() const = 0;

    //! Returns the number of calls to apply().
    virtual int getNumApply() const = 0;

    //! Returns the time spent in initialize().
    virtual double getInitializeTime() const = 0;

    //! Returns the time spent in compute().
    virtual double getComputeTime() const = 0;

    //! Returns the time spent in apply().
    virtual double getApplyTime() const = 0;

};//class Preconditioner

}//namespace Ifpack2

#endif // IFPACK2_PRECONDITIONER_HPP
