/*
 * Copyright(C) 1999-2010
 * Sandia Corporation. Under the terms of Contract
 * DE-AC04-94AL85000 with Sandia Corporation, the U.S. Government retains
 * certain rights in this software.
 *         
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 * 
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *     * Neither the name of Sandia Corporation nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef IOSS_Iopx_Internals_h
#define IOSS_Iopx_Internals_h

#include <exodusII.h>
#include <vector>
#include <string>
#include <cstring>
#include "Ioss_ParallelUtils.h"

typedef int64_t entity_id;

namespace Ioss {
  
  class NodeBlock;
  class EdgeBlock;
  class FaceBlock;
  class ElementBlock;

  class NodeSet;
  class EdgeSet;
  class FaceSet;
  class ElementSet;

  class SideBlock;
  class SideSet;
}
  /*!
   * This set of classes provides a thin wrapper around the exodusII
   * internals.  It supplants several of the exodusII API calls in
   * order to avoid ncredef calls which totally rewrite the existing
   * database and can be very expensive.  These routines provide all
   * required variable, dimension, and attribute definitions to the
   * underlying netcdf file with only a single ncredef call.
   *
   * To use the application must create an Iopx::Internals instance
   * and call the Iopx::Internals::write_meta_data() function.  This
   * function requires several classes as arguments including:
   * <ul>
   * <li> Mesh -- defines mesh global metadata
   * <li> Block -- defines metadata for each block
   * <li> NodeSet -- defines metadata for each nodeset
   * <li> SideSet -- defines metadata for each sideset
   * <li> CommunicationMetaData -- global metadata relating to
   * parallel info.
   * </ul>
   *
   * Calling Iopx::Internals::write_meta_data(), replaces the
   * following exodusII and nemesis API calls:
   * <ul>
   * <li> ex_put_init(),
   * <li> ex_put_elem_block(),
   * <li> ex_put_node_set_param(),
   * <li> ex_put_side_set_param(),
   * <li> ne_put_init_info(),
   * <li> ne_put_loadbal_param(),
   * <li> ne_put_cmap_params(),
   * </ul>
   */
namespace Iopx {
  struct NodeBlock
  {
    NodeBlock() : name(""), id(0), entityCount(0), attributeCount(0), procOffset(0)
    {}

    NodeBlock(const NodeBlock &other) : name(other.name), id(other.id), entityCount(other.entityCount),
					attributeCount(other.attributeCount), procOffset(other.procOffset)
    {}

    NodeBlock(const Ioss::NodeBlock &other);

    NodeBlock& operator=(const NodeBlock& other);

    ~NodeBlock() {}

    bool operator==(const NodeBlock&) const;
    bool operator!=(const NodeBlock& other) const {return !(*this == other);}

    std::string name;
    entity_id id;
    int64_t entityCount;
    int64_t attributeCount;
    int64_t procOffset;
    private:
  };

  struct EdgeBlock
  {
    EdgeBlock() : name(""), id(0), entityCount(0), nodesPerEntity(0), attributeCount(0), procOffset(0)
    {
      std::strcpy(elType, "");
    }

    EdgeBlock(const EdgeBlock &other) : name(other.name), id(other.id), entityCount(other.entityCount),
					nodesPerEntity(other.nodesPerEntity), attributeCount(other.attributeCount),
					procOffset(other.procOffset)
    {
      std::strcpy(elType, other.elType);
    }

    EdgeBlock(const Ioss::EdgeBlock &other);

    EdgeBlock& operator=(const EdgeBlock& other);

    ~EdgeBlock() {}

    bool operator==(const EdgeBlock&) const;
    bool operator!=(const EdgeBlock& other) const {return !(*this == other);}

    char elType[MAX_STR_LENGTH+1];
    std::string name;
    entity_id id;
    int64_t entityCount;
    int64_t nodesPerEntity;
    int64_t attributeCount;
    int64_t procOffset;
    private:
  };

  struct FaceBlock
  {
    FaceBlock() : name(""), id(0), entityCount(0), nodesPerEntity(0), edgesPerEntity(0), attributeCount(0), procOffset(0)
    {
      std::strcpy(elType, "");
    }

    FaceBlock(const FaceBlock &other) : name(other.name), id(other.id), entityCount(other.entityCount),
					nodesPerEntity(other.nodesPerEntity), edgesPerEntity(other.edgesPerEntity),
					attributeCount(other.attributeCount), procOffset(other.procOffset)
    {
      std::strcpy(elType, other.elType);
    }

