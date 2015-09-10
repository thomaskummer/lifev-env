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

#ifndef TPETRA_MAP_DECL_HPP
#define TPETRA_MAP_DECL_HPP

#include <Kokkos_DefaultNode.hpp>
#include <Teuchos_Describable.hpp>

// enums and defines
#include "Tpetra_ConfigDefs.hpp"

// mfh 27 Apr 2013: If HAVE_TPETRA_FIXED_HASH_TABLE is defined (which
// it is by default), then Map will used the fixed-structure hash
// table variant for global-to-local index lookups.  Otherwise, it
// will use the dynamic-structure hash table variant.

#ifndef HAVE_TPETRA_FIXED_HASH_TABLE
#  define HAVE_TPETRA_FIXED_HASH_TABLE 1
#endif // HAVE_TPETRA_FIXED_HASH_TABLE

/// \file Tpetra_Map_decl.hpp
/// \brief Declarations for the Tpetra::Map class and related nonmember constructors.
///
namespace Tpetra {

#ifndef DOXYGEN_SHOULD_SKIP_THIS
  // Forward declaration of Directory.
  template <class LO, class GO, class N> class Directory;

#  ifdef HAVE_TPETRA_FIXED_HASH_TABLE
  namespace Details {
    template<class GlobalOrdinal, class LocalOrdinal>
    class FixedHashTable;
  } // namespace Details
#  else
  namespace Details {
    template<class GlobalOrdinal, class LocalOrdinal>
    class HashTable;
  } // namespace Details
#  endif // HAVE_TPETRA_FIXED_HASH_TABLE
#endif // DOXYGEN_SHOULD_SKIP_THIS

  /// \class Map
  /// \brief Describes a parallel distribution of objects over processes.
  ///
  /// \tparam LocalOrdinal The type of local indices.  Should be an
  ///   integer, and generally should be signed.  A good model of \c
  ///   LocalOrdinal is \c int.  (In Epetra, this is always just \c
  ///   int.)
  ///
  /// \tparam GlobalOrdinal The type of global indices.  Should be an
  ///   integer, and generally should be signed.  Also,
  ///   <tt>sizeof(GlobalOrdinal)</tt> should be greater than to equal
  ///   to <tt>sizeof(LocalOrdinal)</tt>.  For example, if \c
  ///   LocalOrdinal is \c int, good models of \c GlobalOrdinal are \c
  ///   int, \c long, <tt>long long</tt> (if the configure-time option
  ///   Teuchos_ENABLE_LONG_LONG was set), or \c ptrdiff_t.
  ///
  /// \tparam Node A class implementing on-node shared-memory parallel
  ///   operations.  It must implement the
  ///   \ref kokkos_node_api "Kokkos Node API."
  ///   The default \c Node type should suffice for most users.
  ///   The actual default type depends on your Trilinos build options.
  ///
  /// This class describes a distribution of data elements over one or
  /// more processes in a communicator.  Each element has a global
  /// index (of type \c GlobalOrdinal) uniquely associated to it.
  /// Each global index in the Map is "owned" by one or more processes
  /// in the Map's communicator.  The user gets to decide what an
  /// "element" means; examples include a row or column of a sparse
  /// matrix (as in CrsMatrix), or a row of one or more vectors (as in
  /// MultiVector).
  ///
  /// \section Kokkos_Map_prereq Prerequisites
  ///
  /// Before reading the rest of this documentation, it helps to know
  /// something about the Teuchos memory management classes, in
  /// particular Teuchos::RCP, Teuchos::ArrayRCP, and
  /// Teuchos::ArrayView.  You should also know a little bit about MPI
  /// (the Message Passing Interface for distributed-memory
  /// programming).  You won't have to use MPI directly to use Map,
  /// but it helps to be familiar with the general idea of distributed
  /// storage of data over a communicator.
  ///
  /// \section Tpetra_Map_concepts Map concepts
  ///
  /// \subsection Tpetra_Map_local_vs_global Local and global indices
  ///
  /// The distinction between local and global indices and types might
  /// confuse new Tpetra users.  <i>Global</i> indices represent the
  /// elements of a distributed object (such as rows or columns of a
  /// CrsMatrix, or rows of a MultiVector) uniquely over the entire
  /// object, which may be distributed over multiple processes.
  /// <i>Local</i> indices are local to the process that owns them.
  /// If global index G is owned by process P, then there is a unique
  /// local index L on process P corresponding to G.  If the local
  /// index L is valid on process P, then there is a unique global
  /// index G owned by P corresponding to the pair (L, P).  However,
  /// multiple processes might own the same global index (an
  /// "overlapping Map"), so a global index G might correspond to
  /// multiple (L, P) pairs.  In summary, local indices on a process
  /// correspond to object elements (e.g., sparse matrix rows or
  /// columns) owned by that process.
  ///
  /// Tpetra differs from Epetra in that local and global indices may
  /// have different types.  In Epetra, local and global indices both
  /// have type \c int.  In Tpetra, you get to pick the type of each.
  /// For example, you can use a 64-bit integer \c GlobalOrdinal type
  /// to solve problems with more than \f$2^{31}\f$ unknowns, but a
  /// 32-bit integer \c LocalOrdinal type to save bandwidth in sparse
  /// matrix-vector multiply.
  ///
  /// \subsection Tpetra_Map_contig Contiguous or noncontiguous
  ///
  /// A Map is <i>contiguous</i> when each process' list of global IDs
  /// forms an interval and is strictly increasing, and the globally
  /// minimum global ID equals the index base.  Map optimizes for the
  /// contiguous case.  In particular, noncontiguous Maps require
  /// communication in order to figure out which process owns a
  /// particular global ID.  (This communication happens in
  /// getRemoteIndexList().)
  ///
  /// \subsection Tpetra_Map_dist_repl Globally distributed or locally replicated
  ///
  /// "Globally distributed" means that <i>all</i> of the following
  /// are true:
  ///
  /// 1. The map's communicator has more than one process.
  /// 2. There is at least one process in the map's communicator,
  ///    whose local number of elements does not equal the number of
  ///    global elements.  (That is, not all the elements are
  ///    replicated over all the processes.)
  ///
  /// If at least one of the above are not true, then the map is
  /// "locally replicated."  (The two are mutually exclusive.)
  ///
  /// Globally distributed objects are partitioned across multiple
  /// processes in a communicator.  Each process owns at least one
  /// element in the object's Map that is not owned by another
  /// process.  For locally replicated objects, each element in the
  /// object's Map is owned redundantly by all processes in the
  /// object's communicator.  Some algorithms use objects that are too
  /// small to be distributed across all processes.  The upper
  /// Hessenberg matrix in a GMRES iterative solve is a good example.
  /// In other cases, such as with block iterative methods, block dot
  /// product functions produce small dense matrices that are required
  /// by all images.  Replicated local objects handle these
  /// situations.
  template <class LocalOrdinal,
            class GlobalOrdinal = LocalOrdinal,
            class Node = Kokkos::DefaultNode::DefaultNodeType>
  class Map : public Teuchos::Describable {
  public:
    //! @name Typedefs
    //@{

