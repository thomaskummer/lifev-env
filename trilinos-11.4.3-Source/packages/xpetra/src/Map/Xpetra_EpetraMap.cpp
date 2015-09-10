// @HEADER
//
// ***********************************************************************
//
//             Xpetra: A linear algebra interface package
//                  Copyright 2012 Sandia Corporation
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
// Questions? Contact
//                    Jeremie Gaidamour (jngaida@sandia.gov)
//                    Jonathan Hu       (jhu@sandia.gov)
//                    Ray Tuminaro      (rstumin@sandia.gov)
//
// ***********************************************************************
//
// @HEADER
#include "Xpetra_ConfigDefs.hpp"

#ifdef HAVE_XPETRA_EPETRA

#include "Xpetra_EpetraMap.hpp"
#include "Xpetra_EpetraUtils.hpp"

#include "Xpetra_EpetraExceptions.hpp"

namespace Xpetra {

  // Implementation note for constructors: the Epetra_Comm is cloned in the constructor of Epetra_BlockMap. We don't need to keep a reference on it.
  // TODO: use toEpetra() function here.
  EpetraMap::EpetraMap(global_size_t numGlobalElements, int indexBase, const Teuchos::RCP<const Teuchos::Comm<int> > &comm,
                       LocalGlobal lg, const Teuchos::RCP<Node> &node)
  {
    // This test come from Tpetra (Epetra doesn't check if numGlobalElements,indexBase are equivalent across images).
    // In addition, for the test TEST_THROW(M map((myImageID == 0 ? GSTI : 0),0,comm), std::invalid_argument), only one node throw an exception and there is a dead lock.
    std::string errPrefix;
    errPrefix = Teuchos::typeName(*this) + "::constructor(numGlobal,indexBase,comm,lOrG): ";

    if (lg == GloballyDistributed) {
      const int myImageID = comm->getRank();

      // check that numGlobalElements,indexBase is equivalent across images
      global_size_t rootNGE = numGlobalElements;
      int rootIB  = indexBase;
      Teuchos::broadcast<int,global_size_t>(*comm,0,&rootNGE);
      Teuchos::broadcast<int,int>(*comm,0,&rootIB);
      int localChecks[2], globalChecks[2];
      localChecks[0] = -1;   // fail or pass
      localChecks[1] = 0;    // fail reason
      if (numGlobalElements != rootNGE) {
        localChecks[0] = myImageID;
        localChecks[1] = 1;
      }
      else if (indexBase != rootIB) {
        localChecks[0] = myImageID;
        localChecks[1] = 2;
      }
      // REDUCE_MAX will give us the image ID of the highest rank proc that DID NOT pass, as well as the reason
      // these will be -1 and 0 if all procs passed
      Teuchos::reduceAll<int,int>(*comm,Teuchos::REDUCE_MAX,2,localChecks,globalChecks);
      if (globalChecks[0] != -1) {
        if (globalChecks[1] == 1) {
          TEUCHOS_TEST_FOR_EXCEPTION(true,std::invalid_argument,
                             errPrefix << "numGlobal must be the same on all nodes (examine node " << globalChecks[0] << ").");
        }
        else if (globalChecks[1] == 2) {
          TEUCHOS_TEST_FOR_EXCEPTION(true,std::invalid_argument,
                             errPrefix << "indexBase must be the same on all nodes (examine node " << globalChecks[0] << ").");
        }
        else {
          // logic error on our part
          TEUCHOS_TEST_FOR_EXCEPTION(true,std::logic_error,
                             errPrefix << "logic error. Please contact the Tpetra team.");
        }
      }
    }

    // Note: validity of numGlobalElements checked by Epetra.

    IF_EPETRA_EXCEPTION_THEN_THROW_GLOBAL_INVALID_ARG((map_ = (rcp(new Epetra_BlockMap(static_cast<int>(numGlobalElements), 1, indexBase, *toEpetra(comm))))));
  }

