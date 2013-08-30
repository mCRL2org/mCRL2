// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/atermpp/table.h
/// \brief Table.

#ifndef MCRL2_ATERMPP_TABLE_H
#define MCRL2_ATERMPP_TABLE_H

#include "mcrl2/atermpp/indexed_set.h"

namespace atermpp
{
/// \brief table. Has essentially the  same functionality as a std::map< term, term >;
class table:public indexed_set
{
  protected:

    std::vector < std::vector < aterm > > m_values;

  public:
    /// \brief Constructor Creates a table.
    /// This function creates an table given an initial size and a maximum load
    /// percentage. Whenever this percentage is exceeded (which is detected when a new entry is added
    /// using table_put), the table is automatically expanded and all existing entries are rehashed into
    /// the new table. If you know in advance approximately how many items will be in the table, you
    /// may set it up in such a way that no resizing (and thus no rehashing) is necessary. For example,
    /// if you expect about 1000 items in the table, you can create it with its initial size set to 1333 and
    /// a maximum load percentage of 75%. You are not required to do this, it merely saves a runtime
    /// expansion and rehashing of the table which increases efficiency.
    /// \param initial_size A positive integer
    /// \param max_load_pct A positive integer
    table(unsigned int initial_size = 100, unsigned int max_load_pct = 75):
            indexed_set(initial_size,max_load_pct)
    {}

    /// \brief Clears a table.
    /// This function resets an atermtable, without freeing the memory it occupies. Its
    /// effect is the same as the subsequent execution of a destroy and a create of a table, but as no
    /// memory is released and obtained from the C memory management system this function is gen-
    /// erally cheaper. but if subsequent tables differ very much in size, the use of table_destroy and
    /// table_create may be prefered, because in such a way the sizes of the table adapt automatically
    /// to the requirements of the application.
    void clear();

    /// \brief Add / update a (key, value)-pair in a table.
    /// If key does not already exist in the table, this function adds the (key, value)-pair
    /// to the table. Otherwise, it updates the value to value.
    /// \param key A key value.
    /// \param value A value.
    void put(const aterm &key, const aterm &value);

    /// \brief Get the value belonging to a given key in a table.
    /// \param key A key value.
    /// \return The corresponding value.
    const aterm &get(const aterm &key) const;

    /// \brief Remove the (key, value)-pair from table.
    /// \param key A key value.
    bool erase(const aterm &key);

    /// \brief Get a list of all the values in a table.
    /// This function can be useful if you need to iterate over all values in a table. It
    /// returns an term_list containing all the values in the table.
    /// \return A list containing the values of the table.
    aterm_list values() const;

};

} // namespace atermpp

#endif // MCRL2_ATERMPP_TABLE_H