    //! The type of local indices.
    typedef LocalOrdinal local_ordinal_type;
    //! The type of global indices.
    typedef GlobalOrdinal global_ordinal_type;
    //! The type of the Kokkos Node.
    typedef Node node_type;

    //! @name Constructors and destructor
    //@{

    /** \brief Constructor with Tpetra-defined contiguous uniform distribution.
     *
     * This constructor produces a Map with the following contiguous
     * range of <tt>numGlobalElements</tt> elements: <tt>indexBase,
     * indexBase + 1, ..., numGlobalElements + indexBase - 1</tt>.
     * Depending on the \c lg argument, the elements will either be
     * distributed evenly over all the processes in the given
     * communicator \c comm, or replicated on all processes in the
     * communicator.
     *
     * Preconditions on \c numGlobalElements and \c indexBase will
     * only be checked in a debug build (when Trilinos was configured
     * with CMake option <tt>TEUCHOS_ENABLE_DEBUG:BOOL=ON</tt>).  If
     * checks are enabled and any check fails, the constructor will
     * throw std::invalid_argument on all processes in the given
     * communicator.
     *
     * \param numGlobalElements [in] Number of elements in the Map
     *   (over all processes).
     *
     * \param indexBase [in] The base of the global indices in the
     *   Map.  This must be the same on every process in the
     *   communicator.  The index base will also be the smallest
     *   global index in the Map.  (If you don't know what this should
     *   be, use zero.)
     *
     * \param lg [in] Either <tt>GloballyDistributed</tt> or
     *   <tt>LocallyReplicated</tt>.  If <tt>GloballyDistributed</tt>
     *   and the communicator contains P processes, then each process
     *   will own either <tt>numGlobalElements/P</tt> or
     *   <tt>numGlobalElements/P + 1</tt> nonoverlapping contiguous
     *   elements.  If <tt>LocallyReplicated</tt>, then all processes
     *   will get the same set of elements, namely <tt>indexBase,
     *   indexBase + 1, ..., numGlobalElements + indexBase - 1</tt>.
     *
     * \param comm [in] Communicator over which to distribute the
     *   elements.
     *
     * \param node [in/out] Kokkos Node instance.  The type of this
     *   object must match the type of the Node template parameter.
     */
    Map (global_size_t numGlobalElements,
         GlobalOrdinal indexBase,
         const Teuchos::RCP<const Teuchos::Comm<int> > &comm,
         LocalGlobal lg=GloballyDistributed,
         const Teuchos::RCP<Node> &node = Kokkos::Details::getNode<Node>());

    /** \brief Constructor with a user-defined contiguous distribution.
     *
     * If N is the sum of \c numLocalElements over all processes, then
     * this constructor produces a nonoverlapping Map with N elements
     * distributed over all the processes in the given communicator
     * <tt>comm</tt>, with either \c numLocalElements or
     * <tt>numLocalElements+1</tt> contiguous elements on the calling
     * process.
     *
     * Preconditions on \c numGlobalElements, \c numLocalElements, and
     * \c indexBase will only be checked in a debug build (when
     * Trilinos was configured with CMake option
     * <tt>TEUCHOS_ENABLE_DEBUG:BOOL=ON</tt>).  If checks are enabled
     * and any check fails, the constructor will throw
     * std::invalid_argument on all processes in the given
     * communicator.
     *
     * \param numGlobalElements [in] If <tt>numGlobalElements ==
     *   Teuchos::OrdinalTraits<Tpetra::global_size_t>::invalid()</tt>,
     *   then the number of global elements will be computed (via a
     *   global communication) as the sum of numLocalElements over all
     *   processes.  Otherwise, it must equal the sum of
     *   numLocalElements over all processes.
     *
     * \param numLocalElements [in] Number of elements that the
     *   calling process will own in the Map.
     *
     * \param indexBase [in] The base of the global indices in the
     *   Map.  This must be the same on every process in the given
     *   communicator.  For this Map constructor, the index base will
     *   also be the smallest global index in the Map.  If you don't
     *   know what this should be, use zero.
     *
     * \param comm [in] Communicator over which to distribute the
     *   elements.
     *
     * \param node [in/out] Kokkos Node instance.  The type of this
     *   object must match the type of the Node template parameter.
     */
    Map (global_size_t numGlobalElements,
         size_t numLocalElements,
         GlobalOrdinal indexBase,
         const Teuchos::RCP<const Teuchos::Comm<int> > &comm,
         const Teuchos::RCP<Node> &node = Kokkos::Details::getNode<Node>());