  EpetraMap::EpetraMap(global_size_t numGlobalElements, size_t numLocalElements, int indexBase,
                       const Teuchos::RCP<const Teuchos::Comm<int> > &comm, const Teuchos::RCP<Node> &node)
  {
    // This test come from Tpetra
    using Teuchos::outArg;

    const size_t  L0 = Teuchos::OrdinalTraits<size_t>::zero();
    const size_t  L1 = Teuchos::OrdinalTraits<size_t>::one();
    const global_size_t GST0 = Teuchos::OrdinalTraits<global_size_t>::zero();
    const global_size_t GST1 = Teuchos::OrdinalTraits<global_size_t>::one();
    const global_size_t GSTI = Teuchos::OrdinalTraits<global_size_t>::invalid();

    std::string errPrefix;
    errPrefix = Teuchos::typeName(*this) + "::constructor(numGlobal,numLocal,indexBase,platform): ";

    // get a internodal communicator from the Platform
    const int myImageID = comm->getRank();

    global_size_t global_sum;
    { // begin scoping block
      // for communicating failures
      int localChecks[2], globalChecks[2];
      /* compute the global size
         we are computing the number of global elements because exactly ONE of the following is true:
         - the user didn't specify it, and we need it
         - the user did specify it, but we need to
         + validate it against the sum of the local sizes, and
         + ensure that it is the same on all nodes
      */
      Teuchos::reduceAll<int,global_size_t>(*comm,Teuchos::REDUCE_SUM,
                                            Teuchos::as<global_size_t>(numLocalElements),outArg(global_sum));
      /* there are three errors we should be detecting:
         - numGlobalElements != invalid() and it is incorrect/invalid
         - numLocalElements invalid (<0)
      */
      localChecks[0] = -1;
      localChecks[1] = 0;
      if (numLocalElements < L1 && numLocalElements != L0) {
        // invalid
        localChecks[0] = myImageID;
        localChecks[1] = 1;
      }
      else if (numGlobalElements < GST1 && numGlobalElements != GST0 && numGlobalElements != GSTI) {
        // invalid
        localChecks[0] = myImageID;
        localChecks[1] = 2;
      }
      else if (numGlobalElements != GSTI && numGlobalElements != global_sum) {
        // incorrect
        localChecks[0] = myImageID;
        localChecks[1] = 3;
      }
      // now check that indexBase is equivalent across images
      int rootIB = indexBase;
      Teuchos::broadcast<int,int>(*comm,0,&rootIB);   // broadcast one ordinal from node 0
      if (indexBase != rootIB) {
        localChecks[0] = myImageID;
        localChecks[1] = 4;
      }
      // REDUCE_MAX will give us the image ID of the highest rank proc that DID NOT pass
      // this will be -1 if all procs passed
      Teuchos::reduceAll<int,int>(*comm,Teuchos::REDUCE_MAX,2,localChecks,globalChecks);
      if (globalChecks[0] != -1) {
        if (globalChecks[1] == 1) {
          TEUCHOS_TEST_FOR_EXCEPTION(true,std::invalid_argument,
                             errPrefix << "numLocal is not valid on at least one node (possibly node "
                             << globalChecks[0] << ").");
        }
        else if (globalChecks[1] == 2) {
          TEUCHOS_TEST_FOR_EXCEPTION(true,std::invalid_argument,
                             errPrefix << "numGlobal is not valid on at least one node (possibly node "
                             << globalChecks[0] << ").");
        }
        else if (globalChecks[1] == 3) {
          TEUCHOS_TEST_FOR_EXCEPTION(true,std::invalid_argument,
                             errPrefix << "numGlobal doesn't match sum of numLocal (== "
                             << global_sum << ") on at least one node (possibly node "
                             << globalChecks[0] << ").");
        }
        else if (globalChecks[1] == 4) {
          TEUCHOS_TEST_FOR_EXCEPTION(true,std::invalid_argument,
                             errPrefix << "indexBase is not the same on all nodes (examine node "
                             << globalChecks[0] << ").");
        }
        else {
          // logic error on my part
          TEUCHOS_TEST_FOR_EXCEPTION(true,std::logic_error,
                             errPrefix << "logic error. Please contact the Tpetra team.");
        }
      }

    }

    // set numGlobalElements
    if (numGlobalElements == GSTI) {
      numGlobalElements = global_sum;}

    IF_EPETRA_EXCEPTION_THEN_THROW_GLOBAL_INVALID_ARG((map_ = (rcp(new Epetra_BlockMap(static_cast<int>(numGlobalElements), numLocalElements, 1, indexBase, *toEpetra(comm))))));
  }

  // TODO: UnitTest FAILED
  EpetraMap::EpetraMap(global_size_t numGlobalElements, const Teuchos::ArrayView<const int> &elementList, int indexBase,
                       const Teuchos::RCP<const Teuchos::Comm<int> > &comm, const Teuchos::RCP<Node> &node)
  {
    if (numGlobalElements == Teuchos::OrdinalTraits<global_size_t>::invalid()) {
      IF_EPETRA_EXCEPTION_THEN_THROW_GLOBAL_INVALID_ARG((map_ = (rcp(new Epetra_BlockMap(-1, elementList.size(), elementList.getRawPtr(), 1, indexBase, *toEpetra(comm))))));
    } else {
      IF_EPETRA_EXCEPTION_THEN_THROW_GLOBAL_INVALID_ARG((map_ = (rcp(new Epetra_BlockMap(numGlobalElements, elementList.size(), elementList.getRawPtr(), 1, indexBase, *toEpetra(comm))))));
    }
  }







  LookupStatus EpetraMap::getRemoteIndexList(const Teuchos::ArrayView< const GlobalOrdinal > &GIDList, const Teuchos::ArrayView< int > &nodeIDList, const Teuchos::ArrayView< int > &LIDList) const { XPETRA_MONITOR("EpetraMap::getRemoteIndexList"); return toXpetra(map_->RemoteIDList(GIDList.size(), GIDList.getRawPtr(), nodeIDList.getRawPtr(), LIDList.getRawPtr())); }

  LookupStatus EpetraMap::getRemoteIndexList(const Teuchos::ArrayView< const int > &GIDList, const Teuchos::ArrayView< int > &nodeIDList) const { XPETRA_MONITOR("EpetraMap::getRemoteIndexList"); return toXpetra(map_->RemoteIDList(GIDList.size(), GIDList.getRawPtr(), nodeIDList.getRawPtr(), 0)); }

