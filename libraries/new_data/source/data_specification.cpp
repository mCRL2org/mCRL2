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

#include "mcrl2/new_data/data_specification.h"
#include "mcrl2/new_data/detail/sequence_algorithm.h"
#include "mcrl2/new_data/detail/data_utility.h"
#include "mcrl2/new_data/utility.h"
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
    namespace detail {

      /// \cond INTERNAL_DOCS
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
              function_symbol_vector constructors_s(m_specification.constructors(s));

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
        for (aliases_const_range r(aliases(sort)); !r.empty(); r.advance_begin(1))
        {
          if (r.front().name().name()[0] == '@')
          {
            // a reference with internal identifier exists for structured sort so it is already part of specification
            return;
          }
        }

        basic_sort identifier(static_cast< std::string >(fresh_identifier(sorts(), "@struct")));

        structured_sort s_sort(sort);

        add_system_defined_sort(alias(identifier, sort));
        add_system_defined_constructors(boost::make_iterator_range(s_sort.constructor_functions(identifier)));
        add_system_defined_mappings(boost::make_iterator_range(s_sort.projection_functions(identifier)));
        add_system_defined_mappings(boost::make_iterator_range(s_sort.recogniser_functions(identifier)));
        add_system_defined_equations(boost::make_iterator_range(s_sort.constructor_equations(identifier)));
        add_system_defined_equations(boost::make_iterator_range(s_sort.projection_equations(identifier)));
        add_system_defined_equations(boost::make_iterator_range(s_sort.recogniser_equations(identifier)));
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
        dependent_sorts.add(r.front());
      }
      // constructors
      for (constructors_const_range r(constructors()); !r.empty(); r.advance_begin(1))
      {
        dependent_sorts.add(r.front().sort().target_sort());
      }
      // mappings
      for (mappings_const_range r(mappings()); !r.empty(); r.advance_begin(1))
      {
        dependent_sorts.add(r.front().sort());
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
        function_symbol_vector fl(constructors(s));

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

  } // namespace new_data
} // namespace mcrl2

