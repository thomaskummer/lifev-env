// @HEADER
//
// ***********************************************************************
//
//   Zoltan2: A package of combinatorial algorithms for scientific computing
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
// Questions? Contact Karen Devine      (kddevin@sandia.gov)
//                    Erik Boman        (egboman@sandia.gov)
//                    Siva Rajamanickam (srajama@sandia.gov)
//
// ***********************************************************************
//
// @HEADER

/*! \file Zoltan2_PartitioningSolution.hpp
    \brief Defines the PartitioningSolution class.
*/

#ifndef _ZOLTAN2_PARTITIONINGSOLUTION_HPP_
#define _ZOLTAN2_PARTITIONINGSOLUTION_HPP_

#include <Zoltan2_IdentifierMap.hpp>
#include <Zoltan2_Solution.hpp>
#include <Zoltan2_GreedyMWM.hpp>
#include <Zoltan2_CoordinatePartitioningGraph.hpp>
#include <cmath>
#include <algorithm>
#include <vector>
#include <limits>

namespace Teuchos{

/*! \brief Zoltan2_BoxBoundaries is a reduction operation
 * to all reduce the all box boundaries.
*/

template <typename Ordinal, typename T>
class Zoltan2_BoxBoundaries  : public ValueTypeReductionOp<Ordinal,T>
{
private:
    Ordinal size;
    T _EPSILON;

public:
    /*! \brief Default Constructor
     */
    Zoltan2_BoxBoundaries ():size(0), _EPSILON (std::numeric_limits<T>::epsilon()){}

    /*! \brief Constructor
     *   \param nsum  the count of how many sums will be computed at the
     *             start of the list.
     *   \param nmin  following the sums, this many minimums will be computed.
     *   \param nmax  following the minimums, this many maximums will be computed.
     */
    Zoltan2_BoxBoundaries (Ordinal s_):
        size(s_), _EPSILON (std::numeric_limits<T>::epsilon()){}

    /*! \brief Implement Teuchos::ValueTypeReductionOp interface
     */
    void reduce( const Ordinal count, const T inBuffer[], T inoutBuffer[]) const
    {
        for (Ordinal i=0; i < count; i++){
            if (Z2_ABS(inBuffer[i]) >  _EPSILON){
                inoutBuffer[i] = inBuffer[i];
            }
        }
    }
};
} // namespace Teuchos


namespace Zoltan2 {

long measure_stays(partId_t *, int *, partId_t *, long *, partId_t, partId_t);


/*! \brief A PartitioningSolution is a solution to a partitioning problem.

    It is initialized by a PartitioningProblem,
    written to by an algorithm, and may be read by the user or by
    a data migration routine in an input adapter.

    \todo Problem computes metrics using the Solution.  Should
  Solution have a pointer to the metrics, since it may persist after
  the Problem is gone?
    \todo save an RCB tree, so it can be used in repartitioning, and
                supplied to the caller.
    \todo doxyfy the comments in this file.
*/

template <typename Adapter>
  class PartitioningSolution : public Solution
{
public:

#ifndef DOXYGEN_SHOULD_SKIP_THIS
  typedef typename Adapter::gno_t gno_t;
  typedef typename Adapter::scalar_t scalar_t;
  typedef typename Adapter::lno_t lno_t;
  typedef typename Adapter::gid_t gid_t;
  typedef typename Adapter::user_t user_t;
#endif

/*! \brief Constructor when part sizes are not supplied.
 *
 *   The Solution constructor may require global communication.
 *   The rest of the Solution methods do not.
 *
 *    \param env the environment for the application
 *    \param comm the communicator for the problem associated with
 *             this solution
 *    \param idMap  the IdentifierMap corresponding to the solution
 *    \param userWeightDim  the number of weights supplied by the
 *         application for each object.
 *
 *   It is possible that part sizes were supplied on other processes,
 *   so this constructor does do a check to see if part sizes need
 *   to be globally calculated.
 */

  PartitioningSolution( RCP<const Environment> &env,
    RCP<const Comm<int> > &comm,
    RCP<const IdentifierMap<user_t> > &idMap,
    int userWeightDim);

/*! \brief Constructor when part sizes are supplied.
 *
 *   The Solution constructor may require global communication.
 *   The rest of the Solution methods do not.
 *
 *    \param env the environment for the application
 *    \param comm the communicator for the problem associated with
 *                        this solution
 *    \param idMap  the IdentifierMap corresponding to the solution
 *    \param userWeightDim  the number of weights supplied
 *                         by the application
 *    \param reqPartIds  reqPartIds[i] is a list of
 *          of part numbers for weight dimension i.
 *    \param reqPartSizes  reqPartSizes[i] is the list
 *          of part sizes for weight i corresponding to parts in
 *          reqPartIds[i]
 *
 *   If <tt>reqPartIds[i].size()</tt> and <tt>reqPartSizes[i].size()</tt>
 *           are zero for
 *   all processes, it is assumed that part sizes for weight
 *   dimension "i" are uniform.
 *
 *   If across the application there are some part numbers that are not
 *   included in the reqPartIds lists, then those part sizes are assumed
 *   to be 1.0.
 *
 *   \todo handle errors that may arise - like duplicate part numbers
 */

  PartitioningSolution( RCP<const Environment> &env,
    RCP<const Comm<int> > &comm,
    RCP<const IdentifierMap<user_t> > &idMap,
    int userWeightDim, ArrayView<ArrayRCP<partId_t> > reqPartIds,
    ArrayView<ArrayRCP<scalar_t> > reqPartSizes);

  ////////////////////////////////////////////////////////////////////
  // Information that the algorithm may wish to query.

/*! \brief Returns the global number of parts desired in the solution.
 */
  size_t getTargetGlobalNumberOfParts() const { return nGlobalParts_; }

/*! \brief Returns the actual global number of parts provided in setParts().
 */
  size_t getActualGlobalNumberOfParts() const { return nGlobalPartsSolution_; }

/*! \brief Returns the number of parts to be assigned to this process.
 */
  size_t getLocalNumberOfParts() const { return nLocalParts_; }

/*! \brief If parts are divided across processes, return the fraction of
             a part on this process.
    \return zero if parts are not split across processes, approximate
                fraction of the part otherwise.
    \todo More useful to get number of processes owning part?  Or
              not useful at all - remove this?
 */
  scalar_t getLocalFractionOfPart() const { return localFraction_; }

/*! \brief Is the part-to-process distribution is one-to-one.
     \return true if Process p owns part p for all p, and false if the part
                  to process distribution is more complex.

   If this is true, then getPartDistribution() and getProcDistribution()
   return NULL pointers.  If either of the latter two methods is non-NULL,
   then this method returns false.
 */

  bool oneToOnePartDistribution() const { return onePartPerProc_; }

/*! \brief Return a distribution by part.

    \return If any parts are
       divided across processes, then a mapping \c A is returned.
     \c A such that \c A[i] is the lowest numbered process
       owning part \c i.  The length of the array is one greater than the
       global number of parts.
       The value of the last element is the global number of processes.

     Parts are divided across processes only if there are fewer parts
     than processes and the caller did not define "num_local_parts" for
     each process.  In this case, parts are divided somewhat evenly
     across the processes.  This situation is more likely to arise in
     Zoltan2 algorithms than in user applications.

   If either oneToOnePartDistribution() is true or getProcDistribution() is
   non-NULL, then this method returns a NULL pointer.  The three are mutually
   exclusive and collective exhaustive.
 */
  const int *getPartDistribution() const {
    if (partDist_.size() > 0) return &partDist_[0];
    else return NULL;
  }

/*! \brief Return a distribution by process.

    \return If the mapping of parts to processes is not one-to-one, and
      if parts are not divided across processes, then the mapping
      \c A is returned. \c A such that \c A[i] is the first part
      assigned to process \c i.
      (Parts are assigned sequentially to processes.)
      However if <tt> A[i+1]</tt> is equal to \c A[i], there
      are no parts assigned to process \c i.  The length of the array is
      one greater than the number of processes.  The last element of
      the array is the global number of parts.

    If the mapping is one-to-one, or if parts are divided across processes,
    then this method returns NULL pointer, and either
    oneToOnePartDistribution() or getPartDistribution() describes the mapping.
 */
  const partId_t *getProcDistribution() const {
    if (procDist_.size() > 0) return &procDist_[0];
    else return NULL;
  }

/*! \brief Get the number of criteria (the weight dimension).
    \return the number of criteria for which the solution has part sizes.
 */
  int getNumberOfCriteria() const { return weightDim_; }


/*! \brief Determine if balancing criteria (weight dimension) has uniform
                part sizes.  (User can specify differing part sizes.)
    \param idx   A value from 0 to one less than the number of weights per
                   object.
    \return true if part sizes are uniform for this criteria.
 */
  bool criteriaHasUniformPartSizes(int idx) const { return pSizeUniform_[idx];}

/*! \brief Get the size for a given weight dimension and a given part.

    \param idx   A value from 0 to one less than the number of weights per
                       object.
    \param part  A value from 0 to one less than the global number of parts
                   to be computed
    \return   The size for that part.  Part sizes for a given weight
                    dimension sum to 1.0.

      \todo It would be useful to algorithms to get the sum of
           part sizes from a to b, or the sum or a list of parts.
 */
  scalar_t getCriteriaPartSize(int idx, partId_t part) const {
    if (pSizeUniform_[idx])
      return 1.0 / nGlobalParts_;
    else if (pCompactIndex_[idx].size())
      return pSize_[idx][pCompactIndex_[idx][part]];
    else
      return pSize_[idx][part];
  }

/*! \brief Return true if the two weight dimensions have the same
 *          part size information.

    \param c1   A value from 0 through one less than the number of weights.
    \param c2   A value from 0 through one less than the number of weights.
    \return   If weight dimension \c c1 and weight dimension \c c2 have
        the same part size information, the \c true is returned, otherwise
        \c false is returned.

    It may be a problem for some algorithms if there are multiple weight
    dimensions with differing part size constraints to be satisfied.
 */

