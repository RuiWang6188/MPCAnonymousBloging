/***************************************************************************************************
 * Copyright (c) 2017-2018, NVIDIA CORPORATION.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification, are permitted
 * provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright notice, this list of
 *       conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright notice, this list of
 *       conditions and the following disclaimer in the documentation and/or other materials
 *       provided with the distribution.
 *     * Neither the name of the NVIDIA CORPORATION nor the names of its contributors may be used
 *       to endorse or promote products derived from this software without specific prior written
 *       permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL NVIDIA CORPORATION BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TOR (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 **************************************************************************************************/
/*! \file
    \brief Defines container classes and iterators for managing a statically sized vector
      of boolean predicates.
*/
#pragma once

#include <assert.h>
#include <stdint.h>

#include "cutlass/cutlass.h"
#include "cutlass/shape.h"

#include "cutlass/util/platform.h"

namespace cutlass {

////////////////////////////////////////////////////////////////////////////////////////////////////

/*!@defgroup predicate_vector_concept Predicate Vector Concept
@{

Implementations of \ref predicate_vector_concept contain an ordered set of boolean predicates which
may be used as conditionals in other device-side operations. Both random access and iterators
offering sequential access are provided.

@par Predicate Vector
   A \ref predicate_vector_concept satisfies the following expressions
  - <b>at(int idx)</b> - returns the value of the indexed predicate
  - <b>set(int idx, bool value)</b> - sets the value of the indexed predicate
  - <b>begin()</b> - returns a \ref predicate_iterator_concept pointing to the first predicate

@}
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

/*!@defgroup predicate_iterator_concept Predicate Iterator Concept
@{

Implementations of \ref predicate_iterator_concept enables accessing and traversing elements of a
bit vector.

@par Const Predicate Iterator
  A const \ref predicate_iterator_concept satisfies the following expressions
 - <b>++it</b> increments the iterator to the next predicate
 - <b>*it</b> returns the value of the currently pointed-to predicate

@par Mutable Predicate Iterator
 A \ref predicate_iterator_concept that is non-const <b>also</b> satisfies the following expressions
 - <b>it.set(bool value)</b> sets the value of the currently pointed-to predicate

@}
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

/*!@defgroup predicate_tile_adapter Predicate Tile Adapter Concept
@{

Implementations of \ref predicate_tile_adapter provide a mapping between a the elements of a \ref
tile_traits_concept and a \ref predicate_vector_concept.

@par Predicate Tile Adapter
  A \ref predicate_tile_adapter satisfies the following expressions
 - <b>at(int d, int h, int w, int c)</b> - returns the value of a predicate corresponding to the
   access (d, h, w, c) within the tile.

@}
*/

////////////////////////////////////////////////////////////////////////////////////////////////////

/// Statically sized array of bits implementing @concept{predicate_vector_concept}.
template <
    /// Number of predicates conatined in predicate vector
    int kPredicates_,
    /// Number of predicates contained in each byte of internal storage
    int kPredicatesPerByte_ = 4,
    /// Location of first predicate within byte of internal storage
    int kPredicateStart_ = 0>
struct PredicateVector {
  /// Number of bits stored by the PredicateVector
  static int const kPredicates = kPredicates_;

  /// Number of bits stored within each byte of the predicate bit vector
  static int const kPredicatesPerByte = kPredicatesPerByte_;

  /// First bit withing each byte containing predicates
  static int const kPredicateStart = kPredicateStart_;

  // Make sure no one tries to put more than 8 bits in a byte :)
  static_assert(kPredicatesPerByte <= 8, "kPredicatesPerByte must fit within an actual byte");
  // Make sure the "offsetted" bits fit in one byte.
  static_assert(kPredicateStart + kPredicatesPerByte <= 8,
                "The offsetted predicates must fit within an actual byte.");

  /// Storage type of individual elements
  typedef uint32_t Storage;