    /** \brief Constructor with user-defined arbitrary (possibly noncontiguous) distribution.
     *
     * Call this constructor if you have an arbitrary list of global
     * indices for each process in the given communicator.  Those
     * indices need not be contiguous, and the sets of global indices
     * on different processes may overlap.  This is the constructor to
     * use to make a general overlapping distribution.
     *
     * \param numGlobalElements [in] If <tt>numGlobalElements ==
     *   Teuchos::OrdinalTraits<Tpetra::global_size_t>::invalid()</tt>,
     *   the number of global elements will be computed (via a global
     *   communication) as the sum of the counts of local elements.
     *   Otherwise, it must equal the sum of the local elements over
     *   all processes.  This will only be checked if Trilinos'
     *   Teuchos package was built with debug support (CMake Boolean
     *   option <tt>Teuchos_ENABLE_DEBUG:BOOL=ON</tt>).  If
     *   verification fails, the constructor will throw
     *   std::invalid_argument.
     *
     * \param elementList [in] List of global indices owned by the
     *   calling process.
     *
     * \param indexBase [in] The base of the global indices in the
     *   Map.  This must be the same on every process in the given
     *   communicator.  Currently, Map requires that this equal the
     *   global minimum index over all processes' <tt>elementList</tt>
     *   inputs.
     *
     * \param comm [in] Communicator over which to distribute the
     *   elements.
     *
     * \param node [in/out] Kokkos Node instance.  The type of this
     *   object must match the type of the Node template parameter.
     */
    Map (global_size_t numGlobalElements,
         const Teuchos::ArrayView<const GlobalOrdinal> &elementList,
         GlobalOrdinal indexBase,
         const Teuchos::RCP<const Teuchos::Comm<int> > &comm,
         const Teuchos::RCP<Node> &node = Kokkos::Details::getNode<Node>());

    //! Destructor.
    ~Map();

    //@}
    //! @name Attributes
    //@{

    //! The number of elements in this Map.
    inline global_size_t getGlobalNumElements() const { return numGlobalElements_; }

    //! The number of elements belonging to the calling process.
    inline size_t getNodeNumElements() const { return numLocalElements_; }

    //! The index base for this Map.
    inline GlobalOrdinal getIndexBase() const { return indexBase_; }

    //! The minimum local index.
    inline LocalOrdinal getMinLocalIndex() const {
      return Teuchos::OrdinalTraits<LocalOrdinal>::zero();
    }

    /// \brief The maximum local index on the calling process.
    ///
    /// If this process owns no elements, that is, if
    /// <tt>getNodeNumElements() == 0</tt>, then this method returns
    /// <tt>Teuchos::OrdinalTraits<LocalOrdinal>::invalid()</tt>.
    inline LocalOrdinal getMaxLocalIndex() const {
      if (getNodeNumElements () == 0) {
        return Teuchos::OrdinalTraits<LocalOrdinal>::invalid ();
      }
      else { // Local indices are always zero-based.
        return Teuchos::as<LocalOrdinal> (getNodeNumElements () - 1);
      }
    }

    //! The minimum global index owned by the calling process.
    inline GlobalOrdinal getMinGlobalIndex() const { return minMyGID_; }

    //! The maximum global index owned by the calling process.
    inline GlobalOrdinal getMaxGlobalIndex() const { return maxMyGID_; }

    //! The minimum global index over all processes in the communicator.
    inline GlobalOrdinal getMinAllGlobalIndex() const { return minAllGID_; }

    //! The maximum global index over all processes in the communicator.
    inline GlobalOrdinal getMaxAllGlobalIndex() const { return maxAllGID_; }

    /// \brief The local index corresponding to the given global index.
    ///
    /// If the given global index is not owned by this process, return
    /// Teuchos::OrdinalTraits<LocalOrdinal>::invalid().
    LocalOrdinal getLocalElement (GlobalOrdinal globalIndex) const;

    /// \brief The global index corresponding to the given local index.
    ///
    /// If the given local index is not valid on the calling process,
    /// return Teuchos::OrdinalTraits<GlobalOrdinal>::invalid().
    GlobalOrdinal getGlobalElement (LocalOrdinal localIndex) const;

