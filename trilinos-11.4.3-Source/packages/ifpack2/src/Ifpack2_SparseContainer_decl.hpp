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

#ifndef IFPACK2_SPARSECONTAINER_DECL_HPP
#define IFPACK2_SPARSECONTAINER_DECL_HPP

#include "Ifpack2_Container.hpp"
#include "Tpetra_MultiVector.hpp"
#include "Tpetra_Map.hpp"
#include "Tpetra_RowMatrix.hpp"
#include "Tpetra_CrsMatrix.hpp"
#include "Teuchos_ParameterList.hpp"
#include "Teuchos_RefCountPtr.hpp"


/*! 
\brief Ifpack2_SparseContainer: a class for storing and solving linear systems
using sparse matrices.

<P>To understand what an IFPACK2 container is, please refer to the documentation 
of the pure virtual class Ifpack2::Container. Currently, containers are
used by class Ifpack2::BlockRelaxation.

<P>Using block methods, one needs to store all diagonal blocks and
to be also to apply the inverse of each diagonal block. Using
class Ifpack2::SparseContainer, one can store the blocks as sparse
matrices (Tpetra::CrsMatrix), which can be advantageous when the 
blocks are large. Otherwise,
class Ifpack2::DenseContainer is probably more appropriate.

<P>Sparse containers are templated with a type InverseType, which represent the 
class to use in the application of the inverse. (InverseType is not
used in Ifpack2::DenseContainer). In SparseContainer, T must be
an Ifpack2::Preconditioner derived class. The container will allocate
a \c T object, use setParameters() and compute(), then
use \c T every time the linear system as to be solved (using the
ApplyInverse() method of \c T).

\date Last modified on Aug-12.

*/

namespace Ifpack2 {

template<typename MatrixType, typename InverseType >
class SparseContainer : public Container<MatrixType,InverseType> {

public:
  typedef typename MatrixType::scalar_type          MatrixScalar;
  typedef typename MatrixType::local_ordinal_type   MatrixLocalOrdinal;
  typedef typename MatrixType::global_ordinal_type  MatrixGlobalOrdinal;
  typedef typename MatrixType::node_type            MatrixNode;

  typedef typename InverseType::scalar_type         InverseScalar;
  typedef typename InverseType::local_ordinal_type  InverseLocalOrdinal;
  typedef typename InverseType::global_ordinal_type InverseGlobalOrdinal;
  typedef typename InverseType::node_type           InverseNode;

  //@{ Constructors/Destructors.
  //! Constructor.
  SparseContainer(const size_t NumRows, const size_t NumVectors = 1);

  //! Destructor.
  virtual ~SparseContainer();
  //@}

  //@{ Get/Set methods.
  //! Returns the number of rows of the matrix and X/Y.
  virtual size_t getNumRows() const;

  // Returns the number of vectors in X/Y.
  //  virtual size_t getNumVectors() const;

  //! Returns the ID associated to local row i. 
  /*!
   * The set of (local) rows assigned to this container is defined
   * by calling ID(i) = j, where i (from 0 to NumRows()) indicates
   * the container-row, and j indicates the local row in the calling
   * process.
   *
   * This is usually used to recorder the local row ID (on calling process)
   * of the i-th row in the container.
   */
  virtual MatrixLocalOrdinal & ID(const size_t i);

  //! Returns \c true is the container has been successfully initialized.
  virtual bool isInitialized() const;

  //! Returns \c true is the container has been successfully computed.
  virtual bool isComputed() const;

  //! Sets all necessary parameters.  
  virtual void setParameters(const Teuchos::ParameterList& List);

  //@}

  //@{ Mathematical functions.
  /*! 
   * \brief Initializes the container, by completing all the operations based 
   * on matrix structure.
   *
   * \note After a call to Initialize(), no new matrix entries can be
   * added.
   */
  virtual void initialize();
  //! Finalizes the linear system matrix and prepares for the application of the inverse.
  virtual void compute(const Teuchos::RCP<const Tpetra::RowMatrix<MatrixScalar,MatrixLocalOrdinal,MatrixGlobalOrdinal,MatrixNode> >& Matrix);

