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
#include <map>

#include "boost/bind.hpp"

#include "mcrl2/atermpp/algorithm.h"
#include "mcrl2/atermpp/substitute.h"
#include "mcrl2/core/detail/soundness_checks.h"
#include "mcrl2/data/map_substitution.h"
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
#include "mcrl2/data/structured_sort.h"
#include "mcrl2/data/find.h"
#include "mcrl2/data/print.h"
#include "mcrl2/data/detail/internal_format_conversion.h"
#include "mcrl2/data/detail/dependent_sorts.h"

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

      struct sort_map_substitution_adapter {

        atermpp::map< sort_expression, sort_expression > const& m_map;

        atermpp::aterm_appl operator()(atermpp::aterm_appl a) {
          if (is_sort_expression(a))
          {
            atermpp::map< sort_expression, sort_expression >::const_iterator i = m_map.find(sort_expression(a));
     
            if (i != m_map.end())
            {
              return i->second;
            }
          }
     
          return a;
        }

        sort_map_substitution_adapter(atermpp::map< sort_expression, sort_expression > const& map) : m_map(map)
        { }
      };

      template < typename Expression >
      inline bool is_system_defined(data_specification const& s, Expression const& c)
      {
        return s.is_system_defined(c);
      }

      bool has_legacy_name(sort_expression const& s)
      {
        return is_basic_sort(s) && (std::string(basic_sort(s).name()).find("@legacy_") == 0);
      }

      atermpp::map< sort_expression, sort_expression > make_compatible_renaming_map(const data_specification& s)
      {
        // Generates names for a specification assuming that no sorts with name prefix @legacy_ exist
        struct legacy_name_generator {

          std::set< basic_sort > m_generated;

          static std::string sort_name(const sort_expression& target)
          {
            if (target.is_container_sort())
            {
              return container_sort(target).container_type().function().name();
            }
            else
            {
              return "structured_sort";
            }
          }

          // \brief find `THE' identifier for a structured sort or container sort
          basic_sort generate_name(const sort_expression& target)
          {
             basic_sort generated(fresh_identifier(m_generated, std::string("@legacy_").append(sort_name(target))));

             m_generated.insert(generated);

             return generated;
          }
        } generator;

        atermpp::map< sort_expression, sort_expression > renamings;

        // Step one: fix a name for all container sorts (legacy requirement)
        for (data_specification::aliases_const_range r(s.aliases()); !r.empty(); r.advance_begin(1))
        {
          sort_expression reference(s.normalise(r.front().reference()));

          if (renamings.find(reference) == renamings.end())
          {
            for (atermpp::map< sort_expression, sort_expression >::iterator i = renamings.begin(); i != renamings.end(); ++i)
            {
              renamings[atermpp::replace(i->first, reference, r.front().name())] = i->second;
            }

            renamings[reference] = r.front().name();
          }
        }

        for (data_specification::sorts_const_range r(s.sorts()); !r.empty(); r.advance_begin(1))
        {
          if (r.front().is_container_sort() || r.front().is_structured_sort())
          {
            if (renamings.find(r.front()) == renamings.end())
            {
              basic_sort name(generator.generate_name(r.front()));

              for (atermpp::map< sort_expression, sort_expression >::iterator i = renamings.begin(); i != renamings.end(); ++i)
              {
                renamings[atermpp::replace(i->first, r.front(), name)] = i->second;
              }

              renamings[r.front()] = name;
            }
          }
        }

        return renamings;
      }

      /// Compatible conversion to ATerm is needlessly complicated only to appease the type checker
      /// As a side effect data checked against the compatible specification
      /// may refer to names that do not exist at the level data_specification objects.
      /// This function reverts the naming to make data terms usable in combination with data_specification objects.
      /// \note temporary measure until a type checker at data level becomes available
      template < typename Term >
      Term apply_compatibility_renamings(const data_specification& s, Term const& term)
      {
        // Maps container sort expressions to a unique name
        atermpp::map< sort_expression, sort_expression > renamings(make_compatible_renaming_map(s));

        return atermpp::replace(term, sort_map_substitution_adapter(renamings));
      }

      template
      variable_list apply_compatibility_renamings(const data_specification& s, variable_list const& term);
      template
      atermpp::aterm_appl apply_compatibility_renamings(const data_specification& s, atermpp::aterm_appl const& term);

      template < typename Term >
      Term undo_compatibility_renamings(const data_specification& s, Term const& term)
      {
        // Maps container sort expressions to a unique name
        atermpp::map< sort_expression, sort_expression > renamings(make_compatible_renaming_map(s));
        atermpp::map< sort_expression, sort_expression > inverse_renamings;

        for (atermpp::map< sort_expression, sort_expression >::const_iterator i = renamings.begin(); i != renamings.end(); ++i)
        {
          inverse_renamings[i->second] = i->first;
        }

        return atermpp::replace(term, sort_map_substitution_adapter(inverse_renamings));
      }

      template
      variable_list undo_compatibility_renamings(const data_specification& s, variable_list const& term);
      template
      atermpp::aterm_appl undo_compatibility_renamings(const data_specification& s, atermpp::aterm_appl const& term);

      /**
       * \param[in] compatible whether the produced ATerm is compatible with the `format after type checking'
       *
       * The compatible transformation should eventually disappear, it is only
       * here for compatibility with the old parser, type checker and pretty
       * print implementations.
       **/
      atermpp::aterm_appl data_specification_to_aterm_data_spec(const data_specification& s, bool compatible)
      {
        using namespace core::detail;

        if (compatible)
        {
          atermpp::set< atermpp::aterm_appl > sorts;

          // Maps container sort expressions to a unique name
          atermpp::map< sort_expression, sort_expression > renamings(make_compatible_renaming_map(s));

          sort_map_substitution_adapter renaming_substitution(renamings);

          // recursively apply renamings until no longer possible, or when unfolding recursive sorts
          for (data_specification::aliases_const_range r(s.aliases()); !r.empty(); r.advance_begin(1))
          {
            atermpp::map< sort_expression, sort_expression >::const_iterator j = renamings.find(r.front().reference());

            if (renamings.find(r.front().reference()) != renamings.end())
            {
              atermpp::map< sort_expression, sort_expression > partial_renamings(renamings);

              partial_renamings.erase(j->first);

              sorts.insert(alias(r.front().name(), atermpp::replace(r.front().reference(), sort_map_substitution_adapter(partial_renamings))));
            }
            else { // Intruduces another name for a known sort
              sorts.insert(r.front());
            }
          }

          for (atermpp::map< sort_expression, sort_expression >::const_iterator i = renamings.begin(); i != renamings.end(); ++i)
          {
            if (detail::has_legacy_name(i->second))
            {
              sorts.insert(alias(i->second, i->first));
            }
          }

          for (data_specification::sorts_const_range r(s.sorts()); !r.empty(); r.advance_begin(1))
          {
            if ((!r.front().is_basic_sort() || !s.is_alias(basic_sort(r.front()))) &&
			 !r.front().is_container_sort() && !r.front().is_structured_sort())
            {
              sorts.insert(r.front());
            }
          }

          return gsMakeDataSpec(
             gsMakeSortSpec(convert< atermpp::aterm_list >(sorts)),
             gsMakeConsSpec(atermpp::replace(convert< atermpp::aterm_list >(s.constructors()), renaming_substitution)),
             gsMakeMapSpec(atermpp::replace(convert< atermpp::aterm_list >(s.mappings()), renaming_substitution)),
             gsMakeDataEqnSpec(atermpp::replace(convert< atermpp::aterm_list >(s.equations()), renaming_substitution)));
        }
        else
        {
          atermpp::vector< atermpp::aterm_appl > sorts = convert< atermpp::vector< atermpp::aterm_appl > >(s.sorts());

          sorts.insert(sorts.end(), s.aliases().begin(), s.aliases().end());

          return gsMakeDataSpec(
             gsMakeSortSpec(convert< atermpp::aterm_list >(sorts)),
             gsMakeConsSpec(convert< atermpp::aterm_list >(s.constructors())),
             gsMakeMapSpec(convert< atermpp::aterm_list >(s.mappings())),
             gsMakeDataEqnSpec(convert< atermpp::aterm_list >(s.equations())));
        }
      }
    } // namespace detail
    /// \endcond

    /// \pre sort.is_system_defined()
    void data_specification::import_system_defined_sort(sort_expression const& sort)
    {
      assert(sort.is_system_defined());

      // add sorts, constructors, mappings and equations
      if (sort == sort_bool::bool_())
      {
        sort_bool::add_bool_to_specification(*this);
      }
      else if (sort == sort_real::real_())
      {
        sort_real::add_real_to_specification(*this);
      }
      else if (sort == sort_int::int_())
      {
        sort_int::add_int_to_specification(*this);
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
        else if (sort_set::is_set(sort))
        {
          sort_set::add_set_to_specification(*this, element_sort);
        }
        else if (sort_bag::is_bag(sort))
        {
          sort_bag::add_bag_to_specification(*this, element_sort);
        }
      }
      else if (sort.is_structured_sort())
      {
        add_system_defined_sort(sort);

        structured_sort s_sort(sort);

        add_system_defined_constructors(s_sort.constructor_functions(sort));
        add_system_defined_mappings(s_sort.projection_functions(sort));
        add_system_defined_mappings(s_sort.recogniser_functions(sort));
        add_system_defined_equations(s_sort.constructor_equations(sort));
        add_system_defined_equations(s_sort.projection_equations(sort));
        add_system_defined_equations(s_sort.recogniser_equations(sort));
      }

      add_standard_mappings_and_equations(sort);
    }

    template < typename Container, typename Sequence >
    void insert(Container& container, Sequence sequence)
    {
      container.insert(sequence.begin(), sequence.end());
    }

    ///\brief Adds standard sorts when necessary
    ///
    /// Assumes that if constructors of a sort are part of the specification,
    /// then the sort was already imported.
    void data_specification::make_complete()
    {
      std::set< sort_expression > dependent_sorts;

      // make sure that sort bool is part of the specification
      dependent_sorts.insert(sort_bool::bool_());

      // sorts
      dependent_sorts.insert(m_sorts.begin(), m_sorts.end());

      // constructors
      insert(dependent_sorts, make_sort_range(constructors()));

      // mappings
      insert(dependent_sorts, make_sort_range(mappings()));

      // equations
      for (equations_const_range r(equations()); !r.empty(); r.advance_begin(1))
      { // make function sort in case of constants to add the corresponding sort as needed
        insert(dependent_sorts, find_sort_expressions(r.front()));
      }

      make_complete(dependent_sorts);
    }

    template < typename Term >
    void data_specification::gather_sorts(Term const& term, std::set< sort_expression >& sorts)
    {
      std::set< sort_expression > all_sorts;

      find_sort_expressions(term, std::inserter(all_sorts, all_sorts.end()));

      for (std::set< sort_expression >::const_iterator i = sorts.begin(); i != sorts.end(); ++i)
      {
        sorts.insert(normalise(*i));
      }
    }

    template void data_specification::gather_sorts< sort_expression >(sort_expression const&, std::set< sort_expression >&);
    template void data_specification::gather_sorts< data_expression >(data_expression const&, std::set< sort_expression >&);
    template void data_specification::gather_sorts< data_equation >(data_equation const&, std::set< sort_expression >&);
    template void data_specification::gather_sorts< function_symbol >(function_symbol const&, std::set< sort_expression >&);

    // Assumes that a system defined sort s is not (full) part of the specification if:
    //  - the set of sorts does not contain s
    //  - the specification has no constructors for s
    void data_specification::make_complete(std::set< sort_expression > const& sorts)
    {
      std::set< sort_expression > dependent_sorts(sorts);

      find_dependent_sorts(*this, sorts, std::inserter(dependent_sorts, dependent_sorts.end()));

      for (std::set< sort_expression >::const_iterator i = dependent_sorts.begin(); i != dependent_sorts.end(); ++i)
      {
        sort_expression normalised(normalise(*i));

        if (normalised.is_system_defined())
        { // || constructors(normalised).empty()) {
          if (!search_sort(normalised))
          {
            m_sorts.insert(normalised);

            import_system_defined_sort(normalised);
          }
        }
        else if (normalised.is_function_sort() && mappings(normalised).empty())
        { // no if : Bool # *i # *i -> *i for function sort *i, add standard functions
          add_standard_mappings_and_equations(normalised);
        }
      }
    }

    void data_specification::make_complete(data_expression const& e)
    {
      make_complete(find_sort_expressions(e));
    }

    void data_specification::make_complete(data_equation const& e)
    {
      make_complete(find_sort_expressions(e));
    }

    void data_specification::make_complete(sort_expression const& s)
    {
      std::set< sort_expression > sorts;

      sorts.insert(normalise(s));

      make_complete(sorts);
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

    class finiteness_helper {

      protected:

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
          if (s.is_basic_sort())
          {
            return is_finite(basic_sort(s));
          }
          else if (s.is_container_sort())
          {
            return is_finite(container_sort(s));
          }
          else if (s.is_function_sort())
          {
            return is_finite(function_sort(s));
          }
          else if (s.is_structured_sort())
          {
            return is_finite(structured_sort(s));
          }

          return false;
        }

        bool is_finite(const basic_sort& s)
        {
          sort_expression actual_sort = m_specification.find_referenced_sort(s);

          if (actual_sort != s)
          {
            return is_finite(actual_sort);
          }
          else {
            m_visiting.insert(s);

            for (data_specification::constructors_const_range r(m_specification.constructors(s)); !r.empty(); r.advance_begin(1))
            {
              if (r.front().sort().is_function_sort())
              {
                for (boost::iterator_range< dependent_sort_set::const_iterator > c(dependent_sorts(r.front().sort())); !c.empty(); c.advance_begin(1))
                {
                  if (!c.front().is_function_sort())
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
          for (boost::iterator_range< function_sort::domain_const_range::iterator > i(s.domain()); !i.empty(); i.advance_begin(1))
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
          return (s.is_set_sort()) ? is_finite(s.element_sort()) : false;
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

    void data_specification::expression_normaliser::normalise_names_in_aliases()
    {
      atermpp::map< sort_expression, sort_expression > replacements;

      // Step 1, replacing names interpretting A = B as rewrite rule A -> B, with B a basic sort
      for (atermpp::map< sort_expression, sort_expression >::const_iterator i = m_normalised_aliases.begin(); i != m_normalised_aliases.end(); ++i)
      {
        if (i->second.is_basic_sort())
        {
          replacements[i->first] = i->second;
        }
      }

      detail::sort_map_substitution_adapter replacement_helper(replacements);

      for (atermpp::map< sort_expression, sort_expression >::iterator i = m_normalised_aliases.begin(); i != m_normalised_aliases.end(); ++i)
      {
        sort_expression new_right(replacement_helper(i->second));

        if (i->first == new_right)
        {
          throw mcrl2::runtime_error("Alias " + pp(alias(i->first, i->second)) + " can be reduced to " + pp(i->first) + " = " + pp(new_right));
        }
        else
        {
          i->second = new_right;
        }
      }
    }

    void data_specification::expression_normaliser::add(alias const& a)
    {
      m_normalised_aliases.insert(std::make_pair(a.name(), a.reference()));

      if (m_reverse_aliases.find(a.reference()) == m_reverse_aliases.end())
      {
        m_reverse_aliases.insert(std::make_pair(a.reference(), a.name()));
      }
    }

    void data_specification::expression_normaliser::remove(alias const& a)
    {
       m_normalised_aliases.erase(std::find(m_normalised_aliases.lower_bound(a.reference()),
                                            m_normalised_aliases.upper_bound(a.reference()),
                                     std::pair< const sort_expression, sort_expression >(a.reference(), a.name())));
       m_reverse_aliases.erase(std::find(m_reverse_aliases.lower_bound(a.name()),
                                         m_reverse_aliases.upper_bound(a.name()),
                                     std::pair< const sort_expression, sort_expression >(a.name(), a.reference())));
    }

    void data_specification::expression_normaliser::initialise()
    {
      normalise_names_in_aliases();

      // Step 2, replace different names for the same sort by one selected name
      for (atermpp::map< sort_expression, sort_expression >::iterator i = m_normalised_aliases.begin(); i != m_normalised_aliases.end(); ++i)
      {
        for (atermpp::map< sort_expression, sort_expression >::iterator j = boost::next(i, 1); j != m_normalised_aliases.end(); ++j)
        {
          if (i->second == j->second && !detail::has_legacy_name(i->first))
          {
            j->second = i->first;
      
            normalise_names_in_aliases();
          }
        }

        assert(i->first != i->second);
      }

      atermpp::map< sort_expression, sort_expression > original_aliases(m_normalised_aliases);

      // Step 3. normalise right-hand sides (left to right)
      for (atermpp::map< sort_expression, sort_expression >::iterator i = original_aliases.begin(); i != original_aliases.end(); ++i)
      {
        sort_expression right(i->second);

        i->second = i->first;

        i->second = detail::sort_map_substitution_adapter(m_normalised_aliases)(i->second);

        original_aliases[i->first] = right;
      }

      atermpp::map< sort_expression, sort_expression > reversed_aliases(m_normalised_aliases);

      for (atermpp::map< sort_expression, sort_expression >::iterator i = original_aliases.begin(); i != original_aliases.end(); ++i)
      {
        if (!i->second.is_basic_sort())
        {
          reversed_aliases[i->second] = i->first;
        }
      }

      // Step 4. normalise right-hand sides (right to left)
      for (atermpp::map< sort_expression, sort_expression >::iterator i = m_normalised_aliases.begin(); i != m_normalised_aliases.end(); ++i)
      {
        while (original_aliases[i->first] != i->second)
        {
          sort_expression new_right(detail::sort_map_substitution_adapter(reversed_aliases)(i->second));

          if (i->first == new_right)
          {
            throw mcrl2::runtime_error("Fatal error, alias " + pp(alias(i->first, i->second)) + " could be reduced to " + pp(alias(i->first, new_right)));
          }
          else if (new_right != i->second) 
          {
            i->second = new_right;
          }
        }
      }

      for (atermpp::map< sort_expression, sort_expression >::iterator i = m_normalised_aliases.begin(); i != m_normalised_aliases.end(); ++i)
      {
         m_reverse_aliases.insert(std::make_pair(i->second, i->first));
      }
    }

    sort_expression data_specification::expression_normaliser::normalise(sort_expression const& e) const
    {
      return normalise_backward(normalise_forward(e));
    }

    sort_expression data_specification::expression_normaliser::normalise_backward(sort_expression const& e) const
    {
      sort_expression result(e);

      for (atermpp::map< sort_expression, sort_expression >::const_iterator i(m_reverse_aliases.find(e)); i != m_reverse_aliases.end(); )
      {
        return i->second;
      }

      if (e.is_function_sort())
      {
        atermpp::vector< sort_expression > new_domain;

        for (boost::iterator_range< function_sort::domain_const_range::iterator > r(function_sort(e).domain()); !r.empty(); r.advance_begin(1))
        {
          new_domain.push_back(normalise_backward(r.front()));
        }

        result = function_sort(new_domain, normalise_backward(function_sort(e).codomain()));
      }
      else if (e.is_container_sort())
      {
        result = container_sort(container_sort(e).container_type(), normalise_backward(container_sort(e).element_sort()));
      }
      else if (e.is_structured_sort())
      {
        atermpp::vector< structured_sort_constructor > new_constructors;

        for (structured_sort::constructors_const_range r(structured_sort(e).struct_constructors()); !r.empty(); r.advance_begin(1))
        {
          atermpp::vector< structured_sort_constructor_argument > new_arguments;

          for (structured_sort_constructor::arguments_const_range ra(r.front().arguments()); !ra.empty(); ra.advance_begin(1))
          {
            new_arguments.push_back(structured_sort_constructor_argument(normalise_backward(ra.front().sort()), ra.front().name()));
          }

          new_constructors.push_back(structured_sort_constructor(r.front().name(), new_arguments, r.front().recogniser()));
        }

        result = structured_sort(new_constructors);
      }

      result = (result == e) ? result : normalise_backward(result);

      return result;
    }

    sort_expression data_specification::expression_normaliser::normalise_forward(sort_expression const& e) const
    {
      if (e.is_basic_sort())
      {
        return find_referenced_sort(e);
      }
      else if (e.is_function_sort())
      {
        atermpp::vector< sort_expression > new_domain;

        for (boost::iterator_range< function_sort::domain_const_range::iterator > r(function_sort(e).domain()); !r.empty(); r.advance_begin(1))
        {
          new_domain.push_back(normalise(r.front()));
        }

        return function_sort(new_domain, normalise(function_sort(e).codomain()));
      }
      else if (e.is_container_sort())
      {
        return container_sort(container_sort(e).container_type(), normalise(container_sort(e).element_sort()));
      }
      else if (e.is_structured_sort())
      {
        atermpp::vector< structured_sort_constructor > new_constructors;

        for (structured_sort::constructors_const_range r(structured_sort(e).struct_constructors()); !r.empty(); r.advance_begin(1))
        {
          atermpp::vector< structured_sort_constructor_argument > new_arguments;

          for (structured_sort_constructor::arguments_const_range ra(r.front().arguments()); !ra.empty(); ra.advance_begin(1))
          {
            new_arguments.push_back(structured_sort_constructor_argument(normalise(ra.front().sort()), ra.front().name()));
          }

          new_constructors.push_back(structured_sort_constructor(r.front().name(), new_arguments, r.front().recogniser()));
        }

        return structured_sort(new_constructors);
      }

      return e;
    }
    /// \endcond

    /// There are two types of representations of ATerms:
    ///  - the bare specification that does not contain constructor, mappings
    ///    and equations for system defined sorts
    ///  - specification that includes all system defined information (legacy)
    /// The last type must eventually dissapear but is unfortunately still in
    /// use in a substantial amount of source code.
    /// Note, all sorts with name prefix @legacy_ are eliminated
    void data_specification::build_from_aterm(atermpp::aterm_appl const& term)
    {
      assert(core::detail::check_rule_DataSpec(term));

      // Note backwards compatibility measure: alias is no longer a sort_expression
      atermpp::term_list< atermpp::aterm_appl >  term_sorts(atermpp::list_arg1(atermpp::arg1(term)));
      atermpp::term_list< function_symbol >      term_constructors(atermpp::list_arg1(atermpp::arg2(term)));
      atermpp::term_list< function_symbol >      term_mappings(atermpp::list_arg1(atermpp::arg3(term)));
      atermpp::term_list< data_equation >        term_equations(atermpp::list_arg1(atermpp::arg4(term)));

      // Maps container unique name to a container sort expression
      atermpp::set< alias > aliases;

      // Add sorts and aliases for a container or structured sort
      for (atermpp::term_list_iterator< atermpp::aterm_appl > i = term_sorts.begin(); i != term_sorts.end(); ++i)
      {
        if (data::is_alias(*i)) // Compatibility with legacy code
        {
          aliases.insert(*i);
        }
      }

      m_expression_normaliser.initialise(aliases);

      // Step two: Normalise names for container sorts
      for (atermpp::term_list_iterator< atermpp::aterm_appl > i = term_sorts.begin(); i != term_sorts.end(); ++i)
      {
        if (data::is_alias(*i)) // Compatibility with legacy code
        {
          if (!detail::has_legacy_name(alias(*i).name()))
          {
            insert_alias(alias(*i).name(), alias(*i).reference());
          }
        }
        else {
          add_sort(normalise(*i));
        }
      }

      for (atermpp::term_list_iterator< function_symbol > i = term_constructors.begin(); i != term_constructors.end(); ++i)
      {
        function_symbol new_function(i->name(), normalise(i->sort()));

        if (!search_constructor(new_function))
        {
          m_constructors.insert(sort_to_symbol_map::value_type(new_function.sort().target_sort(), new_function));
        }
      }
      for (atermpp::term_list_iterator< function_symbol > i = term_mappings.begin(); i != term_mappings.end(); ++i)
      {
        function_symbol new_function(i->name(), normalise(i->sort()));

        if (!search_mapping(new_function))
        {
          m_mappings.insert(sort_to_symbol_map::value_type(new_function.sort().target_sort(), new_function));
        }
      }

      detail::internal_format_conversion_helper normaliser(*this);

      for (atermpp::term_list_iterator< data_equation > i = term_equations.begin(); i != term_equations.end(); ++i)
      {
        data_equation new_equation(normaliser(*i));

        if (!search_equation(new_equation))
        {
          m_equations.insert(new_equation);
        }
      }

      make_complete();
    }
  } // namespace data
} // namespace mcrl2