    /// \brief Return the process ranks and corresponding local
    ///   indices for the given global indices.
    ///
    /// This operation must always be called as a collective over all
    /// processes in the Map's communicator.  For a distributed
    /// noncontiguous Map, this operation requires communication.
    ///
    /// \param GIDList [in] List of global indices for which to find
    ///   process ranks and local indices.  These global indices need
    ///   not be owned by the calling process.  Indeed, they need not
    ///   be owned by any process.
    /// \param nodeIDList [out] List of process rank corresponding to
    ///   the given global indices.  If a global index does not belong
    ///   to any process, the resulting process rank is -1.
    /// \param LIDList [out] List of local indices (that is, the local
    ///   index on the process that owns them) corresponding to the
    ///   given global indices.  If a global index does not have a
    ///   local index, the resulting local index is
    ///   Teuchos::OrdinalTraits<LocalOrdinal>::invalid().
    ///
    /// \pre nodeIDList.size() == GIDList.size()
    /// \pre LIDList.size() == GIDList.size()
    ///
    /// \return IDNotPresent indicates that for at least one global
    ///   index, we could not find the corresponding process rank.
    ///   Otherwise, return AllIDsPresent.
    ///
    /// \note This is crucial technology used in Export, Import,
    ///   CrsGraph, and CrsMatrix.
    LookupStatus
    getRemoteIndexList (const Teuchos::ArrayView<const GlobalOrdinal>& GIDList,
                        const Teuchos::ArrayView<                int>& nodeIDList,
                        const Teuchos::ArrayView<       LocalOrdinal>& LIDList) const;

    /// \brief Return the process ranks for the given global indices.
    ///
    /// This method must always be called as a collective over all
    /// processes in the Map's communicator.  For a distributed
    /// noncontiguous Map, this operation requires communication.
    ///
    /// \param GIDList [in] List of global indices for which to find
    ///   process ranks and local indices.  These global indices need
    ///   not be owned by the calling process.  Indeed, they need not
    ///   be owned by any process.
    /// \param nodeIDList [out] List of process ranks corresponding to
    ///   the given global indices.  If a global index does not belong
    ///   to any process, the resulting process rank is -1.
    ///
    /// \pre nodeIDList.size() == GIDList.size()
    ///
    /// \return IDNotPresent indicates that for at least one global
    ///   index, we could not find the corresponding process rank.
    ///   Otherwise, return AllIDsPresent.
    ///
    /// \note For a distributed noncontiguous Map, this operation
    ///   requires communication.  This is crucial technology used in
    ///   Export, Import, CrsGraph, and CrsMatrix.
    LookupStatus
    getRemoteIndexList (const Teuchos::ArrayView<const GlobalOrdinal> & GIDList,
                        const Teuchos::ArrayView<                int> & nodeIDList) const;

    /// \brief Return a view of the global indices owned by this process.
    ///
    /// If you call this method on a contiguous Map, it will create
    /// and cache the list of global indices for later use.  Beware of
    /// calling this if the calling process owns a very large number
    /// of global indices.
    Teuchos::ArrayView<const GlobalOrdinal> getNodeElementList() const;

    //@}
    //! @name Boolean tests
    //@{

    //! Whether the given local index is valid for this Map on this process.
    bool isNodeLocalElement (LocalOrdinal localIndex) const;

    //! Whether the given global index is valid for this Map on this process.
    bool isNodeGlobalElement (GlobalOrdinal globalIndex) const;

    /// \brief Whether the range of global indices is uniform.
    ///
    /// This is a conservative quantity.  It need only be true if the
    /// Map was constructed using the first (uniform contiguous)
    /// constructor or a nonmember constructor that calls it.  We
    /// reserve the right to do more work to check this in the future.
    bool isUniform () const;

    /// \brief True if this Map is distributed contiguously, else false.
    ///
    /// Currently, creating this Map using the constructor for a
    /// user-defined arbitrary distribution (that takes a list of
    /// global elements owned on each process) means that this method
    /// always returns false.  We currently make no effort to test
    /// whether the user-provided global indices are actually
    /// contiguous on all the processes.  Many operations may be
    /// faster for contiguous Maps.  Thus, if you know the indices are
    /// contiguous on all processes, you should consider using one of
    /// the constructors for contiguous elements.
    bool isContiguous () const;

    /// \brief Whether this Map is globally distributed or locally replicated.
    ///
    /// \return True if this Map is globally distributed, else false.
    ///
    /// "Globally distributed" means that <i>all</i> of the following
    /// are true:
    ///
    /// <ol>
    /// <li> The map's communicator has more than one process.</li>
    /// <li> There is at least one process in the map's communicator,
    ///    whose local number of elements does not equal the number of
    ///    global elements.  (That is, not all the elements are
    ///    replicated over all the processes.)</li>
    /// </ol>
    ///
    /// If at least one of the above are not true, then the map is
    /// "locally replicated."  (The two are mutually exclusive.)
    ///
    /// Calling this method requires no communication or computation,
    /// because the result is precomputed in Map's constructors.
    bool isDistributed () const;

    /// \brief True if and only if \c map is compatible with this Map.
    ///
    /// Two Maps are "compatible" if all of the following are true:
    ///
    /// <ol>
    /// <li> Their communicators have the same numbers of processes.
    ///    (This is necessary even to call this method.)</li>
    /// <li> They have the same global number of elements.</li>
    /// <li> They have the same number of local elements on each process.</li>
    /// </ol>
    ///
    /// Determining #3 requires communication (a reduction over this
    /// Map's communicator).  This method assumes that the input Map
    /// is valid on all processes in this Map's communicator.
    ///
    /// Compatibility is useful for determining correctness of certain
    /// operations, like assigning one MultiVector X to another Y.  If
    /// X and Y have the same number of columns, and if their Maps are
    /// compatible, then it is legal to assign X to Y or to assign Y
    /// to X.
    ///
    /// If the input Map and this Map have different communicators,
    /// the behavior of this method is currently undefined if the two
    /// communicators have different numbers of processes.
    bool isCompatible (const Map<LocalOrdinal,GlobalOrdinal,Node> &map) const;