  /// Number of bytes needed
  static int const kBytes = (kPredicates + kPredicatesPerByte - 1) / kPredicatesPerByte;

  /// Number of storage elements needed
  static int const kWordCount = (kBytes + sizeof(Storage) - 1) / sizeof(Storage);

 private:
  //
  // Data members
  //

  /// Words of bit vector
  Storage storageData[kWordCount];

  //
  // Methods
  //

  /// Computes the word and bit corresponding to a logical predicate index
  CUTLASS_HOST_DEVICE void computeStorageOffset(int &word, int &bit, int idx) const {
    CUTLASS_ASSERT(idx < kPredicates);

    int byte = (idx / kPredicatesPerByte);
    int bit_offset = (idx % kPredicatesPerByte);

    word = byte / sizeof(Storage);
    int byte_offset = (byte % sizeof(Storage));

    bit = byte_offset * 8 + bit_offset + kPredicateStart;
  }

  /// Accesses a given word with optional assertions
  CUTLASS_HOST_DEVICE Storage &storage(int word) {
    CUTLASS_ASSERT(word < kWordCount);
    return storageData[word];
  }

  /// Accesses a given word with optional assertions
  CUTLASS_HOST_DEVICE Storage const &storage(int word) const {
    CUTLASS_ASSERT(word < kWordCount);
    return storageData[word];
  }

 public:
  //
  // Iterator
  //

  /**
  * @brief A const iterator implementing \ref predicate_iterator_concept enabling sequential
  * read-only access to prediactes.
  * @concept{predicate_iterator_concept}
  */
  class ConstIterator {
    /// Reference to PredicateVector instance
    PredicateVector const &vec_;

    /// Index into PredicateVector
    int bit_;

   public:
    /// Copy constructor
    CUTLASS_HOST_DEVICE
    ConstIterator(ConstIterator const &it) : vec_(it.vec_), bit_(it.bit_) {}

    ///
    CUTLASS_HOST_DEVICE
    ConstIterator(PredicateVector const &_vec, int _start = 0) : vec_(_vec), bit_(_start) {}

    /// Pre-increment
    CUTLASS_HOST_DEVICE
    ConstIterator &operator++() {
      ++bit_;
      return *this;
    }

    /// Pre-decrement
    CUTLASS_HOST_DEVICE
    ConstIterator &operator--() {
      --bit_;
      return *this;
    }

    /// Post-increment
    CUTLASS_HOST_DEVICE
    ConstIterator operator++(int) {
      ConstIterator ret(*this);
      ret.bit_++;
      return ret;
    }

    /// Post-decrement
    CUTLASS_HOST_DEVICE
    ConstIterator operator--(int) {
      ConstIterator ret(*this);
      ret.bit_--;
      return ret;
    }

    /// Returns true if iterators point to the same bit
    CUTLASS_HOST_DEVICE
    bool operator==(ConstIterator const &it) const { return bit_ == it.bit_; }

    /// Returns false if iterators point to the same bit
    CUTLASS_HOST_DEVICE
    bool operator!=(ConstIterator const &it) const { return bit_ != it.bit_; }

    /// Dereferences iterator
    CUTLASS_HOST_DEVICE
    bool operator*() const { return vec_[bit_]; }
  };

  /**
  * @brief An iterator implementing \ref predicate_iterator_concept enabling sequential
  * read and write access to predicates.
  * @concept{predicate_iterator_concept}
  */
  class Iterator {
    /// Reference to PredicateVector instance
    PredicateVector &vec_;

    /// Index into PredicateVector
    int bit_;

   public:
    /// Copy constructor
    CUTLASS_HOST_DEVICE
    Iterator(Iterator const &it) : vec_(it.vec_), bit_(it.bit_) {}

    /// Constructs an iterator from a PredicateVector
    CUTLASS_HOST_DEVICE
    Iterator(PredicateVector &_vec, int _start = 0) : vec_(_vec), bit_(_start) {}

