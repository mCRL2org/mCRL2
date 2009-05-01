// Author(s): Jeroen Keiren, Jeroen van der Wulp
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

#include "boost/bind.hpp"

#include "mcrl2/atermpp/algorithm.h"
#include "mcrl2/core/detail/soundness_checks.h"
#include "mcrl2/core/print.h"
#include "mcrl2/data/map_substitution_adapter.h"
#include "mcrl2/data/utility.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/data/detail/sequence_algorithm.h"
#include "mcrl2/data/detail/data_utility.h"
#include "mcrl2/data/bool.h"
#include "mcrl2/data/pos.h"
#include "mcrl2/data/nat.h"
#include "mcrl2/data/int.h"
#include "mcrl2/data/real.h"
#include "mcrl2/data/list.h"
#include "mcrl2/data/set.h"
#include "mcrl2/data/bag.h"
#include "mcrl2/data/find.h"
#include "mcrl2/data/structured_sort.h"

namespace mcrl2 {

  namespace data {
    /// \cond INTERNAL_DOCS
    namespace detail {

      /// \brief Specialised remove_if for manipulating std::set, std::map, std::multimap containers
      template < typename Container, typename Predicate >
      void remove_if(Container& container, Predicate p) {
        Container temporary;

        std::remove_copy_if(container.begin(), container.end(), std::inserter(temporary, temporary.end()), p);

        container.swap(temporary);
      }

      template < typename Expression >
      inline bool is_system_defined(data_specification const& s, Expression const& c)
      {
        return s.is_system_defined(c);
      }

      /// \brief Set of sorts that depend on the sorts that are added
      class dependent_sort_helper
      {
        public:

          typedef std::set< sort_expression >::const_iterator iterator;
          typedef std::set< sort_expression >::const_iterator const_iterator;

        private:

          data_specification const&   m_specification;

          std::set< sort_expression > m_visited_sorts;

          std::set< sort_expression > m_result;

        private:

          void add_generic_and_check(const sort_expression& s)
          {
            if (m_visited_sorts.find(s) == m_visited_sorts.end())
            {
              m_visited_sorts.insert(s);
              m_result.insert(s);

              add_generic(s);
            }
          }

          void add_generic(const sort_expression& s)
          {
            if (s.is_basic_sort())
            {
              sort_expression actual_sort = m_specification.find_referenced_sort(s);

              if (actual_sort != s && !actual_sort.is_basic_sort()) {
                add_generic_and_check(actual_sort);
              }
              else {
                for (data_specification::constructors_const_range r(m_specification.constructors(actual_sort)); !r.empty(); r.advance_begin(1))
                {
                  if (r.front().sort().is_function_sort())
                  {
                    function_sort f_sort(r.front().sort());

                    for (function_sort::domain_const_range i(f_sort.domain()); !i.empty(); i.advance_begin(1))
                    {
                      add_generic_and_check(i.front());
                    }
                  }
                }
              }
            }
            else if (s.is_container_sort())
            {
              add_generic_and_check(container_sort(s).element_sort());
            }
            else if (s.is_function_sort())
            {
              function_sort f_sort(s);

              for (function_sort::domain_const_range i(f_sort.domain()); !i.empty(); i.advance_begin(1))
              {
                add_generic_and_check(i.front());
              }

              add_generic_and_check(f_sort.codomain());
            }
            else if (s.is_structured_sort())
            {
              structured_sort::constructor_const_range scl(structured_sort(s).struct_constructors());

              for (structured_sort::constructor_const_range::const_iterator i = scl.begin(); i != scl.end(); ++i)
              {
                for (structured_sort_constructor::arguments_const_range j(i->arguments()); !j.empty(); j.advance_begin(1))
                {
                  add_generic_and_check(j.front().sort());
                }
              }
            }
            else if (s.is_alias())
            {
              add_generic_and_check(alias(s).reference());
            }
            else
            {
              assert(false);
            }
          }

        public:

          /// \brief Constructor
          ///
          /// \param[in] specification a data specification
          dependent_sort_helper(data_specification const& specification) :
                                                m_specification(specification)
          {}

          /// \brief Adds the sorts on which a sort expression depends
          ///
          /// \param[in] s A sort expression.
          /// \return All sorts on which s depends.
          void add(const sort_expression& s)
          {
            if (m_visited_sorts.find(s) == m_visited_sorts.end())
            {
              add_generic(s);
            }
          }

          /// \brief Reset internal structures to default after construction
          void clear()
          {
            m_visited_sorts.clear();
            m_result.clear();
          }

          /// \brief Iterator for the sequence of dependent sorts
          const_iterator find(sort_expression const& s) const
          {
            return m_result.find(s);
          }

