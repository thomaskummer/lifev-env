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

#ifndef IFPACK2_LOCALFILTER_DECL_HPP
#define IFPACK2_LOCALFILTER_DECL_HPP

#include "Ifpack2_ConfigDefs.hpp"
#include "Tpetra_ConfigDefs.hpp"
#include "Tpetra_RowMatrix.hpp"
#include "Teuchos_RefCountPtr.hpp"
#include "Teuchos_ScalarTraits.hpp"


namespace Ifpack2 {

/// \class LocalFilter
/// \brief Access only local rows and columns of a sparse matrix.
/// \tparam MatrixType A specialization of either Tpetra::RowMatrix,
///   or Tpetra::CrsMatrix (which is a subclass of Tpetra::CrsMatrix).
///
/// For the template parameter of this class, it is better to use the
/// most specific type you can, as long as that type is the same as
/// the type of the object you plan to give to LocalFilter's
/// constructor.  "Most specific" means, for example,
/// Tpetra::CrsMatrix instead of Tpetra::RowMatrix, if you plan to
/// give a Tpetra::CrsMatrix to this object's constructor.
///
/// This class provides a view of only the local rows and columns of
/// an existing sparse matrix.  "Local rows" are those owned by the
/// row Map on the calling process; "local columns" are owned by both
/// the column Map and the domain Map on the calling process.  The
/// view's communicator contains only the local process (in MPI terms,
/// <tt>MPI_COMM_SELF</tt>), and each process will have its own
/// distinct view of its local part of the matrix.
///
/// Here is an example of how to apply a LocalFilter to an existing
/// Tpetra sparse matrix:
/// \code
/// #include "Ifpack2_LocalFilter.hpp"
/// // ...
/// using Teuchos::RCP;
/// typedef Tpetra::RowMatrix<double> crs_matrix_type;
/// typedef Tpetra::CrsMatrix<double> crs_matrix_type;
///
/// RCP<crs_matrix_type> A = ...;
/// // ... fill the entries of A ...
/// A->FillComplete ();
///
/// Ifpack2::LocalFilter<crs_matrix_type> A_local (A);
/// \endcode
///
/// This class does not necessarily copy the entire sparse matrix; it
/// may choose instead just to "filter out" the nonlocal entries.
///
/// The intended use case of this class is to use Ifpack2 incomplete
/// factorizations as subdomain solvers, where each process has
/// exactly one subdomain.  LocalFilter "hides" the remote columns of
/// the matrix, making it square and thus a suitable input for local
/// linear solvers.
template<class MatrixType>
class LocalFilter :
    virtual public Tpetra::RowMatrix<typename MatrixType::scalar_type,
                                     typename MatrixType::local_ordinal_type,
                                     typename MatrixType::global_ordinal_type,
                                     typename MatrixType::node_type>
{
public:
  //! \name Typedefs
  //@{

  //! The type of the entries of the input MatrixType.
  typedef typename MatrixType::scalar_type scalar_type;

  //! Preserved only for backwards compatibility.  Please use "scalar_type".
  TEUCHOS_DEPRECATED typedef typename MatrixType::scalar_type Scalar;


  //! The type of local indices in the input MatrixType.
  typedef typename MatrixType::local_ordinal_type local_ordinal_type;

  //! Preserved only for backwards compatibility.  Please use "local_ordinal_type".
  TEUCHOS_DEPRECATED typedef typename MatrixType::local_ordinal_type LocalOrdinal;


  //! The type of global indices in the input MatrixType.
  typedef typename MatrixType::global_ordinal_type global_ordinal_type;

  //! Preserved only for backwards compatibility.  Please use "global_ordinal_type".
  TEUCHOS_DEPRECATED typedef typename MatrixType::global_ordinal_type GlobalOrdinal;


  //! The type of the Kokkos Node used by the input MatrixType.
  typedef typename MatrixType::node_type node_type;

  //! Preserved only for backwards compatibility.  Please use "node_type".
  TEUCHOS_DEPRECATED typedef typename MatrixType::node_type Node;


  //! The type of the magnitude (absolute value) of a matrix entry.
  typedef typename Teuchos::ScalarTraits<scalar_type>::magnitudeType magnitude_type;

  //! Preserved only for backwards compatibility.  Please use "magnitude_type".
  TEUCHOS_DEPRECATED typedef typename Teuchos::ScalarTraits<scalar_type>::magnitudeType magnitudeType;

  //@}
  //! \name Constructor and destructor
  //@{

  /// \brief Constructor
  ///
  /// \param A [in] The sparse matrix to which to apply the local
  ///   filter, as a Tpetra::RowMatrix.  (Tpetra::CrsMatrix inherits
  ///   from this, so you may use a Tpetra::CrsMatrix here instead.)
  ///
  /// This class will <i>not</i> modify the input matrix.
  explicit LocalFilter (const Teuchos::RCP<const Tpetra::RowMatrix<scalar_type, local_ordinal_type, global_ordinal_type, node_type> >& Matrix);

  //! Destructor
  virtual ~LocalFilter();

  //@}
  //! \name Matrix Query Methods
  //@{

  //! Returns the communicator.
  virtual const Teuchos::RCP<const Teuchos::Comm<int> > & getComm() const;

  //! Returns the underlying Kokkos Node object.
  virtual Teuchos::RCP<node_type> getNode() const;

  //! Returns the Map that describes the row distribution in this matrix.
  virtual const Teuchos::RCP<const Tpetra::Map<local_ordinal_type,global_ordinal_type,node_type> > & getRowMap() const;

  //! Returns the Map that describes the column distribution in this matrix.
  virtual const Teuchos::RCP<const Tpetra::Map<local_ordinal_type,global_ordinal_type,node_type> > & getColMap() const;

  //! Returns the Map that describes the domain distribution in this matrix.
  virtual const Teuchos::RCP<const Tpetra::Map<local_ordinal_type,global_ordinal_type,node_type> > & getDomainMap() const;

  //! Returns the Map that describes the range distribution in this matrix.
  virtual const Teuchos::RCP<const Tpetra::Map<local_ordinal_type,global_ordinal_type,node_type> > & getRangeMap() const;

  //! The (locally filtered) matrix's graph.
  virtual Teuchos::RCP<const Tpetra::RowGraph<local_ordinal_type,global_ordinal_type,node_type> > getGraph() const;

  //! The number of global rows in this matrix.
  virtual global_size_t getGlobalNumRows() const;

  //! The number of global columns in this matrix.
  virtual global_size_t getGlobalNumCols() const;

  //! The number of rows owned on the calling process.
  virtual size_t getNodeNumRows() const;

  //! The number of columns in the (locally filtered) matrix.
  virtual size_t getNodeNumCols() const;

  //! Returns the index base for global indices for this matrix.
  virtual global_ordinal_type getIndexBase() const;

  //! Returns the global number of entries in this matrix.
  virtual global_size_t getGlobalNumEntries() const;

  //! Returns the local number of entries in this matrix.
  virtual size_t getNodeNumEntries() const;

  /// \brief The current number of entries on this node in the specified global row.
  ///
  /// \return <tt>Teuchos::OrdinalTraits<size_t>::invalid()</tt> if
  ///   the specified row is not owned by this process, otherwise the
  ///   number of entries in that row on this process.
  virtual size_t getNumEntriesInGlobalRow (global_ordinal_type globalRow) const;

  /// \brief The current number of entries on this node in the specified local row.
  ///
  /// \return <tt>Teuchos::OrdinalTraits<size_t>::invalid()</tt> if
  ///   the specified local row is not valid on this process,
  ///   otherwise the number of entries in that row on this process.
  virtual size_t getNumEntriesInLocalRow (local_ordinal_type localRow) const;

  //! The number of global diagonal entries, based on global row/column index comparisons.
  virtual global_size_t getGlobalNumDiags() const;

  //! The number of local diagonal entries, based on global row/column index comparisons.
  virtual size_t getNodeNumDiags() const;

  //! The maximum number of entries across all rows/columns on all processes.
  virtual size_t getGlobalMaxNumRowEntries() const;

  //! The maximum number of entries across all rows/columns on this process.
  virtual size_t getNodeMaxNumRowEntries() const;

  //! Whether this matrix has a well-defined column Map.
  virtual bool hasColMap() const;

  //! Whether this matrix is lower triangular.
  virtual bool isLowerTriangular() const;

  //! Whether this matrix is upper triangular.
  virtual bool isUpperTriangular() const;

  //! Whether the underlying sparse matrix is locally (opposite of globally) indexed.
  virtual bool isLocallyIndexed() const;

  //! Whether the underlying sparse matrix is globally (opposite of locally) indexed.
  virtual bool isGloballyIndexed() const;

  //! Returns \c true if fillComplete() has been called.
  virtual bool isFillComplete() const;

  //! Returns \c true if RowViews are supported.
  virtual bool supportsRowViews() const;

  //@}

  //! @name Extraction Methods
  //@{

  //! Extract a list of entries in a specified global row of this matrix. Put into pre-allocated storage.
  /*!
    \param LocalRow - (In) Global row number for which indices are desired.
    \param Indices - (Out) Global column indices corresponding to values.
    \param Values - (Out) Matrix values.
    \param NumEntries - (Out) Number of indices.

    Note: A std::runtime_error exception is thrown if either \c Indices or \c Values is not large enough to hold the data associated
    with row \c GlobalRow. If \c GlobalRow does not belong to this node, then \c Indices and \c Values are unchanged and \c NumIndices is
    returned as Teuchos::OrdinalTraits<size_t>::invalid().
  */
  virtual void
  getGlobalRowCopy (global_ordinal_type GlobalRow,
                    const Teuchos::ArrayView<global_ordinal_type> &Indices,
                    const Teuchos::ArrayView<scalar_type> &Values,
                    size_t &NumEntries) const;

  //! Extract a list of entries in a specified local row of the graph. Put into storage allocated by calling routine.
  /*!
    \param LocalRow - (In) Local row number for which indices are desired.
    \param Indices - (Out) Local column indices corresponding to values.
    \param Values - (Out) Matrix values.
    \param NumIndices - (Out) Number of indices.

    Note: A std::runtime_error exception is thrown if either \c Indices or \c Values is not large enough to hold the data associated
    with row \c LocalRow. If \c LocalRow is not valid for this node, then \c Indices and \c Values are unchanged and \c NumIndices is
    returned as Teuchos::OrdinalTraits<size_t>::invalid().
  */
  virtual void
  getLocalRowCopy (local_ordinal_type LocalRow,
                   const Teuchos::ArrayView<local_ordinal_type> &Indices,
                   const Teuchos::ArrayView<scalar_type> &Values,
                   size_t &NumEntries) const ;

  //! Extract a const, non-persisting view of global indices in a specified row of the matrix.
  /*!
    \param GlobalRow - (In) Global row number for which indices are desired.
    \param Indices   - (Out) Global column indices corresponding to values.
    \param Values    - (Out) Row values
    \pre <tt>isLocallyIndexed() == false</tt>
    \post <tt>indices.size() == getNumEntriesInGlobalRow(GlobalRow)</tt>

    Note: If \c GlobalRow does not belong to this node, then \c indices is set to null.
  */
  virtual void
  getGlobalRowView (global_ordinal_type GlobalRow,
                    Teuchos::ArrayView<const global_ordinal_type> &indices,
                    Teuchos::ArrayView<const scalar_type> &values) const;

  //! Extract a const, non-persisting view of local indices in a specified row of the matrix.
  /*!
    \param LocalRow - (In) Local row number for which indices are desired.
    \param Indices  - (Out) Global column indices corresponding to values.
    \param Values   - (Out) Row values
    \pre <tt>isGloballyIndexed() == false</tt>
    \post <tt>indices.size() == getNumEntriesInLocalRow(LocalRow)</tt>

    Note: If \c LocalRow does not belong to this node, then \c indices is set to null.
  */
  virtual void
  getLocalRowView (local_ordinal_type LocalRow,
                   Teuchos::ArrayView<const local_ordinal_type> &indices,
                   Teuchos::ArrayView<const scalar_type> &values) const;

  //! \brief Get a copy of the diagonal entries owned by this node, with local row indices.
  /*! Returns a distributed Vector object partitioned according to this matrix's row map, containing the
    the zero and non-zero diagonals owned by this node. */
  virtual void
  getLocalDiagCopy (Tpetra::Vector<scalar_type, local_ordinal_type, global_ordinal_type, node_type> &diag) const;

  //@}
  //! \name Mathematical Methods
  //@{

  /**
   * \brief Scales the RowMatrix on the left with the Vector x.
   *
   * This matrix will be scaled such that A(i,j) = x(i)*A(i,j)
   * where i denoes the global row number of A and
   * j denotes the global column number of A.
   *
   * \param x A vector to left scale this matrix.
   */
  virtual void leftScale(const Tpetra::Vector<scalar_type, local_ordinal_type, global_ordinal_type, node_type>& x);

  /**
   * \brief Scales the RowMatrix on the right with the Vector x.
   *
   * This matrix will be scaled such that A(i,j) = x(j)*A(i,j)
   * where i denoes the global row number of A and
   * j denotes the global column number of A.
   *
   * \param x A vector to right scale this matrix.
   */
  virtual void rightScale(const Tpetra::Vector<scalar_type, local_ordinal_type, global_ordinal_type, node_type>& x);

  /// \brief The Frobenius norm of the (locally filtered) matrix.
  ///
  /// This method may return a different value on each process,
  /// because this method computes the norm of the locally filtered
  /// matrix, which may be different on each process.
  ///
  /// The Frobenius norm of a matrix \f$A\f$ is defined as
  /// \f$\|A\|_F = \sqrt{\sum_{i,j} \|A_{ij}\|^2}\f$.
  virtual magnitude_type getFrobeniusNorm() const;

  //! \brief Computes the operator-multivector application.
  /*! Loosely, performs \f$Y = \alpha \cdot A^{\textrm{mode}} \cdot X + \beta \cdot Y\f$. However, the details of operation
    vary according to the values of \c alpha and \c beta. Specifically
    - if <tt>beta == 0</tt>, apply() <b>must</b> overwrite \c Y, so that any values in \c Y (including NaNs) are ignored.
    - if <tt>alpha == 0</tt>, apply() <b>may</b> short-circuit the operator, so that any values in \c X (including NaNs) are ignored.
  */
  virtual void
  apply (const Tpetra::MultiVector<scalar_type, local_ordinal_type, global_ordinal_type, node_type> &X,
         Tpetra::MultiVector<scalar_type, local_ordinal_type, global_ordinal_type, node_type> &Y,
         Teuchos::ETransp mode = Teuchos::NO_TRANS,
         scalar_type alpha = Teuchos::ScalarTraits<scalar_type>::one(),
         scalar_type beta = Teuchos::ScalarTraits<scalar_type>::zero()) const;

  //! Indicates whether this operator supports applying the adjoint operator.
  virtual bool hasTransposeApply() const;

  //@}

  //! \name Deprecated routines to be removed at some point in the future.
  //@{

  //! Deprecated. Get a persisting const view of the entries in a specified global row of this matrix.
  /*!
    \param GlobalRow - (In) Global row from which to retrieve matrix entries.
    \param Indices - (Out) Indices for the global row.
    \param Values - (Out) Values for the global row.

    Note: If \c GlobalRow does not belong to this node, then \c Indices and \c Values are set to <tt>Teuchos::null</t>>.

    \pre isLocallyIndexed()==false
  */
  TPETRA_DEPRECATED virtual void getGlobalRowView(global_ordinal_type GlobalRow,
                                                  Teuchos::ArrayRCP<const global_ordinal_type> &indices,
                                                  Teuchos::ArrayRCP<const scalar_type>        &values) const;

  //! Deprecated. Get a persisting const view of the entries in a specified local row of this matrix.
  /*!
    \param LocalRow - (In) Local row from which to retrieve matrix entries.
    \param Indices - (Out) Indices for the local row.
    \param Values - (Out) Values for the local row.

    Note: If \c LocalRow is not valid for this node, then \c Indices and \c Values are set to <tt>Teuchos::null</tt>.

    \pre isGloballyIndexed()==false
  */
  TPETRA_DEPRECATED virtual void getLocalRowView(local_ordinal_type LocalRow,
                                                 Teuchos::ArrayRCP<const local_ordinal_type> &indices,
                                                 Teuchos::ArrayRCP<const scalar_type>       &values) const;
  //@}


private:

  //! Pointer to the matrix to be preconditioned.
  Teuchos::RCP<const Tpetra::RowMatrix<scalar_type, local_ordinal_type, global_ordinal_type, node_type> > A_;
  //! Map based on SerialComm_, containing the local rows only.
  Teuchos::RCP<const Tpetra::Map<local_ordinal_type, global_ordinal_type, node_type> > LocalMap_;
  //! Number of rows in the local matrix.
  size_t NumRows_;
  //! Number of nonzeros in the local matrix.
  size_t NumNonzeros_;
  //! Maximum number of nonzero entries in a row for the filtered matrix.
  size_t MaxNumEntries_;
  //! Maximum number of nonzero entries in a row for the unfiltered matrix.
  size_t MaxNumEntriesA_;
  //! NumEntries_[i] contains the nonzero entries in row `i'.
  std::vector<size_t> NumEntries_;
  //! Used in ExtractMyRowCopy, to avoid allocation each time.
  mutable Teuchos::Array<local_ordinal_type> Indices_;
  //! Used in ExtractMyRowCopy, to avoid allocation each time.
  mutable Teuchos::Array<scalar_type> Values_;

};// class LocalFilter

}// namespace Ifpack2

#endif /* IFPACK2_LOCALFILTER_DECL_HPP */
