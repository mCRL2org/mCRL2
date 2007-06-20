// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file atermpp/dictionary.h
/// \brief Add your file description here.

#ifndef MCRL2_ATERMPP_DICTIONARY_H
#define MCRL2_ATERMPP_DICTIONARY_H

#include "atermpp/aterm.h"

namespace atermpp
{
  //---------------------------------------------------------//
  //                     dictionary
  //---------------------------------------------------------//
  class dictionary: public aterm
  {
   public:
      /// Create a new dictionary.
      ///
      dictionary()
        : aterm(ATdictCreate())
      {}

      /// Get the value belonging to a given key in the dictionary.
      ///
      aterm get(aterm key)
      {
        return ATdictGet(to_ATerm(), key.to_ATerm());
      }
      
      /// Add / update a (key, value)-pair in a dictionary.
      /// If key does not already exist in the dictionary, this function adds the (key,
      /// value)-pair to the dictionary. Otherwise, it updates the value to value.
      ///
      void put(aterm key, aterm value)
      {
        m_term = ATdictPut(to_ATerm(), key.to_ATerm(), value.to_ATerm());
      }
      
      /// Remove the (key, value)-pair from the dictionary.
      /// This function can be used to remove an entry from the dictionary.
      ///
      void dict_remove(aterm key)
      {
        m_term = ATdictRemove(to_ATerm(), key.to_ATerm());
      }
  };

} // namespace atermpp

#include "atermpp/aterm_make_match.h"

#endif // MCRL2_ATERMPP_DICTIONARY_H