    /// Pre-increment
    CUTLASS_HOST_DEVICE
    Iterator &operator++() {
      ++bit_;
      return *this;
    }

    /// Pre-decrement
    CUTLASS_HOST_DEVICE
    Iterator &operator--() {
      --bit_;
      return *this;
    }

    /// Post-increment
    CUTLASS_HOST_DEVICE
    Iterator operator++(int) {
      Iterator ret(*this);
      ret.bit_++;
      return ret;
    }

    /// Post-decrement
    CUTLASS_HOST_DEVICE
    Iterator operator--(int) {
      Iterator ret(*this);
      ret.bit_--;
      return ret;
    }

    /// Returns true if iterators point to the same bit
    CUTLASS_HOST_DEVICE
    bool operator==(Iterator const &it) const { return bit_ == it.bit_; }

    /// Returns false if iterators point to the same bit
    CUTLASS_HOST_DEVICE
    bool operator!=(Iterator const &it) const { return bit_ != it.bit_; }

    /// Gets the bit at the pointed to location
    CUTLASS_HOST_DEVICE
    bool get() { return vec_[bit_]; }

    /// Dereferences iterator
    CUTLASS_HOST_DEVICE
    bool operator*() const { return vec_[bit_]; }

    /// Sets the bit at the pointed to location
    CUTLASS_HOST_DEVICE
    void set(bool value = true) { vec_.set(bit_, value); }
  };

  /// Iterator that always returns true
  struct TrivialIterator {
    /// Constructor
    CUTLASS_HOST_DEVICE
    TrivialIterator() {}

    /// Copy constructor
    CUTLASS_HOST_DEVICE
    TrivialIterator(Iterator const &it) {}

    /// Constructs an iterator from a PredicateVector
    CUTLASS_HOST_DEVICE
    TrivialIterator(PredicateVector const &_vec) {}

    /// Pre-increment
    CUTLASS_HOST_DEVICE
    TrivialIterator &operator++() { return *this; }

    /// Post-increment
    CUTLASS_HOST_DEVICE
    TrivialIterator operator++(int) { return *this; }

    /// Dereferences iterator
    CUTLASS_HOST_DEVICE
    bool operator*() const { return true; }
  };

 public:
  //
  // Methods
  //

  /// Initialize the predicate vector
  CUTLASS_HOST_DEVICE PredicateVector(bool value = true) { fill(value); }

  /// Fills all predicates with a given value
  CUTLASS_HOST_DEVICE void fill(bool value = true) {
    Storage item = (value ? ~Storage(0) : Storage(0));

    CUTLASS_PRAGMA_UNROLL
    for (int i = 0; i < kWordCount; ++i) {
      storage(i) = item;
    }
  }

  /// Accesses a bit within the predicate vector.
  CUTLASS_HOST_DEVICE bool operator[](int idx) const { return at(idx); }

  /// Accesses a bit within the predicate vector.
  CUTLASS_HOST_DEVICE bool at(int idx) const {
    int bit, word;
    computeStorageOffset(word, bit, idx);

    return ((storage(word) >> bit) & 1);
  }

  /// Set a bit within the predicate vector.
  CUTLASS_HOST_DEVICE void set(int idx, bool value = true) {
    int bit, word;
    computeStorageOffset(word, bit, idx);

    Storage disable_mask = (~(Storage(1) << bit));
    Storage enable_mask = (Storage(value) << bit);

    storage(word) = ((storage(word) & disable_mask) | enable_mask);
  }

  /// Computes the intersection of two identical predicate vectors.
  CUTLASS_HOST_DEVICE PredicateVector &operator&=(PredicateVector const &predicates) {
    CUTLASS_PRAGMA_UNROLL
    for (int i = 0; i < kWordCount; ++i) {
      storage(i) = (storage(i) & predicates.storage(i));
    }
    return *this;
  }

