// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "mcrl2/atermpp/detail/function_symbol_pool.h"

#include "mcrl2/utilities/logger.h"
#include "mcrl2/utilities/unused.h"

#include <algorithm>

using namespace atermpp;
using namespace atermpp::detail;
using namespace mcrl2::utilities;

function_symbol_pool::function_symbol_pool()
{
  mcrl2::utilities::mcrl2_unused(m_function_symbols_creates);
  mcrl2::utilities::mcrl2_unused(m_function_symbols_hits);

  // Initialize the default function symbols.
  m_as_int = create("<aterm_int>", 0);
  m_as_list = create("<list_constructor>", 2);
  m_as_empty_list = create("<empty_list>", 0);

  // Initialize the global copies of these default function symbols.
  g_as_int = m_as_int;
  g_as_list = m_as_list;
  g_as_empty_list = m_as_empty_list;
}

function_symbol_pool::~function_symbol_pool()
{
  print_performance_stats();
}

function_symbol function_symbol_pool::create(const std::string& name, const std::size_t arity, const bool check_for_registered_functions)
{
  if (EnableFunctionSymbolMetrics) { ++m_function_symbols_creates; }

  auto it = m_symbol_set.find(name, arity);
  if (it != m_symbol_set.end())
  {
    if (EnableFunctionSymbolMetrics) { ++m_function_symbols_hits; }

    // The element already exists so return it.
    return function_symbol(_function_symbol::ref(&(*it)));
  }
  else
  {
    _function_symbol& symbol = m_symbol_set.emplace(name, arity);
    if (check_for_registered_functions)
    {
      // Check whether there is a registered prefix p such that name equal pn where n is a number.
      // In that case prevent that pn will be generated as a fresh function name.
      std::size_t start_of_index = name.find_last_not_of("0123456789") + 1;

      if (start_of_index < name.size()) // Otherwise there is no trailing number.
      {
        std::string potential_number = name.substr(start_of_index); // Get the trailing string after prefix_ of function_name.
        std::string prefix = name.substr(0, start_of_index);
        auto prefix_it = m_prefix_to_register_function_map.find(prefix);
        if (prefix_it != m_prefix_to_register_function_map.end())  // points to the prefix.
        {
          try
          {
            std::size_t number = std::stol(potential_number);
            *prefix_it->second = std::max(*prefix_it->second, number + 1); // Set the index belonging to the found prefix to at least a safe number+1.
          }
          catch (std::exception&)
          {
            // Can be std::invalid_argument or an out_of_range exception.
            // In both cases nothing needs to be done, and the exception can be ignored.
          }
        }
      }
    }

    return function_symbol(_function_symbol::ref(&symbol));
  }
}

void function_symbol_pool::destroy(_function_symbol* f)
{
  assert(f != nullptr);
  assert(f->reference_count() == 0);

  // Remove it from the function symbol pool.
  m_symbol_set.erase(*f);
}

void function_symbol_pool::deregister(const std::string& prefix)
{
  m_prefix_to_register_function_map.erase(prefix);
}

std::shared_ptr<std::size_t> function_symbol_pool::register_prefix(const std::string& prefix)
{
  auto it = m_prefix_to_register_function_map.find(prefix);
  if (it != m_prefix_to_register_function_map.end())
  {
    return it->second;
  }
  else
  {
    std::size_t index = get_sufficiently_large_postfix_index(prefix);
    std::shared_ptr<std::size_t> shared_index = std::make_shared<std::size_t>(index);
    m_prefix_to_register_function_map[prefix] = shared_index;
    return shared_index;
  }
}

std::size_t function_symbol_pool::get_sufficiently_large_postfix_index(const std::string& prefix) const
{
  std::size_t index = 0;
  for (const auto& f : m_symbol_set)
  {
    const std::string& function_name = f.name();

    if (function_name.compare(0, prefix.size(), prefix) == 0)   // The function name starts with the prefix
    {
      std::string potential_number = function_name.substr(prefix.size()); // Get the trailing string after prefix_ of function_name.
      std::size_t end_of_number;
      try
      {
        std::size_t number = std::stol(potential_number, &end_of_number);
        if (end_of_number == potential_number.size()) // A proper number was read.
        {
          if (number >= index)
          {
            index = number + 1;
          }
        }
      }
      catch (std::exception&)
      {
        // Can be std::invalid_argument or an out_of_range exception.
        // In both cases nothing needs to be done, and the exception can be ignored.
      }
    }
  }

  return index;
}

void function_symbol_pool::print_performance_stats() const noexcept
{
  if (EnableFunctionSymbolHashtableMetrics)
  {
    m_symbol_set.print_performance_statistics();
  }

  if (EnableFunctionSymbolMetrics)
  {
    mCRL2log(mcrl2::log::debug, "Performance") << "g_function_symbol_pool: Stores " << size() << " function symbols. create() found "
                                << m_function_symbols_hits
                                << " out of "
                                << m_function_symbols_creates
                                << " times ("
                                << static_cast<double>(m_function_symbols_hits) / static_cast<double>(m_function_symbols_creates) * 100
                                << " %)\n";
  }

  if (EnableReferenceCountMetrics)
  {
    mCRL2log(mcrl2::log::debug, "Performance") << "g_function_symbol_pool: all reference counts changed " << _function_symbol::reference_count_changes() << " times.\n";
  }
}
