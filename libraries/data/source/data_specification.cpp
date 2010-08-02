// Author(s): Jeroen Keiren, Jeroen van der Wulp, Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/data_specification.h
/// \brief The class data_specification.

#include <algorithm>
#include <functional>
#include <iostream>
#include <map>

#include "boost/bind.hpp"

#include "mcrl2/atermpp/algorithm.h"
#include "mcrl2/atermpp/substitute.h"
#include "mcrl2/core/detail/soundness_checks.h"
#include "mcrl2/data/map_substitution.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/data/detail/sequence_algorithm.h"
#include "mcrl2/data/application.h"
#include "mcrl2/data/print.h"
#include "mcrl2/data/detail/internal_format_conversion.h"
#include "mcrl2/data/detail/dependent_sorts.h"

namespace mcrl2 {

  namespace data {
    /// \cond INTERNAL_DOCS

    namespace detail {

      /**
       * \param[in] compatible whether the produced ATerm is compatible with the `format after type checking'
       *
       * The compatible transformation should eventually disappear, it is only
       * here for compatibility with the old parser, type checker and pretty
       * print implementations.
       **/
      atermpp::aterm_appl data_specification_to_aterm_data_spec(const data_specification& s)
      { 
        using namespace core::detail;
        
        if (s.m_data_specification_is_type_checked)
        { 
          return gsMakeDataSpec(
             gsMakeSortSpec(atermpp::convert< atermpp::aterm_list >(s.m_sorts) +
                            atermpp::convert< atermpp::aterm_list >(data_specification::aliases_const_range(s.m_aliases))),
             gsMakeConsSpec(atermpp::convert< atermpp::aterm_list >(data_specification::constructors_const_range(s.m_constructors))),
             gsMakeMapSpec(atermpp::convert< atermpp::aterm_list >(data_specification::constructors_const_range(s.m_mappings))),
             gsMakeDataEqnSpec(atermpp::convert< atermpp::aterm_list >(s.m_equations)));
        }
        else
        { 
          return s.m_non_typed_checked_data_spec;
        }
      }
    } // namespace detail
    /// \endcond


    class finiteness_helper 
    { protected:

        typedef std::set< sort_expression >             dependent_sort_set;
        data_specification const&                       m_specification;
        std::map< sort_expression, dependent_sort_set > m_dependent_sorts;
        std::multiset< sort_expression >                m_visiting;
        dependent_sort_set const& dependent_sorts(sort_expression const& s)
        {
          std::map< sort_expression, dependent_sort_set >::iterator i = m_dependent_sorts.find(s);
          if (i == m_dependent_sorts.end())
          {
            i = m_dependent_sorts.insert(i, std::make_pair(s, static_cast< dependent_sort_set const& >(
						data::find_dependent_sorts(m_specification, s))));
          }
          return i->second;
        }

        bool search(dependent_sort_set const& source, sort_expression const& s)
        {
          return source.find(s) != source.end();
        }

      public:

        finiteness_helper(data_specification const& specification) : m_specification(specification)
        { }

        bool is_finite(const sort_expression& s)
        {
          if (is_basic_sort(s))
          {
            return is_finite(basic_sort(s));
          }
          else if (is_container_sort(s))
          {
            return is_finite(container_sort(s));
          }
          else if (is_function_sort(s))
          {
            return is_finite(function_sort(s));
          }
          else if (is_structured_sort(s))
          {
            return is_finite(structured_sort(s));
          }

          return false;
        }

        bool is_finite(const basic_sort& s)
        {
          // sort_expression actual_sort = m_specification.find_referenced_sort(s);
          sort_expression actual_sort = s;

          if (actual_sort != s)
          {
            return is_finite(actual_sort);
          }
          else {
            m_visiting.insert(s);

            for (data_specification::constructors_const_range r(m_specification.constructors(s)); !r.empty(); r.advance_begin(1))
            {
              if (is_function_sort(r.front().sort()))
              {
                for (boost::iterator_range< dependent_sort_set::const_iterator > c(dependent_sorts(r.front().sort())); !c.empty(); c.advance_begin(1))
                {
                  if (!is_function_sort(c.front()))
                  {
                    if ((c.front() == s) || (m_visiting.find(c.front()) == m_visiting.end() && !is_finite(c.front())))
                    {
                      return false;
                    }
                  }
                }
              }
            }

            m_visiting.erase(m_visiting.find(s));
          }

          return !search(dependent_sorts(s), s) && !m_specification.constructors(actual_sort).empty();
        }

