// @HEADER
// ***********************************************************************
//
//          Tpetra: Templated Linear Algebra Services Package
//                 Copyright (2008) Sandia Corporation
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
// Questions? Contact Michael A. Heroux (maherou@sandia.gov)
//
// ************************************************************************
// @HEADER

#ifndef TPETRA_CRSGRAPH_DECL_HPP
#define TPETRA_CRSGRAPH_DECL_HPP

#include <Teuchos_CompileTimeAssert.hpp>
#include <Teuchos_Describable.hpp>
#include <Teuchos_ParameterListAcceptorDefaultBase.hpp>

#include <Kokkos_DefaultNode.hpp>
#include <Kokkos_DefaultKernels.hpp>

#include "Tpetra_ConfigDefs.hpp"
#include "Tpetra_RowGraph.hpp"
#include "Tpetra_DistObject.hpp"
#include "Tpetra_Exceptions.hpp"


namespace Tpetra {

#ifndef DOXYGEN_SHOULD_SKIP_THIS
  // forward declaration
  template <class S, class LO, class GO, class N, class SpMatOps>
  class CrsMatrix;
#endif

  /// \struct RowInfo
  /// \brief Allocation information for a locally owned row in a
  ///   CrsGraph or CrsMatrix
  ///
  /// A RowInfo instance identifies a locally owned row uniquely by
  /// its local index, and contains other information useful for
  /// inserting entries into the row.  It is the return value of
  /// CrsGraph's getRowInfo() or updateAllocAndValues() methods.
  struct RowInfo {
    size_t localRow;
    size_t allocSize;
    size_t numEntries;
    size_t offset1D;
  };

  enum ELocalGlobal {
    LocalIndices,
    GlobalIndices
  };

