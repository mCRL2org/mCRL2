// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING).
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/atermpp/dictionary.h
/// \brief Dictionary based on ATerms.

#ifndef MCRL2_ATERMPP_DICTIONARY_H
#define MCRL2_ATERMPP_DICTIONARY_H

#include <boost/utility.hpp>
#include "mcrl2/atermpp/aterm.h"

namespace atermpp
{
  /// \brief Dictionary based on ATerms.
  ///
  class dictionary: public aterm, boost::noncopyable
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
        return ATdictGet(*this, key);
      }
      
      /// Add / update a (key, value)-pair in a dictionary.
      /// If key does not already exist in the dictionary, this function adds the (key,
      /// value)-pair to the dictionary. Otherwise, it updates the value to value.
      ///
      void put(aterm key, aterm value)
      {
        m_term = ATdictPut(*this, key, value);
      }
      
      /// Remove the (key, value)-pair from the dictionary.
      /// This function can be used to remove an entry from the dictionary.
      ///
      void remove(aterm key)
      {
        m_term = ATdictRemove(*this, key);
      }
  };

} // namespace atermpp

#include "mcrl2/atermpp/aterm_make_match.h"

#endif // MCRL2_ATERMPP_DICTIONARY_H
