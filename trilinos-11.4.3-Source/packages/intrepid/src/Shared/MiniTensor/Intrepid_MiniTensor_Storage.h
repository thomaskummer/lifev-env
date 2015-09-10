// @HEADER
// ************************************************************************
//
//                           Intrepid Package
//                 Copyright (2007) Sandia Corporation
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
// Questions: Alejandro Mota (amota@sandia.gov)
//
// ************************************************************************
// @HEADER

#if !defined(Intrepid_MiniTensor_Storage_h)
#define Intrepid_MiniTensor_Storage_h

#include "Intrepid_MiniTensor_Definitions.h"
#include "Teuchos_ArrayRCP.hpp"

namespace Intrepid {
namespace MiniTensor {

///
/// Base Storage class. Simple linear access memory model.
///
template<typename T>
class Storage
{
public:
  ///
  /// Component type
  ///
  typedef T type;

  ///
  /// Default constructor
  ///
  Storage() {}

  ///
  /// Constructor to given size
  /// \param number_entries number of entries
  ///
  explicit
  Storage(Index const number_entries) {resize(number_entries);}

  ///
  /// Simple destructor
  ///
  virtual
  ~Storage() {}

  ///
  /// Entry access
  /// \param i the index
  ///
  virtual
  T const &
  operator[](Index const i) const = 0;

  ///
  /// Entry access
  /// \param i the index
  ///
  virtual
  T &
  operator[](Index const i) = 0;

  ///
  /// \return number of entries
  ///
  virtual
  Index
  size() const = 0;

  ///
  /// Resize the storage (assume destructive)
  /// \param number_entries number of entries
  ///
  virtual
  void
  resize(Index const number_entries) = 0;

  ///
  /// Clear the storage
  ///
  virtual
  void
  clear() = 0;

private:

  ///
  /// No copy constructor
  ///
  Storage(Storage<T> const & s);

  ///
  /// No copy assignment
  ///
  Storage<T> &
  operator=(Storage<T> const & s);

};

///
/// Storage with raw pointers
///
template<typename T>
class StorageRaw: public Storage<T>
{
public:
  ///
  /// Default constructor
  ///
  StorageRaw();

  ///
  /// Constructor to given size
  /// \param number_entries number of entries
 ///
  explicit
  StorageRaw(Index const number_entries);

  ///
  /// Simple destructor
  ///
  ~StorageRaw();

  ///
  /// Entry access
  /// \param i the index
  ///
  T const &
  operator[](Index const i) const;

  ///
  /// Entry access
  /// \param i the index
  ///
  T &
  operator[](Index const i);

  ///
  /// \return number of entries
  ///
  Index
  size() const;

  ///
  /// Resize the storage (assume destructive)
  /// \param number_entries number of entries
  ///
  void
  resize(Index const number_entries);

  ///
  /// Clear the storage
  ///
  void
  clear();

private:

  ///
  /// No copy constructor
  ///
  StorageRaw(StorageRaw<T> const & s);

  ///
  /// No copy assignment
  ///
  StorageRaw<T> &
  operator=(StorageRaw<T> const & s);

  ///
  /// Size
  ///
  Index
  size_;

  ///
  /// Raw pointer
  ///
  T *
  pointer_;

};

///
/// Storage with an STL vector
///
template<typename T>
class StorageSTLVector: public Storage<T>
{
public:
  ///
  /// Default constructor
  ///
  StorageSTLVector();

  ///
  /// Constructor to given size
  /// \param number_entries number of entries
  ///
  explicit
  StorageSTLVector(Index const number_entries);

  ///
  /// Simple destructor
  ///
  ~StorageSTLVector();

  ///
  /// Entry access
  /// \param i the index
  ///
  T const &
  operator[](Index const i) const;

  ///
  /// Entry access
  /// \param i the index
  ///
  T &
  operator[](Index const i);

  ///
  /// \return number of entries
  ///
  Index
  size() const;

  ///
  /// Resize the storage (assume destructive)
  /// \param number_entries number of entries
  ///
  void
  resize(Index const number_entries);

  ///
  /// Clear the storage
  ///
  void
  clear();

private:

  ///
  /// No copy constructor
  ///
  StorageSTLVector(StorageSTLVector<T> const & s);

  ///
  /// No copy assignment
  ///
  StorageSTLVector<T> &
  operator=(StorageSTLVector<T> const & s);

  ///
  /// STL Vector
  ///
  std::vector<T>
  storage_;

};

///
/// Storage with Teuchos RCP Array
///
template<typename T>
class StorageRCPArray: public Storage<T>
{
public:
  ///
  /// Default constructor
  ///
  StorageRCPArray();

  ///
  /// Constructor that initializes to NaNs
  /// \param number_entries number of entries
  ///
  explicit
  StorageRCPArray(Index const number_entries);

  ///
  /// Simple destructor
  ///
  ~StorageRCPArray();

  ///
  /// Entry access
  /// \param i the index
  ///
  T const &
  operator[](Index const i) const;

  ///
  /// Entry access
  /// \param i the index
  ///
  T &
  operator[](Index const i);

  ///
  /// \return number of entries
  ///
  Index
  size() const;

  ///
  /// Resize the storage (assume destructive)
  /// \param number_entries number of entries
  ///
  void
  resize(Index const number_entries);

  ///
  /// Clear the storage
  ///
  void
  clear();

private:

  ///
  /// No copy constructor
  ///
  StorageRCPArray(StorageRCPArray<T> const & s);

  ///
  /// No copy assignment
  ///
  StorageRCPArray<T> &
  operator=(StorageRCPArray<T> const & s);

  ///
  /// Teuchos RCP array
  ///
  Teuchos::ArrayRCP<T>
  storage_;

};

} // namespace MiniTensor
} // namespace Intrepid

#include "Intrepid_MiniTensor_Storage.i.h"

#endif // Intrepid_MiniTensor_Storage_h
