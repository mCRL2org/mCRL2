///////////////////////////////////////////////////////////////////////////////
/// \file sort_init.h
//
/// Convenience functions for creating sorts.

#ifndef LPE_SORT_INIT_H
#define LPE_SORT_INIT_H

#include "lpe/sort.h"
#include "libstruct.h"

namespace lpe {

namespace sort_init {

  /// Returns the predefined sort real.
  inline
  sort real()
  {
    return gsMakeSortExprReal();
  }
  
  /// Returns the predefined sort int.
  inline
  sort int_()
  {
    return gsMakeSortExprInt();
  }
  
  /// Returns the predefined sort pos.
  inline
  sort pos()
  {
    return gsMakeSortExprPos();
  }
  
  /// Returns the predefined sort nat.
  inline
  sort nat()
  {
    return gsMakeSortExprNat();
  }
  
  /// Returns the predefined sort int.
  inline
  sort bool_()
  {
    return gsMakeSortExprBool();
  }

  inline bool is_real(aterm_appl t) { return t == real(); }
  inline bool is_int (aterm_appl t) { return t == int_(); }
  inline bool is_pos (aterm_appl t) { return t == pos(); }
  inline bool is_nat (aterm_appl t) { return t == nat(); }                                 
  inline bool is_bool(aterm_appl t) { return t == bool_(); }                                 

} // namespace sort_init

} // namespace lpe

#endif // LPE_SORT_INIT_H
