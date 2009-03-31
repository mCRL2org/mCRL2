// Author(s): Jeroen Keiren, Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/new_data/data_specification.h
/// \brief The class data_specification.

#include <algorithm>
#include <functional>
#include <iostream>

#include <boost/bind.hpp>

#include "mcrl2/atermpp/algorithm.h"
#include "mcrl2/new_data/substitution.h"
#include "mcrl2/new_data/utility.h"
#include "mcrl2/new_data/detail/compatibility.h"
#include "mcrl2/new_data/data_specification.h"
#include "mcrl2/new_data/detail/sequence_algorithm.h"
#include "mcrl2/new_data/detail/data_utility.h"
#include "mcrl2/new_data/bool.h"
#include "mcrl2/new_data/pos.h"
#include "mcrl2/new_data/nat.h"
#include "mcrl2/new_data/int.h"
#include "mcrl2/new_data/real.h"
#include "mcrl2/new_data/list.h"
#include "mcrl2/new_data/set.h"
#include "mcrl2/new_data/bag.h"
#include "mcrl2/new_data/find.h"
#include "mcrl2/new_data/structured_sort.h"

namespace mcrl2 {

  namespace new_data {
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
      /// \endcond

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
                function_symbol_vector constructors_s(m_specification.constructors(actual_sort));