    /// \brief True if and only if \c map is identical to this Map.
    ///
    /// "Identical" is stronger than "compatible."  Two Maps are
    /// identical if all of the following are true:
    /// <ol>
    /// <li> Their communicators are <i>congruent</i> (have the same
    ///    number of processes, in the same order: this corresponds to
    ///    the \c MPI_IDENT or \c MPI_CONGRUENT return values of
    ///    MPI_Comm_compare).</li>
    /// <li> They have the same min and max global indices.</li>
    /// <li> They have the same global number of elements.</li>
    /// <li> They are either both distributed, or both not distributed.</li>
    /// <li> Their index bases are the same.</li>
    /// <li> They have the same number of local elements on each process.</li>
    /// <li> They have the same global indices on each process.</li>
    /// </ol>
    ///
    /// "Identical" (isSameAs()) includes and is stronger than
    /// "compatible" (isCompatible()).
    ///
    /// A Map corresponds to a block permutation over process ranks
    /// and global element indices.  Two Maps with different numbers
    /// of processes in their communicators cannot be compatible, let
    /// alone identical.  Two identical Maps correspond to the same
    /// permutation.
    ///
    /// If the input Map and this Map have different communicators,
    /// the behavior of this method is undefined if the two
    /// communicators have different numbers of processes.
    bool isSameAs (const Map<LocalOrdinal,GlobalOrdinal,Node> &map) const;

    //@}
    //! Accessors for the Teuchos::Comm and Kokkos Node objects.
    //@{

    //! Get this Map's Comm object.
    const Teuchos::RCP<const Teuchos::Comm<int> > & getComm() const;

    //! Get this Map's Node object.
    const Teuchos::RCP<Node> & getNode() const;

    //@}
    //! Implementation of \c Teuchos::Describable
    //@{

    //! Return a simple one-line description of this object.
    std::string description() const;

    //! Print this object with the given verbosity level to the given Teuchos::FancyOStream.
    void
    describe (Teuchos::FancyOStream &out,
              const Teuchos::EVerbosityLevel verbLevel=Teuchos::Describable::verbLevel_default) const;

    //@}
    //! Advanced methods
    //@{

    //! Create a shallow copy of this Map, with a different Node type.
    template <class Node2>
    RCP<const Map<LocalOrdinal, GlobalOrdinal, Node2> > clone (const RCP<Node2>& node2) const;

    /// \brief Return a new Map with processes with zero elements removed.
    ///
    /// \warning This method is only for expert users.  Understanding
    ///   how to use this method correctly requires some familiarity
    ///   with semantics of MPI communicators.
    ///
    /// \warning We make no promises of backwards compatibility for
    ///   this method.  It may go away or change at any time.
    ///
    /// This method first computes a new communicator, which contains
    /// only those processes in this Map's communicator (the "original
    /// communicator") that have a nonzero number of elements in this
    /// Map (the "original Map").  It then returns a new Map
    /// distributed over the new communicator.  The new Map represents
    /// the same distribution as the original Map, except that
    /// processes containing zero elements are not included in the new
    /// Map or its communicator.  On processes not included in the new
    /// Map or communicator, this method returns
    /// <tt>Teuchos::null</tt>.
    ///
    /// The returned Map always has a distinct communicator from this
    /// Map's original communicator.  The new communicator contains a
    /// subset of processes from the original communicator.  Even if
    /// the number of processes in the new communicator equals the
    /// number of processes in the original communicator, the new
    /// communicator is distinct.  (In an MPI implementation, the new
    /// communicator is created using MPI_Comm_split.)  Furthermore, a
    /// process may have a different rank in the new communicator, so
    /// be wary of classes that like to store the rank rather than
    /// asking the communicator for it each time.
    ///
    /// This method must be called collectively on the original
    /// communicator.  It leaves the original Map and communicator
    /// unchanged.
    ///
    /// This method was intended for applications such as algebraic
    /// multigrid or other multilevel preconditioners.  Construction
    /// of each level of the multilevel preconditioner typically
    /// requires constructing sparse matrices, which in turn requires
    /// all-reduces over all participating processes at that level.
    /// Matrix sizes at successively coarser levels shrink
    /// geometrically.  At the coarsest levels, some processes might
    /// be left with zero rows of the matrix, or the multigrid
    /// implementation might "rebalance" (redistribute the matrix) and
    /// intentionally leave some processes with zero rows.  Removing
    /// processes with zero rows makes the all-reduces and other
    /// communication operations cheaper.
    RCP<const Map<LocalOrdinal, GlobalOrdinal, Node> >
    removeEmptyProcesses () const;

    /// \brief Replace this Map's communicator with a subset communicator.
    ///
    /// \warning This method is only for expert users.  Understanding
    ///   how to use this method correctly requires some familiarity
    ///   with semantics of MPI communicators.
    ///
    /// \warning We make no promises of backwards compatibility for
    ///   this method.  It may go away or change at any time.
    ///
    /// \pre The input communicator's processes are a subset of this
    ///   Map's current communicator's processes.
    /// \pre On processes which are not included in the input
    ///   communicator, the input communicator is null.
    ///
    /// This method must be called collectively on the original
    /// communicator.  It leaves the original Map and communicator
    /// unchanged.
    ///
    /// \note This method differs from removeEmptyProcesses(), in that
    ///   it does not assume that excluded processes have zero
    ///   entries.  For example, one might wish to remove empty
    ///   processes from the row Map of a CrsGraph using
    ///   removeEmptyProcesses(), and then apply the resulting subset
    ///   communicator to the column, domain, and range Maps of the
    ///   same graph.  For the latter three Maps, one would in general
    ///   use this method instead of removeEmptyProcesses(), giving
    ///   the new row Map's communicator to this method.
    RCP<const Map<LocalOrdinal, GlobalOrdinal, Node> >
    replaceCommWithSubset (const Teuchos::RCP<const Teuchos::Comm<int> >& newComm) const;
    //@}