          /// \brief Iterator for the sequence of dependent sorts
          const_iterator begin() const
          {
            return m_result.begin();
          }

          /// \brief Iterator for past-the-end of sequence of dependent sorts
          const_iterator end() const
          {
            return m_result.end();
          }
      };

    } // namespace detail
    /// \endcond

    /// \pre m_sorts.find(sort) == m_sorts.empty()
    void data_specification::import_system_defined_sort(sort_expression const& sort)
    {
      assert(sort.is_standard());

      // add sorts, constructors, mappings and equations
      if (sort == sort_real_::real_())
      {
        sort_real_::add_real__to_specification(*this);
      }
      else if (sort == sort_int_::int_())
      {
        sort_int_::add_int__to_specification(*this);
      }
      else if (sort == sort_nat::nat())
      {
        sort_nat::add_nat_to_specification(*this);
      }
      else if (sort == sort_pos::pos())
      {
        sort_pos::add_pos_to_specification(*this);
      }
      else if (sort.is_container_sort())
      {
        sort_expression element_sort(container_sort(sort).element_sort());

        if (sort_list::is_list(sort))
        {
          sort_list::add_list_to_specification(*this, element_sort);
        }
        else if (sort_set_::is_set_(sort))
        {
          sort_set_::add_set__to_specification(*this, element_sort);
        }
        else if (sort_bag::is_bag(sort))
        {
          sort_bag::add_bag_to_specification(*this, element_sort);
        }
      }
      else if (sort.is_structured_sort())
      {
        m_sorts.insert(sort);
        m_sys_sorts.put(sort, sort);

        structured_sort s_sort(sort);

        add_system_defined_constructors(boost::make_iterator_range(s_sort.constructor_functions(sort)));
        add_system_defined_mappings(boost::make_iterator_range(s_sort.projection_functions(sort)));
        add_system_defined_mappings(boost::make_iterator_range(s_sort.recogniser_functions(sort)));
        add_system_defined_equations(boost::make_iterator_range(s_sort.constructor_equations(sort)));
        add_system_defined_equations(boost::make_iterator_range(s_sort.projection_equations(sort)));
        add_system_defined_equations(boost::make_iterator_range(s_sort.recogniser_equations(sort)));
      }
    }

    ///\brief Adds standard sorts when necessary
    ///
    /// Assumes that if constructors of a sort are part of the specification,
    /// then the sort was already imported.
    void data_specification::make_system_defined_complete()
    {
      if (m_sorts.find(sort_bool_::bool_()) == m_sorts.end()) {
        sort_bool_::add_bool__to_specification(*this);
      }

      detail::dependent_sort_helper dependent_sorts(*this);

      // sorts
      for (sorts_const_range r(sorts()); !r.empty(); r.advance_begin(1))
      {
        dependent_sorts.add(function_sort(sort_bool_::bool_(), r.front()));
      }
      // constructors
      for (constructors_const_range r(constructors()); !r.empty(); r.advance_begin(1))
      { // make function sort in case of constants to add the corresponding sort as needed
        dependent_sorts.add((r.front().sort().is_function_sort()) ? r.front().sort() :
                                      function_sort(sort_bool_::bool_(), r.front().sort()));
      }
      // mappings
      for (mappings_const_range r(mappings()); !r.empty(); r.advance_begin(1))
      { // make function sort in case of constants to add the corresponding sort as needed
        dependent_sorts.add((r.front().sort().is_function_sort()) ? r.front().sort() :
                                      function_sort(sort_bool_::bool_(), r.front().sort()));
      }

      for (detail::dependent_sort_helper::const_iterator i = dependent_sorts.begin(); i != dependent_sorts.end(); ++i)
      {
        if (i->is_standard() && constructors(*i).empty()) {
          import_system_defined_sort(*i);
        }
      }
    }

    void data_specification::make_system_defined_complete(sort_expression const& s)
    {
      detail::dependent_sort_helper dependent_sorts(*this);

      dependent_sorts.add(s);

      for (detail::dependent_sort_helper::const_iterator i = dependent_sorts.begin(); i != dependent_sorts.end(); ++i)
      {
        if (i->is_standard() && (!search_sort(*i) || constructors(*i).empty())) {
          import_system_defined_sort(*i);
        }
      }

      // for compatibilty with specifications imported from ATerm
      if (s.is_standard() && (!search_sort(s) || constructors(s).empty()))
      {
        import_system_defined_sort(s);
      }
    }

