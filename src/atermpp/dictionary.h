// ======================================================================
//
// Copyright (c) 2004, 2005 Wieger Wesselink
//
// ----------------------------------------------------------------------
//
// file          : atermpp/dictionary.h
// date          : 25-10-2005
// version       : 1.0
//
// author(s)     : Wieger Wesselink  <J.W.Wesselink@tue.nl>
//
// ======================================================================

#ifndef ATERM_DICTIONARY_H
#define ATERM_DICTIONARY_H

/** @file
  * This is a C++ wrapper around the ATerm library.
  */

#include "atermpp/aterm.h"

namespace atermpp
{
  //---------------------------------------------------------//
  //                     dictionary
  //---------------------------------------------------------//
  class dictionary: public aterm
  {
   public:
      /**
        * Create a new dictionary.
        **/
      dictionary()
        : aterm(ATdictCreate())
      {}

      /**
        * Get the value belonging to a given key in the dictionary.
        **/
      aterm get(aterm key)
      {
        return ATdictGet(to_ATerm(), key.to_ATerm());
      }
      
      /**
        * Add / update a (key, value)-pair in a dictionary.
        * If key does not already exist in the dictionary, this function adds the (key,
        * value)-pair to the dictionary. Otherwise, it updates the value to value.
        **/
      void put(aterm key, aterm value)
      {
        m_term = ATdictPut(to_ATerm(), key.to_ATerm(), value.to_ATerm());
      }
      
      /**
        * Remove the (key, value)-pair from the dictionary.
        * This function can be used to remove an entry from the dictionary.
        **/
      void dict_remove(aterm key)
      {
        m_term = ATdictRemove(to_ATerm(), key.to_ATerm());
      }
  };

} // namespace atermpp

#include "atermpp/aterm_make_match.h"

#endif // ATERM_DICTIONARY_H