  protected:
    // This lets other specializations of Map access all of this
    // specialization's internal methods and data, so that we can
    // implement clone() without exposing the details of Map to users.
    template <class LO, class GO, class N> friend class Map;

    /// \brief Default constructor (that does nothing).
    ///
    /// We use this in clone() and removeEmptyProcesses(), where we
    /// have the information to initialize the Map more efficiently
    /// ourselves, without going through one of the three usual Map
    /// construction paths.
    Map() {}

  private:
    /// \brief Create this Map's Directory, if it hasn't been created already.
    ///
    /// This method must be called collectively over all processes in
    /// the Map's communicator.
    ///
    /// This is declared "const" so that we can call it in
    /// getRemoteIndexList() to create the Directory on demand.
    void setupDirectory () const;

    /// \brief Determine whether this map is globally distributed or locally replicated.
    ///
    /// \return True if the map is globally distributed, else false.
    ///
    /// This operation requires communication (a single all-reduce).
    /// See the documentation of \c isDistributed() for definitions
    /// of "globally distributed" and "locally replicated."
    ///
    /// Map invokes this method in its constructors if necessary, to
    /// set the \c distributed_ flag (and thus the return value of \c
    /// isDistributed()).  Map doesn't need to call \c checkIsDist()
    /// when using the uniform contiguous constructor with
    /// lg=GloballyDistributed, since then checking the number of
    /// processes in the communicator suffices.
    bool checkIsDist() const;

    //! Copy constructor (declared but not defined; do not use).
    Map(const Map<LocalOrdinal,GlobalOrdinal,Node> & source);

    //! Assignment operator (declared but not defined; do not use).
    Map<LocalOrdinal,GlobalOrdinal,Node>&
    operator= (const Map<LocalOrdinal,GlobalOrdinal,Node> & source);

    //! The communicator over which this Map is distributed.
    Teuchos::RCP<const Teuchos::Comm<int> > comm_;

    /// \brief The Kokkos Node instance (for shared-memory parallelism).
    ///
    /// Map doesn't need node yet, but it likely will later. In the
    /// meantime, passing a Node to Map means that we don't have to
    /// pass a Node to downstream classes such as MultiVector, Vector,
    /// CrsGraph and CrsMatrix.
    Teuchos::RCP<Node> node_;

    //! The index base for global IDs in this Map.
    GlobalOrdinal indexBase_;
    //! The number of global IDs located in this Map across all nodes.
    global_size_t numGlobalElements_;
    //! The number of global IDs located in this Map on this node.
    size_t numLocalElements_;
    //! The minimum and maximum global IDs located in this Map on this node.
    GlobalOrdinal minMyGID_, maxMyGID_;
    //! The minimum and maximum global IDs located in this Map across all nodes.
    GlobalOrdinal minAllGID_, maxAllGID_;
    //! First, last contiguous GID for use-cases of partially contiguous maps
    GlobalOrdinal firstContiguousGID_, lastContiguousGID_;
    /// \brief Whether the range of global indices is uniform.
    ///
    /// This is only true if the Map was constructed using the first
    /// (uniform contiguous) constructor or a nonmember constructor
    /// that calls it.
    bool uniform_;
    //! Whether the range of global indices are contiguous and ordered.
    bool contiguous_;
    //! Whether this map's global indices are non-identically distributed among different nodes.
    bool distributed_;

    /// \brief A mapping from local IDs to global IDs.
    ///
    /// By definition, this mapping is local; it only contains global
    /// IDs owned by this process.  This mapping is created in two
    /// cases:
    ///
    /// <ol>
    /// <li> It is always created for a noncontiguous Map, in the
    ///    noncontiguous version of the Map constructor.</li>
    /// <li> In getNodeElementList(), on demand (if it wasn't created
    ///    before).</li>
    /// </ol>
    ///
    /// The potential for on-demand creation is why this member datum
    /// is declared "mutable".  Note that other methods, such as
    /// describe(), may invoke getNodeElementList().
    mutable Teuchos::ArrayRCP<GlobalOrdinal> lgMap_;

#ifdef HAVE_TPETRA_FIXED_HASH_TABLE
    //! Type of the table that maps global IDs to local IDs.
    typedef Details::FixedHashTable<GlobalOrdinal, LocalOrdinal> global_to_local_table_type;
#else
    //! Type of the table that maps global IDs to local IDs.
    typedef Details::HashTable<GlobalOrdinal, LocalOrdinal> global_to_local_table_type;
#endif // HAVE_TPETRA_FIXED_HASH_TABLE

    /// \brief A mapping from global IDs to local IDs.
    ///
    /// This is a local mapping.  Directory implements the global
    /// mapping for all global indices (both remote and locally
    /// owned).  This object corresponds roughly to
    /// Epetra_BlockMapData's LIDHash_ hash table (which also maps
    /// from global to local indices).
    ///
    /// This mapping is built only for a noncontiguous map, by the
    /// noncontiguous map constructor.  For noncontiguous maps, the
    /// getLocalElement() and isNodeGlobalElement() methods use
    /// this mapping.
    RCP<global_to_local_table_type> glMap_;