    void data_specification::purge_system_defined()
    {
      detail::remove_if(m_sorts, boost::bind(detail::is_system_defined< sort_expression >, *this, _1));

      detail::remove_if(m_constructors,
          boost::bind(detail::is_system_defined< function_symbol >, *this,
                boost::bind(&atermpp::multimap< sort_expression, function_symbol >::value_type::second, _1)));

      detail::remove_if(m_mappings,
          boost::bind(detail::is_system_defined< function_symbol >, *this,
                boost::bind(&atermpp::multimap< sort_expression, function_symbol >::value_type::second, _1)));

      detail::remove_if(m_equations, boost::bind(detail::is_system_defined< data_equation >, *this, _1));
    }

    /// \brief Checks whether a sort is certainly finite.
    ///
    /// \param[in] s A sort expression
    /// \return true if s can be determined to be finite,
    ///      false otherwise.
    bool data_specification::is_certainly_finite(const sort_expression& s) const
    {
      // Check for recursive occurrence.
      detail::dependent_sort_helper dependent_sorts(*this);

      dependent_sorts.add(s);

      if (dependent_sorts.find(s) != dependent_sorts.end())
      {
        return false;
      }

      if (s.is_basic_sort())
      {
        sort_expression actual_sort = find_referenced_sort(s);

        if (actual_sort != s && !actual_sort.is_basic_sort())
        {
          return is_certainly_finite(actual_sort);
        }

        for (data_specification::constructors_const_range r(constructors(actual_sort)); !r.empty(); r.advance_begin(1))
        {
          dependent_sorts.clear();
          dependent_sorts.add(r.front().sort().target_sort());
          for (detail::dependent_sort_helper::const_iterator j = dependent_sorts.begin(); j != dependent_sorts.end(); ++j)
          {
            if (!is_certainly_finite(*j))
            {
              return false;
            }
          }
        }

        return !constructors(actual_sort).empty();
      }
      else if (s.is_container_sort())
      {
        container_sort cs(s);
        if(cs.is_set_sort())
        {
          return is_certainly_finite(cs.element_sort());
        }
        return false;
      }
      else if (s.is_function_sort())
      {
        function_sort fs(s);
        for (function_sort::domain_const_range i(fs.domain()); !i.empty(); i.advance_begin(1))
        {
          if (!is_certainly_finite(i.front()))
          {
            return false;
          }
        }

        if (fs.codomain() == s)
        {
          return false;
        }

        return is_certainly_finite(fs.codomain());
      }
      else if (s.is_structured_sort())
      {
        dependent_sorts.clear();
        dependent_sorts.add(s);
        for (detail::dependent_sort_helper::const_iterator i = dependent_sorts.begin(); i != dependent_sorts.end(); ++i)
        {
          if (!is_certainly_finite(*i))
          {
            return false;
          }
        }
        return true;
      }
      else if (s.is_alias())
      {
        return is_certainly_finite(alias(s).reference());
      }
      else
      {
        assert(false);
      }

      return false;
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

    /// There are two types of representations of ATerms:
    ///  - the bare specification that does not contain constructor, mappings
    ///    and equations for system defined sorts
    ///  - specification that includes all system defined information (legacy)
    /// The last type must eventually dissapear but is unfortunately still in
    /// use in a substantial amount of source code.
    void data_specification::build_from_aterm(atermpp::aterm_appl const& term)
    {
      assert(core::detail::check_rule_DataSpec(term));

      atermpp::term_list< sort_expression > term_sorts(atermpp::list_arg1(atermpp::arg1(term)));
      atermpp::term_list< function_symbol > term_constructors(atermpp::list_arg1(atermpp::arg2(term)));
      atermpp::term_list< function_symbol > term_mappings(atermpp::list_arg1(atermpp::arg3(term)));
      atermpp::term_list< data_equation >   term_equations(atermpp::list_arg1(atermpp::arg4(term)));

      // gather non-system-defined sorts
      for (atermpp::term_list_iterator< sort_expression > i = term_sorts.begin(); i != term_sorts.end(); ++i)
      {
        if (i->is_alias() && alias(*i).reference().is_standard())
        {
          import_system_defined_sort(alias(*i).reference());
        }
        if (i->is_standard())
        {
          import_system_defined_sort(*i);
        }
        else
        {
          add_sort(*i);
        }
      }

      // gather constructors
      sort_to_symbol_map functions;

      for (atermpp::term_list_iterator< function_symbol > i = term_constructors.begin(); i != term_constructors.end(); ++i)
      {
        functions.insert(sort_to_symbol_map::value_type(i->sort().target_sort(), *i));
      }

      // Sort expression substitutions for substituting parts of the sorts of expressions
      atermpp::map< sort_expression, sort_expression > renamings;

      ltr_aliases_map initial_aliases(m_aliases_by_name);

      // replace alias that names the sort under which the sort is implemented
      for (ltr_aliases_map::const_iterator i = initial_aliases.begin(); i != initial_aliases.end(); ++i)
      {
        if (i->first.name().find("@legacy_") != std::string::npos)
        {
          renamings[i->first] = i->second;

          remove_alias(alias(i->first, i->second));
          remove_sort(i->first);
        }
        else if (i->second.is_container_sort() || i->second.is_structured_sort())
        {
          renamings[i->first] = i->second;

          // Specification contains equations that depend on the sort so add to the set of sorts
          if (constructors(i->second).empty())
          { // re-add sort
            m_sorts.erase(i->second);

            import_system_defined_sort(i->second);
          }
        }
      }

      map_substitution_adapter< atermpp::map< sort_expression, sort_expression > > renaming_substitution(renamings);

      for (atermpp::term_list_iterator< function_symbol > i = term_constructors.begin(); i != term_constructors.end(); ++i)
      {
        function_symbol new_function(atermpp::replace(*i, renaming_substitution));

        if (!search_constructor(new_function))
        {
          m_constructors.insert(sort_to_symbol_map::value_type(i->sort().target_sort(), new_function));
        }
      }
      for (atermpp::term_list_iterator< function_symbol > i = term_mappings.begin(); i != term_mappings.end(); ++i)
      {
        function_symbol new_function(atermpp::replace(*i, renaming_substitution));

        if (!search_mapping(new_function))
        {
          m_mappings.insert(sort_to_symbol_map::value_type(i->sort().target_sort(), new_function));
        }
      }
      for (atermpp::term_list_iterator< data_equation > i = term_equations.begin(); i != term_equations.end(); ++i)
      {
        m_equations.insert(atermpp::replace(*i, renaming_substitution));
      }

      remove_all_system_defined(*this);
    }

    /// \cond INTERNAL_DOCS
    namespace detail {
      atermpp::aterm_appl data_specification_to_aterm_data_spec(const data_specification& s)
      {
        struct local {
          static std::string sort_name(const sort_expression& target)
          {
            if (target.is_container_sort())
            {
              return container_sort(target).container_name();
            }
            else
            {
              return "structured_sort";
            }
          }

          // \brief find `THE' identifier for a structured sort or container sort
          static basic_sort find_suitable_identifier(const atermpp::set< sort_expression >& context,
                                                     const data_specification& specification,
                                                     const sort_expression& target)
          {
             data_specification::aliases_const_range aliases(specification.aliases(target));

             if (!aliases.empty())
             {
               for (data_specification::aliases_const_range::const_iterator i(aliases.begin()); i != aliases.end(); ++i)
               {
                 if (i->reference() == target)
                 {
                   return i->name();
                 }
               }
             }

             return basic_sort(static_cast< std::string >(fresh_identifier(boost::make_iterator_range(context),
                                        std::string("@legacy_").append(sort_name(target)))));
          }
        };

        typedef atermpp::set< sort_expression > sorts_set;

        sorts_set sorts = boost::copy_range< sorts_set >(s.aliases());

        // Sort expression substitutions for substituting parts of the sorts of expressions
        atermpp::map< sort_expression, sort_expression > renamings;

        // remove structured sorts and container sorts
        for (data_specification::sorts_const_range r(s.sorts()); !r.empty(); r.advance_begin(1))
        {
          sort_expression current = r.front();

          if (current.is_container_sort() || current.is_structured_sort())
          {
            basic_sort main_identifier(local::find_suitable_identifier(sorts, s, current));

            // update aliases
            for (sorts_set::iterator c = sorts.begin(), ca = sorts.begin(); ca++ != sorts.end(); c = ca)
            {
              if (c->is_alias())
              {
                alias alias_c(*c);

                if ((alias_c.name() != main_identifier) && (alias_c.reference() == current))
                {
                  sorts.insert(alias(alias_c.name(), main_identifier));
                  sorts.erase(c);
                }
              }
            }

            renamings[current] = main_identifier;

            // erase sort for compatibility with legacy data implementation
            sorts.erase(current);

            // make sure that an alias exists with the (possibly generated) name
            sorts.insert(alias(main_identifier, current));
          }
          else if (!current.is_alias())
          {
            sorts.insert(current);
          }
        }

        using namespace core::detail;

        return gsMakeDataSpec(
           gsMakeSortSpec(convert< atermpp::aterm_list >(sorts)),
           gsMakeConsSpec(atermpp::replace(convert< atermpp::aterm_list >(s.constructors()), make_map_substitution_adapter(renamings))),
           gsMakeMapSpec(atermpp::replace(convert< atermpp::aterm_list >(s.mappings()), make_map_substitution_adapter(renamings))),
           gsMakeDataEqnSpec(atermpp::replace(convert< atermpp::aterm_list >(s.equations()), make_map_substitution_adapter(renamings))));
      }
    } // namespace detail
    /// \endcond

  } // namespace data
} // namespace mcrl2

