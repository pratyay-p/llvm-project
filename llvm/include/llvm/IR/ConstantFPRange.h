//===- ConstantFPRange.h - Represent a range for floating-point -*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// Represent a range of possible values that may occur when the program is run
// for a floating-point value. This keeps track of a lower and upper bound for
// the constant.
//
// Range = [Lower, Upper] U (MayBeQNaN ? QNaN : {}) U (MayBeSNaN ? SNaN : {})
// Specifically, [inf, -inf] represents an empty set.
// Note:
// 1. Bounds are inclusive.
// 2. -0 is considered to be less than 0. That is, range [0, 0] doesn't contain
// -0.
// 3. Currently wrapping ranges are not supported.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_IR_CONSTANTFPRANGE_H
#define LLVM_IR_CONSTANTFPRANGE_H

#include "llvm/ADT/APFloat.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Support/Compiler.h"
#include <optional>

namespace llvm {

class raw_ostream;
struct KnownFPClass;

/// This class represents a range of floating-point values.
class [[nodiscard]] ConstantFPRange {
  APFloat Lower, Upper;
  bool MayBeQNaN : 1;
  bool MayBeSNaN : 1;

  /// Create empty constant range with same semantics.
  ConstantFPRange getEmpty() const {
    return ConstantFPRange(getSemantics(), /*IsFullSet=*/false);
  }

  /// Create full constant range with same semantics.
  ConstantFPRange getFull() const {
    return ConstantFPRange(getSemantics(), /*IsFullSet=*/true);
  }

  void makeEmpty();
  void makeFull();

  /// Initialize a full or empty set for the specified semantics.
  LLVM_ABI explicit ConstantFPRange(const fltSemantics &Sem, bool IsFullSet);

public:
  /// Initialize a range to hold the single specified value.
  LLVM_ABI explicit ConstantFPRange(const APFloat &Value);

  /// Initialize a range of values explicitly.
  /// Note: If \p LowerVal is greater than \p UpperVal, please use the canonical
  /// form [Inf, -Inf].
  LLVM_ABI ConstantFPRange(APFloat LowerVal, APFloat UpperVal, bool MayBeQNaN,
                           bool MayBeSNaN);

  /// Create empty constant range with the given semantics.
  static ConstantFPRange getEmpty(const fltSemantics &Sem) {
    return ConstantFPRange(Sem, /*IsFullSet=*/false);
  }

  /// Create full constant range with the given semantics.
  static ConstantFPRange getFull(const fltSemantics &Sem) {
    return ConstantFPRange(Sem, /*IsFullSet=*/true);
  }

  /// Helper for (-inf, inf) to represent all finite values.
  LLVM_ABI static ConstantFPRange getFinite(const fltSemantics &Sem);

  /// Helper for [-inf, inf] to represent all non-NaN values.
  LLVM_ABI static ConstantFPRange getNonNaN(const fltSemantics &Sem);

  /// Create a range which doesn't contain NaNs.
  static ConstantFPRange getNonNaN(APFloat LowerVal, APFloat UpperVal) {
    return ConstantFPRange(std::move(LowerVal), std::move(UpperVal),
                           /*MayBeQNaN=*/false, /*MayBeSNaN=*/false);
  }

  /// Create a range which may contain NaNs.
  static ConstantFPRange getMayBeNaN(APFloat LowerVal, APFloat UpperVal) {
    return ConstantFPRange(std::move(LowerVal), std::move(UpperVal),
                           /*MayBeQNaN=*/true, /*MayBeSNaN=*/true);
  }

  /// Create a range which only contains NaNs.
  LLVM_ABI static ConstantFPRange getNaNOnly(const fltSemantics &Sem,
                                             bool MayBeQNaN, bool MayBeSNaN);

  /// Produce the smallest range such that all values that may satisfy the given
  /// predicate with any value contained within Other is contained in the
  /// returned range.  Formally, this returns a superset of
  /// 'union over all y in Other . { x : fcmp op x y is true }'.  If the exact
  /// answer is not representable as a ConstantFPRange, the return value will be
  /// a proper superset of the above.
  ///
  /// Example: Pred = ole and Other = float [2, 5] returns Result = [-inf, 5]
  LLVM_ABI static ConstantFPRange
  makeAllowedFCmpRegion(FCmpInst::Predicate Pred, const ConstantFPRange &Other);

