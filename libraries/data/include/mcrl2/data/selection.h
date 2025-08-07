// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/selection.h
/// \brief Provides selection utility functionality

#ifndef MCRL2_DATA_SELECTION_H
#define MCRL2_DATA_SELECTION_H

#include "mcrl2/data/data_specification.h"
#include "mcrl2/data/find.h"
#include "mcrl2/data/fset.h"



namespace mcrl2::data
{

/** \brief Component for selecting a subset of equations that are actually used in an encompassing specification
 *
 * This component can be used with the constructor of data::rewriter
 * derived classes to select a smaller set of equations that are used as
 * rewrite rules. This limited set of rewrite rules should be enough for the
 * purpose of rewriting objects that occur in the encompassing
 * specification (the context).
 *
 * \note Use of this component can have a dramatic effect of rewriter
 * initialisation time and overall performance.
 **/
class used_data_equation_selector
{
  private:

    std::set< function_symbol > m_used_symbols;

    bool add_all;

    void add_symbol(const function_symbol &f)
    {
      m_used_symbols.insert(f);
    }

    template < typename Range >
    void add_symbols(Range const& r)
    {
      m_used_symbols.insert(r.begin(), r.end());
    }

  protected:
    void add_data_specification_symbols(const data_specification& specification)
    {
      // Always add if:Bool#Bool#Bool->Bool; This symbol is used in the prover.
      add_symbol(if_(sort_bool::bool_()));
      // Always add and,or:Bool#Bool->Bool and not:Bool->Bool; This symbol is generated when eliminating quantifiers
      add_symbol(sort_bool::and_());
      add_symbol(sort_bool::or_());
      add_symbol(sort_bool::not_());

      // Add all constructors of all sorts as they may be used when enumerating over these sorts
      std::set< sort_expression > sorts(specification.sorts().begin(),specification.sorts().end());
      for (const sort_expression& sort: sorts)
      {
        add_symbols(specification.constructors(sort));
        // Always add equality and inequality of each sort. The one point rewriter is using this for instance.
        add_symbol(equal_to(sort));
        add_symbol(not_equal_to(sort));

        // Always add insert for an FSet(S) function, as it is used when enumerating the elements of an FSet.
        if (sort_fset::is_fset(sort))
        {
          const container_sort &s = atermpp::down_cast<container_sort>(sort);
          add_symbol(sort_fset::insert(s.element_sort()));
        }

        // Always add the if:Bool#S#S->S for every sort as enumerating elements over function sorts S1 #... # S # ... # Sn -> S
        // rewriting over functions of this shape.
        add_symbol(if_(sort));
      }

      std::map< data_equation, std::set< function_symbol > > symbols_for_equation;

      for (const data_equation& equation: specification.equations())
      {
        std::set< function_symbol > used_symbols;

        data::detail::make_find_function_symbols_traverser<data::data_expression_traverser>(std::inserter(used_symbols, used_symbols.end())).apply(equation.lhs());
        symbols_for_equation[equation].swap(used_symbols);
      }

      /* Calculate the closure under the function symbols that can be introduced by applying equations, such that all
         the function symbols that can occur during rewriting are present in the datatype */
      std::vector<bool> equations_of_which_symbols_have_been_added(specification.equations().size(),false);
      bool stable=false;
      while (!stable)
      {
        stable=true;
        std::size_t equation_count=0;
        for (const data_equation& e: specification.equations())
        {
          if (!equations_of_which_symbols_have_been_added[equation_count] &&
              std::includes(m_used_symbols.begin(), m_used_symbols.end(), symbols_for_equation[e].begin(), symbols_for_equation[e].end()))
          {
            add_function_symbols(e.rhs());
            add_function_symbols(e.condition());
            equations_of_which_symbols_have_been_added[equation_count]=true;
            stable=false;
          }
          equation_count++;
        }
      }
    }

  public:

    /// \brief Check whether the symbol is used.
    bool operator()(const data::function_symbol& f) const
    {
      if (add_all)
      {
        return true;
      }
      return m_used_symbols.count(f)>0;
    }

    /// \brief Check whether data equation relates to used symbols, and therefore is important.
    bool operator()(const data_equation& e) const
    {
      if (add_all)
      {
        return true;
      }

      std::set< function_symbol > used_symbols;

      data::detail::make_find_function_symbols_traverser<data::data_expression_traverser>(std::inserter(used_symbols, used_symbols.end())).apply(e.lhs());

      return std::includes(m_used_symbols.begin(), m_used_symbols.end(), used_symbols.begin(), used_symbols.end());
    }

    void add_function_symbols(const data_expression& t)
    {
      data::detail::make_find_function_symbols_traverser<data::data_expression_traverser>(std::inserter(m_used_symbols, m_used_symbols.end())).apply(t);
    }

    /// \brief default constructor
    used_data_equation_selector():
       add_all(true)
    {}

    /// \brief context is a range of function symbols
    template <typename Range>
    used_data_equation_selector(data_specification const& data_spec, Range const& context):
       add_all(false)
    {
      add_symbols(context);
      add_data_specification_symbols(data_spec);
    }

    used_data_equation_selector(const data_specification& specification,
                                const std::set<function_symbol>& function_symbols,
                                const std::set<data::variable>& global_variables,
                                const bool do_not_remove_function_symbols
                               ):add_all(do_not_remove_function_symbols)
    {
      // Compensate for symbols that could be used as part of an instantiation of free variables
      for (const variable& global_variable: global_variables)
      {
        add_symbols(specification.constructors(global_variable.sort()));
        add_symbols(specification.mappings(global_variable.sort()));
      }
      m_used_symbols.insert(function_symbols.begin(), function_symbols.end());
      add_data_specification_symbols(specification);
    }

    /// \brief select all equations
    used_data_equation_selector(const data_specification& /* specification */):
       add_all(true)
    {
      /* add_symbols(specification.constructors());
      add_symbols(specification.mappings());
      add_data_specification_symbols(specification); */
    }
};

} // namespace mcrl2::data



#endif // MCRL2_DATA_SELECTION_H
