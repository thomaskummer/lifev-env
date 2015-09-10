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

//-----------------------------------------------------
// Ifpack2::BorderedOperator is a translation of the
// LOCA_BorderedSolver_EpetraHouseholder
// implementation written by Eric Phipps.
// DMD.
//------------------------------------------------------


#ifndef IFPACK2_BORDEREDOPERATOR_DECL_HPP
#define IFPACK2_BORDEREDOPERATOR_DECL_HPP

#include "Ifpack2_ConfigDefs.hpp"
#include "Kokkos_DefaultNode.hpp"

#include "Tpetra_Operator.hpp"
#include "Teuchos_ParameterList.hpp"
#include "Teuchos_ScalarTraits.hpp"
#include <iostream>

namespace Ifpack2 {
//! Ifpack2 bordered operator

/*!
  Ifpack2::BorderedOperator is a concrete class
extending Tpetra::Operator and defining an interface.

  This class extends Tpetra::Operator, providing the additional methods:
  - compute() computes everything required to apply the
    bordered operator, using matrix values  (and assuming that the
    sparsity of the matrix has not been changed);
  - isComputed() should return true if the bordered operator
    has been successfully computed, false otherwise.
  - getRHS() returns a reference to the matrix to be preconditioned.
  - getLHS() returns a reference to the matrix to be preconditioned.

The bordered operator is applied by apply()
(which returns if isComputed() is false). 
Each time compute() is called, the object re-computes the actual values of
the bordered operator.

<b>Title of Method Description</b>
*/

template<class Scalar, class LocalOrdinal = int, class GlobalOrdinal = LocalOrdinal, class Node = Kokkos::DefaultNode::DefaultNodeType >
class BorderedOperator : virtual public Tpetra::Operator<Scalar,LocalOrdinal,GlobalOrdinal,Node > {

  public:

    //! Destructor.
    virtual ~BorderedOperator(){}

    /** \name Methods implementing Tpetra::Operator. */
    //@{

    //! Returns the Map associated with the domain of this operator, which must be compatible with X.getMap().
    virtual const Teuchos::RCP<const Tpetra::Map<LocalOrdinal,GlobalOrdinal,Node> > & getDomainMap() const;

    //! Returns the Map associated with the range of this operator, which must be compatible with Y.getMap().
    virtual const Teuchos::RCP<const Tpetra::Map<LocalOrdinal,GlobalOrdinal,Node> > & getRangeMap() const;


    bool hasTransposeApply() const;


    //! Applies the effect of the bordered operator.
    void apply(const Tpetra::MultiVector<Scalar,LocalOrdinal,GlobalOrdinal,Node> &X, 
                             Tpetra::MultiVector<Scalar,LocalOrdinal,GlobalOrdinal,Node> &Y, 
                       Teuchos::ETransp mode = Teuchos::NO_TRANS,
                       Scalar alpha = Teuchos::ScalarTraits<Scalar>::one(),
                       Scalar beta = Teuchos::ScalarTraits<Scalar>::zero()) const;

    //@}

  //! constuctor with Tpetra::Operator input.
  BorderedOperator(const Teuchos::RCP< const Tpetra::Operator<Scalar, LocalOrdinal, GlobalOrdinal, Node > > &A);

  private:

  //! reference to the operator
  Teuchos::RCP<const Tpetra::Operator<Scalar,LocalOrdinal,GlobalOrdinal,Node> > A_;

};//class BorderedOperator

}//namespace Ifpack2

#endif // IFPACK2_BORDEREDOPERATOR_DECL_HPP