  bool criteriaHaveSamePartSizes(int c1, int c2) const;

  ////////////////////////////////////////////////////////////////////
  // Method used by the algorithm to set results.

  /*! \brief The algorithm uses setParts to set the solution.
   *
   *   \param gnoList  A list of global numbers.
   *
   *   \param partList  The part assigned to gnoList[i] by the algorithm
   *      should be in partList[i].  The partList is allocated and written
   *      by the algorithm.
   *
   *   \param dataDidNotMove The algorithm did not change the order of the
   *      data provided by the model; that is, it did not move the data
   *      to other processes or reorganize within the process.  Thus,
   *      the gnoList and partList are ordered in the same way as the
   *      array provided by the model.  Setting this flag to true avoids
   *      processing to remap the data to the original process that provided
   *      the data.
   *
   * The global numbers supplied by the algorithm do not need to be
   * those representing the global Ids of that process.  But
   * all global numbers should be assigned a part by exactly one
   * process.
   *
   * setParts() must be called by all processes in the problem, as
   * the part for each global identifier supplied by each process
   * in its InputAdapter is found and saved in this PartitioningSolution.
   */

  void setParts(ArrayRCP<const gno_t> &gnoList,
    ArrayRCP<partId_t> &partList, bool dataDidNotMove);

  ////////////////////////////////////////////////////////////////////

  /*! \brief Remap a new partition for maximum overlap with an input partition.
   *
   * Assumptions for this version:
   * input part assignment == processor rank for every local object.
   * assuming nGlobalParts <= num ranks
   * TODO:  Write a version that takes the input part number as input;
   *        this change requires input parts in adapters to be provided in
   *        the Adapter.
   * TODO:  For repartitioning, compare to old remapping results; see Zoltan1.
   */

  void RemapParts();

  ////////////////////////////////////////////////////////////////////
  // Results that may be queried by the user, by migration methods,
  // or by metric calculation methods.
  // We return raw pointers so users don't have to learn about our
  // pointer wrappers.

  /*! \brief Return the communicator associated with the solution.
   */
  const RCP<const Comm<int> > &getCommunicator() const { return comm_;}

  /*! \brief Returns the local number of Ids.
   */
  size_t getLocalNumberOfIds() const { return gids_.size(); }

  /*! \brief Returns the user's global ID list.
   */
  const gid_t *getIdList() const { return gids_.getRawPtr(); }

  /*! \brief Returns the part list corresponding to the global ID list.
   */
  const zoltan2_partId_t *getPartList() const { return parts_.getRawPtr();}

  /*! \brief Returns the process list corresponding to the global ID list.
      \return The return value is a NULL pointer if part IDs are
                synonomous with process IDs.
   */
  const int *getProcList() const { return procs_.getRawPtr();}


  /*! \brief set the Part Box boundaries as a result of geometric partitioning algorithm.
   */
  void setPartBoxes(RCP < vector <Zoltan2::coordinateModelPartBox <scalar_t, partId_t> > > outPartBoxes){
      this->partBoxes = outPartBoxes;
  }

  /*! \brief returns the part box boundary list.
   */
  RCP < vector <Zoltan2::coordinateModelPartBox <scalar_t, partId_t> > > getPartBoxes(){
      return this->partBoxes;
  }

  /*! \brief returns the communication graph as a result of geometric partitioning algorithm.
   */
  void getCommunicationGraph(
          const Teuchos::Comm<int> *comm,
          ArrayRCP <partId_t> &comXAdj,
          ArrayRCP <partId_t> &comAdj
  ) {

      if(comXAdj_.getRawPtr() == NULL && comAdj_.getRawPtr() == NULL){

          partId_t ntasks =  this->getActualGlobalNumberOfParts();
          if (partId_t (this->getTargetGlobalNumberOfParts()) > ntasks){
              ntasks = this->getTargetGlobalNumberOfParts();
          }
          RCP < vector <Zoltan2::coordinateModelPartBox <scalar_t, partId_t> > > pBoxes = this->getGlobalBoxBoundaries(comm);
          int dim = (*pBoxes)[0].getDim();
          GridHash < scalar_t, partId_t> grid(
                  pBoxes,
                  ntasks,
                  dim);
          grid.getAdjArrays(comXAdj_, comAdj_);
      }
      comAdj = comAdj_;
      comXAdj = comXAdj_;

  }

  RCP < vector <Zoltan2::coordinateModelPartBox <scalar_t, partId_t> > > getGlobalBoxBoundaries(
          const Teuchos::Comm<int> *comm){

      partId_t ntasks =  this->getActualGlobalNumberOfParts();
      if (partId_t (this->getTargetGlobalNumberOfParts()) > ntasks){
          ntasks = this->getTargetGlobalNumberOfParts();
      }

      RCP < vector <Zoltan2::coordinateModelPartBox <scalar_t, partId_t> > > pBoxes = this->getPartBoxes();

      int dim = (*pBoxes)[0].getDim();


      scalar_t *localPartBoundaries = new scalar_t[ntasks * 2 *dim];

      memset(localPartBoundaries, 0, sizeof(scalar_t) * ntasks * 2 *dim);

      scalar_t *globalPartBoundaries = new scalar_t[ntasks * 2 *dim];
      memset(globalPartBoundaries, 0, sizeof(scalar_t) * ntasks * 2 *dim);

      scalar_t *localPartMins = localPartBoundaries;
      scalar_t *localPartMaxs = localPartBoundaries + ntasks * dim;

      scalar_t *globalPartMins = globalPartBoundaries;
      scalar_t *globalPartMaxs = globalPartBoundaries + ntasks * dim;

      partId_t boxCount = pBoxes->size();
      for (partId_t i = 0; i < boxCount; ++i){
          partId_t pId = (*pBoxes)[i].getpId();
          //cout << "me:" << comm->getRank() << " has:" << pId << endl;

          scalar_t *lmins = (*pBoxes)[i].getlmins();
          scalar_t *lmaxs = (*pBoxes)[i].getlmaxs();

          for (int j = 0; j < dim; ++j){
              localPartMins[dim * pId + j] = lmins[j];
              localPartMaxs[dim * pId + j] = lmaxs[j];
              /*
              cout << "me:" << comm->getRank()  <<
                      " dim * pId + j:"<< dim * pId + j <<
                      " localMin:" << localPartMins[dim * pId + j] <<
                      " localMax:" << localPartMaxs[dim * pId + j] << endl;
                      */
          }
      }

      Teuchos::Zoltan2_BoxBoundaries<int, scalar_t> reductionOp(ntasks * 2 *dim);

      reduceAll<int, scalar_t>(*comm, reductionOp,
              ntasks * 2 *dim, localPartBoundaries, globalPartBoundaries
      );
      RCP < std::vector <coordinateModelPartBox <scalar_t, partId_t> > > pB(new std::vector <coordinateModelPartBox <scalar_t, partId_t> > (), true) ;
      for (partId_t i = 0; i < ntasks; ++i){
          Zoltan2::coordinateModelPartBox <scalar_t, partId_t> tpb(
                  i,
                  dim,
                  globalPartMins + dim * i,
                  globalPartMaxs + dim * i);

          /*
          for (int j = 0; j < dim; ++j){
              cout << "me:" << comm->getRank()  <<
                      " dim * pId + j:"<< dim * i + j <<
                      " globalMin:" << globalPartMins[dim * i + j] <<
                      " globalMax:" << globalPartMaxs[dim * i + j] << endl;
          }
          */
          pB->push_back(tpb);
      }
      delete []localPartBoundaries;
      delete []globalPartBoundaries;
      //RCP < std::vector <Zoltan2::coordinateModelPartBox <scalar_t, partId_t> > > tmpRCPBox(pB, true);
      this->partBoxes = pB;
      return this->partBoxes;
  }
  /*! \brief Create an import list from the export list.
   *
   *  \param numExtra The amount of related information of type
   *            \c Extra that you would like to associate with the data.
   *  \param xtraInfo  The extra information related to your global Ids.
   *       The information for the <tt>k-th</tt> global ID would begin at
   *       <tt> xtraInfo[k*numExtra]</tt> and end before
   *       <tt> xtraInfo[(k+1)*numExtra]</tt>.
   *  \param imports on return is the list of global Ids assigned to
   *        this process under the Solution.
   *  \param newXtraInfo on return is the extra information associated
   *     with the global Ids in the import list.
   *
   * The list returned in getPartList() is an export list, detailing
   * to which part each object should be moved.  This method provides
   * a new list, listing the global IDs of the objects to be imported
   * to my part or parts.
   *
   * Because this method does global communication, it can also
   * send useful data related to the global IDs.  For example, if the global IDs
   * represent matrix rows, the extra data could be the number of non zeros
   * in the row.
   *
   * \todo A version which takes the export part numbers and returns the
   *            import part numbers in addition to the global IDs.  Although
   *            this can be done with the extra data, it might be better
   *            to do it explicitly.
   */