  /** \class CrsGraph
   * \brief A distributed graph accessed by rows (adjacency lists) and stored sparsely.

   \tparam LocalOrdinal The type of local indices.  Same as the \c
     LocalOrdinal template parameter of \c Map objects used by this
     graph.  (In Epetra, this is just \c int.)  The default type is
     \c int, which should suffice for most users.  This type must be
     big enough to store the local (per process) number of rows or
     columns.

   \tparam GlobalOrdinal The type of global indices.  Same as the \c
     GlobalOrdinal template parameter of \c Map objects used by this
     graph.  (In Epetra, this is just \c int.  One advantage of
     Tpetra over Epetra is that you can use a 64-bit integer type here
     if you want to solve big problems.)  The default type is
     <tt>LocalOrdinal</tt>.  This type must be big enough to store the
     global (over all processes in the communicator) number of rows or
     columns.

   \tparam Node A class implementing on-node shared-memory parallel
     operations.  It must implement the
     \ref kokkos_node_api "Kokkos Node API."
     The default \c Node type should suffice for most users.
     The actual default type depends on your Trilinos build options.

   \tparam LocalMatOps Type implementing local sparse
     graph-(multi)vector multiply and local sparse triangular solve.
     It must implement the \ref kokkos_crs_ops "Kokkos CRS Ops API."
     The default \c LocalMatOps type should suffice for most users.
     The actual default type depends on your Trilinos build options.

   This class implements a distributed-memory parallel sparse graph.
   It provides access by rows to the elements of the graph, as if the
   local data were stored in compressed sparse row format (adjacency
   lists, in graph terms).  (Implementations are <i>not</i> required
   to store the data in this way internally.)  This class has an
   interface like that of \c Epetra_CrsGraph, but also allows
   insertion of data into nonowned rows, much like \c
   Epetra_FECrsGraph.

   \section Tpetra_CrsGraph_prereq Prerequisites

   Before reading the rest of this documentation, it helps to know
   something about the Teuchos memory management classes, in
   particular Teuchos::RCP, Teuchos::ArrayRCP, and Teuchos::ArrayView.
   You should also know a little bit about MPI (the Message Passing
   Interface for distributed-memory programming).  You won't have to
   use MPI directly to use CrsGraph, but it helps to be familiar with
   the general idea of distributed storage of data over a
   communicator.  Finally, you should read the documentation of Map.

   \section Tpetra_CrsMatrix_local_vs_global Local vs. global indices and nonlocal insertion

   Graph entries can be added using either local or global coordinates
   for the indices. The accessors isGloballyIndexed() and
   isLocallyIndexed() indicate whether the indices are currently
   stored as global or local indices. Many of the class methods are
   divided into global and local versions, which differ only in
   whether they accept/return indices in the global or local
   coordinate space. Some of these methods may only be used if the
   graph coordinates are in the appropriate coordinates.  For example,
   getGlobalRowView() returns a View to the indices in global
   coordinates; if the indices are not in global coordinates, then no
   such View can be created.

   The global/local distinction does distinguish between operation on
   the global/local graph. Almost all methods operate on the local
   graph, i.e., the rows of the graph associated with the local node,
   per the distribution specified by the row map. Access to non-local
   rows requires performing an explicit communication via the
   import/export capabilities of the CrsGraph object; see
   DistObject. However, the method insertGlobalIndices() is an
   exception to this rule, as non-local rows are allowed to be added
   via the local graph. These rows are stored in the local graph and
   communicated to the appropriate node on the next call to
   globalAssemble() or fillComplete() (the latter calls the former).
   */
  template <class LocalOrdinal,
            class GlobalOrdinal = LocalOrdinal,
            class Node = Kokkos::DefaultNode::DefaultNodeType,
            class LocalMatOps = typename Kokkos::DefaultKernels<void,LocalOrdinal,Node>::SparseOps >
  class CrsGraph :
    public RowGraph<LocalOrdinal,GlobalOrdinal,Node>,
    public DistObject<GlobalOrdinal,LocalOrdinal,GlobalOrdinal,Node>,
    public Teuchos::ParameterListAcceptorDefaultBase
  {
    template <class S, class LO, class GO, class N, class SpMatOps>
    friend class CrsMatrix;
    template <class LO2, class GO2, class N2, class SpMatOps2>
    friend class CrsGraph;

  public:
    typedef LocalOrdinal                         local_ordinal_type;
    typedef GlobalOrdinal                        global_ordinal_type;
    typedef Node                                 node_type;
    typedef Map<LocalOrdinal,GlobalOrdinal,Node> map_type;

    //! @name Constructor/Destructor Methods
    //@{

    /// \brief Constructor specifying fixed number of entries for each row.
    ///
    /// \param rowMap [in] Distribution of rows of the graph.
    ///
    /// \param maxNumEntriesPerRow [in] Maximum number of graph
    ///   entries per row.  If pftype==DynamicProfile, this is only a
    ///   hint, and you can set this to zero without affecting
    ///   correctness.  If pftype==StaticProfile, this sets the amount
    ///   of storage allocated, and you cannot exceed this number of
    ///   entries in any row.
    ///
    /// \param pftype [in] Whether to allocate storage dynamically
    ///   (DynamicProfile) or statically (StaticProfile).
    ///
    /// \param params [in/out] Optional list of parameters.  If not
    ///   null, any missing parameters will be filled in with their
    ///   default values.
    CrsGraph (const RCP<const map_type>& rowMap,
              size_t maxNumEntriesPerRow,
              ProfileType pftype = DynamicProfile,
              const RCP<ParameterList>& params = null);

    /// \brief Constructor specifying (possibly different) number of entries in each row.
    ///
    /// \param rowMap [in] Distribution of rows of the graph.
    ///
    /// \param NumEntriesPerRowToAlloc [in] Maximum number of graph
    ///   entries to allocate for each row.  If
    ///   pftype==DynamicProfile, this is only a hint.  If
    ///   pftype==StaticProfile, this sets the amount of storage
    ///   allocated, and you cannot exceed the allocated number of
    ///   entries for any row.
    ///
    /// \param pftype [in] Whether to allocate storage dynamically
    ///   (DynamicProfile) or statically (StaticProfile).
    ///
    /// \param params [in/out] Optional list of parameters.  If not
    ///   null, any missing parameters will be filled in with their
    ///   default values.
    CrsGraph (const RCP<const Map<LocalOrdinal,GlobalOrdinal,Node> >& rowMap,
              const ArrayRCP<const size_t>& NumEntriesPerRowToAlloc,
              ProfileType pftype = DynamicProfile,
              const RCP<ParameterList>& params = null);

    /// \brief Constructor specifying column Map and fixed number of entries for each row.
    ///
    /// \param rowMap [in] Distribution of rows of the graph.
    ///
    /// \param colMap [in] Distribution of columns of the graph.
    ///
    /// \param maxNumEntriesPerRow [in] Maximum number of graph
    ///   entries per row.  If pftype==DynamicProfile, this is only a
    ///   hint, and you can set this to zero without affecting
    ///   correctness.  If pftype==StaticProfile, this sets the amount
    ///   of storage allocated, and you cannot exceed this number of
    ///   entries in any row.
    ///
    /// \param pftype [in] Whether to allocate storage dynamically
    ///   (DynamicProfile) or statically (StaticProfile).
    ///
    /// \param params [in/out] Optional list of parameters.  If not
    ///   null, any missing parameters will be filled in with their
    ///   default values.
    CrsGraph (const RCP<const Map<LocalOrdinal,GlobalOrdinal,Node> >& rowMap,
              const RCP<const Map<LocalOrdinal,GlobalOrdinal,Node> >& colMap,
              size_t maxNumEntriesPerRow,
              ProfileType pftype = DynamicProfile,
              const RCP<ParameterList>& params = null);

    /// \brief Constructor specifying column Map and number of entries in each row.
    ///
    /// \param rowMap [in] Distribution of rows of the graph.
    ///
    /// \param colMap [in] Distribution of columns of the graph.
    ///
    /// \param NumEntriesPerRowToAlloc [in] Maximum number of graph
    ///   entries to allocate for each row.  If
    ///   pftype==DynamicProfile, this is only a hint.  If
    ///   pftype==StaticProfile, this sets the amount of storage
    ///   allocated, and you cannot exceed the allocated number of
    ///   entries for any row.
    ///
    /// \param pftype [in] Whether to allocate storage dynamically
    ///   (DynamicProfile) or statically (StaticProfile).
    ///
    /// \param params [in/out] Optional list of parameters.  If not
    ///   null, any missing parameters will be filled in with their
    ///   default values.
    CrsGraph (const RCP<const Map<LocalOrdinal,GlobalOrdinal,Node> >& rowMap,
              const RCP<const Map<LocalOrdinal,GlobalOrdinal,Node> >& colMap,
              const ArrayRCP<const size_t> &NumEntriesPerRowToAlloc,
              ProfileType pftype = DynamicProfile,
              const RCP<ParameterList>& params = null);

    /// \brief Constructor specifying column Map and arrays containing the graph in sorted, local ids.
    ///
    ///
    /// \param rowMap [in] Distribution of rows of the graph.
    ///
    /// \param colMap [in] Distribution of columns of the graph.
    ///
    /// \param rowPointers [in] The beginning of each row in the graph,
    ///   as in a CSR "rowptr" array.  The length of this vector should be
    ///   equal to the number of rows in the graph, plus one.  This last
    ///   entry should store the nunber of nonzeros in the graph.
    ///
    /// \param columnIndices [in] The local indices of the columns,
    ///   as in a CSR "colind" array.  The length of this vector
    ///   should be equal to the number of unknowns in the graph.
    ///
    /// \param params [in/out] Optional list of parameters.  If not
    ///   null, any missing parameters will be filled in with their
    ///   default values.
    CrsGraph (const RCP<const Map<LocalOrdinal,GlobalOrdinal,Node> >& rowMap,
              const RCP<const Map<LocalOrdinal,GlobalOrdinal,Node> >& colMap,
              const ArrayRCP<size_t> & rowPointers,
              const ArrayRCP<LocalOrdinal> & columnIndices,
              const RCP<ParameterList>& params = null);


    /// \brief Create a cloned CrsGraph for a different Node type.
    ///
    /// This method creates a new CrsGraph on a specified Kokkos Node
    /// type, with all of the entries of this CrsGraph object.
    ///
    /// \param node2 [in] Kokkos Node instance for constructing the
    ///   clone CrsGraph and its constituent objects.
    ///
    /// \param params [in/out] Optional list of parameters. If not
    ///   null, any missing parameters will be filled in with their
    ///   default values.  See the list below for valid options.
    ///
    /// Parameters accepted by this method:
    /// - "Static profile clone" [bool, default: true] If \c true,
    ///   creates the clone with a static allocation profile. If
    ///   false, a dynamic allocation profile is used.
    /// - "Locally indexed clone" [bool] If \c true, fills clone
    ///   using this graph's column map and local indices (requires
    ///   that this graph have a column map.) If false, fills clone
    ///   using global indices and does not provide a column map. By
    ///   default, will use local indices only if this graph is using
    ///   local indices.
    /// - "fillComplete clone" [boolean, default: true] If \c true,
    ///   calls fillComplete() on the cloned CrsGraph object, with
    ///   parameters from \c params sublist "CrsGraph". The domain map
    ///   and range maps passed to fillComplete() are those of the map
    ///   being cloned, if they exist. Otherwise, the row map is used.
    template<class Node2>
    RCP<CrsGraph<LocalOrdinal, GlobalOrdinal, Node2,
                 typename Kokkos::DefaultKernels<void, LocalOrdinal, Node2>::SparseOps> >
    clone (const Teuchos::RCP<Node2> &node2,
           const Teuchos::RCP<Teuchos::ParameterList> &params = null) const
    {
      using Teuchos::arcp;
      using Teuchos::ArrayRCP;
      using Teuchos::RCP;
      using Teuchos::ParameterList;
      using std::endl;
      typedef CrsGraph<LocalOrdinal, GlobalOrdinal, Node2,
        typename Kokkos::DefaultKernels<void,LocalOrdinal,Node2>::SparseOps> CrsGraph2;
      typedef Map<LocalOrdinal, GlobalOrdinal, Node2> Map2;
      const char tfecfFuncName[] = "clone()";

      bool fillCompleteClone  = true;
      bool useLocalIndices    = hasColMap();
      ProfileType pftype = StaticProfile;
      if (! params.is_null ()) {
        fillCompleteClone = params->get ("fillComplete clone", fillCompleteClone);
        useLocalIndices = params->get ("Locally indexed clone", useLocalIndices);
        if (params->get ("Static profile clone",true) == false) {
          pftype = DynamicProfile;
        }
      }

      TEUCHOS_TEST_FOR_EXCEPTION_CLASS_FUNC(
        hasColMap() == false && useLocalIndices == true,
        std::runtime_error,
        ": You asked clone() to use local indices (by setting the \"Locally "
        "indexed clone\" parameter to true), but the source graph does not "
        "yet have a column Map, so this is impossible.");

      RCP<const Map2> clonedRowMap = rowMap_->template clone<Node2> (node2);
      RCP<CrsGraph2> clonedGraph;
      ArrayRCP<const size_t> numEntries;
      size_t numEntriesForAll = 0;
      if (indicesAreAllocated() == false) {
        if (numAllocPerRow_ != null) {
          numEntries = numAllocPerRow_;
        }
        else {
          numEntriesForAll = numAllocForAllRows_;
        }
      }
      else if (numRowEntries_ != null) {
        numEntries = numRowEntries_;
      }
      else if (nodeNumAllocated_ == 0) {
        numEntriesForAll = 0;
      }
      else {
        // left with the case that we have optimized storage. in this case, we have to construct a list of row sizes.
        TEUCHOS_TEST_FOR_EXCEPTION_CLASS_FUNC(
          getProfileType() != StaticProfile, std::logic_error,
          "Internal logic error. Please report this to Tpetra team.");
        const size_t numRows = getNodeNumRows();
        numEntriesForAll = 0;
        ArrayRCP<size_t> numEnt;
        if (numRows) {
          numEnt = arcp<size_t> (numRows);
        }
        for (size_t i=0; i<numRows; ++i) {
          numEnt[i] = rowPtrs_[i+1] - rowPtrs_[i];
        }
        numEntries = numEnt;
      }

      RCP<ParameterList> graphparams = sublist(params,"CrsGraph");
      if (useLocalIndices) {
        RCP<const Map2> clonedColMap = colMap_->template clone<Node2>(node2);
        if (numEntries == null) {
          clonedGraph = rcp (new CrsGraph2 (clonedRowMap, clonedColMap, numEntriesForAll, pftype, graphparams));
        }
        else {
          clonedGraph = rcp (new CrsGraph2 (clonedRowMap, clonedColMap, numEntries, pftype, graphparams));
        }
      }
      else {
        if (numEntries.is_null ()) {
          clonedGraph = rcp (new CrsGraph2 (clonedRowMap, numEntriesForAll, pftype, graphparams));
        }
        else {
          clonedGraph = rcp (new CrsGraph2 (clonedRowMap, numEntries, pftype, graphparams));
        }
      }
      // done with these
      numEntries = null;
      numEntriesForAll = 0;

      if (useLocalIndices) {
        clonedGraph->allocateIndices(LocalIndices);
        if (this->isLocallyIndexed ()) {
          ArrayView<const LocalOrdinal> linds;
          for (LocalOrdinal lrow = rowMap_->getMinLocalIndex ();
               lrow <= rowMap_->getMaxLocalIndex ();
               ++lrow) {
            this->getLocalRowView (lrow, linds);
            if (linds.size ()) {
              clonedGraph->insertLocalIndices (lrow, linds);
            }
          }
        }
        else { // this->isGloballyIndexed()
          Array<LocalOrdinal> linds;
          for (LocalOrdinal lrow =  rowMap_->getMinLocalIndex();
               lrow <= rowMap_->getMaxLocalIndex();
               ++lrow) {
            size_t theNumEntries;
            linds.resize (this->getNumEntriesInLocalRow (lrow));
            this->getLocalRowCopy (rowMap_->getGlobalElement (lrow), linds (), theNumEntries);
            if (theNumEntries) {
              clonedGraph->insertLocalIndices (lrow, linds (0, theNumEntries));
            }
          }
        }
      }
      else { /* useGlobalIndices */
        clonedGraph->allocateIndices (GlobalIndices);
        if (this->isGloballyIndexed ()) {
          ArrayView<const GlobalOrdinal> ginds;
          for (GlobalOrdinal grow =  rowMap_->getMinGlobalIndex();
               grow <= rowMap_->getMaxGlobalIndex();
               ++grow) {
            this->getGlobalRowView (grow, ginds);
            if (ginds.size ()) {
              clonedGraph->insertGlobalIndices (grow, ginds);
            }
          }
        }
        else { // this->isLocallyIndexed()
          Array<GlobalOrdinal> ginds;
          for (GlobalOrdinal grow =  rowMap_->getMinGlobalIndex();
               grow <= rowMap_->getMaxGlobalIndex();
               ++grow) {
            size_t theNumEntries;
            ginds.resize (this->getNumEntriesInGlobalRow (grow));
            this->getGlobalRowCopy (grow, ginds (), theNumEntries);
            if (theNumEntries) {
              clonedGraph->insertGlobalIndices (grow, ginds (0,theNumEntries));
            }
          }
        }
      }

      if (fillCompleteClone) {
        RCP<ParameterList> fillparams = sublist (params, "fillComplete");
        try {
          RCP<const Map2> clonedRangeMap;
          RCP<const Map2> clonedDomainMap;
          if (rangeMap_ != null && rangeMap_ != rowMap_) {
            clonedRangeMap  = rangeMap_->template clone<Node2>(node2);
          }
          else {
            clonedRangeMap = clonedRowMap;
          }
          if (domainMap_ != null && domainMap_ != rowMap_) {
            clonedDomainMap = domainMap_->template clone<Node2>(node2);
          }
          else {
            clonedDomainMap = clonedRowMap;
          }
          clonedGraph->fillComplete (clonedDomainMap, clonedRangeMap, fillparams);
        }
        catch (std::exception &e) {
          const bool caughtExceptionOnClone = true;
          TEUCHOS_TEST_FOR_EXCEPTION_CLASS_FUNC(
            caughtExceptionOnClone, std::runtime_error,
            ": Caught the following exception while calling fillComplete() on "
            "clone of type" << endl << Teuchos::typeName (*clonedGraph)
            << endl << ":" << endl << e.what() << endl);
        }
      }
      return clonedGraph;
    }

    //! Destructor.
    virtual ~CrsGraph();

    //@}
    //! @name Implementation of Teuchos::ParameterListAcceptor
    //@{

    //! Set the given list of parameters (must be nonnull).
    void setParameterList (const RCP<ParameterList>& params);

    //! Default parameter list suitable for validation.
    RCP<const ParameterList> getValidParameters () const;

    //@}
    //! @name Insertion/Removal Methods
    //@{

    /// \brief Insert global indices into the graph.
    ///
    /// \pre \c globalRow is a valid index in the row Map.  It need
    ///   not be owned by the calling process.
    /// \pre <tt>isLocallyIndexed() == false</tt>
    /// \pre <tt>isStorageOptimized() == false</tt>
    ///
    /// \post <tt>indicesAreAllocated() == true</tt>
    /// \post <tt>isGloballyIndexed() == true</tt>
    ///
    /// If \c globalRow does not belong to the graph on this process,
    /// then it will be communicated to the appropriate process when
    /// globalAssemble() is called.  (That method will be called
    /// automatically during the next call to fillComplete().)
    /// Otherwise, the entries will be inserted into the part of the
    /// graph owned by the calling process.
    ///
    /// If the graph row already contains entries at the indices
    /// corresponding to values in \c indices, then the redundant
    /// indices will be eliminated.  This may happen either at
    /// insertion or during the next call to fillComplete().
    void
    insertGlobalIndices (GlobalOrdinal globalRow,
                         const ArrayView<const GlobalOrdinal>& indices);

    //! Insert local indices into the graph.
    /**
       \pre \c localRow is a local row belonging to the graph on this process.
       \pre <tt>isGloballyIndexed() == false</tt>
       \pre <tt>isStorageOptimized() == false</tt>
       \pre <tt>hasColMap() == true</tt>

       \post <tt>indicesAreAllocated() == true</tt>
       \post <tt>isLocallyIndexed() == true</tt>

       \note If the graph row already contains entries at the indices
         corresponding to values in \c indices, then the redundant
         indices will be eliminated; this may happen at insertion or
         during the next call to fillComplete().
    */
    void
    insertLocalIndices (const LocalOrdinal localRow,
                        const ArrayView<const LocalOrdinal> &indices);

    //! Remove all graph indices from the specified local row.
    /**
       \pre \c localRow is a local row of this graph.
       \pre <tt>isGloballyIndexed() == false</tt>
       \pre <tt>isStorageOptimized() == false</tt>

       \post <tt>getNumEntriesInLocalRow(localRow) == 0</tt>
       \post <tt>indicesAreAllocated() == true</tt>
       \post <tt>isLocallyIndexed() == true</tt>
    */
    void removeLocalIndices (LocalOrdinal localRow);

    //@}
    //! @name Transformational Methods
    /**
       Each of the methods in this group is a global collective. It is
       necessary to call these mehtods on all nodes participating in the
       communicator associated with this graph.
    */
    //@{

    /// \brief Communicate non-local contributions to other processes.
    ///
    /// This method is called automatically by fillComplete().
    /// Most users do not need to call this themselves,
    /// though we do permit this.
    void globalAssemble ();

    /*! Resume fill operations.
      After calling fillComplete(), resumeFill() must be called before initiating any changes to the graph.

      resumeFill() may be called repeatedly.

      \post  <tt>isFillActive() == true<tt>
      \post  <tt>isFillComplete() == false<tt>
    */
    void resumeFill (const RCP<ParameterList> &params = null);

    /*! \brief Signal that data entry is complete, specifying domain and range maps.

      Off-process indices are distributed (via globalAssemble()),
      indices are sorted, redundant indices are eliminated, and
      global indices are transformed to local indices.

      \pre <tt>isFillActive() == true<tt>
      \pre <tt>isFillComplete()() == false<tt>

      \post <tt>isFillActive() == false<tt>
      \post <tt>isFillComplete() == true<tt>

      Parameters:
      - "Optimize Storage" (\c bool): Default is false.  If true,
        then isStorageOptimized() returns true after fillComplete
        finishes.  See isStorageOptimized() for consequences.
    */
    void
    fillComplete (const RCP<const Map<LocalOrdinal,GlobalOrdinal,Node> > &domainMap,
                  const RCP<const Map<LocalOrdinal,GlobalOrdinal,Node> > &rangeMap,
                  const RCP<ParameterList> &params = null);

    /*! \brief Signal that data entry is complete.

      Off-node entries are distributed (via globalAssemble()), repeated entries are summed, and global indices are transformed to local indices.

      \note This method calls fillComplete( getRowMap(), getRowMap(), os ). See parameter options there.
    */
    void fillComplete (const RCP<ParameterList> &params = null);

    /// \brief Perform a fillComplete on a graph that already has data, via setAllIndices().
    ///
    /// The graph must already have filled local 1-D storage (lclInds1D_
    /// and rowPtrs_).  If the graph has been constructed in any other way,
    /// this method will throw an exception.  This routine is needed to
    /// support other Trilinos packages and should not be called by ordinary
    /// users.
    ///
    /// \warning This method is intended for expert developer use
    ///   only, and should never be called by user code.
    void
    expertStaticFillComplete (const RCP<const Map<LocalOrdinal,GlobalOrdinal,Node> > & domainMap,
                              const RCP<const Map<LocalOrdinal,GlobalOrdinal,Node> > & rangeMap,
                              const RCP<const Import<LocalOrdinal,GlobalOrdinal,Node> > &importer=Teuchos::null,
                              const RCP<const Export<LocalOrdinal,GlobalOrdinal,Node> > &exporter=Teuchos::null,
                              const RCP<ParameterList> &params=Teuchos::null);
    //@}
    //! @name Methods implementing RowGraph.
    //@{

    //! Returns the communicator.
    const RCP<const Comm<int> > & getComm() const;

    //! Returns the underlying node.
    RCP<Node> getNode() const;

    //! Returns the Map that describes the row distribution in this graph.
    const RCP<const Map<LocalOrdinal,GlobalOrdinal,Node> > & getRowMap() const;

    //! \brief Returns the Map that describes the column distribution in this graph.
    const RCP<const Map<LocalOrdinal,GlobalOrdinal,Node> > & getColMap() const;

    //! Returns the Map associated with the domain of this graph.
    const RCP<const Map<LocalOrdinal,GlobalOrdinal,Node> > & getDomainMap() const;

    //! Returns the Map associated with the domain of this graph.
    const RCP<const Map<LocalOrdinal,GlobalOrdinal,Node> > & getRangeMap() const;

    //! Returns the importer associated with this graph.
    RCP<const Import<LocalOrdinal,GlobalOrdinal,Node> > getImporter() const;

    //! Returns the exporter associated with this graph.
    RCP<const Export<LocalOrdinal,GlobalOrdinal,Node> > getExporter() const;

    //! Returns the number of global rows in the graph.
    /** Undefined if isFillActive().
     */
    global_size_t getGlobalNumRows() const;

    //! \brief Returns the number of global columns in the graph.
    /** Returns the number of entries in the domain map of the matrix.
        Undefined if isFillActive().
    */
    global_size_t getGlobalNumCols() const;

    //! Returns the number of graph rows owned on the calling node.
    size_t getNodeNumRows() const;

    //! Returns the number of columns connected to the locally owned rows of this graph.
    /** Throws std::runtime_error if <tt>hasColMap() == false</tt>
     */
    size_t getNodeNumCols() const;

    //! Returns the index base for global indices for this graph.
    GlobalOrdinal getIndexBase() const;

    //! Returns the global number of entries in the graph.
    /** Undefined if isFillActive().
     */
    global_size_t getGlobalNumEntries() const;

    //! Returns the local number of entries in the graph.
    size_t getNodeNumEntries() const;

    //! \brief Returns the current number of entries on this node in the specified global row.
    /*! Returns OrdinalTraits<size_t>::invalid() if the specified global row does not belong to this graph. */
    size_t getNumEntriesInGlobalRow(GlobalOrdinal globalRow) const;

    //! Returns the current number of entries on this node in the specified local row.
    /*! Returns OrdinalTraits<size_t>::invalid() if the specified local row is not valid for this graph. */
    size_t getNumEntriesInLocalRow(LocalOrdinal localRow) const;

    //! \brief Returns the total number of indices allocated for the graph, across all rows on this node.
    /*! This is the allocation available to the user. Actual allocation may be larger, for example, after
      calling fillComplete(), and thus this does not necessarily reflect the memory consumption of the
      this graph.

      This quantity is computed during the actual allocation. Therefore, if <tt>indicesAreAllocated() == false</tt>,
      this method returns <tt>OrdinalTraits<size_t>::invalid()</tt>.
    */
    size_t getNodeAllocationSize() const;

    //! \brief Returns the current number of allocated entries for this node in the specified global row .
    /** Throws exception std::runtime_error if the specified global row does not belong to this node. */
    size_t getNumAllocatedEntriesInGlobalRow(GlobalOrdinal globalRow) const;

    //! Returns the current number of allocated entries on this node in the specified local row.
    /** Throws exception std::runtime_error if the specified local row is not valid for this node. */
    size_t getNumAllocatedEntriesInLocalRow(LocalOrdinal localRow) const;

    //! \brief Returns the number of global diagonal entries, based on global row/column index comparisons.
    /** Undefined if isFillActive().
     */
    global_size_t getGlobalNumDiags() const;

    //! \brief Returns the number of local diagonal entries, based on global row/column index comparisons.
    /** Undefined if isFillActive().
     */
    size_t getNodeNumDiags() const;

    /// \brief Maximum number of entries in all rows over all processes.
    ///
    /// \note Undefined if isFillActive().
    ///
    /// \note This is the same as the result of a global maximum of
    ///   getNodeMaxNumRowEntries() over all processes.  That may not
    ///   necessarily mean what you think it does if some rows of the
    ///   matrix are owned by multiple processes.  In particular, some
    ///   processes might only own some of the entries in a particular
    ///   row.  This method only counts the number of entries in each
    ///   row that a process owns, not the total number of entries in
    ///   the row over all processes.
    size_t getGlobalMaxNumRowEntries() const;

    //! \brief Maximum number of entries in all rows owned by the calling process.
    /** Undefined if isFillActive().
     */
    size_t getNodeMaxNumRowEntries() const;

    /// \brief Whether the graph has a column Map.
    ///
    /// A CrsGraph has a column Map either because it was given to its
    /// constructor, or because it was constructed in fillComplete().
    /// Calling fillComplete() always makes a column Map if the graph
    /// does not already have one.
    ///
    /// A column Map lets the graph
    ///
    ///   - use local indices for storing entries in each row, and
    ///   - compute an Import from the domain Map to the column Map.
    ///
    /// The latter is mainly useful for a graph associated with a
    /// CrsMatrix.
    bool hasColMap() const;

    /// \brief Whether the graph is locally lower triangular.
    ///
    /// \pre <tt>! isFillActive()</tt>.
    ///   If fill is active, this method's behavior is undefined.
    ///
    /// \note This is entirely a local property.  That means this
    ///   method may return different results on different processes.
    bool isLowerTriangular() const;

    /// \brief Whether the graph is locally upper triangular.
    ///
    /// \pre <tt>! isFillActive()</tt>.
    ///   If fill is active, this method's behavior is undefined.
    ///
    /// \note This is entirely a local property.  That means this
    ///   method may return different results on different processes.
    bool isUpperTriangular() const;

    //! \brief If graph indices are in the local range, this function returns true. Otherwise, this function returns false. */
    bool isLocallyIndexed() const;

    //! \brief If graph indices are in the global range, this function returns true. Otherwise, this function returns false. */
    bool isGloballyIndexed() const;

    //! Returns \c true if fillComplete() has been called and the graph is in compute mode.
    bool isFillComplete() const;

    //! Returns \c true if resumeFill() has been called and the graph is in edit mode.
    bool isFillActive() const;

    //! Indicates whether the graph indices in all rows are known to be sorted.
    /** A fill-complete graph is always sorted, as is a newly constructed graph. A graph is sorted immediately after
        calling resumeFill(), but any changes to the graph may result in the sorting status becoming unknown (and therefore, presumed unsorted.)
    */
    bool isSorted() const;

    //! \brief Returns \c true if storage has been optimized.
    /**
       Optimized storage means that the allocation of each row is equal to the
       number of entries. The effect is that a pass through the matrix, i.e.,
       during a mat-vec, requires minimal memory traffic. One limitation of
       optimized storage is that no new indices can be added to the graph.
    */
    bool isStorageOptimized() const;

    //! Returns \c true if the graph was allocated with static data structures.
    ProfileType getProfileType() const;

    //! Extract a list of elements in a specified global row of the graph. Put into pre-allocated storage.
    /*!
      \param LocalRow - (In) Global row number for which indices are desired.
      \param Indices - (Out) Global column indices corresponding to values.
      \param NumIndices - (Out) Number of indices.

      Note: A std::runtime_error exception is thrown indices is not large enough to hold the column indices associated
      with row \c GlobalRow. If \c GlobalRow does not belong to this node, then \c indices is unchanged and \c NumIndices is
      returned as OrdinalTraits<size_t>::invalid().
    */
    void getGlobalRowCopy(GlobalOrdinal GlobalRow,
                          const ArrayView<GlobalOrdinal> &Indices,
                          size_t &NumIndices
                          ) const;

    //! Extract a list of elements in a specified local row of the graph. Put into storage allocated by calling routine.
    /*!
      \param LocalRow - (In) Local row number for which indices are desired.
      \param Indices - (Out) Local column indices corresponding to values.
      \param NumIndices - (Out) Number of indices.

      Note: A std::runtime_error exception is thrown indices is not large enough to hold the column indices associated
      with row \c LocalRow. If \c LocalRow is not valid for this node, then \c indices is unchanged and \c NumIndices is
      returned as OrdinalTraits<size_t>::invalid().

      \pre <tt>isLocallyIndexed()==true</tt> or <tt>hasColMap() == true</tt>
    */
    void getLocalRowCopy(LocalOrdinal LocalRow,
                         const ArrayView<LocalOrdinal> &indices,
                         size_t &NumIndices
                         ) const;

    //! Extract a const, non-persisting view of global indices in a specified row of the graph.
    /*!
      \param GlobalRow - (In) Global row number for which indices are desired.
      \param Indices   - (Out) Global column indices corresponding to values.
      \pre <tt>isLocallyIndexed() == false</tt>
      \post <tt>indices.size() == getNumEntriesInGlobalRow(GlobalRow)</tt>

      Note: If \c GlobalRow does not belong to this node, then \c indices is set to null.
    */
    void getGlobalRowView(GlobalOrdinal GlobalRow, ArrayView<const GlobalOrdinal> &Indices) const;

    //! Extract a const, non-persisting view of local indices in a specified row of the graph.
    /*!
      \param LocalRow - (In) Local row number for which indices are desired.
      \param Indices  - (Out) Global column indices corresponding to values.
      \pre <tt>isGloballyIndexed() == false</tt>
      \post <tt>indices.size() == getNumEntriesInLocalRow(LocalRow)</tt>

      Note: If \c LocalRow does not belong to this node, then \c indices is set to null.
    */
    void getLocalRowView(LocalOrdinal LocalRow, ArrayView<const LocalOrdinal> &indices) const;

    //@}
    //! @name Overridden from Teuchos::Describable
    //@{

    /** \brief Return a simple one-line description of this object. */
    std::string description() const;

    /** \brief Print the object with some verbosity level to an FancyOStream object. */
    void describe(Teuchos::FancyOStream &out, const Teuchos::EVerbosityLevel verbLevel=Teuchos::Describable::verbLevel_default) const;

    //@}
    //! @name Implementation of DistObject
    //@{

    virtual bool 
    checkSizes (const SrcDistObject& source);

    virtual void 
    copyAndPermute (const SrcDistObject& source,
		    size_t numSameIDs,
		    const Teuchos::ArrayView<const LocalOrdinal> &permuteToLIDs,
		    const Teuchos::ArrayView<const LocalOrdinal> &permuteFromLIDs);

    virtual void
    packAndPrepare (const SrcDistObject& source,
		    const Teuchos::ArrayView<const LocalOrdinal> &exportLIDs,
		    Teuchos::Array<GlobalOrdinal> &exports,
		    const Teuchos::ArrayView<size_t> & numPacketsPerLID,
		    size_t& constantNumPackets,
		    Distributor &distor);

    virtual void 
    unpackAndCombine (const Teuchos::ArrayView<const LocalOrdinal> &importLIDs,
		      const Teuchos::ArrayView<const GlobalOrdinal> &imports,
		      const Teuchos::ArrayView<size_t> &numPacketsPerLID,
		      size_t constantNumPackets,
		      Distributor &distor,
		      CombineMode CM);
    //@}
    //! \name Advanced methods, at increased risk of deprecation.
    //@{

    /// \brief Set the graph's data directly, using 1-D storage.
    ///
    /// \pre <tt>hasColMap() == true</tt>
    /// \pre <tt>rowPointers.size() != getNodeNumRows()+1</tt>
    /// \pre No insert routines have been called.
    ///
    /// \warning This method is intended for expert developer use
    ///   only, and should never be called by user code.
    void
    setAllIndices (const ArrayRCP<size_t> & rowPointers,
                   const ArrayRCP<LocalOrdinal> & columnIndices);

    //! Get an ArrayRCP of the row-offsets.
    /*!  The returned buffer exists in host-memory. This method may return Teuchos::null
      if "Delete Row Pointers" was \c true on fillComplete().
    */
    ArrayRCP<const size_t> getNodeRowPtrs() const;

    //! Get an ArrayRCP of the packed column-indices.
    /*!  The returned buffer exists in host-memory.
     */
    ArrayRCP<const LocalOrdinal> getNodePackedIndices() const;

    /// \brief Replace the current domain Map and Import with the given parameters.
    ///
    /// \warning This method is ONLY for use by experts.
    /// \warning We make NO promises of backwards compatibility.
    ///   This method may change or disappear at any time.
    ///
    /// \pre <tt>isFillComplete() == true<tt>
    /// \pre <tt>isFillActive() == false<tt>
    /// \pre Either the given Import object is null, or the target Map
    ///   of the given Import is the same as this graph's column Map.
    /// \pre Either the given Import object is null, or the source Map
    ///    of the given Import is the same as this graph's domain Map.
    void
    replaceDomainMapAndImporter (const Teuchos::RCP<const map_type>& newDomainMap,
                                 Teuchos::RCP<const Tpetra::Import<LocalOrdinal,GlobalOrdinal,Node> >& newImporter);

    /// \brief Remove processes owning zero rows from the Maps and their communicator.
    ///
    /// \warning This method is ONLY for use by experts.  We highly
    ///   recommend using the nonmember function of the same name
    ///   defined in Tpetra_DistObject_decl.hpp.
    ///
    /// \warning We make NO promises of backwards compatibility.
    ///   This method may change or disappear at any time.
    ///
    /// \param newMap [in] This <i>must</i> be the result of calling
    ///   the removeEmptyProcesses() method on the row Map.  If it
    ///   is not, this method's behavior is undefined.  This pointer
    ///   will be null on excluded processes.
    ///
    /// This method satisfies the strong exception guarantee, as
    /// long the destructors of Export, Import, and Map do not throw
    /// exceptions.  This means that either the method returns
    /// normally (without throwing an exception), or there are no
    /// externally visible side effects.  However, this does not
    /// guarantee no deadlock when the graph's original communicator
    /// contains more than one process.  In order to prevent
    /// deadlock, you must still wrap this call in a try/catch block
    /// and do an all-reduce over all processes in the original
    /// communicator to test whether the call succeeded.  This
    /// safety measure should usually be unnecessary, since the
    /// method call should only fail on user error or failure to
    /// allocate memory.
    virtual void
    removeEmptyProcessesInPlace (const Teuchos::RCP<const Map<LocalOrdinal, GlobalOrdinal, Node> >& newMap);
    //@}

  private:
    // We forbid copy construction by declaring this method private
    // and not implementing it.
    CrsGraph (const CrsGraph<LocalOrdinal,GlobalOrdinal,Node> &Source);

    // We forbid assignment (operator=) by declaring this method
    // private and not implementing it.
    CrsGraph<LocalOrdinal,GlobalOrdinal,Node>&
    operator= (const CrsGraph<LocalOrdinal,GlobalOrdinal,Node> &rhs);

  protected:
    typedef typename LocalMatOps::template graph<LocalOrdinal,Node>::graph_type local_graph_type;

    // these structs are conveniences, to cut down on the number of
    // arguments to some of the methods below.
    struct SLocalGlobalViews {
      ArrayView<const GlobalOrdinal> ginds;
      ArrayView<const LocalOrdinal>  linds;
    };
    struct SLocalGlobalNCViews {
      ArrayView<GlobalOrdinal>       ginds;
      ArrayView<LocalOrdinal>        linds;
    };
    //
    // Allocation
    //
    bool indicesAreAllocated() const;

    void allocateIndices (ELocalGlobal lg);

    template <class T>
    ArrayRCP<T> allocateValues1D () const;
    template <class T>
    ArrayRCP<Array<T> > allocateValues2D () const;

    template <ELocalGlobal lg, class T>
    RowInfo updateAllocAndValues (RowInfo rowinfo, size_t allocSize, Array<T>& rowVals);

    //! \name Methods governing changes between global and local indices
    //@{

    /// \brief Set collectively whether the graph uses global or local indices.
    ///
    /// If at least one process has set local indices, set all the
    /// processes to use local indices.  Likewise, if at least one
    /// process has set global indices, set all the processes to use
    /// global indices.
    ///
    /// \note To developers: See this method's internal comments.
    void computeIndexState();
    void makeColMap (); //!< Make the column Map.
    void makeIndicesLocal ();
    void makeImportExport ();

    //@}
    //! \name Methods for inserting indices or transforming values
    //@{

    template<ELocalGlobal lg>
    size_t filterIndices (const SLocalGlobalNCViews &inds) const;

    template<class T>
    size_t
    filterGlobalIndicesAndValues (const ArrayView<GlobalOrdinal>& ginds,
                                  const ArrayView<T>& vals) const;
    template<class T>
    size_t
    filterLocalIndicesAndValues (const ArrayView<LocalOrdinal>& linds,
                                 const ArrayView<T>& vals) const;

    /// \brief Insert indices into the given row.
    ///
    /// \pre <tt>! (lg == LocalIndices && I == GlobalIndices)</tt>.
    ///   It does not make sense to give this method local column
    ///   indices (meaning that the graph has a column Map), yet to
    ///   ask it to store global indices.
    ///
    /// \param rowInfo [in] Result of CrsGraph's getRowInfo() or
    ///   updateAllocAndValues() methods, for the locally owned row
    ///   (whose local index is <tt>rowInfo.localRow</tt>) for which
    ///   you want to insert indices.
    ///
    /// \param newInds [in] View of the column indices to insert.  If
    ///   <tt>lg == GlobalIndices</tt>, then newInds.ginds, a
    ///   <tt>Teuchos::ArrayView<const GlobalOrdinal></tt>, contains
    ///   the (global) column indices to insert.  Otherwise, if <tt>lg
    ///   == LocalIndices</tt>, then newInds.linds, a
    ///   <tt>Teuchos::ArrayView<const LocalOrdinal></tt>, contains
    ///   the (local) column indices to insert.
    ///
    /// \param lg If <tt>lg == GlobalIndices</tt>, then the input
    ///   indices (in \c newInds) are global indices.  Otherwise, if
    ///   <tt>lg == LocalIndices</tt>, the input indices are local
    ///   indices.
    ///
    /// \param I If <tt>lg == GlobalIndices</tt>, then this method
    ///   will store the input indices as global indices.  Otherwise,
    ///   if <tt>I == LocalIndices</tt>, this method will store the
    ///   input indices as local indices.
    size_t
    insertIndices (const RowInfo& rowInfo,
                   const SLocalGlobalViews& newInds,
                   const ELocalGlobal lg,
                   const ELocalGlobal I);

    /// \brief Insert indices and their values into the given row.
    ///
    /// \tparam Scalar The type of a single value.  When this method
    ///   is called by CrsMatrix, \c Scalar corresponds to the first
    ///   template parameter of CrsMatrix.
    ///
    /// \pre <tt>! (lg == LocalIndices && I == GlobalIndices)</tt>.
    ///   It does not make sense to give this method local column
    ///   indices (meaning that the graph has a column Map), yet to
    ///   ask it to store global indices.
    ///
    /// \param rowInfo [in] Result of CrsGraph's getRowInfo() or
    ///   updateAllocAndValues() methods, for the locally owned row
    ///   (whose local index is <tt>rowInfo.localRow</tt>) for which
    ///   you want to insert indices.
    ///
    /// \param newInds [in] View of the column indices to insert.  If
    ///   <tt>lg == GlobalIndices</tt>, then newInds.ginds, a
    ///   <tt>Teuchos::ArrayView<const GlobalOrdinal></tt>, contains
    ///   the (global) column indices to insert.  Otherwise, if <tt>lg
    ///   == LocalIndices</tt>, then newInds.linds, a
    ///   <tt>Teuchos::ArrayView<const LocalOrdinal></tt>, contains
    ///   the (local) column indices to insert.
    ///
    /// \param oldRowVals [out] View of the current values.  They will
    ///   be overwritten with the new values.
    ///
    /// \param newRowVals [in] View of the new values.  They will be
    ///   copied over the old values.
    ///
    /// \param lg If <tt>lg == GlobalIndices</tt>, then the input
    ///   indices (in \c newInds) are global indices.  Otherwise, if
    ///   <tt>lg == LocalIndices</tt>, the input indices are local
    ///   indices.
    ///
    /// \param I If <tt>lg == GlobalIndices</tt>, then this method
    ///   will store the input indices as global indices.  Otherwise,
    ///   if <tt>I == LocalIndices</tt>, this method will store the
    ///   input indices as local indices.
    template<class Scalar>
    void
    insertIndicesAndValues (const RowInfo& rowInfo,
                            const SLocalGlobalViews& newInds,
                            const ArrayView<Scalar>& oldRowVals,
                            const ArrayView<const Scalar>& newRowVals,
                            const ELocalGlobal lg,
                            const ELocalGlobal I);
    void
    insertGlobalIndicesImpl (const LocalOrdinal myRow,
                             const ArrayView<const GlobalOrdinal> &indices);
    void
    insertLocalIndicesImpl (const LocalOrdinal myRow,
                            const ArrayView<const LocalOrdinal> &indices);
    //! Like insertLocalIndices(), but with column Map filtering.
    void
    insertLocalIndicesFiltered (const LocalOrdinal localRow,
                                const ArrayView<const LocalOrdinal> &indices);

    //! Like insertGlobalIndices(), but with column Map filtering.
    void
    insertGlobalIndicesFiltered (const GlobalOrdinal localRow,
                                 const ArrayView<const GlobalOrdinal> &indices);

    /// \brief Transform the given values using local indices.
    ///
    /// \param rowInfo [in] Information about a given row of the graph.
    ///
    /// \param rowVals [in/out] The values to be transformed.  They
    ///   correspond to the row indicated by rowInfo.
    ///
    /// \param inds [in] The (local) indices in the row, for which
    ///   to transform the corresponding values in rowVals.
    ///
    /// \param newVals [in] Values to use for transforming rowVals.
    ///   It's probably OK for these to alias rowVals.
    ///
    /// \param f [in] A binary function used to transform rowVals.
    ///
    /// This method transforms the values using the expression
    /// \code
    /// newVals[k] = f( rowVals[k], newVals[j] );
    /// \endcode
    /// where k is the local index corresponding to
    /// <tt>inds[j]</tt>.  It ignores elements of \c inds that are
    /// not owned by the calling process.
    template<class Scalar, class BinaryFunction>
    void
    transformLocalValues (RowInfo rowInfo,
                          const Teuchos::ArrayView<Scalar>& rowVals,
                          const Teuchos::ArrayView<const LocalOrdinal>& inds,
                          const Teuchos::ArrayView<const Scalar>& newVals,
                          BinaryFunction f) const;

    /// \brief Transform the given values using global indices.
    ///
    /// \param rowInfo [in] Information about a given row of the graph.
    ///
    /// \param rowVals [in/out] The values to be transformed.  They
    ///   correspond to the row indicated by rowInfo.
    ///
    /// \param inds [in] The (global) indices in the row, for which
    ///   to transform the corresponding values in rowVals.
    ///
    /// \param newVals [in] Values to use for transforming rowVals.
    ///   It's probably OK for these to alias rowVals.
    ///
    /// \param f [in] A binary function used to transform rowVals.
    template<class Scalar, class BinaryFunction>
    void
    transformGlobalValues (RowInfo rowInfo,
                           const Teuchos::ArrayView<Scalar>& rowVals,
                           const Teuchos::ArrayView<const GlobalOrdinal>& inds,
                           const Teuchos::ArrayView<const Scalar>& newVals,
                           BinaryFunction f) const;

    //@}
    //! \name Methods for sorting and merging column indices.
    //@{

    //! Whether duplicate column indices in each row have been merged.
    bool isMerged () const;

    /// \brief Report that we made a local modification to its structure.
    ///
    /// Call this after making a local change to the graph's
    /// structure.  Changing the structure locally invalidates the "is
    /// sorted" and "is merged" states.
    void setLocallyModified ();

    //! Sort the column indices in all the rows.
    void sortAllIndices ();

    //! Sort the column indices in the given row.
    void sortRowIndices (RowInfo rowinfo);

    /// \brief Sort the column indices and their values in the given row.
    ///
    /// \tparam Scalar The type of the values.  When calling this
    ///   method from CrsMatrix, this should be the same as the
    ///   <tt>Scalar</tt> template parameter of CrsMatrix.
    ///
    /// \param rowinfo [in] Result of getRowInfo() for the row.
    ///
    /// \param values [in/out] On input: values for the given row.  If
    ///   indices is an array of the column indices in the row, then
    ///   values and indices should have the same number of entries,
    ///   and indices[k] should be the column index corresponding to
    ///   values[k].  On output: the same values, but sorted in the
    ///   same order as the (now sorted) column indices in the row.
    template <class Scalar>
    void sortRowIndicesAndValues (RowInfo rowinfo, ArrayView<Scalar> values);

    /// \brief Merge duplicate row indices in all of the rows.
    ///
    /// \pre The graph is locally indexed:
    ///   <tt>isGloballyIndexed() == false</tt>.
    ///
    /// \pre The graph has not already been merged: <tt>isMerged()
    ///   == false</tt>.  That is, this function would normally only
    ///   be called after calling sortIndices().
    void mergeAllIndices ();

    /// \brief Merge duplicate row indices in the given row.
    ///
    /// \pre The graph is not already storage optimized:
    ///   <tt>isStorageOptimized() == false</tt>
    void mergeRowIndices (RowInfo rowinfo);


    /// \brief Merge duplicate row indices in the given row, along
    ///   with their corresponding values.
    ///
    /// This method is only called by CrsMatrix, for a CrsMatrix whose
    /// graph is this CrsGraph instance.  It is only called when the
    /// matrix owns the graph, not when the matrix was constructed
    /// with a const graph.
    ///
    /// \pre The graph is not already storage optimized:
    ///   <tt>isStorageOptimized() == false</tt>
    template<class Scalar>
    void
    mergeRowIndicesAndValues (RowInfo rowinfo,
                              const Teuchos::ArrayView<Scalar>& rowValues);
    //@}

    /// Set the domain and range Maps, and invalidate the Import
    /// and/or Export objects if necessary.
    ///
    /// If the domain Map has changed, invalidate the Import object
    /// (if there is one).  Likewise, if the range Map has changed,
    /// invalidate the Export object (if there is one).
    ///
    /// \param domainMap [in] The new domain Map
    /// \param rangeMap [in] The new range Map
    void
    setDomainRangeMaps (const RCP<const Map<LocalOrdinal,GlobalOrdinal,Node> > &domainMap,
                        const RCP<const Map<LocalOrdinal,GlobalOrdinal,Node> > &rangeMap);

    void staticAssertions() const;
    // global consts
    void clearGlobalConstants();
    void computeGlobalConstants();
    // graph data accessors
    RowInfo                         getRowInfo(size_t myRow) const;
    ArrayView<const LocalOrdinal>   getLocalView(RowInfo rowinfo) const;
    ArrayView<LocalOrdinal>         getLocalViewNonConst(RowInfo rowinfo);
    ArrayView<const GlobalOrdinal>  getGlobalView(RowInfo rowinfo) const;
    ArrayView<GlobalOrdinal>        getGlobalViewNonConst(RowInfo rowinfo);

    /// \brief Find the column offset corresponding to the given
    ///   (local) column index.
    ///
    /// The name of this method is a bit misleading.  It does not
    /// actually find the column index.  Instead, it takes a local
    /// column index \c ind, and returns the corresponding offset
    /// into the raw array of column indices (whether that be 1-D or
    /// 2-D storage).
    ///
    /// \param rowinfo [in] Result of getRowInfo() for the given row.
    /// \param ind [in] (Local) column index for which to find the offset.
    /// \param hint [in] Hint for where to find \c ind in the column
    ///   indices for the given row.  If colInds is the ArrayView of
    ///   the (local) column indices for the given row, and if
    ///   <tt>colInds[hint] == ind</tt>, then the hint is correct.
    ///   The hint is ignored if it is out of range (that is,
    ///   greater than or equal to the number of entries in the
    ///   given row).
    ///
    /// The hint optimizes for the case of calling this method
    /// several times with the same row (as it would be in
    /// transformLocalValues) when several index inputs occur in
    /// consecutive sequence.  This may occur (for example) when
    /// there are multiple degrees of freedom per mesh point, and
    /// users are handling the assignment of degrees of freedom to
    /// global indices manually (rather than letting BlockMap take
    /// care of it).  In that case, users might choose to assign the
    /// degrees of freedom for a mesh point to consecutive global
    /// indices.  Epetra implements the hint for this reason.
    ///
    /// The hint only costs two comparisons (one to check range, and
    /// the other to see if the hint was correct), and it can save
    /// searching for the indices (which may take a lot more than
    /// two comparisons).
    size_t
    findLocalIndex (RowInfo rowinfo,
                    LocalOrdinal ind,
                    size_t hint = 0) const;

    /// Find the column offset corresponding to the given (local)
    /// column index, given a view of the (local) column indices.
    ///
    /// The name of this method is a bit misleading.  It does not
    /// actually find the column index.  Instead, it takes a local
    /// column index \c ind, and returns the corresponding offset
    /// into the raw array of column indices (whether that be 1-D or
    /// 2-D storage).
    ///
    /// It is best to use this method if you plan to call it several
    /// times for the same row, like in transformLocalValues().  In
    /// that case, it amortizes the overhead of calling
    /// getLocalView().
    ///
    /// \param rowinfo [in] Result of getRowInfo() for the given row.
    /// \param ind [in] (Local) column index for which to find the offset.
    /// \param colInds [in] View of all the (local) column indices
    ///   for the given row.
    /// \param hint [in] Hint for where to find \c ind in the column
    ///   indices for the given row.  If colInds is the ArrayView of
    ///   the (local) column indices for the given row, and if
    ///   <tt>colInds[hint] == ind</tt>, then the hint is correct.
    ///   The hint is ignored if it is out of range (that is,
    ///   greater than or equal to the number of entries in the
    ///   given row).
    ///
    /// See the documentation of the three-argument version of this
    /// method for an explanation and justification of the hint.
    size_t
    findLocalIndex (RowInfo rowinfo,
                    LocalOrdinal ind,
                    ArrayView<const LocalOrdinal> colInds,
                    size_t hint = 0) const;

    /// \brief Find the column offset corresponding to the given (global) column index.
    ///
    /// The name of this method is a bit misleading.  It does not
    /// actually find the column index.  Instead, it takes a global
    /// column index \c ind, and returns the corresponding offset
    /// into the raw array of column indices (whether that be 1-D or
    /// 2-D storage).
    size_t findGlobalIndex (RowInfo rowinfo, GlobalOrdinal ind, size_t hint = 0) const;

    // local Kokkos objects
    void fillLocalGraph(const RCP<ParameterList> &params);
    const RCP<const local_graph_type> getLocalGraph() const;
    const RCP<local_graph_type> getLocalGraphNonConst();
    // debugging
    void checkInternalState() const;

    //! The Map describing the distribution of rows of the graph.
    RCP<const Map<LocalOrdinal,GlobalOrdinal,Node> > rowMap_;
    //! The Map describing the distribution of columns of the graph.
    RCP<const Map<LocalOrdinal,GlobalOrdinal,Node> > colMap_;
    //! The Map describing the range of the (matrix corresponding to the) graph.
    RCP<const Map<LocalOrdinal,GlobalOrdinal,Node> > rangeMap_;
    //! The Map describing the domain of the (matrix corresponding to the) graph.
    RCP<const Map<LocalOrdinal,GlobalOrdinal,Node> > domainMap_;

    /// \brief The Import from the domain Map to the column Map.
    ///
    /// This gets constructed by fillComplete.  It may be null if
    /// the domain Map and the column Map are the same, since no
    /// Import is necessary in that case for sparse matrix-vector
    /// multiply.
    RCP<const Import<LocalOrdinal,GlobalOrdinal,Node> > importer_;

    /// \brief The Export from the row Map to the range Map.
    ///
    /// This gets constructed by fillComplete.  It may be null if
    /// the row Map and the range Map are the same, since no Export
    /// is necessary in that case for sparse matrix-vector multiply.
    RCP<const Export<LocalOrdinal,GlobalOrdinal,Node> > exporter_;

    // local data, stored in a Kokkos::CrsGraph. only initialized after fillComplete()
    RCP<local_graph_type> lclGraph_;

    // Local and Global Counts
    // nodeNumEntries_ and nodeNumAllocated_ are required to be always consistent
    // nodeMaxNumEntries_, nodeNumDiags_ and the global quantities are computed during fillComplete() and only valid when isFillComplete()
    global_size_t globalNumEntries_, globalNumDiags_, globalMaxNumRowEntries_;
    size_t          nodeNumEntries_,   nodeNumDiags_,   nodeMaxNumRowEntries_, nodeNumAllocated_;

    //! Whether the graph was allocated with static or dynamic profile.
    ProfileType pftype_;

    /// \brief The maximum number of entries to allow in each locally owned row, per row.
    ///
    /// This is an argument to some of the graph's constructors.
    /// Either this or numAllocForAllRows_ is used, but not both.
    ///
    /// If this is not set in the constructor, it is allocated
    /// temporarily, if necessary, in allocateIndices().  In that same
    /// method, it is used to allocate the row offsets array, then
    /// discarded (set to null) unconditionally.
    ArrayRCP<const size_t> numAllocPerRow_;

    /// \brief The maximum number of entries to allow in each locally owned row.
    ///
    /// This is an argument to some of the graph's constructors.
    /// Either this or numAllocPerRow_ is used, but not both.
    size_t numAllocForAllRows_;

    // graph indices. before allocation, all are null.
    // after allocation, except during makeIndicesLocal(), one of local or global is null.
    // we will never have 1D and 2D structures being non-null
    // this is host memory
    // 1D == StaticAllocation, 2D == DynamicAllocation
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    // 1D/Static structures
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    //! lclInds1D_ are the indices for all rows
    ArrayRCP< LocalOrdinal>                     lclInds1D_;
    //! gblInds1D_ are the indices for all rows
    ArrayRCP<GlobalOrdinal>                     gblInds1D_;
    // offset to the beg entries of each row. only used for 1D (Static) allocation.
    // i.e., indices for row R are lclInds1D_[i] for i in [b,e) where b = rowPtrs_[R] and e = rowPtrs_[R+1]
    // only the first numRowEntries_[R] of these are valid
    // both of these are null for 2D (Dynamic) allocations
    // rowPtrs_ has length N+1, while numRowEntries_ has length N
    // we may delete this to save memory on fillComplete, if "Delete Row Pointers" is specified
    ArrayRCP<size_t> rowPtrs_;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    // 2D/Dynamic structures.
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    //! <tt>lclInds2D_[r]</tt> are the indices for row \c r.
    ArrayRCP<Array< LocalOrdinal> > lclInds2D_;

    //! <tt>gblInds2D_[r]</tt> are the indices for row \c r.
    ArrayRCP<Array<GlobalOrdinal> > gblInds2D_;

    /// \brief The number of local entries in each locally owned row.
    ///
    /// This is deallocated in fillComplete() if fillComplete()'s
    /// "Optimize Storage" parameter is set to \c true.
    ArrayRCP<size_t> numRowEntries_;

    bool indicesAreAllocated_;
    bool indicesAreLocal_;
    bool indicesAreGlobal_;
    bool fillComplete_;
    //! Whether the graph is locally lower triangular.
    bool lowerTriangular_;
    //! Whether the graph is locally upper triangular.
    bool upperTriangular_;
    //! Whether the graph's indices are sorted in each row, on this process.
    bool indicesAreSorted_;
    /// \brief Whether the graph's indices are non-redundant (merged)
    ///   in each row, on this process.
    bool noRedundancies_;
    //! Whether this process has computed local constants.
    bool haveLocalConstants_;
    //! Whether all processes have computed global constants.
    bool haveGlobalConstants_;

    //! Nonlocal data given to insertGlobalValues or sumIntoGlobalValues.
    std::map<GlobalOrdinal, std::deque<GlobalOrdinal> > nonlocals_;

    bool haveRowInfo_;

    inline bool hasRowInfo() const {
#ifdef HAVE_TPETRA_DEBUG
      bool actuallyHasRowInfo = true;
      if (indicesAreAllocated() && getProfileType() == StaticProfile && rowPtrs_ == null) {
        actuallyHasRowInfo = false;
      }
      TEUCHOS_TEST_FOR_EXCEPTION(
        actuallyHasRowInfo != haveRowInfo_,
        std::logic_error, "Internal logic error. Please contact Tpetra team.");
#endif // HAVE_TPETRA_DEBUG
      return haveRowInfo_;
    }
  }; // class CrsGraph

  /** \brief Non-member function to create an empty CrsGraph given a row map and a non-zero profile.

      \return A dynamically allocated (DynamicProfile) graph with specified number of nonzeros per row (defaults to zero).

      \relatesalso CrsGraph
   */
  template <class LocalOrdinal, class GlobalOrdinal, class Node>
  Teuchos::RCP<CrsGraph<LocalOrdinal,GlobalOrdinal,Node> >
  createCrsGraph (const Teuchos::RCP<const Map<LocalOrdinal,GlobalOrdinal,Node> > &map,
                   size_t maxNumEntriesPerRow = 0,
                   const Teuchos::RCP<Teuchos::ParameterList>& params = Teuchos::null)
  {
    using Teuchos::rcp;
    typedef CrsGraph<LocalOrdinal, GlobalOrdinal, Node> graph_type;
    return rcp (new graph_type (map, maxNumEntriesPerRow, DynamicProfile, params));
  }


} // namespace Tpetra

#endif
