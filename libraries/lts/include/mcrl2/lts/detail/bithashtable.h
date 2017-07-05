// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lts/bithashtable.h

#ifndef MCRL2_LTS_DETAIL_BITHASHTABLE_H
#define MCRL2_LTS_DETAIL_BITHASHTABLE_H

#include <vector>
// #include "mcrl2/utilities/exception.h"
#include "mcrl2/lps/next_state_generator.h"

namespace mcrl2
{
namespace lts
{

class bit_hash_table
{
  private:
    std::vector < bool > m_bit_hash_table;
    std::unordered_map<std::size_t, std::size_t> m_number_translator;

    std::size_t calc_hash(const lps::state& state)
    {
      std::hash<atermpp::aterm> hasher;
      return hasher(state) % m_bit_hash_table.size();
    }

  public:
    bit_hash_table()=default;

    bit_hash_table(const std::size_t size) :
      m_bit_hash_table(size,false)
    {};


    void add_states(lps::next_state_generator::transition_t::state_probability_list states)
    {
      for(lps::next_state_generator::transition_t::state_probability_list::const_iterator i=states.begin(); i!=states.end(); ++i)
      {
        add_state(i->state());
      }
    }

    std::pair<std::size_t, bool> add_state(const lps::state& state)
    {
      std::size_t i = calc_hash(state);
      bool is_new = !m_bit_hash_table[i];
      if (is_new)
      { 
        m_bit_hash_table[i] = true;
        std::size_t new_index=m_number_translator.size();
        m_number_translator[i]=new_index;
        return std::pair<std::size_t, bool>(new_index, true);
      }
      else
      {
        return std::pair<std::size_t, bool>(m_number_translator.at(i), false);
      }
    }

    std::size_t state_index(const lps::state& state)
    {
      assert(m_bit_hash_table[calc_hash(state)]);
      return m_number_translator.at(calc_hash(state));
    }
};


}
}

#endif // MCRL2_LTS_DETAIL_BITHASHTABLE_H