  template <typename Extra>
    size_t convertSolutionToImportList(
      int numExtra,
      ArrayRCP<Extra> &xtraInfo,
      ArrayRCP<typename Adapter::gid_t> &imports,
      ArrayRCP<Extra> &newXtraInfo) const;

  /*! \brief Get the parts belonging to a process.
   *  \param procId a process rank
   *  \param numParts on return will be set the number of parts belonging
   *                    to the process.
   *  \param partMin on return will be set to minimum part number
   *  \param partMax on return will be set to maximum part number
   *
   * Normally \c numParts is at least one. But if there are more processes
   * than parts, one of two things can happen.  Either there are processes
   * with no parts, and so \c numParts will be zero, or a part may be
   * split across more than one process, in which \c numParts will
   * be non-zero but less than 1.
   *
   * In the latter case, \c numParts is 1.0 divided by the number of
   * processes that share the part.
   */

  void getPartsForProc(int procId, double &numParts, partId_t &partMin,
    partId_t &partMax) const
  {
    env_->localInputAssertion(__FILE__, __LINE__, "invalid process id",
      procId >= 0 && procId < comm_->getSize(), BASIC_ASSERTION);

    procToPartsMap(procId, numParts, partMin, partMax);
  }

  /*! \brief Get the processes containing a part.
   *  \param partId a part number from 0 to one less than the global number
   *                of parts.
   *  \param procMin on return will be set to minimum proc number
   *  \param procMax on return will be set to maximum proc number
   *
   * Normally \c procMin and \c procMax are the same value and a part
   * is assigned to one process.  But if there are more processes than
   * parts, it's possible that a part will be divided across more than
   * one process.
   */
  void getProcsForPart(partId_t partId, int &procMin, int &procMax) const
  {
    env_->localInputAssertion(__FILE__, __LINE__, "invalid part id",
      partId >= 0 && partId < nGlobalParts_, BASIC_ASSERTION);

    partToProcsMap(partId, procMin, procMax);
  }

private:
  void partToProc(bool doCheck, bool haveNumLocalParts, bool haveNumGlobalParts,
    int numLocalParts, int numGlobalParts);

  void procToPartsMap(int procId, double &numParts, partId_t &partMin,
    partId_t &partMax) const;

  void partToProcsMap(partId_t partId, int &procMin, int &procMax) const;

  void setPartDistribution();

  void setPartSizes(ArrayView<ArrayRCP<partId_t> > reqPartIds,
    ArrayView<ArrayRCP<scalar_t> > reqPartSizes);

  void computePartSizes(int wdim, ArrayView<partId_t> ids,
    ArrayView<scalar_t> sizes);

  void broadcastPartSizes(int wdim);


  RCP<const Environment> env_;             // has application communicator
  RCP<const Comm<int> > comm_;             // the problem communicator
  RCP<const IdentifierMap<user_t> > idMap_;

  //part box boundaries as a result of geometric partitioning algorithm.
  RCP < vector <Zoltan2::coordinateModelPartBox <scalar_t, partId_t> > > partBoxes;
  ArrayRCP <partId_t> comXAdj_; //communication graph xadj
  ArrayRCP <partId_t> comAdj_; //communication graph adj.

  partId_t nGlobalParts_;// target global number of parts
  partId_t nLocalParts_; // number of parts to be on this process

  scalar_t localFraction_; // approx fraction of a part on this process
  int weightDim_;      // if user has no weights, this is 1

  // If process p is to be assigned part p for all p, then onePartPerProc_
  // is true. Otherwise it is false, and either procDist_ or partDist_
  // describes the allocation of parts to processes.
  //
  // If parts are never split across processes, then procDist_ is defined
  // as follows:
  //
  //   partId              = procDist_[procId]
  //   partIdNext          = procDist_[procId+1]
  //   globalNumberOfParts = procDist_[numProcs]
  //
  // meaning that the parts assigned to process procId range from
  // [partId, partIdNext).  If partIdNext is the same as partId, then
  // process procId has no parts.
  //
  // If the number parts is less than the number of processes, and the
  // user did not specify "num_local_parts" for each of the processes, then
  // parts are split across processes, and partDist_ is defined rather than
  // procDist_.
  //
  //   procId              = partDist_[partId]
  //   procIdNext          = partDist_[partId+1]
  //   globalNumberOfProcs = partDist_[numParts]
  //
  // which implies that the part partId is shared by processes in the
  // the range [procId, procIdNext).
  //
  // We use std::vector so we can use upper_bound algorithm

  bool             onePartPerProc_;   // either this is true...
  std::vector<int>      partDist_;      // or this is defined ...
  std::vector<partId_t> procDist_;      // or this is defined.
  bool procDistEquallySpread_;        // if procDist_ is used and
                                      // #parts > #procs and
                                      // num_local_parts is not specified,
                                      // parts are evenly distributed to procs

  // In order to minimize the storage required for part sizes, we
  // have three different representations.
  //
  // If the part sizes for weight dimension w are all the same, then:
  //    pSizeUniform_[w] = true
  //    pCompactIndex_[w].size() = 0
  //    pSize_[w].size() = 0
  //
  // and the size for part p is 1.0 / nparts.
  //
  // If part sizes differ for each part in weight dimension w, but there
  // are no more than 64 distinct sizes:
  //    pSizeUniform_[w] = false
  //    pCompactIndex_[w].size() = number of parts
  //    pSize_[w].size() = number of different sizes
  //
  // and the size for part p is pSize_[pCompactIndex_[p]].
  //
  // If part sizes differ for each part in weight dimension w, and there
  // are more than 64 distinct sizes:
  //    pSizeUniform_[w] = false
  //    pCompactIndex_[w].size() = 0
  //    pSize_[w].size() = nparts
  //
  // and the size for part p is pSize_[p].
  //
  // NOTE: If we expect to have similar cases, i.e. a long list of scalars
  //   where it is highly possible that just a few unique values appear,
  //   then we may want to make this a class.  The advantage is that we
  //   save a long list of 1-byte indices instead of a long list of scalars.

  ArrayRCP<bool> pSizeUniform_;
  ArrayRCP<ArrayRCP<unsigned char> > pCompactIndex_;
  ArrayRCP<ArrayRCP<scalar_t> > pSize_;

  ////////////////////////////////////////////////////////////////
  // The algorithm sets these values upon completion.

  ArrayRCP<const gid_t>  gids_;   // User's global IDs
  ArrayRCP<partId_t> parts_;      // part number assigned to gids_[i]

  bool haveSolution_;

  partId_t nGlobalPartsSolution_; // global number of parts in solution

  ////////////////////////////////////////////////////////////////
  // The solution calculates this from the part assignments,
  // unless onePartPerProc_.