    FaceBlock(const Ioss::FaceBlock &other);

    FaceBlock& operator=(const FaceBlock& other);

    ~FaceBlock() {}

    bool operator==(const FaceBlock&) const;
    bool operator!=(const FaceBlock& other) const {return !(*this == other);}

    char elType[MAX_STR_LENGTH+1];
    std::string name;
    entity_id id;
    int64_t entityCount;
    int64_t nodesPerEntity;
    int64_t edgesPerEntity;
    int64_t attributeCount;
    int64_t procOffset;
    private:
  };

  struct ElemBlock
  {
    ElemBlock() : name(""), id(0), entityCount(0),
		  nodesPerEntity(0), edgesPerEntity(0), facesPerEntity(0),
		  attributeCount(0), procOffset(0)
    {
      std::strcpy(elType, "");
    }

    ElemBlock(const ElemBlock &other) : name(other.name), id(other.id), entityCount(other.entityCount),
					nodesPerEntity(other.nodesPerEntity),
					edgesPerEntity(other.edgesPerEntity),
					facesPerEntity(other.facesPerEntity),
					attributeCount(other.attributeCount),
					procOffset(other.procOffset)
    {
      std::strcpy(elType, other.elType);
    }

    ElemBlock(const Ioss::ElementBlock &other);

    ElemBlock& operator=(const ElemBlock& other);

    ~ElemBlock() {}

    bool operator==(const ElemBlock&) const;
    bool operator!=(const ElemBlock& other) const {return !(*this == other);}

    char elType[MAX_STR_LENGTH+1];
    std::string name;
    entity_id id;
    int64_t entityCount;
    int64_t nodesPerEntity;
    int64_t edgesPerEntity;
    int64_t facesPerEntity;
    int64_t attributeCount;
    int64_t procOffset;
    private:
  };

  struct NodeSet
  {
    NodeSet() : name(""), id(0), entityCount(0), dfCount(0), procOffset(0) { }
    NodeSet(const NodeSet &other) : name(other.name), id(other.id), entityCount(other.entityCount),
				    attributeCount(other.attributeCount), dfCount(other.dfCount),
				    procOffset(other.procOffset) {}
    NodeSet(const Ioss::NodeSet &other);
    bool operator==(const NodeSet&) const;
    bool operator!=(const NodeSet& other) const {return !(*this == other);}

    std::string name;
    entity_id id;
    int64_t entityCount;
    int64_t attributeCount;
    int64_t dfCount;
    int64_t procOffset;
  };

  struct EdgeSet
  {
    EdgeSet() : name(""), id(0), entityCount(0), dfCount(0), procOffset(0) { }
    EdgeSet(const EdgeSet &other) : name(other.name), id(other.id), entityCount(other.entityCount),
				    attributeCount(other.attributeCount), dfCount(other.dfCount),
				    procOffset(other.procOffset) {}
    EdgeSet(const Ioss::EdgeSet &other);
    bool operator==(const EdgeSet&) const;
    bool operator!=(const EdgeSet& other) const {return !(*this == other);}

    std::string name;
    entity_id id;
    int64_t entityCount;
    int64_t attributeCount;
    int64_t dfCount;
    int64_t procOffset;
  };

  struct FaceSet
  {
    FaceSet() : name(""), id(0), entityCount(0), dfCount(0), procOffset(0) { }
    FaceSet(const FaceSet &other) : name(other.name), id(other.id), entityCount(other.entityCount),
				    attributeCount(other.attributeCount), dfCount(other.dfCount),
				    procOffset(other.procOffset) {}
    FaceSet(const Ioss::FaceSet &other);
    bool operator==(const FaceSet&) const;
    bool operator!=(const FaceSet& other) const {return !(*this == other);}

    std::string name;
    entity_id id;
    int64_t entityCount;
    int64_t attributeCount;
    int64_t dfCount;
    int64_t procOffset;
  };

  struct ElemSet
  {
    ElemSet() : name(""), id(0), entityCount(0), dfCount(0), procOffset(0) { }
    ElemSet(const ElemSet &other) : name(other.name), id(other.id), entityCount(other.entityCount),
				    attributeCount(other.attributeCount), dfCount(other.dfCount),
				    procOffset(other.procOffset) {}
    ElemSet(const Ioss::ElementSet &other);
    bool operator==(const ElemSet&) const;
    bool operator!=(const ElemSet& other) const {return !(*this == other);}