  //! Computes Y = alpha * M^{-1} X + beta*Y
  /*! Here the X and Y are the size of the global problem the container was extracted from to begin with.
   */
  virtual void apply(const Tpetra::MultiVector<MatrixScalar,MatrixLocalOrdinal,MatrixGlobalOrdinal,MatrixNode>& X,
		     Tpetra::MultiVector<MatrixScalar,MatrixLocalOrdinal,MatrixGlobalOrdinal,MatrixNode>& Y,
		     Teuchos::ETransp mode=Teuchos::NO_TRANS,
		     MatrixScalar alpha=Teuchos::ScalarTraits< MatrixScalar >::one(),
		     MatrixScalar beta=Teuchos::ScalarTraits< MatrixScalar >::zero());

  //! Computes Y = alpha * diag(D) * M^{-1} (diag(D) X) + beta*Y
  /*! Here D, X and Y are the size of the global problem the container was extracted from to begin with.  D has to be 
      a single Vector, while X and Y can be MultiVectors.   This function is designed to support techniques with overlap,
      such as Schwarz methods.
  */ 
  virtual void weightedApply(const Tpetra::MultiVector<MatrixScalar,MatrixLocalOrdinal,MatrixGlobalOrdinal,MatrixNode>& X,
			     Tpetra::MultiVector<MatrixScalar,MatrixLocalOrdinal,MatrixGlobalOrdinal,MatrixNode>& Y,
			     const Tpetra::Vector<MatrixScalar,MatrixLocalOrdinal,MatrixGlobalOrdinal,MatrixNode>& D,
			     Teuchos::ETransp mode=Teuchos::NO_TRANS,
			     MatrixScalar alpha=Teuchos::ScalarTraits< MatrixScalar >::one(),
			     MatrixScalar beta=Teuchos::ScalarTraits< MatrixScalar >::zero());


  //@}

  //@{ Miscellaneous methods
  //! Destroys all data.
  virtual void destroy();
  //@}

  //! Prints basic information on iostream. This function is used by operator<<.
  virtual std::ostream& print(std::ostream& os) const;

  //! @name Overridden from Teuchos::Describable 
  //@{

  /** \brief Return a simple one-line description of this object. */
  virtual std::string description() const;

  /** \brief Print the object with some verbosity level to an FancyOStream object. */
  virtual void describe(Teuchos::FancyOStream &out, const Teuchos::EVerbosityLevel verbLevel=Teuchos::Describable::verbLevel_default) const;

  //@}

private:
  // private copy constructor
  SparseContainer(const SparseContainer<MatrixType,InverseType>& rhs);

  //! Sets the number of vectors for LHS/RHS.
  virtual void setNumVectors(const size_t NumVectors);

  //! Extract the submatrices identified by the ID set int ID().
  virtual void extract(const Teuchos::RCP<const Tpetra::RowMatrix<MatrixScalar,MatrixLocalOrdinal,MatrixGlobalOrdinal,MatrixNode> >& Matrix);
  
  //! Number of rows in the local matrix.
  size_t NumRows_; 
  //! Number of vectors in the local linear system.
  size_t NumVectors_; 
  //! Linear map on which the local matrix is based.
  Teuchos::RCP<Tpetra::Map<InverseLocalOrdinal,InverseGlobalOrdinal,InverseNode> > Map_;
  //! Pointer to the local matrix.
  Teuchos::RCP<Tpetra::CrsMatrix<InverseScalar,InverseLocalOrdinal,InverseGlobalOrdinal,InverseNode> > Matrix_;
  //! Solution vector.
  mutable Teuchos::RCP<Tpetra::MultiVector<InverseScalar,InverseLocalOrdinal,InverseGlobalOrdinal,InverseNode> > Y_;
  //! Input vector for local problems
  mutable Teuchos::RCP<Tpetra::MultiVector<InverseScalar,InverseLocalOrdinal,InverseGlobalOrdinal,InverseNode> > X_;
  //! Contains the subrows/subcols of A that will be inserted in Matrix_.
  std::vector<MatrixLocalOrdinal> GID_;
  //! If \c true, the container has been successfully initialized.
  bool IsInitialized_;
  //! If \c true, the container has been successfully computed.
  bool IsComputed_;
  //! Serial communicator (containing only MPI_COMM_SELF if MPI is used).
  Teuchos::RCP<Teuchos::Comm<int> > LocalComm_;
  //! Pointer to an Ifpack2_Preconditioner object whose ApplyInverse() defined the action of the inverse of the local matrix.
  Teuchos::RCP<InverseType> Inverse_;

  Teuchos::ParameterList List_;

};

}// namespace Ifpack2



#endif // IFPACK2_SPARSECONTAINER_HPP
