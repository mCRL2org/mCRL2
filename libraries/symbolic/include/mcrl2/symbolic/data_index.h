// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MCRL2_SYMBOLIC_DATA_INDEX_H
#define MCRL2_SYMBOLIC_DATA_INDEX_H

#include "mcrl2/atermpp/standard_containers/indexed_set.h"
#include "mcrl2/utilities/configuration.h"
#include "mcrl2/core/detail/print_utility.h"
#include "mcrl2/data/data_expression.h"

namespace mcrl2::symbolic
{

/// \brief A bidirectional mapping between data expressions of a given sort and numbers.
/// \details It keeps the data expressions in a term container (atermpp::indexed_set), so that threads other than
///          the owner can insert: the keys are protected as a whole, not each by the thread that created it.
class data_expression_index : public atermpp::indexed_set<data::data_expression, mcrl2::utilities::detail::GlobalThreadSafe>
{
    using super = atermpp::indexed_set<data::data_expression, mcrl2::utilities::detail::GlobalThreadSafe>;

    friend std::ostream& operator<<(std::ostream&, const data_expression_index&);

  protected:
    data::sort_expression m_sort;

  public:
    data_expression_index(const data::sort_expression& sort)
      : m_sort(sort)
    {}

    /// \brief An index that number_of_threads threads can use at the same time (see utilities::indexed_set).
    data_expression_index(const data::sort_expression& sort, std::size_t number_of_threads)
      : super(number_of_threads),
        m_sort(sort)
    {}

    std::pair<size_type, bool> insert(const key_type& key)
    {
      assert(key.sort() == m_sort);
      return super::insert(key);
    }

    std::pair<size_type, bool> insert(const key_type& key, std::size_t thread_index)
    {
      assert(key.sort() == m_sort);
      return super::insert(key, thread_index);
    }
    
    const data::sort_expression& sort() const
    {
      return m_sort;
    }
};

inline
std::ostream& operator<<(std::ostream& out, const data_expression_index& x)
{
  out << "data index with sort = " << x.sort() << " values = " << core::detail::print_list(x);
  return out;
}

} // namespace mcrl2::symbolic

#endif // MCRL2_SYMBOLIC_DATA_INDEX_H
