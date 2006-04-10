// ======================================================================
//
// Copyright (c) 2004, 2005 Wieger Wesselink
//
// ----------------------------------------------------------------------
//
// file          : atermpp/table.h
// date          : 25-10-2005
// version       : 1.0
//
// author(s)     : Wieger Wesselink  <J.W.Wesselink@tue.nl>
//
// ======================================================================

///////////////////////////////////////////////////////////////////////////////
/// \file aterm_table.h
/// Contains the definition of the aterm_table class.

#ifndef ATERM_TABLE_H
#define ATERM_TABLE_H

#include "atermpp/aterm.h"
#include "atermpp/aterm_list.h"

namespace atermpp
{
  //---------------------------------------------------------//
  //                     table
  //---------------------------------------------------------//
  class table
  {
   protected:
      ATermTable m_table;

   public:
      /// Create an table.
      /// This function creates an table given an initial size and a maximum load
      /// percentage. Whenever this percentage is exceeded (which is detected when a new entry is added
      /// using table_put), the table is automatically expanded and all existing entries are rehashed into
      /// the new table. If you know in advance approximately how many items will be in the table, you
      /// may set it up in such a way that no resizing (and thus no rehashing) is necessary. For example,
      /// if you expect about 1000 items in the table, you can create it with its initial size set to 1333 and
      /// a maximum load percentage of 75%. You are not required to do this, it merely saves a runtime
      /// expansion and rehashing of the table which increases efficiency.
      ///
      table(unsigned int initial_size, unsigned int max_load_pct)
        : m_table(ATtableCreate(initial_size, max_load_pct))
      {}
      
      /// Destroy an table.
      /// Contrary to aterm_dictionaries, aterm_tables are themselves not aterms. This
      /// means they are not freed by the garbage collector when they are no longer referred to. Therefore,
      /// when the table is no longer needed, the user should release the resources allocated by the table
      /// by calling table_destroy. All references the table has to aterms will then also be removed, so
      /// that those may be freed by the garbage collector (if no other references to them exist of course).
      ///
      ~table()
      {
        ATtableDestroy(m_table);
      }
      
      /// Reset an table.
      /// This function resets an ermtable, without freeing the memory it occupies. Its
      /// effect is the same as the subsequent execution of a destroy and a create of a table, but as no
      /// memory is released and obtained from the C memeory management system this function is gen-
      /// erally cheaper. but if subsequent tables differ very much in size, the use of table_destroy and
      /// table_create may be prefered, because in such a way the sizes of the table adapt automatically
      /// to the requirements of the application.
      ///
      void reset()
      {
        ATtableReset(m_table);
      }
      
      /// Add / update a (key, value)-pair in a table.
      /// If key does not already exist in the table, this function adds the (key, value)-pair
      /// to the table. Otherwise, it updates the value to value.
      ///
      void put(aterm key, aterm value)
      {
        ATtablePut(m_table, key.to_ATerm(), value.to_ATerm());
      }
      
      /// Get the value belonging to a given key in a table.
      ///
      aterm get(aterm key)
      {
        return ATtableGet(m_table, key.to_ATerm());
      }
      
      /// Remove the (key, value)-pair from table.
      ///
      void remove(aterm key)
      {
        ATtableRemove(m_table, key.to_ATerm());
      }
      
      /// Get an term_list of all the keys in a table.
      /// This function can be useful if you need to iterate over all elements in a table. It
      /// returns an term_list containing all the keys in the table. The corresponding values of each key
      /// you are interested in can then be retrieved through respective calls to table_get.
      ///
      term_list table_keys()
      {
        return term_list(ATtableKeys(m_table));
      }
  };

} // namespace atermpp

#endif // ATERM_TABLE_H
