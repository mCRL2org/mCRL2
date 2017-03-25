// Author(s): Wieger Wesselink, Jan Friso Groote. Based on the aterm library by Paul Klint and others.
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/atermpp/detail/function_symbol.h
/// \brief A foundational class for function symbols. 

#ifndef DETAIL_FUNCTION_SYMBOL_H
#define DETAIL_FUNCTION_SYMBOL_H

#include <string>

const size_t FUNCTION_SYMBOL_BLOCK_CLASS=14;
const size_t FUNCTION_SYMBOL_BLOCK_SIZE=1<<FUNCTION_SYMBOL_BLOCK_CLASS;
const size_t FUNCTION_SYMBOL_BLOCK_MASK=FUNCTION_SYMBOL_BLOCK_SIZE-1;


namespace atermpp
{
class function_symbol;
class function_symbol_generator;

namespace detail
{
size_t addressf(const function_symbol &t);

/* The type _function_symbol is used to store information about function_symbols. */
struct _function_symbol
{
    size_t arity;
    _function_symbol* next;
    mutable size_t reference_count;
    std::string name;
}; 

extern detail::_function_symbol** function_symbol_index_table;
extern size_t function_symbol_index_table_number_of_elements;

bool check_that_the_function_symbol_points_to_memory_containing_a_function(const detail::_function_symbol* f);

inline
bool is_valid_function_symbol(const detail::_function_symbol* f)
{
  return (f->reference_count>0) && check_that_the_function_symbol_points_to_memory_containing_a_function(f);
}

// set index such that no function symbol exists with the name 'prefix + std::to_string(n)'
// for all values n >= index
extern size_t get_sufficiently_large_postfix_index(const std::string& prefix_);

// Ugly class, to be replaced by a lambda function.
struct index_increaser
{
  size_t* m_initial_index;
  size_t* m_index;

  index_increaser()
   : m_index(nullptr)
  {}

  index_increaser(size_t& initial_index, size_t& index)
   : m_initial_index(&initial_index), m_index(&index)
  {}

  index_increaser(const index_increaser& other)
   : m_initial_index(other.m_initial_index),
     m_index(other.m_index)
  {}

  index_increaser& operator=(const index_increaser& other)
  {
    m_initial_index=other.m_initial_index;
    m_index=other.m_index;
    return *this;
  }

  void operator ()(size_t new_index)
  {
    *m_initial_index=new_index;
    if (*m_index<new_index)
    {
      *m_index=new_index;
    }
  }
};

// register a prefix for a function symbol, such that the index of this prefix can be increased when
// some other process makes a function symbol with the same prefix.
extern void register_function_symbol_prefix_string(const std::string& prefix, index_increaser& increase_index);

// deregister a prefix for a function symbol.
extern void deregister_function_symbol_prefix_string(const std::string& prefix);

} // namespace detail
} // namespace atermpp

#endif // DETAIL_FUNCTION_SYMBOL_H