  Teuchos::ArrayView< const int > EpetraMap::getNodeElementList() const { XPETRA_MONITOR("EpetraMap::getNodeElementList"); return ArrayView< const int >(map_->MyGlobalElements(), map_->NumMyElements()); /* Note: this method return a const array, so it is safe to use directly the internal array. */ }

  //typedef Kokkos::DefaultNode::DefaultNodeType Node;
  const Teuchos::RCP<Kokkos::DefaultNode::DefaultNodeType> EpetraMap::getNode() const { XPETRA_MONITOR("EpetraMap::getNode"); return Kokkos::DefaultNode::getDefaultNode(); } //removed &






  std::string EpetraMap::description() const {
    XPETRA_MONITOR("EpetraMap::description");

    // This implementation come from Tpetra_Map_def.hpp (without modification)
    std::ostringstream oss;
    oss << Teuchos::Describable::description();
    oss << "{getGlobalNumElements() = " << getGlobalNumElements()
        << ", getNodeNumElements() = " << getNodeNumElements()
        << ", isContiguous() = " << isContiguous()
        << ", isDistributed() = " << isDistributed()
        << "}";
    return oss.str();
  }

  void EpetraMap::describe( Teuchos::FancyOStream &out, const Teuchos::EVerbosityLevel verbLevel) const {
    XPETRA_MONITOR("EpetraMap::describe");

    const Teuchos::RCP<const Teuchos::Comm<int> > comm_ = getComm();

    // This implementation come from Tpetra_Map_def.hpp (without modification)
    using std::endl;
    using std::setw;
    using Teuchos::VERB_DEFAULT;
    using Teuchos::VERB_NONE;
    using Teuchos::VERB_LOW;
    using Teuchos::VERB_MEDIUM;
    using Teuchos::VERB_HIGH;
    using Teuchos::VERB_EXTREME;

    const size_t nME = getNodeNumElements();
    Teuchos::ArrayView<const int> myEntries = getNodeElementList();
    int myImageID = comm_->getRank();
    int numImages = comm_->getSize();

    Teuchos::EVerbosityLevel vl = verbLevel;
    if (vl == VERB_DEFAULT) vl = VERB_LOW;

    size_t width = 1;
    for (size_t dec=10; dec<getGlobalNumElements(); dec *= 10) {
      ++width;
    }
    width = std::max<size_t>(width,12) + 2;

    Teuchos::OSTab tab(out);

    if (vl == VERB_NONE) {
      // do nothing
    }
    else if (vl == VERB_LOW) {
      out << this->description() << endl;
    }
    else {  // MEDIUM, HIGH or EXTREME
      for (int imageCtr = 0; imageCtr < numImages; ++imageCtr) {
        if (myImageID == imageCtr) {
          if (myImageID == 0) { // this is the root node (only output this info once)
            out << endl
                << "Number of Global Entries = " << getGlobalNumElements()  << endl
                << "Maximum of all GIDs      = " << getMaxAllGlobalIndex() << endl
                << "Minimum of all GIDs      = " << getMinAllGlobalIndex() << endl
                << "Index Base               = " << getIndexBase()         << endl;
          }
          out << endl;
          if (vl == VERB_HIGH || vl == VERB_EXTREME) {
            out << "Number of Local Elements   = " << nME           << endl
                << "Maximum of my GIDs         = " << getMaxGlobalIndex() << endl
                << "Minimum of my GIDs         = " << getMinGlobalIndex() << endl;
            out << endl;
          }
          if (vl == VERB_EXTREME) {
            out << std::setw(width) << "Node ID"
                << std::setw(width) << "Local Index"
                << std::setw(width) << "Global Index"
                << endl;
            for (size_t i=0; i < nME; i++) {
              out << std::setw(width) << myImageID
                  << std::setw(width) << i
                  << std::setw(width) << myEntries[i]
                  << endl;
            }
            out << std::flush;
          }
        }
        // Do a few global ops to give I/O a chance to complete
        comm_->barrier();
        comm_->barrier();
        comm_->barrier();
      }
    }
  }





  const Epetra_Map & toEpetra(const Map<int,int> &map) {
    // TODO: throw exception
    const EpetraMap & epetraMap = dynamic_cast<const EpetraMap &>(map);
    return epetraMap.getEpetra_Map();
  }

  const Epetra_Map & toEpetra(const RCP< const Map<int, int> > &map) {
    XPETRA_RCP_DYNAMIC_CAST(const EpetraMap, map, epetraMap, "toEpetra");
    return epetraMap->getEpetra_Map();
  }

//   const RCP< const Map<int, int> > toXpetra(const RCP< const Epetra_Map > &map) {
//     return rcp( new EpetraMap(map) );
//   }

  const RCP< const Map<int, int> > toXpetra(const Epetra_BlockMap &map) {
    RCP<const Epetra_BlockMap> m = rcp(new Epetra_BlockMap(map));
    return rcp( new EpetraMap(m) );
  }
  //

}

#endif