    /// \brief Object that can find the process rank and local index
    ///   for any given global index.
    ///
    /// Creating this object is a collective operation over all
    /// processes in the Map's communicator.  getRemoteIndexList() is
    /// the only method that needs this object, and it also happens to
    /// be a collective.  Thus, we create the Directory on demand in
    /// getRemoteIndexList().  This saves the communication cost of
    /// creating the Directory, for some Maps which are never involved
    /// in an Import or Export operation.  For example, a nonsquare
    /// sparse matrix (CrsMatrix) with row and range Maps the same
    /// would never need to construct an Export object.  This is a
    /// common case for the prolongation or restriction operators in
    /// algebraic multigrid.
    ///
    /// \warning Never allow this pointer to escape the Map.  The
    ///   directory must hold an RCP to this Map, which must be
    ///   non-owning to prevent a circular dependency.  Therefore,
    ///   allowing the Directory to persist beyond this Map would
    ///   result in a dangling RCP. We avoid this by not sharing the
    ///   Directory.
    ///
    /// \note This is declared "mutable" so that getRemoteIndexList()
    ///   can create the Directory on demand.
    mutable Teuchos::RCP<Directory<LocalOrdinal,GlobalOrdinal,Node> > directory_;

  }; // Map class

  /// \brief Nonmember constructor for a locally replicated Map with
  ///   the default Kokkos Node.
  ///
  /// This method returns a Map instantiated on the default Kokkos
  /// Node type, Kokkos::DefaultNode::DefaultNodeType.  The Map is
  /// configured to use zero-based indexing.
  ///
  /// \param numElements [in] Number of elements on each process.
  ///   Each process gets the same set of elements, namely <tt>0, 1,
  ///   ..., numElements - 1</tt>.
  ///
  /// \param comm [in] The Map's communicator.
  ///
  /// \relatesalso Map
  template <class LocalOrdinal, class GlobalOrdinal>
  Teuchos::RCP<const Map<LocalOrdinal,GlobalOrdinal> >
  createLocalMap (size_t numElements, const Teuchos::RCP<const Teuchos::Comm<int> >& comm);

  /// \brief Nonmember constructor for a locally replicated Map with
  ///   a specified Kokkos Node.
  ///
  /// This method returns a Map instantiated on the given Kokkos Node
  /// instance.  The Map is configured to use zero-based indexing.
  ///
  /// \param numElements [in] Number of elements on each process.
  ///   Each process gets the same set of elements, namely <tt>0, 1,
  ///   ..., numElements - 1</tt>.
  ///
  /// \param comm [in] The Map's communicator.
  ///
  /// \param node [in] The Kokkos Node instance.  If not provided, we
  ///   will construct an instance of the correct type for you.
  ///
  /// \relatesalso Map
  template <class LocalOrdinal, class GlobalOrdinal, class Node>
  Teuchos::RCP<const Map<LocalOrdinal,GlobalOrdinal,Node> >
  createLocalMapWithNode (size_t numElements,
                          const Teuchos::RCP<const Teuchos::Comm<int> >& comm,
                          const Teuchos::RCP<Node>& node = Kokkos::Details::getNode<Node> ());

  /** \brief Non-member constructor for a uniformly distributed, contiguous Map with the default Kokkos Node.

      This method returns a Map instantiated on the Kokkos default node type, Kokkos::DefaultNode::DefaultNodeType.

      The Map is configured to use zero-based indexing.

      \relatesalso Map
   */
  template <class LocalOrdinal, class GlobalOrdinal>
  Teuchos::RCP< const Map<LocalOrdinal,GlobalOrdinal> >
  createUniformContigMap(global_size_t numElements, const Teuchos::RCP< const Teuchos::Comm< int > > &comm);

  /** \brief Non-member constructor for a uniformly distributed, contiguous Map with a user-specified Kokkos Node.

      The Map is configured to use zero-based indexing.

      \relatesalso Map
   */
  template <class LocalOrdinal, class GlobalOrdinal, class Node>
  Teuchos::RCP< const Map<LocalOrdinal,GlobalOrdinal,Node> >
  createUniformContigMapWithNode(global_size_t numElements,
                                 const Teuchos::RCP< const Teuchos::Comm< int > > &comm,
                                 const Teuchos::RCP< Node > &node = Kokkos::Details::getNode<Node>());

  /** \brief Non-member constructor for a (potentially) non-uniformly distributed, contiguous Map with the default Kokkos Node.

      This method returns a Map instantiated on the Kokkos default node type, Kokkos::DefaultNode::DefaultNodeType.

      The Map is configured to use zero-based indexing.

      \relatesalso Map
   */
  template <class LocalOrdinal, class GlobalOrdinal>
  Teuchos::RCP<const Map<LocalOrdinal,GlobalOrdinal,Kokkos::DefaultNode::DefaultNodeType> >
  createContigMap (global_size_t numElements,
                   size_t localNumElements,
                   const Teuchos::RCP<const Teuchos::Comm<int> > &comm);

