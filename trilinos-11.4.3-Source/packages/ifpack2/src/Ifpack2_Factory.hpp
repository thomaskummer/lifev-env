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

#ifndef IFPACK2_FACTORY_HPP
#define IFPACK2_FACTORY_HPP

#include "Ifpack2_ConfigDefs.hpp"
#include "Ifpack2_Preconditioner.hpp"
#include "Ifpack2_Relaxation.hpp"
#include "Ifpack2_Diagonal.hpp"
#include "Ifpack2_Chebyshev.hpp"
#include "Ifpack2_RILUK.hpp"
#include "Ifpack2_ILUT.hpp"
#include "Ifpack2_Krylov.hpp"
#include "Ifpack2_AdditiveSchwarz.hpp"

/** Classes and functions for templated preconditioning.  */
namespace Ifpack2 {

/** \brief Return true if the specified preconditioner type supports
 * unsymmetric matrices. */
bool supportsUnsymmetric(const std::string& prec_type);

//! A factory class to create Ifpack2 preconditioners.
/*!
Ifpack2::Factory contains just one method: create().
Using Ifpack2::Factory::create(), users can easily create a variety of
Ifpack2 preconditioners. 

create requires 3 arguments:
- a string, indicating the preconditioner to be built;
- a pointer to a Tpetra::RowMatrix, representing the matrix
  to be used to define the preconditioner;
- an optional integer (defaulted to 0), that specifies the amount of
  overlap among the processes.

The first argument can assume the following values:
- \c "DIAGONAL"  : returns an instance of Ifpack2::Diagonal.
- \c "RELAXATION"  : returns an instance of Ifpack2::Relaxation.
- \c "CHEBYSHEV"   : returns an instance of Ifpack2::Chebyshev (overlap is ignored).
- \c "ILUT"        : returns an instance of Ifpack2::ILUT.
- \c "RILUK"       : returns an instance of Ifpack2::RILUK.
- \c "Krylov"       : returns an instance of Ifpack2::Krylov.
- otherwise, create() returns Teuchos::null.


<P> The following fragment of code shows the
basic usage of this class.
\code
#include "Ifpack2_Factory.hpp"

...
typedef double Scalar;
typedef int    LocalOrdinal;
typedef int    GlobalOrdinal;
typedef Tpetra::DefaultPlatform::DefaultPlatformType::NodeType Node;
typedef Tpetra::CrsMatrix<Scalar,LocalOrdinal,GlobalOrdinal,Node> TCrsMatrix;
typedef Ifpack2::Preconditioner<Scalar,LocalOrdinal,GlobalOrdinal,Node> TPrecond;
...
Ifpack2::Factory factory;

Teuchos::RCP<TCrsMatrix> A; // A is fillComplete()'d.
std::string PrecType = "ILUT"; // use incomplete LU on each process
Teuchos::RCP<TPrecond> prec = factory.create(PrecType, A);

Teuchos::ParameterList params;
params.set("fact: ilut level-of-fill", 5.0); // use ILUT(fill=5, drop=0)

prec->setParameters(List);
prec->initialize();
prec->compute();

// now prec can be used as a preconditioner

\endcode

*/
class Factory {
public:

  /** \brief Creates an instance of Ifpack2_Preconditioner given the string
   * name of the preconditioner type (throws exception if given unrecognized name).
   *
   * \param PrecType (In) - String name of preconditioner type to be created. 
   *
   * \param Matrix (In) - Matrix used to define the preconditioner
   *
   * \param overlap (In) - specified overlap, defaulted to 0.
   *
   * Returns <tt>0</tt> if the preconditioner with that input name does not
   * exist.  Otherwise, return a newly created preconditioner object.  Note
   * that the client is responsible for calling <tt>delete</tt> on the
   * returned object once it is finished using it!
   */
  template<class MatrixType>
  static
  Teuchos::RCP<Ifpack2::Preconditioner<typename MatrixType::scalar_type,
                                       typename MatrixType::local_ordinal_type,
                                       typename MatrixType::global_ordinal_type,
                                       typename MatrixType::node_type> >
  create(const std::string& prec_type,
         const Teuchos::RCP<const MatrixType>& matrix,
         const int overlap = 0);
};

/////////////////////////////////////////////
/////////////////////////////////////////////

template<class MatrixType>
Teuchos::RCP<Ifpack2::Preconditioner<typename MatrixType::scalar_type,typename MatrixType::local_ordinal_type,typename MatrixType::global_ordinal_type,typename MatrixType::node_type> >
Factory::create(const std::string& prec_type,
                const Teuchos::RCP<const MatrixType>& matrix,
                const int overlap)
{
  typedef typename MatrixType::scalar_type Scalar;
  typedef typename MatrixType::local_ordinal_type LocalOrdinal;
  typedef typename MatrixType::global_ordinal_type GlobalOrdinal;
  typedef typename MatrixType::node_type Node;
  (void)overlap;
  Teuchos::RCP<Ifpack2::Preconditioner<Scalar,LocalOrdinal,GlobalOrdinal,Node> > prec;

  bool one_mpi_rank=(matrix->getComm()->getSize()==1);

  if (prec_type == "ILUT") {
    /* Note: ILUT doesn't work for multiple MPI ranks... you have to use AdditiveSchwarz */
    if(one_mpi_rank) prec = Teuchos::rcp(new Ifpack2::ILUT<MatrixType>(matrix));
    else prec = Teuchos::rcp(new Ifpack2::AdditiveSchwarz<MatrixType,Ifpack2::ILUT<MatrixType> >(matrix,overlap));
  }
  else if (prec_type == "RILUK") {
    prec = Teuchos::rcp(new Ifpack2::RILUK<MatrixType>(matrix));
  }
  else if (prec_type == "RELAXATION") {
    prec = Teuchos::rcp(new Ifpack2::Relaxation<MatrixType>(matrix));
  }
  else if (prec_type == "CHEBYSHEV") {
    prec = Teuchos::rcp(new Ifpack2::Chebyshev<MatrixType>(matrix));
  }
  else if (prec_type == "DIAGONAL") {
    prec = Teuchos::rcp(new Ifpack2::Diagonal<MatrixType>(matrix));
  }
  else if (prec_type == "SCHWARZ") {
    prec = Teuchos::rcp(new Ifpack2::AdditiveSchwarz<MatrixType,Ifpack2::ILUT<MatrixType> >(matrix,overlap));
  }
  else if (prec_type == "KRYLOV") {
    prec = Teuchos::rcp(new Ifpack2::Krylov< MatrixType,Ifpack2::Preconditioner<Scalar,LocalOrdinal,GlobalOrdinal,Node> >(matrix));
  }
  else {
    std::ostringstream os;
    os << "Ifpack2::Factory::Create ERROR, invalid preconditioner type ("
       << prec_type << ")";
    std::string str = os.str();
    throw std::runtime_error(str);
  }
  return prec;
}

} //namespace Ifpack2

#endif