    std::string name;
    entity_id id;
    int64_t entityCount;
    int64_t attributeCount;
    int64_t dfCount;
    int64_t procOffset;
  };

  struct SideSet
  {
    SideSet() : name(""), id(0), entityCount(0), dfCount(0), procOffset(0), dfProcOffset(0) { }
    SideSet(const Ioss::SideBlock &other);
    SideSet(const Ioss::SideSet   &other);
    bool operator==(const SideSet&) const;
    bool operator!=(const SideSet& other) const {return !(*this == other);}

    std::string name;
    entity_id id;
    int64_t entityCount;
    int64_t dfCount;
    int64_t procOffset;
    int64_t dfProcOffset;
  };

  class Redefine
  {
  public:
    explicit Redefine(int exoid);
    ~Redefine();

  private:
    Redefine(const Redefine& from); // do not implement
    Redefine& operator=(const Redefine& from); // do not implement
    int exodusFilePtr;
  };

  class Mesh
  {
  public:
    Mesh() :   dimensionality(0)
      {}

      Mesh(int dim, char* the_title)
	:  dimensionality(dim)
	{
	  std::strncpy(title, the_title, MAX_LINE_LENGTH+1);
	  title[MAX_LINE_LENGTH] = '\0';
	}

	char title[MAX_LINE_LENGTH+1];
	int dimensionality;

	std::vector<NodeBlock> nodeblocks;
	std::vector<EdgeBlock> edgeblocks;
	std::vector<FaceBlock> faceblocks;
	std::vector<ElemBlock> elemblocks;
	std::vector<NodeSet>   nodesets;
	std::vector<EdgeSet>   edgesets;
	std::vector<FaceSet>   facesets;
	std::vector<ElemSet>   elemsets;
	std::vector<SideSet>   sidesets;
  };

  class Internals
  {
  public:
    Internals(int exoid, int maximum_name_length, const Ioss::ParallelUtils &util);

    int write_meta_data(Mesh &mesh);

    /*!  A restart file may contain an attribute which contains
     *   information about the processor count and current processor id
     *   * when the file was written.  This code checks whether that
     *   information matches the current processor count and id.  If it
     *   * exists, but doesn't match, a warning message is printed.
     *   Eventually, this will be used to determine whether certain
     *   decomposition-related data in the file is valid or has been
     *   invalidated by a join/re-spread to a different number of
     *   processors.
     */
    bool check_processor_info(int processor_count, int processor_id);

    void update_last_time_attribute(double value);
    bool read_last_time_attribute(double *value);

  private:
    Internals(const Internals& from); // do not implement
    Internals& operator=(const Internals& from); // do not implement

    void get_global_counts(Mesh &mesh);
    
    int put_metadata(const Mesh &mesh);
    int put_metadata(const std::vector<NodeBlock> &nodeblocks);
    int put_metadata(const std::vector<EdgeBlock> &edgeblocks);
    int put_metadata(const std::vector<FaceBlock> &faceblocks);
    int put_metadata(const std::vector<ElemBlock> &elemblocks);

    int put_metadata(const std::vector<NodeSet> &nodesets);
    int put_metadata(const std::vector<EdgeSet> &edgesets);
    int put_metadata(const std::vector<FaceSet> &facesets);
    int put_metadata(const std::vector<ElemSet> &elemsets);

    int put_metadata(const std::vector<SideSet> &sidesets);

    int put_non_define_data(const Mesh &mesh);
    int put_non_define_data(const std::vector<NodeBlock> &nodeblocks);
    int put_non_define_data(const std::vector<EdgeBlock> &edgeblocks);
    int put_non_define_data(const std::vector<FaceBlock> &faceblocks);
    int put_non_define_data(const std::vector<ElemBlock> &elemblocks);

    int put_non_define_data(const std::vector<NodeSet> &nodesets);
    int put_non_define_data(const std::vector<EdgeSet> &edgesets);
    int put_non_define_data(const std::vector<FaceSet> &facesets);
    int put_non_define_data(const std::vector<ElemSet> &elemsets);

    int put_non_define_data(const std::vector<SideSet> &sidesets);

    int max_name_length() const {return maximumNameLength;}
    
    int exodusFilePtr;
    int nodeMapVarID[3];
    int elementMapVarID[2];
    int commIndexVar;
    int elemCommIndexVar;
    int maximumNameLength; 
    Ioss::ParallelUtils parallelUtil;
  };
}
#endif /* IOSS_Iopx_Internals_h */