  /// Produce the largest range such that all values in the returned range
  /// satisfy the given predicate with all values contained within Other.
  /// Formally, this returns a subset of
  /// 'intersection over all y in Other . { x : fcmp op x y is true }'.  If the
  /// exact answer is not representable as a ConstantFPRange, the return value
  /// will be a proper subset of the above.
  ///
  /// Example: Pred = ole and Other = float [2, 5] returns [-inf, 2]
  LLVM_ABI static ConstantFPRange
  makeSatisfyingFCmpRegion(FCmpInst::Predicate Pred,
                           const ConstantFPRange &Other);

  /// Produce the exact range such that all values in the returned range satisfy
  /// the given predicate with any value contained within Other. Formally, this
  /// returns { x : fcmp op x Other is true }.
  ///
  /// Example: Pred = olt and Other = float 3 returns [-inf, 3)
  /// If the exact answer is not representable as a ConstantFPRange, returns
  /// std::nullopt.
  LLVM_ABI static std::optional<ConstantFPRange>
  makeExactFCmpRegion(FCmpInst::Predicate Pred, const APFloat &Other);

  /// Does the predicate \p Pred hold between ranges this and \p Other?
  /// NOTE: false does not mean that inverse predicate holds!
  LLVM_ABI bool fcmp(FCmpInst::Predicate Pred,
                     const ConstantFPRange &Other) const;

  /// Return the lower value for this range.
  const APFloat &getLower() const { return Lower; }

  /// Return the upper value for this range.
  const APFloat &getUpper() const { return Upper; }

  bool containsNaN() const { return MayBeQNaN || MayBeSNaN; }
  bool containsQNaN() const { return MayBeQNaN; }
  bool containsSNaN() const { return MayBeSNaN; }
  LLVM_ABI bool isNaNOnly() const;

  /// Get the semantics of this ConstantFPRange.
  const fltSemantics &getSemantics() const { return Lower.getSemantics(); }

  /// Return true if this set contains all of the elements possible
  /// for this data-type.
  LLVM_ABI bool isFullSet() const;

  /// Return true if this set contains no members.
  LLVM_ABI bool isEmptySet() const;

  /// Return true if the specified value is in the set.
  LLVM_ABI bool contains(const APFloat &Val) const;

  /// Return true if the other range is a subset of this one.
  LLVM_ABI bool contains(const ConstantFPRange &CR) const;

  /// If this set contains a single element, return it, otherwise return null.
  /// If \p ExcludesNaN is true, return the non-NaN single element.
  LLVM_ABI const APFloat *getSingleElement(bool ExcludesNaN = false) const;

  /// Return true if this set contains exactly one member.
  /// If \p ExcludesNaN is true, return true if this set contains exactly one
  /// non-NaN member.
  bool isSingleElement(bool ExcludesNaN = false) const {
    return getSingleElement(ExcludesNaN) != nullptr;
  }

  /// Return true if the sign bit of all values in this range is 1.
  /// Return false if the sign bit of all values in this range is 0.
  /// Otherwise, return std::nullopt.
  LLVM_ABI std::optional<bool> getSignBit() const;

  /// Return true if this range is equal to another range.
  LLVM_ABI bool operator==(const ConstantFPRange &CR) const;
  /// Return true if this range is not equal to another range.
  bool operator!=(const ConstantFPRange &CR) const { return !operator==(CR); }

  /// Return the FPClassTest which will return true for the value.
  LLVM_ABI FPClassTest classify() const;

  /// Print out the bounds to a stream.
  LLVM_ABI void print(raw_ostream &OS) const;

  /// Allow printing from a debugger easily.
  LLVM_ABI void dump() const;

  /// Return the range that results from the intersection of this range with
  /// another range.
  LLVM_ABI ConstantFPRange intersectWith(const ConstantFPRange &CR) const;

  /// Return the smallest range that results from the union of this range
  /// with another range.  The resultant range is guaranteed to include the
  /// elements of both sets, but may contain more.
  LLVM_ABI ConstantFPRange unionWith(const ConstantFPRange &CR) const;
};

inline raw_ostream &operator<<(raw_ostream &OS, const ConstantFPRange &CR) {
  CR.print(OS);
  return OS;
}

} // end namespace llvm

#endif // LLVM_IR_CONSTANTFPRANGE_H
