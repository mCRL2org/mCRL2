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

#include "mcrl2/data/data_expression.h"
#include "mcrl2/utilities/indexed_set.h"

namespace mcrl2::symbolic
{

/// \brief A bidirectional mapping between data expressions of a given sort and numbers
class data_expression_index
{
    friend std::ostream& operator<<(std::ostream&, const data_expression_index&);

  protected:
    data::sort_expression m_sort;
    mcrl2::utilities::indexed_set<data::data_expression> m_values;

  public:
    data_expression_index(const data::sort_expression& sort)
      : m_sort(sort)
    {}

    /// \brief Returns the index of the given value. If the value is not present yet, it will be added.
    std::uint32_t index(const data::data_expression& value)
    {
      assert(value.sort() == m_sort);
      return m_values.insert(value).first;
    }

    /// \brief Returns the value corresponding to index
    const data::data_expression& value(std::uint32_t i) const
    {
      return m_values[i];
    }

    const data::sort_expression& sort() const
    {
      return m_sort;
    }

    bool has_value(const data::data_expression& value) const
    {
      return m_values.find(value) != m_values.end();
    }

    bool has_index(std::uint32_t i) const
    {
      return i < m_values.size();
    }

    std::size_t size() const
    {
      return m_values.size();
    }

    auto begin() const
    {
      return m_values.begin();
    }

    auto end() const
    {
      return m_values.end();
    }
};

inline
std::ostream& operator<<(std::ostream& out, const data_expression_index& x)
{
  out << "data index with sort = " << x.sort() << " values = " << core::detail::print_list(x.m_values);
  return out;
}

} // namespace mcrl2::symbolic

#endif // MCRL2_SYMBOLIC_DATA_INDEX_H