                for (function_symbol_vector::const_iterator i = constructors_s.begin(); i != constructors_s.end(); ++i)
                {
                  if (i->sort().is_function_sort())
                  {
                    function_sort f_sort(i->sort());

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
    /// \endcond INTERNAL_DOCS

    /// \pre m_sorts.find(sort) == m_sorts.empty()
    void data_specification::import_system_defined_sort(sort_expression const& sort)
    {
      assert(sort.is_standard());

      // add sorts, constructors, mappings and equations
      if (sort == sort_real_::real_())
      {
        sort_real_::add_real__to_specification(*this);
        if (m_sorts.find(sort_int_::int_()) == m_sorts.end())
        {
          import_system_defined_sort(sort_int_::int_());
        }
      }
      else if (sort == sort_int_::int_())
      {
        sort_int_::add_int__to_specification(*this);
        if (m_sorts.find(sort_nat::nat()) == m_sorts.end())
        {
          import_system_defined_sort(sort_nat::nat());
        }
      }
      else if (sort == sort_nat::nat())
      {
        add_system_defined_sort(sort_nat::natpair());
        sort_nat::add_nat_to_specification(*this);
        if (m_sorts.find(sort_pos::pos()) == m_sorts.end())
        {
          sort_pos::add_pos_to_specification(*this);
        }
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

        add_system_defined_constructors(boost::make_iterator_range(s_sort.constructor_functions(sort)));
        add_system_defined_mappings(boost::make_iterator_range(s_sort.projection_functions(sort)));
        add_system_defined_mappings(boost::make_iterator_range(s_sort.recogniser_functions(sort)));
        add_system_defined_equations(boost::make_iterator_range(s_sort.constructor_equations(sort)));
        add_system_defined_equations(boost::make_iterator_range(s_sort.projection_equations(sort)));
        add_system_defined_equations(boost::make_iterator_range(s_sort.recogniser_equations(sort)));
      }
    }

    ///\brief Adds standard sorts when necessary
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
        if (i->is_standard() && (m_sorts.find(*i) == m_sorts.end())) {
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
        if (i->is_standard() && (m_sorts.find(*i) == m_sorts.end())) {
          import_system_defined_sort(*i);
        }
      }
    }

    void data_specification::purge_system_defined()
    {
      detail::remove_if(m_sorts, boost::bind(detail::is_system_defined< sort_expression >, *this, _1));

      detail::remove_if(m_constructors,
          boost::bind(detail::is_system_defined< function_symbol >, *this,
                boost::bind(&atermpp::multimap< sort_expression, function_symbol >::value_type::second, _1)));

      detail::remove_if(m_mappings, boost::bind(detail::is_system_defined< function_symbol >, *this, _1));

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

        function_symbol_vector fl(constructors(actual_sort));

        for (function_symbol_vector::const_iterator i = fl.begin(); i != fl.end(); ++i)
        {
          dependent_sorts.clear();
          dependent_sorts.add(i->sort().target_sort());
          for (detail::dependent_sort_helper::const_iterator j = dependent_sorts.begin(); j != dependent_sorts.end(); ++j)
          {
            if (!is_certainly_finite(*j))
            {
              return false;
            }
          }
        }

        return !fl.empty();
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

    void data_specification::build_from_aterm(atermpp::aterm_appl const& term)
    {
      typedef mutable_map_substitution< atermpp::aterm_appl, atermpp::aterm_appl > substitution_type;

      m_sorts        = detail::aterm_sort_spec_to_sort_expression_set(atermpp::arg1(term));
      m_constructors = detail::aterm_cons_spec_to_constructor_map(atermpp::arg2(term));
      m_mappings     = detail::aterm_map_spec_to_function_set(atermpp::arg3(term));

      // Sort expression substitutions for substituting parts of the sorts of expressions
      substitution_type renamings;

      atermpp::set< alias > all_aliases(boost::copy_range< atermpp::set< alias > >(aliases()));

      // replace alias that names the sort under which the sort is implemented
      for (atermpp::set< alias >::const_iterator i(all_aliases.begin()); i != all_aliases.end(); ++i)
      {
        sort_expression referenced_sort(i->reference());

        if ((referenced_sort.is_container_sort() || referenced_sort.is_structured_sort()) &&
              (m_constructors.equal_range(i->name()).first != m_constructors.end() ||
               m_mappings.find(i->name()) != m_mappings.end()))
        { // assume that the container or structured sort is implemented under the current alias
          basic_sort main_identifier(i->name());

          // update aliases
          for (atermpp::set< sort_expression >::const_iterator c = m_sorts.begin(), ca = m_sorts.begin(); ca++ != m_sorts.end(); c = ca)
          {
            if (c->is_alias())
            {
              alias alias_c(*c);

              if ((alias_c.name() != main_identifier) && (alias_c.reference() == referenced_sort))
              {
                m_sorts.insert(alias(alias_c.name(), referenced_sort));
                m_sorts.erase(c);
              }
            }
          }

          renamings[main_identifier] = referenced_sort;

          // Specification contains equations that depend on the sort so add to the set of sorts
          m_sorts.insert(referenced_sort);
        }
      }

      m_constructors = detail::aterm_cons_spec_to_constructor_map(atermpp::replace(atermpp::arg2(term), renamings));
      m_mappings = detail::aterm_map_spec_to_function_set(atermpp::replace(atermpp::arg3(term), renamings));
      m_equations = detail::aterm_data_eqn_spec_to_equation_set(atermpp::replace(atermpp::arg4(term), renamings));
    }

    namespace detail {
      // mutable_map_substitution is used because at present there is no other decent method to accomplish the same
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
        typedef atermpp::set< function_symbol > constructors_set;
        typedef atermpp::set< function_symbol > mappings_set;
        typedef atermpp::set< data_equation >   equations_set;

        sorts_set sorts = boost::copy_range< sorts_set >(s.aliases());

        // Sort expression substitutions for substituting parts of the sorts of expressions
        mutable_map_substitution< atermpp::aterm_appl, atermpp::aterm_appl > renamings;

        // remove structured sorts and container sorts
        for (data_specification::sorts_const_range r(s.sorts()); !r.empty(); r.advance_begin(1))
        {
          sort_expression current = r.front();

          if (current.is_container_sort() || current.is_structured_sort())
          {
            basic_sort main_identifier(local::find_suitable_identifier(sorts, s, current));

            sorts.insert(alias(main_identifier, current));

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
          }
          else if (!current.is_alias())
          {
            sorts.insert(current);
          }
        }

        using namespace core::detail;

        return gsMakeDataSpec(
           gsMakeSortSpec(convert< atermpp::aterm_list >(sorts)),
           gsMakeConsSpec(atermpp::replace(boost::copy_range< atermpp::aterm_list >(s.constructors()), renamings)),
           gsMakeMapSpec(atermpp::replace(boost::copy_range< atermpp::aterm_list >(s.mappings()), renamings)),
           gsMakeDataEqnSpec(atermpp::replace(boost::copy_range< atermpp::aterm_list >(s.equations()), renamings)));
      }
    } // namespace detail

  } // namespace new_data
} // namespace mcrl2