  /** \brief Non-member constructor for a (potentially) non-uniformly distributed, contiguous Map with a user-specified Kokkos Node.

      The Map is configured to use zero-based indexing.

      \relatesalso Map
   */
  template <class LocalOrdinal, class GlobalOrdinal, class Node>
  Teuchos::RCP<const Map<LocalOrdinal,GlobalOrdinal,Node> >
  createContigMapWithNode (global_size_t numElements,
                           size_t localNumElements,
                           const Teuchos::RCP<const Teuchos::Comm<int> > &comm,
                           const Teuchos::RCP<Node> &node);

  /** \brief Non-member constructor for a non-contiguous Map with the default Kokkos Node.

      This method returns a Map instantiated on the Kokkos default node type, Kokkos::DefaultNode::DefaultNodeType.

      The Map is configured to use zero-based indexing.

      \relatesalso Map
   */
  template <class LocalOrdinal, class GlobalOrdinal>
  Teuchos::RCP<const Map<LocalOrdinal,GlobalOrdinal,Kokkos::DefaultNode::DefaultNodeType> >
  createNonContigMap (const ArrayView<const GlobalOrdinal> &elementList,
                      const RCP<const Teuchos::Comm<int> > &comm);

  /** \brief Non-member constructor for a non-contiguous Map with a user-specified Kokkos Node.

      The Map is configured to use zero-based indexing.

      \relatesalso Map
   */
  template <class LocalOrdinal, class GlobalOrdinal, class Node>
  Teuchos::RCP< const Map<LocalOrdinal,GlobalOrdinal,Node> >
  createNonContigMapWithNode (const ArrayView<const GlobalOrdinal> &elementList,
                              const RCP<const Teuchos::Comm<int> > &comm,
                              const RCP<Node> &node);

  /** \brief Non-member constructor for a contiguous Map with user-defined weights and a user-specified Kokkos Node.

      The Map is configured to use zero-based indexing.

      \relatesalso Map
   */
  template <class LocalOrdinal, class GlobalOrdinal, class Node>
  Teuchos::RCP< const Map<LocalOrdinal,GlobalOrdinal,Node> >
  createWeightedContigMapWithNode (int thisNodeWeight,
                                   global_size_t numElements,
                                   const Teuchos::RCP<const Teuchos::Comm<int> > &comm,
                                   const Teuchos::RCP<Node> &node);

  /** \brief Creates a one-to-one version of the given Map where each GID is owned by only one process.

      The user must guarantee there are no duplicate GID on the same processor. Unexepected behavior may result.

      \relatesalso Map
   */
  template<class LocalOrdinal, class GlobalOrdinal, class Node>
  Teuchos::RCP< const Map<LocalOrdinal,GlobalOrdinal,Node> >
  createOneToOne(Teuchos::RCP<const Map<LocalOrdinal,GlobalOrdinal,Node> > &M);

} // Tpetra namespace

#include "Tpetra_Directory_decl.hpp"

namespace Tpetra {
  template <class LocalOrdinal, class GlobalOrdinal, class Node>
  template <class Node2>
  RCP<const Map<LocalOrdinal, GlobalOrdinal, Node2> >
  Map<LocalOrdinal,GlobalOrdinal,Node>::clone (const RCP<Node2> &node2) const
  {
    typedef Map<LocalOrdinal,GlobalOrdinal,Node2> Map2;
    RCP<Map2> map = rcp (new Map2 ());
    // Fill the new Map with shallow copies of all of the original
    // Map's data.  This is safe because Map is immutable, so users
    // can't change the original Map.
    map->comm_              = comm_;
    map->indexBase_         = indexBase_;
    map->numGlobalElements_ = numGlobalElements_;
    map->numLocalElements_  = numLocalElements_;
    map->minMyGID_          = minMyGID_;
    map->maxMyGID_          = maxMyGID_;
    map->minAllGID_         = minAllGID_;
    map->maxAllGID_         = maxAllGID_;
    map->firstContiguousGID_= firstContiguousGID_;
    map->lastContiguousGID_ = lastContiguousGID_;
    map->uniform_           = uniform_;
    map->contiguous_        = contiguous_;
    map->distributed_       = distributed_;
    map->lgMap_             = lgMap_;
    map->glMap_             = glMap_;
    // New Map gets the new Node instance.
    map->node_              = node2;
    // mfh 02 Apr 2013: While Map only needs to create the Directory
    // on demand in getRemoteIndexList, we have a Directory here that
    // we can clone inexpensively, so there is no harm in creating it
    // here.
    if (! directory_.is_null ()) {
      // The weak reference prevents circularity.
      map->directory_ = directory_->template clone<Node2> (map.create_weak ());
    }
    return map;
  }
} // namespace Tpetra

/// \brief True if map1 is the same as (in the sense of isSameAs()) map2, else false.
/// \relatesalso Tpetra::Map
template <class LocalOrdinal, class GlobalOrdinal, class Node>
bool operator== (const Tpetra::Map<LocalOrdinal,GlobalOrdinal,Node> &map1,
                 const Tpetra::Map<LocalOrdinal,GlobalOrdinal,Node> &map2)
{ return map1.isSameAs (map2); }

/// \brief True if map1 is not the same as (in the sense of isSameAs()) map2, else false.
/// \relatesalso Tpetra::Map
template <class LocalOrdinal, class GlobalOrdinal, class Node>
bool operator!= (const Tpetra::Map<LocalOrdinal,GlobalOrdinal,Node> &map1,
                 const Tpetra::Map<LocalOrdinal,GlobalOrdinal,Node> &map2)
{ return ! map1.isSameAs (map2); }

#endif // TPETRA_MAP_DECL_HPP