  ArrayRCP<int> procs_;       // process rank assigned to gids_[i]
};

////////////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////////////

template <typename Adapter>
  PartitioningSolution<Adapter>::PartitioningSolution(
    RCP<const Environment> &env,
    RCP<const Comm<int> > &comm,
    RCP<const IdentifierMap<user_t> > &idMap, int userWeightDim)
    : env_(env), comm_(comm), idMap_(idMap),
      partBoxes(),comXAdj_(), comAdj_(),
      nGlobalParts_(0), nLocalParts_(0),
      localFraction_(0),  weightDim_(),
      onePartPerProc_(false), partDist_(), procDist_(),
      procDistEquallySpread_(false),
      pSizeUniform_(), pCompactIndex_(), pSize_(),
      gids_(), parts_(), haveSolution_(false), nGlobalPartsSolution_(0),
      procs_()
{
  weightDim_ = (userWeightDim ? userWeightDim : 1);

  setPartDistribution();

  // We must call setPartSizes() because part sizes may have
  // been provided by the user on other processes.

  ArrayRCP<partId_t> *noIds = new ArrayRCP<partId_t> [weightDim_];
  ArrayRCP<scalar_t> *noSizes = new ArrayRCP<scalar_t> [weightDim_];
  ArrayRCP<ArrayRCP<partId_t> > ids(noIds, 0, weightDim_, true);
  ArrayRCP<ArrayRCP<scalar_t> > sizes(noSizes, 0, weightDim_, true);

  setPartSizes(ids.view(0, weightDim_), sizes.view(0, weightDim_));

  env_->memory("After construction of solution");
}

template <typename Adapter>
  PartitioningSolution<Adapter>::PartitioningSolution(
    RCP<const Environment> &env,
    RCP<const Comm<int> > &comm,
    RCP<const IdentifierMap<user_t> > &idMap, int userWeightDim,
    ArrayView<ArrayRCP<partId_t> > reqPartIds,
    ArrayView<ArrayRCP<scalar_t> > reqPartSizes)
    : env_(env), comm_(comm), idMap_(idMap),
      partBoxes(),comXAdj_(), comAdj_(),
      nGlobalParts_(0), nLocalParts_(0),
      localFraction_(0),  weightDim_(),
      onePartPerProc_(false), partDist_(), procDist_(),
      procDistEquallySpread_(false),
      pSizeUniform_(), pCompactIndex_(), pSize_(),
      gids_(), parts_(), haveSolution_(false), nGlobalPartsSolution_(0),
      procs_()
{
  weightDim_ = (userWeightDim ? userWeightDim : 1);

  setPartDistribution();

  setPartSizes(reqPartIds, reqPartSizes);

  env_->memory("After construction of solution");
}

template <typename Adapter>
  void PartitioningSolution<Adapter>::setPartDistribution()
{
  // Did the caller define num_global_parts and/or num_local_parts?

  const ParameterList &pl = env_->getParameters();
  size_t haveGlobalNumParts=0, haveLocalNumParts=0;
  int numLocal=0, numGlobal=0;
  double val;

  const Teuchos::ParameterEntry *pe = pl.getEntryPtr("num_global_parts");

  if (pe){
    val = pe->getValue<double>(&val);  // TODO: KDD Skip this double get
    haveGlobalNumParts = 1;            // TODO: KDD Should be unnecessary once
    numGlobal = static_cast<int>(val); // TODO: KDD paramlist handles long long.
    nGlobalParts_ = partId_t(numGlobal); // TODO: KDD  also do below.
  }

  pe = pl.getEntryPtr("num_local_parts");

  if (pe){
    val = pe->getValue<double>(&val);
    haveLocalNumParts = 1;
    numLocal = static_cast<int>(val);
    nLocalParts_ = partId_t(numLocal);
  }

  try{
    // Sets onePartPerProc_, partDist_, and procDist_

    partToProc(true, haveLocalNumParts, haveGlobalNumParts,
      numLocal, numGlobal);
  }
  Z2_FORWARD_EXCEPTIONS

  int nprocs = comm_->getSize();
  int rank = comm_->getRank();

  if (onePartPerProc_){
    nGlobalParts_ = nprocs;
    nLocalParts_ = 1;
  }
  else if (partDist_.size() > 0){   // more procs than parts
    nGlobalParts_ = partDist_.size() - 1;
    int pstart = partDist_[0];
    for (partId_t i=1; i <= nGlobalParts_; i++){
      int pend = partDist_[i];
      if (rank >= pstart && rank < pend){
        int numOwners = pend - pstart;
        nLocalParts_ = 1;
        localFraction_ = 1.0 / numOwners;
        break;
      }
      pstart = pend;
    }
  }
  else if (procDist_.size() > 0){  // more parts than procs
    nGlobalParts_ = procDist_[nprocs];
    nLocalParts_ = procDist_[rank+1] - procDist_[rank];
  }
  else {
    throw logic_error("partToProc error");
  }

}

template <typename Adapter>
  void PartitioningSolution<Adapter>::setPartSizes(
    ArrayView<ArrayRCP<partId_t> > ids, ArrayView<ArrayRCP<scalar_t> > sizes)
{
  int wdim = weightDim_;
  bool fail=false;

  size_t *countBuf = new size_t [wdim*2];
  ArrayRCP<size_t> counts(countBuf, 0, wdim*2, true);

  fail = ((ids.size() != wdim) || (sizes.size() != wdim));

  for (int w=0; !fail && w < wdim; w++){
    counts[w] = ids[w].size();
    if (ids[w].size() != sizes[w].size()) fail=true;
  }

  env_->globalBugAssertion(__FILE__, __LINE__, "bad argument arrays", fail==0,
    COMPLEX_ASSERTION, comm_);

  // Are all part sizes the same?  This is the common case.

  ArrayRCP<scalar_t> *emptySizes= new ArrayRCP<scalar_t> [wdim];
  pSize_ = arcp(emptySizes, 0, wdim);

  ArrayRCP<unsigned char> *emptyIndices= new ArrayRCP<unsigned char> [wdim];
  pCompactIndex_ = arcp(emptyIndices, 0, wdim);

  bool *info = new bool [wdim];
  pSizeUniform_ = arcp(info, 0, wdim);
  for (int w=0; w < wdim; w++)
    pSizeUniform_[w] = true;

  if (nGlobalParts_ == 1){
    return;   // there's only one part in the whole problem
  }

  size_t *ptr1 = counts.getRawPtr();
  size_t *ptr2 = counts.getRawPtr() + wdim;

  try{
    reduceAll<int, size_t>(*comm_, Teuchos::REDUCE_MAX, wdim, ptr1, ptr2);
  }
  Z2_THROW_OUTSIDE_ERROR(*env_);

  bool zero = true;

  for (int w=0; w < wdim; w++)
    if (counts[wdim+w] > 0){
      zero = false;
      pSizeUniform_[w] = false;
    }

  if (zero) // Part sizes for all criteria are uniform.
    return;

  // Compute the part sizes for criteria for which part sizes were
  // supplied.  Normalize for each criteria so part sizes sum to one.

  int nprocs = comm_->getSize();
  int rank = comm_->getRank();

  for (int w=0; w < wdim; w++){
    if (pSizeUniform_[w]) continue;

    // Send all ids and sizes to one process.
    // (There is no simple gather method in Teuchos.)

    partId_t length = ids[w].size();
    partId_t *allLength = new partId_t [nprocs];
    Teuchos::gatherAll<int, partId_t>(*comm_, 1, &length,
      nprocs, allLength);

    if (rank == 0){
      int total = 0;
      for (int i=0; i < nprocs; i++)
        total += allLength[i];

      partId_t *partNums = new partId_t [total];
      scalar_t *partSizes = new scalar_t [total];

      ArrayView<partId_t> idArray(partNums, total);
      ArrayView<scalar_t> sizeArray(partSizes, total);

      if (length > 0){
        for (int i=0; i < length; i++){
          *partNums++ = ids[w][i];
          *partSizes++ = sizes[w][i];
        }
      }

      for (int p=1; p < nprocs; p++){
        if (allLength[p] > 0){
          Teuchos::receive<int, partId_t>(*comm_, p,
            allLength[p], partNums);
          Teuchos::receive<int, scalar_t>(*comm_, p,
            allLength[p], partSizes);
          partNums += allLength[p];
          partSizes += allLength[p];
        }
      }

      delete [] allLength;

      try{
        computePartSizes(w, idArray, sizeArray);
      }
      Z2_FORWARD_EXCEPTIONS

      delete [] idArray.getRawPtr();
      delete [] sizeArray.getRawPtr();
    }
    else{
      delete [] allLength;
      if (length > 0){
        Teuchos::send<int, partId_t>(*comm_, length, ids[w].getRawPtr(), 0);
        Teuchos::send<int, scalar_t>(*comm_, length, sizes[w].getRawPtr(), 0);
      }
    }

    broadcastPartSizes(w);
  }
}

template <typename Adapter>
  void PartitioningSolution<Adapter>::broadcastPartSizes(int wdim)
{
  env_->localBugAssertion(__FILE__, __LINE__, "preallocations",
    pSize_.size()>wdim &&
    pSizeUniform_.size()>wdim && pCompactIndex_.size()>wdim,
    COMPLEX_ASSERTION);

  int rank = comm_->getRank();
  int nprocs = comm_->getSize();
  partId_t nparts = nGlobalParts_;

  if (nprocs < 2)
    return;

  char flag=0;

  if (rank == 0){
    if (pSizeUniform_[wdim] == true)
      flag = 1;
    else if (pCompactIndex_[wdim].size() > 0)
      flag = 2;
    else
      flag = 3;
  }

  try{
    Teuchos::broadcast<int, char>(*comm_, 0, 1, &flag);
  }
  Z2_THROW_OUTSIDE_ERROR(*env_);

  if (flag == 1){
    if (rank > 0)
      pSizeUniform_[wdim] = true;

    return;
  }

  if (flag == 2){

    // broadcast the indices into the size list

    unsigned char *idxbuf = NULL;

    if (rank > 0){
      idxbuf = new unsigned char [nparts];
      env_->localMemoryAssertion(__FILE__, __LINE__, nparts, idxbuf);
    }
    else{
      env_->localBugAssertion(__FILE__, __LINE__, "index list size",
        pCompactIndex_[wdim].size() == nparts, COMPLEX_ASSERTION);
      idxbuf = pCompactIndex_[wdim].getRawPtr();
    }

    try{
      // broadcast of unsigned char is not supported
      Teuchos::broadcast<int, char>(*comm_, 0, nparts,
        reinterpret_cast<char *>(idxbuf));
    }
    Z2_THROW_OUTSIDE_ERROR(*env_);

    if (rank > 0)
      pCompactIndex_[wdim] = arcp(idxbuf, 0, nparts, true);

    // broadcast the list of different part sizes

    unsigned char maxIdx=0;
    for (partId_t p=0; p < nparts; p++)
      if (idxbuf[p] > maxIdx) maxIdx = idxbuf[p];

    int numSizes = maxIdx + 1;

    scalar_t *sizeList = NULL;

    if (rank > 0){
      sizeList = new scalar_t [numSizes];
      env_->localMemoryAssertion(__FILE__, __LINE__, numSizes, sizeList);
    }
    else{
      env_->localBugAssertion(__FILE__, __LINE__, "wrong number of sizes",
        numSizes == pSize_[wdim].size(), COMPLEX_ASSERTION);

      sizeList = pSize_[wdim].getRawPtr();
    }

    try{
      Teuchos::broadcast<int, scalar_t>(*comm_, 0, numSizes, sizeList);
    }
    Z2_THROW_OUTSIDE_ERROR(*env_);

    if (rank > 0)
      pSize_[wdim] = arcp(sizeList, 0, numSizes, true);

    return;
  }

  if (flag == 3){

    // broadcast the size of each part

    scalar_t *sizeList = NULL;

    if (rank > 0){
      sizeList = new scalar_t [nparts];
      env_->localMemoryAssertion(__FILE__, __LINE__, nparts, sizeList);
    }
    else{
      env_->localBugAssertion(__FILE__, __LINE__, "wrong number of sizes",
        nparts == pSize_[wdim].size(), COMPLEX_ASSERTION);

      sizeList = pSize_[wdim].getRawPtr();
    }

    try{
      Teuchos::broadcast<int, scalar_t >(*comm_, 0, nparts, sizeList);
    }
    Z2_THROW_OUTSIDE_ERROR(*env_);

    if (rank > 0)
      pSize_[wdim] = arcp(sizeList, 0, nparts);

    return;
  }
}

template <typename Adapter>
  void PartitioningSolution<Adapter>::computePartSizes(int wdim,
    ArrayView<partId_t> ids, ArrayView<scalar_t> sizes)
{
  int len = ids.size();

  if (len == 0){
    pSizeUniform_[wdim] = true;
    return;
  }

  env_->localBugAssertion(__FILE__, __LINE__, "bad array sizes",
    len>0 && sizes.size()==len, COMPLEX_ASSERTION);

  env_->localBugAssertion(__FILE__, __LINE__, "bad index",
    wdim>=0 && wdim<weightDim_, COMPLEX_ASSERTION);

  env_->localBugAssertion(__FILE__, __LINE__, "preallocations",
    pSize_.size()>wdim &&
    pSizeUniform_.size()>wdim && pCompactIndex_.size()>wdim,
    COMPLEX_ASSERTION);

  // Check ids and sizes and find min, max and average sizes.
  // If sizes are very close to uniform, call them uniform parts.

  partId_t nparts = nGlobalParts_;
  unsigned char *buf = new unsigned char [nparts];
  env_->localMemoryAssertion(__FILE__, __LINE__, nparts, buf);
  memset(buf, 0, nparts);
  ArrayRCP<unsigned char> partIdx(buf, 0, nparts, true);

  scalar_t epsilon = 10e-5 / nparts;
  scalar_t min=sizes[0], max=sizes[0], sum=0;

  for (int i=0; i < len; i++){
    partId_t id = ids[i];
    scalar_t size = sizes[i];

    env_->localInputAssertion(__FILE__, __LINE__, "invalid part id",
      id>=0 && id<nparts, BASIC_ASSERTION);

    env_->localInputAssertion(__FILE__, __LINE__, "invalid part size", size>=0,
      BASIC_ASSERTION);

    // TODO: we could allow users to specify multiple sizes for the same
    // part if we add a parameter that says what we are to do with them:
    // add them or take the max.

    env_->localInputAssertion(__FILE__, __LINE__,
      "multiple sizes provided for one part", partIdx[id]==0, BASIC_ASSERTION);

    partIdx[id] = 1;    // mark that we have a size for this part

    if (size < min) min = size;
    if (size > max) max = size;
    sum += size;
  }

  if (sum == 0){

    // User has given us a list of parts of size 0, we'll set
    // the rest to them to equally sized parts.

    scalar_t *allSizes = new scalar_t [2];
    env_->localMemoryAssertion(__FILE__, __LINE__, 2, allSizes);

    ArrayRCP<scalar_t> sizeArray(allSizes, 0, 2, true);

    int numNonZero = nparts - len;

    allSizes[0] = 0.0;
    allSizes[1] = 1.0 / numNonZero;

    for (partId_t p=0; p < nparts; p++)
      buf[p] = 1;                 // index to default part size

    for (int i=0; i < len; i++)
      buf[ids[i]] = 0;            // index to part size zero

    pSize_[wdim] = sizeArray;
    pCompactIndex_[wdim] = partIdx;

    return;
  }

  if (max - min <= epsilon){
    pSizeUniform_[wdim] = true;
    return;
  }

  // A size for parts that were not specified:
  scalar_t avg = sum / nparts;

  // We are going to merge part sizes that are very close.  This takes
  // computation time now, but can save considerably in the storage of
  // all part sizes on each process.  For example, a common case may
  // be some parts are size 1 and all the rest are size 2.

  scalar_t *tmp = new scalar_t [len];
  env_->localMemoryAssertion(__FILE__, __LINE__, len, tmp);
  memcpy(tmp, sizes.getRawPtr(), sizeof(scalar_t) * len);
  ArrayRCP<scalar_t> partSizes(tmp, 0, len, true);

  std::sort(partSizes.begin(), partSizes.end());

  // create a list of sizes that are unique within epsilon

  Array<scalar_t> nextUniqueSize;
  nextUniqueSize.push_back(partSizes[len-1]);   // largest
  scalar_t curr = partSizes[len-1];
  int avgIndex = len;
  bool haveAvg = false;
  if (curr - avg <= epsilon)
     avgIndex = 0;

  for (int i=len-2; i >= 0; i--){
    scalar_t val = partSizes[i];
    if (curr - val > epsilon){
      nextUniqueSize.push_back(val);  // the highest in the group
      curr = val;
      if (avgIndex==len && val > avg && val - avg <= epsilon){
        // the average would be in this group
        avgIndex = nextUniqueSize.size() - 1;
        haveAvg = true;
      }
    }
  }

  partSizes.clear();

  size_t numSizes = nextUniqueSize.size();
  int sizeArrayLen = numSizes;

  if (numSizes < 64){
    // We can store the size for every part in a compact way.

    // Create a list of all sizes in increasing order

    if (!haveAvg) sizeArrayLen++;   // need to include average

    scalar_t *allSizes = new scalar_t [sizeArrayLen];
    env_->localMemoryAssertion(__FILE__, __LINE__, sizeArrayLen, allSizes);
    ArrayRCP<scalar_t> sizeArray(allSizes, 0, sizeArrayLen, true);

    int newAvgIndex = sizeArrayLen;

    for (int i=numSizes-1, idx=0; i >= 0; i--){

      if (newAvgIndex == sizeArrayLen){

        if (haveAvg && i==avgIndex)
          newAvgIndex = idx;

        else if (!haveAvg && avg < nextUniqueSize[i]){
          newAvgIndex = idx;
          allSizes[idx++] = avg;
        }
      }

      allSizes[idx++] = nextUniqueSize[i];
    }

    env_->localBugAssertion(__FILE__, __LINE__, "finding average in list",
      newAvgIndex < sizeArrayLen, COMPLEX_ASSERTION);

    for (int i=0; i < nparts; i++){
      buf[i] = newAvgIndex;   // index to default part size
    }

    sum = (nparts - len) * allSizes[newAvgIndex];

    for (int i=0; i < len; i++){
      int id = ids[i];
      scalar_t size = sizes[i];
      int index;

      // Find the first size greater than or equal to this size.

      if (size < avg && avg - size <= epsilon)
        index = newAvgIndex;
      else{
        typename ArrayRCP<scalar_t>::iterator found =
          std::lower_bound(sizeArray.begin(), sizeArray.end(), size);

        env_->localBugAssertion(__FILE__, __LINE__, "size array",
          found != sizeArray.end(), COMPLEX_ASSERTION);

        index = found - sizeArray.begin();
      }

      buf[id] = index;
      sum += allSizes[index];
    }

    for (int i=0; i < sizeArrayLen; i++){
      sizeArray[i] /= sum;
    }

    pCompactIndex_[wdim] = partIdx;
    pSize_[wdim] = sizeArray;
  }
  else{
    // To have access to part sizes, we must store nparts scalar_ts on
    // every process.  We expect this is a rare case.

    tmp = new scalar_t [nparts];
    env_->localMemoryAssertion(__FILE__, __LINE__, nparts, tmp);

    sum += ((nparts - len) * avg);

    for (int i=0; i < nparts; i++){
      tmp[i] = avg/sum;
    }

    for (int i=0; i < len; i++){
      tmp[ids[i]] = sizes[i]/sum;
    }

    pSize_[wdim] = arcp(tmp, 0, nparts);
  }
}


template <typename Adapter>
  void PartitioningSolution<Adapter>::setParts(
    ArrayRCP<const gno_t> &gnoList, ArrayRCP<partId_t> &partList,
    bool dataDidNotMove)
{
  env_->debug(DETAILED_STATUS, "Entering setParts");

  size_t len = partList.size();

  // Find the actual number of parts in the solution, which can
  // be more or less than the nGlobalParts_ target.
  // (We may want to compute the imbalance of a given solution with
  // respect to a desired solution.  This solution may have more or
  // fewer parts that the desired solution.)

  partId_t lMax=0, lMin=0, gMax, gMin;

  if (len > 0)
    IdentifierTraits<partId_t>::minMax(partList.getRawPtr(), len, lMin, lMax);

  IdentifierTraits<partId_t>::globalMinMax(*comm_, len == 0,
    lMin, lMax, gMin, gMax);

  nGlobalPartsSolution_ = gMax - gMin + 1;

  if (dataDidNotMove) {
    // Case where the algorithm provides the part list in the same order
    // that it received the gnos.

    if (idMap_->gnosAreGids()){
      gids_ = Teuchos::arcp_reinterpret_cast<const gid_t>(gnoList);
    }
    else{
      gid_t *gidList = new gid_t [len];
      env_->localMemoryAssertion(__FILE__, __LINE__, len, gidList);
      ArrayView<gid_t> gidView(gidList, len);

      const gno_t *gnos = gnoList.getRawPtr();
      ArrayView<gno_t> gnoView(const_cast<gno_t *>(gnos), len);

      try{
        idMap_->gidTranslate(gidView, gnoView, TRANSLATE_LIB_TO_APP);
      }
      Z2_FORWARD_EXCEPTIONS

      gids_ = arcp<const gid_t>(gidList, 0, len);
    }
    parts_ = partList;
  }
  else {
    // Case where the algorithm moved the data.
    // KDDKDD Should we require the algorithm to return the parts in
    // KDDKDD the same order as the gnos provided by the model?

    // We send part information to the process that "owns" the global number.

    ArrayView<gid_t> emptyView;
    ArrayView<int> procList;

    if (len){
      int *tmp = new int [len];
      env_->localMemoryAssertion(__FILE__, __LINE__, len, tmp);
      procList = ArrayView<int>(tmp, len);
    }

    idMap_->gnoGlobalTranslate (gnoList.view(0,len), emptyView, procList);

    int remotelyOwned = 0;
    int rank = comm_->getRank();
    int nprocs = comm_->getSize();

    for (size_t i=0; !remotelyOwned && i < len; i++){
      if (procList[i] != rank)
        remotelyOwned = 1;
    }

    int anyRemotelyOwned=0;

    try{
      reduceAll<int, int>(*comm_, Teuchos::REDUCE_MAX, 1,
        &remotelyOwned, &anyRemotelyOwned);
    }
    Z2_THROW_OUTSIDE_ERROR(*env_);

    if (anyRemotelyOwned){

      // Send the owners of these gnos their part assignments.

      Array<int> countOutBuf(nprocs, 0);
      Array<int> countInBuf(nprocs, 0);

      Array<gno_t> outBuf(len*2, 0);

      if (len > 0){
        Array<lno_t> offsetBuf(nprocs+1, 0);

        for (size_t i=0; i < len; i++){
          countOutBuf[procList[i]]+=2;
        }

        offsetBuf[0] = 0;
        for (int i=0; i < nprocs; i++)
          offsetBuf[i+1] = offsetBuf[i] + countOutBuf[i];

        for (size_t i=0; i < len; i++){
          int p = procList[i];
          int off = offsetBuf[p];
          outBuf[off] = gnoList[i];
          outBuf[off+1] = static_cast<gno_t>(partList[i]);
          offsetBuf[p]+=2;
        }
      }

      ArrayRCP<gno_t> inBuf;

      try{
        AlltoAllv<gno_t>(*comm_, *env_,
          outBuf(), countOutBuf(), inBuf, countInBuf());
      }
      Z2_FORWARD_EXCEPTIONS;

      outBuf.clear();
      countOutBuf.clear();

      gno_t newLen = 0;
      for (int i=0; i < nprocs; i++)
        newLen += countInBuf[i];

      countInBuf.clear();

      newLen /= 2;

      ArrayRCP<partId_t> parts;
      ArrayRCP<const gno_t> myGnos;

      if (newLen > 0){

        gno_t *tmpGno = new gno_t [newLen];
        env_->localMemoryAssertion(__FILE__, __LINE__, newLen, tmpGno);

        partId_t *tmpPart = new partId_t [newLen];
        env_->localMemoryAssertion(__FILE__, __LINE__, newLen, tmpPart);

        size_t next = 0;
        for (lno_t i=0; i < newLen; i++){
          tmpGno[i] = inBuf[next++];
          tmpPart[i] = inBuf[next++];
        }

        parts = arcp(tmpPart, 0, newLen);
        myGnos = arcp(tmpGno, 0, newLen);
      }

      gnoList = myGnos;
      partList = parts;
      len = newLen;
    }

    delete [] procList.getRawPtr();

    if (idMap_->gnosAreGids()){
      gids_ = Teuchos::arcp_reinterpret_cast<const gid_t>(gnoList);
    }
    else{
      gid_t *gidList = new gid_t [len];
      env_->localMemoryAssertion(__FILE__, __LINE__, len, gidList);
      ArrayView<gid_t> gidView(gidList, len);

      const gno_t *gnos = gnoList.getRawPtr();
      ArrayView<gno_t> gnoView(const_cast<gno_t *>(gnos), len);

      try{
        idMap_->gidTranslate(gidView, gnoView, TRANSLATE_LIB_TO_APP);
      }
      Z2_FORWARD_EXCEPTIONS

      gids_ = arcp<const gid_t>(gidList, 0, len);
    }

    parts_ = partList;
  }

  // Now determine which process gets each object, if not one-to-one.

  if (!onePartPerProc_){

    int *procs = new int [len];
    env_->localMemoryAssertion(__FILE__, __LINE__, len, procs);
    procs_ = arcp<int>(procs, 0, len);

    partId_t *parts = partList.getRawPtr();

    if (procDist_.size() > 0){    // parts are not split across procs

      int procId;
      for (size_t i=0; i < len; i++){
        partToProcsMap(parts[i], procs[i], procId);
      }
    }
    else{  // harder - we need to split the parts across multiple procs

      lno_t *partCounter = new lno_t [nGlobalPartsSolution_];
      env_->localMemoryAssertion(__FILE__, __LINE__, nGlobalPartsSolution_,
        partCounter);

      int numProcs = comm_->getSize();

      //MD NOTE: there was no initialization for partCounter.
      //I added the line below, correct me if I am wrong.
      memset(partCounter, 0, sizeof(lno_t) * nGlobalPartsSolution_);

      for (ArrayRCP<partId_t>::size_type i=0; i < partList.size(); i++)
        partCounter[parts[i]]++;

      lno_t *procCounter = new lno_t [numProcs];
      env_->localMemoryAssertion(__FILE__, __LINE__, numProcs, procCounter);

      int proc1;
      int proc2 = partDist_[0];

      for (partId_t part=1; part < nGlobalParts_; part++){
        proc1 = proc2;
        proc2 = partDist_[part+1];
        int numprocs = proc2 - proc1;

        double dNum = partCounter[part];
        double dProcs = numprocs;

        //cout << "dNum:" << dNum << " dProcs:" << dProcs << endl;
        double each = floor(dNum/dProcs);
        double extra = fmod(dNum,dProcs);

        for (int proc=proc1, i=0; proc<proc2; proc++, i++){
          if (i < extra)
            procCounter[proc] = lno_t(each) + 1;
          else
            procCounter[proc] = lno_t(each);
        }
      }

      delete [] partCounter;

      for (ArrayRCP<partId_t>::size_type i=0; i < partList.size(); i++){
        if (partList[i] >= nGlobalParts_){
          // Solution has more parts that targeted.  These
          // objects just remain on this process.
          procs[i] = comm_->getRank();
          continue;
        }
        partId_t partNum = parts[i];
        proc1 = partDist_[partNum];
        proc2 = partDist_[partNum + 1];

        int proc;
        for (proc=proc1; proc < proc2; proc++){
          if (procCounter[proc] > 0){
            procs[i] = proc;
            procCounter[proc]--;
            break;
          }
        }
        env_->localBugAssertion(__FILE__, __LINE__, "part to proc",
          proc < proc2, COMPLEX_ASSERTION);
      }

      delete [] procCounter;
    }
  }

  // Now that parts_ info is back on home process, remap the parts.
  // TODO:  The parts will be inconsistent with the proc assignments after
  // TODO:  remapping.  This problem will go away after we separate process
  // TODO:  mapping from setParts.  But for MueLu's use case, the part
  // TODO:  remapping is all that matters; they do not use the process mapping.
  int doRemap = 0;
  const Teuchos::ParameterEntry *pe =
                 env_->getParameters().getEntryPtr("remap_parts");
  if (pe) doRemap = pe->getValue(&doRemap);
  if (doRemap) RemapParts();

  haveSolution_ = true;

  env_->memory("After Solution has processed algorithm's answer");
  env_->debug(DETAILED_STATUS, "Exiting setParts");
}

template <typename Adapter>
  template <typename Extra>
    size_t PartitioningSolution<Adapter>::convertSolutionToImportList(
      int numExtra, ArrayRCP<Extra> &xtraInfo,
      ArrayRCP<typename Adapter::gid_t> &imports,       // output
      ArrayRCP<Extra> &newXtraInfo) const               // output
{
  env_->localInputAssertion(__FILE__, __LINE__, "no solution yet",
    haveSolution_, BASIC_ASSERTION);
  env_->debug(DETAILED_STATUS, "Entering convertSolutionToImportList");

  int numProcs                = comm_->getSize();
  size_t localNumIds          = gids_.size();

  // How many to each process?
  Array<int> counts(numProcs, 0);
  if (onePartPerProc_)
    for (size_t i=0; i < localNumIds; i++)
      counts[parts_[i]]++;
  else
    for (size_t i=0; i < localNumIds; i++)
      counts[procs_[i]]++;

  Array<gno_t> offsets(numProcs+1, 0);
  for (int i=1; i <= numProcs; i++){
    offsets[i] = offsets[i-1] + counts[i-1];
  }

  Array<gid_t> gidList(localNumIds);
  Array<Extra> numericInfo;

  if (numExtra > 0)
    numericInfo.resize(localNumIds);

  if (onePartPerProc_){
    for (size_t i=0; i < localNumIds; i++){
      lno_t idx = offsets[parts_[i]];
      gidList[idx] = gids_[i];
      if (numExtra > 0)
        numericInfo[idx] = xtraInfo[i];
      offsets[parts_[i]] = idx + 1;
    }
  }
  else {
    for (size_t i=0; i < localNumIds; i++){
      lno_t idx = offsets[procs_[i]];
      gidList[idx] = gids_[i];
      if (numExtra > 0)
        numericInfo[idx] = xtraInfo[i];
      offsets[procs_[i]] = idx + 1;
    }
  }

  Array<int> recvCounts(numProcs, 0);

  try{
    AlltoAllv<gid_t>(*comm_, *env_, gidList(),
      counts(), imports, recvCounts());
  }
  catch (std::exception &e){
    throw std::runtime_error("alltoallv 1");
  }

  if (numExtra > 0){
    try{
      AlltoAllv<Extra>(*comm_, *env_, xtraInfo(),
        counts(), newXtraInfo, recvCounts());
    }
    catch (std::exception &e){
      throw std::runtime_error("alltoallv 2");
    }
  }

  env_->debug(DETAILED_STATUS, "Exiting convertSolutionToImportList");
  return imports.size();
}

template <typename Adapter>
  void PartitioningSolution<Adapter>::procToPartsMap(int procId,
    double &numParts, partId_t &partMin, partId_t &partMax) const
{
  if (onePartPerProc_){
    numParts = 1.0;
    partMin = partMax = procId;
  }
  else if (procDist_.size() > 0){
    partMin = procDist_[procId];
    partMax = procDist_[procId+1] - 1;
    numParts = procDist_[procId+1] - partMin;
  }
  else{
    // find the first p such that partDist_[p] > procId

    std::vector<int>::const_iterator entry;
    entry = std::upper_bound(partDist_.begin(), partDist_.end(), procId);

    size_t partIdx = entry - partDist_.begin();
    int numProcs = partDist_[partIdx] - partDist_[partIdx-1];
    partMin = partMax = int(partIdx) - 1;
    numParts = 1.0 / numProcs;
  }
}

template <typename Adapter>
  void PartitioningSolution<Adapter>::partToProcsMap(partId_t partId,
    int &procMin, int &procMax) const
{
  if (partId >= nGlobalParts_){
    // setParts() may be given an initial solution which uses a
    // different number of parts than the desired solution.  It is
    // still a solution.  We keep it on this process.
    procMin = procMax = comm_->getRank();
  }
  else if (onePartPerProc_){
    procMin = procMax = int(partId);
  }
  else if (procDist_.size() > 0){
    if (procDistEquallySpread_) {
      // Avoid binary search.
      double fProcs = comm_->getSize();
      double fParts = nGlobalParts_;
      double each = fParts / fProcs;
      procMin = int(partId / each);
      while (procDist_[procMin] > partId) procMin--;
      while (procDist_[procMin+1] <= partId) procMin++;
      procMax = procMin;
    }
    else {
      // find the first p such that procDist_[p] > partId.
      // For now, do a binary search.

      std::vector<partId_t>::const_iterator entry;
      entry = std::upper_bound(procDist_.begin(), procDist_.end(), partId);

      size_t procIdx = entry - procDist_.begin();
      procMin = procMax = int(procIdx) - 1;
    }
  }
  else{
    procMin = partDist_[partId];
    procMax = partDist_[partId+1] - 1;
  }
}

template <typename Adapter>
  bool PartitioningSolution<Adapter>::criteriaHaveSamePartSizes(
    int c1, int c2) const
{
  if (c1 < 0 || c1 >= weightDim_ || c2 < 0 || c2 >= weightDim_ )
    throw logic_error("criteriaHaveSamePartSizes error");

  bool theSame = false;

  if (c1 == c2)
    theSame = true;

  else if (pSizeUniform_[c1] == true && pSizeUniform_[c2] == true)
    theSame = true;

  else if (pCompactIndex_[c1].size() == pCompactIndex_[c2].size()){
    theSame = true;
    bool useIndex = pCompactIndex_[c1].size() > 0;
    if (useIndex){
      for (partId_t p=0; theSame && p < nGlobalParts_; p++)
        if (pSize_[c1][pCompactIndex_[c1][p]] !=
            pSize_[c2][pCompactIndex_[c2][p]])
          theSame = false;
    }
    else{
      for (partId_t p=0; theSame && p < nGlobalParts_; p++)
        if (pSize_[c1][p] != pSize_[c2][p])
          theSame = false;
    }
  }

  return theSame;
}

/*! \brief  Compute the assignment of parts to processes.
 *    \param doCheck  if true, do a global check to reconcile the
 *       numLocalParts and numGlobalParts values on all processes.
 *       If false, we assume numLocalParts and numGlobalParts are
 *       the same across processes.
 *   \param haveNumLocalParts true if this process set the num_local_parts
 *         parameter, false otherwise
 *   \param numLocalParts the number of local parts specified for this
 *        process as a parameter, if any
 *   \param haveNumGlobalParts true if this process set the num_global_parts
 *         parameter, false otherwise
 *   \param numGlobalParts the number of global parts specified by this
 *        process as a parameter, if any
 */

template <typename Adapter>
  void PartitioningSolution<Adapter>::partToProc(
    bool doCheck, bool haveNumLocalParts, bool haveNumGlobalParts,
    int numLocalParts, int numGlobalParts)
{
  int nprocs = comm_->getSize();
  ssize_t reducevals[4];
  ssize_t sumHaveGlobal=0, sumHaveLocal=0;
  ssize_t sumGlobal=0, sumLocal=0;
  ssize_t maxGlobal=0, maxLocal=0;
  ssize_t vals[4] = {haveNumGlobalParts, haveNumLocalParts,
      numGlobalParts, numLocalParts};

  partDist_.clear();
  procDist_.clear();

  if (doCheck){

    try{
      reduceAll<int, ssize_t>(*comm_, Teuchos::REDUCE_SUM, 4, vals, reducevals);
    }
    Z2_THROW_OUTSIDE_ERROR(*env_);

    sumHaveGlobal = reducevals[0];
    sumHaveLocal = reducevals[1];
    sumGlobal = reducevals[2];
    sumLocal = reducevals[3];

    env_->localInputAssertion(__FILE__, __LINE__,
      "Either all procs specify num_global/local_parts or none do",
      (sumHaveGlobal == 0 || sumHaveGlobal == nprocs) &&
      (sumHaveLocal == 0 || sumHaveLocal == nprocs),
      BASIC_ASSERTION);
  }
  else{
    if (haveNumLocalParts)
      sumLocal = numLocalParts * nprocs;
    if (haveNumGlobalParts)
      sumGlobal = numGlobalParts * nprocs;

    sumHaveGlobal = haveNumGlobalParts ? nprocs : 0;
    sumHaveLocal = haveNumLocalParts ? nprocs : 0;

    maxLocal = numLocalParts;
    maxGlobal = numGlobalParts;
  }

  if (!haveNumLocalParts && !haveNumGlobalParts){
    onePartPerProc_ = true;   // default if user did not specify
    return;
  }

  if (haveNumGlobalParts){
    if (doCheck){
      vals[0] = numGlobalParts;
      vals[1] = numLocalParts;
      try{
        reduceAll<int, ssize_t>(
          *comm_, Teuchos::REDUCE_MAX, 2, vals, reducevals);
      }
      Z2_THROW_OUTSIDE_ERROR(*env_);

      maxGlobal = reducevals[0];
      maxLocal = reducevals[1];

      env_->localInputAssertion(__FILE__, __LINE__,
        "Value for num_global_parts is different on different processes.",
        maxGlobal * nprocs == sumGlobal, BASIC_ASSERTION);
    }

    if (sumLocal){
      env_->localInputAssertion(__FILE__, __LINE__,
        "Sum of num_local_parts does not equal requested num_global_parts",
        sumLocal == numGlobalParts, BASIC_ASSERTION);

      if (sumLocal == nprocs && maxLocal == 1){
        onePartPerProc_ = true;   // user specified one part per proc
        return;
      }
    }
    else{
      if (maxGlobal == nprocs){
        onePartPerProc_ = true;   // user specified num parts is num procs
        return;
      }
    }
  }

  // If we are here, we do not have #parts == #procs.

  if (sumHaveLocal == nprocs){
    //
    // We will go by the number of local parts specified.
    //

    try{
      procDist_.resize(nprocs+1);
    }
    catch (std::exception &e){
      throw(std::bad_alloc());
    }

    int *procArray = &procDist_[0];

    try{
      partId_t tmp = partId_t(numLocalParts);
      gatherAll<int, partId_t>(*comm_, 1, &tmp, nprocs, procArray + 1);
    }
    Z2_THROW_OUTSIDE_ERROR(*env_);

    procArray[0] = 0;

    for (int proc=0; proc < nprocs; proc++)
      procArray[proc+1] += procArray[proc];
  }
  else{
    //
    // We will allocate global number of parts to the processes.
    //
    double fParts = numGlobalParts;
    double fProcs = nprocs;

    if (fParts < fProcs){

      try{
        partDist_.resize(size_t(fParts+1));
      }
      catch (std::exception &e){
        throw(std::bad_alloc());
      }

      int *partArray = &partDist_[0];

      double each = floor(fProcs / fParts);
      double extra = fmod(fProcs, fParts);
      partDist_[0] = 0;

      for (partId_t part=0; part < numGlobalParts; part++){
        int numOwners = int(each + ((part<extra) ? 1 : 0));
        partArray[part+1] = partArray[part] + numOwners;
      }

      env_->globalBugAssertion(__FILE__, __LINE__, "#parts != #procs",
        partDist_[numGlobalParts] == nprocs, COMPLEX_ASSERTION, comm_);
    }
    else if (fParts > fProcs){

      // User did not specify local number of parts per proc;
      // Distribute the parts evenly among the procs.

      procDistEquallySpread_ = true;

      try{
        procDist_.resize(size_t(fProcs+1));
      }
      catch (std::exception &e){
        throw(std::bad_alloc());
      }

      int *procArray = &procDist_[0];

      double each = floor(fParts / fProcs);
      double extra = fmod(fParts, fProcs);
      procArray[0] = 0;

      for (int proc=0; proc < nprocs; proc++){
        partId_t numParts = partId_t(each + ((proc<extra) ? 1 : 0));
        procArray[proc+1] = procArray[proc] + numParts;
      }

      env_->globalBugAssertion(__FILE__, __LINE__, "#parts != #procs",
        procDist_[nprocs] == numGlobalParts, COMPLEX_ASSERTION, comm_);
    }
    else{
      env_->globalBugAssertion(__FILE__, __LINE__,
        "should never get here", 1, COMPLEX_ASSERTION, comm_);
    }
  }
}

////////////////////////////////////////////////////////////////////
// Remap a new part assignment vector for maximum overlap with an input
// part assignment.
//
// Assumptions for this version:
//   input part assignment == processor rank for every local object.
//   assuming nGlobalParts_ <= num ranks
// TODO:  Write a version that takes the input part number as input;
//        this change requires input parts in adapters to be provided in
//        the Adapter.
// TODO:  For repartitioning, compare to old remapping results; see Zoltan1.

template <typename Adapter>
void PartitioningSolution<Adapter>::RemapParts()
{
  size_t len = parts_.size();

  partId_t me = comm_->getRank();
  int np = comm_->getSize();

  if (np < nGlobalParts_) {
    if (me == 0)
      cout << "Remapping not yet supported for "
           << "num_global_parts " << nGlobalParts_
           << " > num procs " << np << endl;
    return;
  }
  // Build edges of a bipartite graph with np + nGlobalParts_ vertices,
  // and edges between a process vtx and any parts to which that process'
  // objects are assigned.
  // Weight edge[parts_[i]] by the number of objects that are going from
  // this rank to parts_[i].
  // We use a std::map, assuming the number of unique parts in the parts_ array
  // is small to keep the binary search efficient.
  // TODO We use the count of objects to move; should change to SIZE of objects
  // to move; need SIZE function in Adapter.

  std::map<partId_t, long> edges;
  long lstaying = 0;  // Total num of local objects staying if we keep the
                      // current mapping. TODO:  change to SIZE of local objs
  long gstaying = 0;  // Total num of objects staying in the current partition

  for (size_t i = 0; i < len; i++) {
    edges[parts_[i]]++;                // TODO Use obj size instead of count
    if (parts_[i] == me) lstaying++;    // TODO Use obj size instead of count
  }

  Teuchos::reduceAll<int, long>(*comm_, Teuchos::REDUCE_SUM, 1,
                                &lstaying, &gstaying);
//TODO  if (gstaying == Adapter::getGlobalNumObjs()) return;  // Nothing to do

  partId_t *remap = NULL;

  int nedges = edges.size();

  // Gather the graph to rank 0.
  partId_t tnVtx = np + nGlobalParts_;  // total # vertices
  int *idx = NULL;    // Pointer index into graph adjacencies
  int *sizes = NULL;  // nedges per rank
  if (me == 0) {
    idx = new int[tnVtx+1];
    sizes = new int[np];
  }
  if (np > 1)
    Teuchos::gather<int, int>(&nedges, 1, sizes, 1, 0, *comm_);
  else
    sizes[0] = nedges;

  // prefix sum to build the idx array
  if (me == 0) {
    idx[0] = 0;
    for (int i = 0; i < np; i++)
      idx[i+1] = idx[i] + sizes[i];
  }

  // prepare to send edges
  int cnt = 0;
  partId_t *bufv = NULL;
  long *bufw = NULL;
  if (nedges) {
    bufv = new partId_t[nedges];
    bufw = new long[nedges];
    // Create buffer with edges (me, part[i]) and weight edges[parts_[i]].
    for (std::map<partId_t, long>::iterator it = edges.begin();
         it != edges.end(); it++) {
      bufv[cnt] = it->first;  // target part
      bufw[cnt] = it->second; // weight
      cnt++;
    }
  }

  // Prepare to receive edges on rank 0
  partId_t *adj = NULL;
  long *wgt = NULL;
  if (me == 0) {
//SYM    adj = new partId_t[2*idx[np]];  // need 2x space to symmetrize later
//SYM    wgt = new long[2*idx[np]];  // need 2x space to symmetrize later
    adj = new partId_t[idx[np]];
    wgt = new long[idx[np]];
  }

  Teuchos::gatherv<int, partId_t>(bufv, cnt, adj, sizes, idx, 0, *comm_);
  Teuchos::gatherv<int, long>(bufw, cnt, wgt, sizes, idx, 0, *comm_);
  delete [] bufv;
  delete [] bufw;

  // Now have constructed graph on rank 0.
  // Call the matching algorithm

  int doRemap;
  if (me == 0) {
    // We have the "LHS" vertices of the bipartite graph; need to create
    // "RHS" vertices.
    for (int i = 0; i < idx[np]; i++) {
      adj[i] += np;  // New RHS vertex number; offset by num LHS vertices
    }

    // Build idx for RHS vertices
    for (partId_t i = np; i < tnVtx; i++) {
      idx[i+1] = idx[i];  // No edges for RHS vertices
    }

#ifdef KDDKDD_DEBUG
    cout << "IDX ";
    for (partId_t i = 0; i <= tnVtx; i++) cout << idx[i] << " ";
    cout << endl;

    cout << "ADJ ";
    for (partId_t i = 0; i < idx[tnVtx]; i++) cout << adj[i] << " ";
    cout << endl;

    cout << "WGT ";
    for (partId_t i = 0; i < idx[tnVtx]; i++) cout << wgt[i] << " ";
    cout << endl;
#endif

    // Perform matching on the graph
    partId_t *match = new partId_t[tnVtx];
    for (partId_t i = 0; i < tnVtx; i++) match[i] = i;
    partId_t nmatches =
             Zoltan2::GreedyMWM<partId_t, long>(idx, adj, wgt, tnVtx, match);

#ifdef KDDKDD_DEBUG
    cout << "After matching:  " << nmatches << " found" << endl;
    for (partId_t i = 0; i < tnVtx; i++)
      cout << "match[" << i << "] = " << match[i]
           << ((match[i] != i &&
               (i < np && match[i] != i+np))
                  ? " *" : " ")
           << endl;
#endif

    // See whether there were nontrivial changes in the matching.
    bool nontrivial = false;
    if (nmatches) {
      for (partId_t i = 0; i < np; i++) {
        if ((match[i] != i) && (match[i] != (i+np))) {
          nontrivial = true;
          break;
        }
      }
    }

    // Process the matches
    if (nontrivial) {
      remap = new partId_t[nGlobalParts_];
      for (partId_t i = 0; i < nGlobalParts_; i++) remap[i] = -1;

      bool *used = new bool[nGlobalParts_];
      for (partId_t i = 0; i < nGlobalParts_; i++) used[i] = false;

      // First, process all matched parts
      for (partId_t i = 0; i < nGlobalParts_; i++) {
        partId_t tmp = i + np;
        if (match[tmp] != tmp) {
          remap[i] = match[tmp];
          used[match[tmp]] = true;
        }
      }

      // Second, process unmatched parts; keep same part number if possible
      for (partId_t i = 0; i < nGlobalParts_; i++) {
        if (remap[i] > -1) continue;
        if (!used[i]) {
          remap[i] = i;
          used[i] = true;
        }
      }

      // Third, process unmatched parts; give them the next unused part
      for (partId_t i = 0, uidx = 0; i < nGlobalParts_; i++) {
        if (remap[i] > -1) continue;
        while (used[uidx]) uidx++;
        remap[i] = uidx;
        used[uidx] = true;
      }
      delete [] used;
    }
    delete [] match;

#ifdef KDDKDD_DEBUG
    cout << "Remap vector: ";
    for (partId_t i = 0; i < nGlobalParts_; i++) cout << remap[i] << " ";
    cout << endl;
#endif

    long newgstaying = Zoltan2::measure_stays(remap, idx, adj, wgt,
                                              nGlobalParts_, np);
    doRemap = (newgstaying > gstaying);
    cout << "gstaying " << gstaying << " measure(input) "
         << Zoltan2::measure_stays(NULL, idx, adj, wgt,
                                   nGlobalParts_, np)
         << " newgstaying " << newgstaying
         << " nontrivial " << nontrivial
         << " doRemap " << doRemap << endl;
  }
  delete [] idx;
  delete [] sizes;
  delete [] adj;
  delete [] wgt;

  Teuchos::broadcast<int, int>(*comm_, 0, 1, &doRemap);

  if (doRemap) {
    if (me != 0) remap = new partId_t[nGlobalParts_];
    Teuchos::broadcast<int, partId_t>(*comm_, 0, nGlobalParts_, remap);
    for (size_t i = 0; i < len; i++) {
      parts_[i] = remap[parts_[i]];
    }
  }

  delete [] remap;  // TODO May want to keep for repartitioning as in Zoltan
}


}  // namespace Zoltan2

#endif
