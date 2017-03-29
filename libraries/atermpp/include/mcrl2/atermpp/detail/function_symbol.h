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

// const size_t FUNCTION_SYMBOL_BLOCK_CLASS=14;
// const size_t FUNCTION_SYMBOL_BLOCK_SIZE=1<<FUNCTION_SYMBOL_BLOCK_CLASS;
// const size_t FUNCTION_SYMBOL_BLOCK_MASK=FUNCTION_SYMBOL_BLOCK_SIZE-1;


namespace atermpp
{
class function_symbol;
class function_symbol_generator;

namespace detail
{
size_t addressf(const function_symbol &t);
void initialise_aterm_administration();
void initialise_function_map_administration();

// A function is determined by its name and its arity. 
class _function_symbol_primary_data
{
  protected:
    const std::string m_name;  // The primary data cannot be altered after creation. 
    const size_t m_arity;

  public:
    _function_symbol_primary_data(const std::string& name, size_t arity)
     : m_name(name), m_arity(arity)
    {}

    const std::string& name() const
    {
      return m_name;
    }

    size_t arity() const
    {
      return m_arity;
    }

    bool operator==(const _function_symbol_primary_data& f) const
    {
      return m_arity==f.m_arity && m_name==f.m_name;
    }
};

// Each function symbol has a reference count, and sometimes a sequence number
// as special information. 
class _function_symbol_auxiliary_data
{
  protected:
    size_t m_reference_count;

  public:

    _function_symbol_auxiliary_data(size_t reference_count)
     : m_reference_count(reference_count)
    {}

    size_t reference_count() const
    {
      return m_reference_count;
    }

    size_t& reference_count()
    {
      return m_reference_count;
    }
};

// A function symbol is a pair of essential and auxiliary information, stored in an unordered set.
typedef std::pair<const _function_symbol_primary_data, _function_symbol_auxiliary_data> _function_symbol;


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

namespace std
{

// Specialisation of the standard hash function for 
template<>
struct hash<atermpp::detail::_function_symbol_primary_data>
{
  std::size_t operator()(const atermpp::detail::_function_symbol_primary_data& f) const
  {
    std::hash<std::string> string_hasher;
    size_t h=string_hasher(f.name());
    return (h ^ f.arity());
  }
};
}

#endif // DETAIL_FUNCTION_SYMBOL_H