  /// Computes the union of two identical predicate vectors.
  CUTLASS_HOST_DEVICE PredicateVector &operator|=(PredicateVector const &predicates) {
    CUTLASS_PRAGMA_UNROLL
    for (int i = 0; i < kWordCount; ++i) {
      storage(i) = (storage(i) | predicates.storage(i));
    }
    return *this;
  }

  /// Returns true if entire predicate array is zero.
  CUTLASS_HOST_DEVICE bool is_zero() const {
    Storage mask(0);
    for (int byte = 0; byte < sizeof(Storage); ++byte) {
      Storage byte_mask = (((1 << kPredicatesPerByte) - 1) << kPredicateStart);
      mask |= (byte_mask << (byte * 8));
    }
    uint32_t result = 0;
    for (int word = 0; word < kWordCount; ++word) {
      result |= storage(word);
    }
    return result == 0;
  }

  /// Returns an iterator to the start of the bit vector
  CUTLASS_DEVICE
  Iterator begin() { return Iterator(*this); }

  /// Returns an iterator
  CUTLASS_DEVICE
  Iterator end() { return Iterator(*this, kPredicates); }

  /// Returns a ConstIterator
  CUTLASS_DEVICE
  ConstIterator const_begin() const { return ConstIterator(*this); }

  /// Returns a ConstIterator
  CUTLASS_DEVICE
  ConstIterator const_end() const { return ConstIterator(*this, kPredicates); }
};

////////////////////////////////////////////////////////////////////////////////////////////////////

/// Always returns true predicate.
struct TrivialPredicateTileAdapter {
  /// Ctor.
  CUTLASS_HOST_DEVICE TrivialPredicateTileAdapter() {}

  /// The value at location (d, h, w, c).
  CUTLASS_HOST_DEVICE bool at(int, int, int, int) const { return true; }
};

////////////////////////////////////////////////////////////////////////////////////////////////////

/// Adapter to enable random access to predicates via logical coordinate within a tile.
template <typename PredicateVector_, typename Iterations_>
struct PredicateTileAdapter {
  /// The vector of predicates.
  typedef PredicateVector_ PredicateVector;
  /// The iterations.
  typedef Iterations_ Iterations;

 private:
  /// The predicates.
  PredicateVector &predicates;

 public:
  /// Ctor.
  CUTLASS_DEVICE PredicateTileAdapter(PredicateVector &predicates_) : predicates(predicates_) {}

  /// Get the value at location (d, h, w, c).
  CUTLASS_DEVICE bool at(int d, int h, int w, int c) const {
    int const bit = ComputeOffsetFromShape<Iterations>::get(d, h, w, c);
    return predicates.at(bit);
  }

  /// Set the value at location (d, h, w, c).
  CUTLASS_DEVICE void set(int d, int h, int w, int c, bool value) {
    int const bit = ComputeOffsetFromShape<Iterations>::get(d, h, w, c);
    predicates.set(bit, value);
  }
};

////////////////////////////////////////////////////////////////////////////////////////////////////

/// Adapter to enable random access to predicates via logical coordinate within a tile.
template <typename PredicateVector_, typename Iterations_>
struct ConstPredicateTileAdapter {
  /// The vector of predicates.
  typedef PredicateVector_ PredicateVector;
  /// The iterations.
  typedef Iterations_ Iterations;

 private:
  /// The predicates.
  PredicateVector const &predicates;

 public:
  /// Ctor.
  CUTLASS_DEVICE ConstPredicateTileAdapter(PredicateVector const &predicates_)
      : predicates(predicates_) {}

  /// Get the value at location (d, h, w, c).
  CUTLASS_DEVICE bool at(int d, int h, int w, int c) const {
    int const bit = ComputeOffsetFromShape<Iterations>::get(d, h, w, c);
    return predicates.at(bit);
  }
};

////////////////////////////////////////////////////////////////////////////////////////////////////

}  // namespace cutlass