        bool is_finite(const function_sort& s)
        {
          for (boost::iterator_range< sort_expression_list::iterator > i(s.domain()); !i.empty(); i.advance_begin(1))
          {
            if (m_visiting.find(i.front()) == m_visiting.end() && !is_finite(i.front()))
            {
              return false;
            }
          }

          return (s.codomain() != s) ? is_finite(s.codomain()) : false;
        }

        bool is_finite(const container_sort& s)
        {
          return (s.container_name() == set_container()) ? is_finite(s.element_sort()) : false;
        }

        bool is_finite(const alias& s)
        {
          return is_finite(s.reference());
        }

        bool is_finite(const structured_sort& s)
        {
          m_visiting.insert(s);

          for (boost::iterator_range< dependent_sort_set::const_iterator > c(dependent_sorts(s)); !c.empty(); c.advance_begin(1))
          {
            if (m_visiting.find(c.front()) == m_visiting.end() && !is_finite(c.front()))
            {
              return false;
            }
          }

          m_visiting.erase(m_visiting.find(s));

          return true;
        }
    };

    /// \brief Checks whether a sort is certainly finite.
    ///
    /// \param[in] s A sort expression
    /// \return true if s can be determined to be finite,
    ///      false otherwise.
    bool data_specification::is_certainly_finite(const sort_expression& s) const
    {
      return finiteness_helper(*this).is_finite(s);
    }

    bool data_specification::is_well_typed() const
    {
      // check 1)
      if (!detail::check_data_spec_sorts(constructors(), m_sorts))
      {
        std::clog << "data_specification::is_well_typed() failed: not all of the sorts appearing in the constructors "
                  << pp(constructors()) << " are declared in " << pp(m_sorts) << std::endl;
        return false;
      }

      // check 2)
      if (!detail::check_data_spec_sorts(mappings(), m_sorts))
      {
        std::clog << "data_specification::is_well_typed() failed: not all of the sorts appearing in the mappings "
                  << pp(mappings()) << " are declared in " << pp(m_sorts) << std::endl;
        return false;
      }

      return true;
    }
    /// \endcond

    /// There are two types of representations of ATerms:
    ///  - the bare specification that does not contain constructor, mappings
    ///    and equations for system defined sorts
    ///  - specification that includes all system defined information (legacy)
    /// The last type must eventually disappear but is unfortunately still in
    /// use in a substantial amount of source code.
    /// Note, all sorts with name prefix @legacy_ are eliminated
    void data_specification::build_from_aterm(atermpp::aterm_appl const& term)
    { 
      assert(core::detail::check_rule_DataSpec(term));
      assert(m_data_specification_is_type_checked); // It is not allowed to build up the data
                                                    // structures on the basis of a non type checked
                                                    // data specification. It may contain undefined types
                                                    // and non typed identifiers, with which the data
                                                    // specification cannot deal properly.

      // Note backwards compatibility measure: alias is no longer a sort_expression
      atermpp::term_list< atermpp::aterm_appl >  term_sorts(atermpp::list_arg1(atermpp::arg1(term)));
      atermpp::term_list< function_symbol >      term_constructors(atermpp::list_arg1(atermpp::arg2(term)));
      atermpp::term_list< function_symbol >      term_mappings(atermpp::list_arg1(atermpp::arg3(term)));
      atermpp::term_list< data_equation >        term_equations(atermpp::list_arg1(atermpp::arg4(term)));

      // Store the sorts and aliases.
      for (atermpp::term_list_iterator< atermpp::aterm_appl > i = term_sorts.begin(); i != term_sorts.end(); ++i)
      { if (data::is_alias(*i)) // Compatibility with legacy code
        { // if (!detail::has_legacy_name(alias(*i).name()))
          { add_alias(*i);
          }
        }
        else 
        { add_sort(*i);
        } 
      }

      // Store the constructors.
      for (atermpp::term_list_iterator< function_symbol > i = term_constructors.begin(); i != term_constructors.end(); ++i)
      { 
        // m_constructors.insert(sort_to_symbol_map::value_type(i->sort().target_sort(), *i));
        add_constructor(*i);
      }

      // Store the mappings.
      for (atermpp::term_list_iterator< function_symbol > i = term_mappings.begin(); i != term_mappings.end(); ++i)
      { // m_mappings.insert(sort_to_symbol_map::value_type(i->sort().target_sort(), *i));
        add_mapping(*i);
      }

      // Store the equations.
      for (atermpp::term_list_iterator< data_equation > i = term_equations.begin(); i != term_equations.end(); ++i)
      { 
        add_equation(*i);
      }

      // data_is_not_necessarily_normalised_anymore();
    }
  } // namespace data
} // namespace mcrl2

