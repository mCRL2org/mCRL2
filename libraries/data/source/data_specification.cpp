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
#include "mcrl2/atermpp/substitute.h"
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
#include "mcrl2/data/structured_sort.h"
#include "mcrl2/data/find.h"
#include "mcrl2/data/print.h"

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
      /// TODO rewrite using find functionality
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

          void add_generic(const basic_sort& s)
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

          void add_generic(const container_sort& s)
          {
            add_generic_and_check(container_sort(s).element_sort());
          }

          void add_generic(const function_sort& s)
          {
            for (function_sort::domain_const_range i(s.domain()); !i.empty(); i.advance_begin(1))
            {
              add_generic_and_check(i.front());
            }

            add_generic_and_check(s.codomain());
          }

          void add_generic(const structured_sort& s)
          {
            for (structured_sort::constructors_const_range r(s.struct_constructors()); !r.empty(); r.advance_begin(1))
            {
              for (structured_sort_constructor::arguments_const_range j(r.front().arguments()); !j.empty(); j.advance_begin(1))
              {
                add_generic_and_check(j.front().sort());
              }
            }
          }

          void add_generic(const sort_expression& s)
          {
            if (s.is_basic_sort())
            {
              add_generic(basic_sort(s));
            }
            else if (s.is_container_sort())
            {
              add_generic(container_sort(s));
            }
            else if (s.is_function_sort())
            {
              add_generic(function_sort(s));
            }
            else if (s.is_structured_sort())
            {
              add_generic(structured_sort(s));
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
          /// \param[in] assume_self_dependence add the sort as well as all dependent sorts
          /// \return All sorts on which s depends.
          void add(const sort_expression& s, bool assume_self_dependence = false)
          {
            if (assume_self_dependence)
            {
              m_result.insert(s);
            }
            if (m_visited_sorts.find(s) == m_visited_sorts.end())
            {
              add_generic(s);
            }
          }

          /// \brief Adds the sorts on which a sort expression depends
          ///
          /// \param[in] s A sort expression.
          /// \param[in] assume_self_dependence add the sort as well as all dependent sorts
          /// \return All sorts on which s depends.
          template < typename ForwardTraversalIterator >
          void add(boost::iterator_range< ForwardTraversalIterator > const& range, bool assume_self_dependence = false)
          {
            for (ForwardTraversalIterator i = range.begin(); i != range.end(); ++i)
            {
              add(*i, assume_self_dependence);
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

    // Assumes that the right-hand sides of aliases are normal form
    sort_expression data_specification::normalise(sort_expression const& e) const
    {
      if (e.is_basic_sort())
      {
        return find_referenced_sort(e);
      }
      else if (m_aliases_by_sort.find(e) == m_aliases_by_sort.end())
      { // only normalise when no name has been introduced for the sort (needed for recursive structured sorts)
        if (e.is_function_sort())
        {
          atermpp::vector< sort_expression > new_domain;
       
          for (function_sort::domain_const_range r(function_sort(e).domain()); !r.empty(); r.advance_begin(1))
          {
            new_domain.push_back(normalise(r.front()));
          }
       
          return function_sort(new_domain, normalise(function_sort(e).codomain()));
        }
        else if (e.is_container_sort())
        {
          return container_sort(container_sort(e).container_name(), normalise(container_sort(e).element_sort()));
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
      }

      return e;
    }

    /// \pre sort.is_system_defined()
    void data_specification::import_system_defined_sort(sort_expression const& sort)
    {
      assert(sort.is_system_defined());

      // add sorts, constructors, mappings and equations
      if (sort == sort_bool_::bool_())
      {
        sort_bool_::add_bool__to_specification(*this);
      }
      else if (sort == sort_real_::real_())
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

    // Assumes that a system defined sort s is not (full) part of the specification if:
    //  - the set of sorts does not contain s
    //  - the specification has no constructors for s
    void data_specification::make_complete(detail::dependent_sort_helper const& dependent_sorts)
    {
      for (detail::dependent_sort_helper::const_iterator i = dependent_sorts.begin(); i != dependent_sorts.end(); ++i)
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

    ///\brief Adds standard sorts when necessary
    ///
    /// Assumes that if constructors of a sort are part of the specification,
    /// then the sort was already imported.
    void data_specification::make_complete()
    {
      detail::dependent_sort_helper dependent_sorts(*this);

      // make sure that sort bool is part of the specification
      dependent_sorts.add(sort_bool_::bool_(), true);

      // sorts
      for (sorts_const_range r(sorts()); !r.empty(); r.advance_begin(1))
      {
        dependent_sorts.add(r.front(), true);
      }
      // constructors
      for (constructors_const_range r(constructors()); !r.empty(); r.advance_begin(1))
      { // make function sort in case of constants to add the corresponding sort as needed
        dependent_sorts.add(r.front().sort(), true);
      }
      // mappings
      for (mappings_const_range r(mappings()); !r.empty(); r.advance_begin(1))
      { // make function sort in case of constants to add the corresponding sort as needed
        dependent_sorts.add(r.front().sort(), true);
      }
      // equations
      for (equations_const_range r(equations()); !r.empty(); r.advance_begin(1))
      { // make function sort in case of constants to add the corresponding sort as needed
        dependent_sorts.add(boost::make_iterator_range(find_all_sort_expressions(r.front())), true);
      }

      make_complete(dependent_sorts);
    }

    template < typename Term >
    void data_specification::gather_sorts(Term const& term, std::set< sort_expression >& sorts)
    {
      find_all_sort_expressions(term, std::inserter(sorts, sorts.end()));
    }

    template void data_specification::gather_sorts< sort_expression >(sort_expression const&, std::set< sort_expression >&);
    template void data_specification::gather_sorts< data_expression >(data_expression const&, std::set< sort_expression >&);
    template void data_specification::gather_sorts< data_equation >(data_equation const&, std::set< sort_expression >&);
    template void data_specification::gather_sorts< function_symbol >(function_symbol const&, std::set< sort_expression >&);

    void data_specification::make_complete_helper(std::set< sort_expression > const& sorts)
    {
      detail::dependent_sort_helper dependent_sorts(*this);

      for (std::set< sort_expression >::const_iterator i = sorts.begin(); i != sorts.end(); ++i)
      {
        dependent_sorts.add(*i, true);
      }

      make_complete(dependent_sorts);
    }

    void data_specification::make_complete(data_expression const& e)
    {
      detail::dependent_sort_helper dependent_sorts(*this);

      dependent_sorts.add(boost::make_iterator_range(find_all_sort_expressions(e)), true);

      make_complete(dependent_sorts);
    }

    void data_specification::make_complete(data_equation const& e)
    {
      detail::dependent_sort_helper dependent_sorts(*this);

      dependent_sorts.add(boost::make_iterator_range(find_all_sort_expressions(e)), true);

      make_complete(dependent_sorts);
    }

    void data_specification::make_complete(sort_expression const& s)
    {
      detail::dependent_sort_helper dependent_sorts(*this);

      // for compatibilty with specifications imported from ATerm
      dependent_sorts.add(s, true);

      make_complete(dependent_sorts);
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
    /// Note, all sorts with name prefix @legacy_ are eliminated
    void data_specification::build_from_aterm(atermpp::aterm_appl const& term)
    {
      assert(core::detail::check_rule_DataSpec(term));

      atermpp::term_list< sort_expression > term_sorts(atermpp::list_arg1(atermpp::arg1(term)));
      atermpp::term_list< function_symbol > term_constructors(atermpp::list_arg1(atermpp::arg2(term)));
      atermpp::term_list< function_symbol > term_mappings(atermpp::list_arg1(atermpp::arg3(term)));
      atermpp::term_list< data_equation >   term_equations(atermpp::list_arg1(atermpp::arg4(term)));

      // Maps container unique name to a container sort expression
      atermpp::map< sort_expression, sort_expression >      renamings;

      // Maps container sort expressions to names
      atermpp::multimap< sort_expression, sort_expression > other_names;

      // Add sorts and aliases for a container or structured sort
      for (atermpp::term_list_iterator< sort_expression > i = term_sorts.begin(); i != term_sorts.end(); ++i)
      {
        if (i->is_alias())
        {
          basic_sort      name(alias(*i).name());
          sort_expression reference(alias(*i).reference());

          for (atermpp::map< sort_expression, sort_expression >::iterator j = renamings.begin(); j != renamings.end(); ++j)
          {
            j->second = atermpp::replace(j->second, name, reference);
          }

          if (reference.is_container_sort() || reference.is_structured_sort())
          {
            if (!reference.is_basic_sort() || renamings.find(reference) == renamings.end())
            { // no other name for the sort
              renamings[name] = atermpp::replace(reference, make_map_substitution_adapter(renamings));
            }
            else
            {
              other_names.insert(std::pair< sort_expression, sort_expression >(reference, atermpp::replace(name, make_map_substitution_adapter(renamings))));
            }
          }
          else
          {
            renamings.insert(std::pair< sort_expression, sort_expression >(name, reference));
          }
        }
      }

      // Step two: Normalise names for container sorts
      for (atermpp::map< sort_expression, sort_expression >::const_iterator i = renamings.begin(); i != renamings.end(); ++i)
      {
        if (std::string(basic_sort(i->first).name()).find("@legacy_") != 0)
        {
          add_alias(alias(i->first, i->second));
        }
      }

      map_substitution< atermpp::map< sort_expression, sort_expression > const& > renaming_substitution(renamings);

      for (atermpp::multimap< sort_expression, sort_expression >::const_iterator i = other_names.begin(); i != other_names.end(); ++i)
      {
        add_alias(alias(i->second, atermpp::replace(i->first, renaming_substitution)));
      }

      for (atermpp::term_list_iterator< sort_expression > i = term_sorts.begin(); i != term_sorts.end(); ++i)
      {
        if (!i->is_alias())
        {
          add_sort(atermpp::replace(*i, renaming_substitution));
        }
      }

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
        data_equation new_equation(atermpp::replace(*i, renaming_substitution));

        if (!search_equation(new_equation))
        {
          m_equations.insert(new_equation);
        }
      }
    }

    /// \cond INTERNAL_DOCS
    namespace detail {
      atermpp::aterm_appl data_specification_to_aterm_data_spec(const data_specification& s)
      {
        // Generates names for a specification assuming that no sorts with name prefix @legacy_ exist
        struct name_generator {

          std::set< basic_sort > m_generated;

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
          basic_sort generate_name(const sort_expression& target)
          {
             basic_sort generated(static_cast< std::string >(
                        fresh_identifier(boost::make_iterator_range(m_generated),
                                        std::string("@legacy_").append(sort_name(target)))));

             m_generated.insert(generated);

             return generated;
          }
        } generator;

        atermpp::set< sort_expression > sorts;

        // Maps container sort expressions to a unique name
        atermpp::map< sort_expression, sort_expression >      renamings;

        // Maps container sort expressions to names
        atermpp::multimap< sort_expression, sort_expression > other_names;

        // Step one: fix a name for all container sorts (legacy requirement)
        for (data_specification::aliases_const_range r(s.aliases()); !r.empty(); r.advance_begin(1))
        {
          sort_expression reference(s.normalise(r.front().reference()));

          if (renamings.find(reference) == renamings.end())
          {
            atermpp::map< sort_expression, sort_expression > copy_renamings(renamings);

            for (atermpp::map< sort_expression, sort_expression >::iterator i = copy_renamings.begin(); i != copy_renamings.end(); ++i)
            {
              renamings[atermpp::replace(i->first, reference, r.front().name())] = i->second;
            }

            renamings[reference] = r.front().name();
          }
          else
          {
            other_names.insert(std::pair< sort_expression, sort_expression >(reference, r.front().name()));
          }
        }

        for (data_specification::sorts_const_range r(s.sorts()); !r.empty(); r.advance_begin(1))
        {
          if (r.front().is_container_sort() || r.front().is_structured_sort())
          {
            if (renamings.find(r.front()) == renamings.end())
            {
              basic_sort name(generator.generate_name(r.front()));

              renamings[r.front()] = name;
            }
          }
        }

        map_substitution< atermpp::map< sort_expression, sort_expression > const& > renaming_substitution(renamings);

        // recursively apply renamings until no longer possible, or when unfolding recursive sorts
        for (data_specification::aliases_const_range r(s.aliases()); !r.empty(); r.advance_begin(1))
        {
          atermpp::map< sort_expression, sort_expression >::const_iterator j = renamings.find(r.front().reference());

          if (renamings.find(r.front().reference()) != renamings.end())
          {
            std::map< sort_expression, sort_expression > partial_renamings(renamings);

            partial_renamings.erase(j->first);

            sorts.insert(alias(r.front().name(), atermpp::replace(r.front().reference(), make_map_substitution_adapter(partial_renamings))));
          }
          else {
            sorts.insert(alias(r.front().name(), atermpp::replace(r.front().reference(), renaming_substitution)));
          }
        }

        for (data_specification::sorts_const_range r(s.sorts()); !r.empty(); r.advance_begin(1))
        {
          if (r.front().is_container_sort() || r.front().is_structured_sort())
          {
            sorts.insert(alias(renamings[r.front()], atermpp::replace(r.front(), renaming_substitution)));
          }
          else if (!r.front().is_alias())
          { // incidentally, store in set of sorts
            sorts.insert(atermpp::replace(r.front(), renaming_substitution));
          }
        }

        using namespace core::detail;

        return gsMakeDataSpec(
           gsMakeSortSpec(convert< atermpp::aterm_list >(sorts)),
           gsMakeConsSpec(atermpp::replace(convert< atermpp::aterm_list >(s.constructors()), renaming_substitution)),
           gsMakeMapSpec(atermpp::replace(convert< atermpp::aterm_list >(s.mappings()), renaming_substitution)),
           gsMakeDataEqnSpec(atermpp::replace(convert< atermpp::aterm_list >(s.equations()), renaming_substitution)));
      }
    } // namespace detail
    /// \endcond

  } // namespace data
} // namespace mcrl2

