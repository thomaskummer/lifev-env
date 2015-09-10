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

#ifndef IFPACK2_DIAGONAL_DEF_HPP
#define IFPACK2_DIAGONAL_DEF_HPP

#include "Ifpack2_Diagonal_decl.hpp"
#include "Ifpack2_Condest.hpp"

namespace Ifpack2 {

template<class MatrixType>
Diagonal<MatrixType>::Diagonal(const Teuchos::RCP<const MatrixType>& A)
 : isInitialized_(false),
   isComputed_(false),
   domainMap_(A->getDomainMap()),
   rangeMap_(A->getRangeMap()),
   matrix_(A),
   inversediag_(),
   numInitialize_(0),
   numCompute_(0),
   numApply_(0),
   condEst_ (-Teuchos::ScalarTraits<magnitudeType>::one ())
{
}

template<class MatrixType>
Diagonal<MatrixType>::Diagonal(const Teuchos::RCP<const Tpetra::Vector<Scalar,LocalOrdinal,GlobalOrdinal,Node> >& diag)
 : isInitialized_(false),
   isComputed_(false),
   domainMap_(),
   rangeMap_(),
   matrix_(),
   inversediag_(diag),
   numInitialize_(0),
   numCompute_(0),
   numApply_(0),
   condEst_ (-Teuchos::ScalarTraits<magnitudeType>::one ())
{
}

template<class MatrixType>
Diagonal<MatrixType>::~Diagonal()
{
}

template<class MatrixType>
void Diagonal<MatrixType>::setParameters(const Teuchos::ParameterList& /*params*/)
{
}

template<class MatrixType>
void Diagonal<MatrixType>::initialize()
{
  if (isInitialized_) return;

  // Precompute diagonal offsets so we don't have to search for them later.
  if (matrix_ != Teuchos::null) {
    matrix_->getLocalDiagOffsets(offsets_);
  }

  isInitialized_ = true;
  ++numInitialize_;
}

template<class MatrixType>
void Diagonal<MatrixType>::compute()
{
  if (!isInitialized_) initialize();

  isComputed_ = false;

  if (matrix_ == Teuchos::null) {
    isComputed_ = true;
    return;
  }

  // Get the diagonal entries using the precomputed offsets and invert them.
  Teuchos::RCP<Tpetra::Vector<Scalar,LocalOrdinal,GlobalOrdinal,Node> > tmp_vec = Teuchos::rcp(new Tpetra::Vector<Scalar,LocalOrdinal,GlobalOrdinal,Node>(matrix_->getRowMap()));
  matrix_->getLocalDiagCopy(*tmp_vec, offsets_());
  tmp_vec->reciprocal(*tmp_vec);

  inversediag_ = tmp_vec;

  isComputed_ = true;
  ++numCompute_;
}

template<class MatrixType>
void Diagonal<MatrixType>::apply(const Tpetra::MultiVector<Scalar,LocalOrdinal,GlobalOrdinal,Node>& X,
             Tpetra::MultiVector<Scalar,LocalOrdinal,GlobalOrdinal,Node>& Y,
             Teuchos::ETransp /*mode*/,
                 Scalar alpha,
                 Scalar beta) const
{
  // This method will not just call applyTempl() for now to avoid doing the extra work of
  // copying data to intermediate vectors to convert scalar types.

  TEUCHOS_TEST_FOR_EXCEPTION(!isComputed(), std::runtime_error,
    "Ifpack2::Diagonal::apply() ERROR, compute() hasn't been called yet.");

  ++numApply_;
  Y.elementWiseMultiply(alpha, *inversediag_, X, beta);
}

template<class MatrixType>
template<class DomainScalar, class RangeScalar>
void Diagonal<MatrixType>::applyTempl(const Tpetra::MultiVector<DomainScalar,LocalOrdinal,GlobalOrdinal,Node>& X,
             Tpetra::MultiVector<RangeScalar,LocalOrdinal,GlobalOrdinal,Node>& Y,
             Teuchos::ETransp /*mode*/,
                 RangeScalar alpha,
                 RangeScalar beta) const
{
  typedef typename Tpetra::MultiVector<RangeScalar,LocalOrdinal,GlobalOrdinal,Node> RangeMultiVectorType;
  typedef typename Tpetra::Vector<RangeScalar,LocalOrdinal,GlobalOrdinal,Node> RangeVectorType;

  TEUCHOS_TEST_FOR_EXCEPTION(!isComputed(), std::runtime_error,
    "Ifpack2::Diagonal::apply() ERROR, compute() hasn't been called yet.");

  TEUCHOS_TEST_FOR_EXCEPTION(X.getNumVectors() != Y.getNumVectors(), std::runtime_error,
     "Ifpack2::Diagonal::apply() ERROR: X.getNumVectors() != Y.getNumVectors().");

  ++numApply_;

  Teuchos::RCP<RangeMultiVectorType> Xtmp = rcp(new RangeMultiVectorType(X.getMap(), X.getNumVectors()));
  Teuchos::RCP<RangeVectorType> invtmp = rcp(new RangeVectorType(inversediag_->getMap()));

  for (size_t j = 0; j < X.getNumVectors(); ++j) {
    Teuchos::ArrayRCP<const DomainScalar> xVals = X.getVector( j )->get1dView();
    Teuchos::ArrayRCP<RangeScalar> xValsRange = Xtmp->getVectorNonConst( j )->get1dViewNonConst();
    if( xVals.size() ) {
      std::transform( xVals.begin(), xVals.end(), xValsRange.begin(), Teuchos::asFunc<RangeScalar>() );
    }
  }
  Teuchos::ArrayRCP<const Scalar> invVals = inversediag_->get1dView();
  Teuchos::ArrayRCP<RangeScalar> invValsRange = invtmp->get1dViewNonConst();
  if( invVals.size() ) {
    std::transform( invVals.begin(), invVals.end(), invValsRange.begin(), Teuchos::asFunc<RangeScalar>() );
  }

  Y.elementWiseMultiply(alpha, *invtmp, *Xtmp, beta);
}

template<class MatrixType>
typename Teuchos::ScalarTraits<typename MatrixType::scalar_type>::magnitudeType
Diagonal<MatrixType>::computeCondEst(
                     CondestType CT,
                     LocalOrdinal MaxIters,
                     magnitudeType Tol,
                     const Teuchos::Ptr<const Tpetra::RowMatrix<Scalar,LocalOrdinal,GlobalOrdinal,Node> > &matrix)
{
  const magnitudeType minusOne = Teuchos::ScalarTraits<magnitudeType>::one ();

  if (!isComputed()) { // cannot compute right now
    return minusOne;
  }
  // NOTE: this is computing the *local* condest
  if (condEst_ == minusOne) {
    condEst_ = Ifpack2::Condest(*this, CT, MaxIters, Tol, matrix);
  }
  return(condEst_);
}

template <class MatrixType>
int Diagonal<MatrixType>::getNumInitialize() const {
  return(numInitialize_);
}

template <class MatrixType>
int Diagonal<MatrixType>::getNumCompute() const {
  return(numCompute_);
}

template <class MatrixType>
int Diagonal<MatrixType>::getNumApply() const {
  return(numApply_);
}

template <class MatrixType>
double Diagonal<MatrixType>::getInitializeTime() const {
  return(initializeTime_);
}

template<class MatrixType>
double Diagonal<MatrixType>::getComputeTime() const {
  return(computeTime_);
}

template<class MatrixType>
double Diagonal<MatrixType>::getApplyTime() const {
  return(applyTime_);
}

template <class MatrixType>
std::string Diagonal<MatrixType>::description() const
{
  return std::string("Ifpack2::Diagonal");
}

template <class MatrixType>
void Diagonal<MatrixType>::describe(Teuchos::FancyOStream &out, const Teuchos::EVerbosityLevel verbLevel) const
{
  if (verbLevel != Teuchos::VERB_NONE) {
    out << this->description() << std::endl;
    out << "  numApply: " << numApply_ << std::endl;
  }
}

}//namespace Ifpack2

#endif
