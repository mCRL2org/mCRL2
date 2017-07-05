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
#include <unordered_map>

namespace atermpp
{
class function_symbol;
class function_symbol_generator;

namespace detail
{
struct constant_function_symbols;
void initialise_aterm_administration();
void initialise_function_map_administration();

// A function is determined by its name and its arity. 
class _function_symbol_primary_data
{
  protected:
    const std::string m_name;  // The primary data cannot be altered after creation. 
    const std::size_t m_arity;

  public:
    _function_symbol_primary_data(const std::string& name, std::size_t arity)
     : m_name(name), m_arity(arity)
    {}

    const std::string& name() const
    {
      return m_name;
    }

    std::size_t arity() const
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
    std::size_t m_reference_count;

  public:

    _function_symbol_auxiliary_data(std::size_t reference_count)
     : m_reference_count(reference_count)
    {}

    std::size_t reference_count() const
    {
      return m_reference_count;
    }

    std::size_t& reference_count()
    {
      return m_reference_count;
    }
};

// set index such that no function symbol exists with the name 'prefix + std::to_string(n)'
// for all values n >= index
extern std::size_t get_sufficiently_large_postfix_index(const std::string& prefix_);

// Ugly class, to be replaced by a lambda function.
struct index_increaser
{
  std::size_t* m_initial_index;
  std::size_t* m_index;

  index_increaser()
   : m_index(nullptr)
  {}

  index_increaser(std::size_t& initial_index, std::size_t& index)
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

  void operator ()(std::size_t new_index)
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

// Specialisation of the standard hash function for _function_symbol.
template<>
struct hash<atermpp::detail::_function_symbol_primary_data>
{
  std::size_t operator()(const atermpp::detail::_function_symbol_primary_data& f) const
  {
    std::hash<std::string> string_hasher;
    std::size_t h=string_hasher(f.name());
    return (h ^ f.arity());
  }
};
}

namespace atermpp
{
namespace detail
{

// A function symbol is a pair of essential and auxiliary information, stored in an unordered set.
typedef std::pair<const _function_symbol_primary_data, _function_symbol_auxiliary_data> _function_symbol;


// This is the class to store function symbols. It is just an unordered map.
class function_symbol_store_class: 
        public std::unordered_map<detail::_function_symbol_primary_data, 
                                  detail::_function_symbol_auxiliary_data>
{
  protected:
    bool& m_function_symbol_store_is_defined;

  public:
  
    // Constructor. Also initialise other basic aterm data structures.
    function_symbol_store_class(bool& function_symbol_store_is_defined)
      : m_function_symbol_store_is_defined(function_symbol_store_is_defined)
    {
      if (!m_function_symbol_store_is_defined)
      {
        m_function_symbol_store_is_defined=true;
      }
    }

    // \brief Destructor. Indicate that the function store does not exist anymore.
    ~function_symbol_store_class()
    {
      m_function_symbol_store_is_defined=false;
    }
};
} // namespace detail
} // namespace atermpp





#endif // DETAIL_FUNCTION_SYMBOL_H

