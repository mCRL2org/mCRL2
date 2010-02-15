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
#include "boost/iterator/transform_iterator.hpp"

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
#include "mcrl2/data/application.h"
#include "mcrl2/data/find.h"
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
      atermpp::aterm_appl data_specification_to_aterm_data_spec(const data_specification& s, bool compatible)
      { using namespace core::detail;

        return gsMakeDataSpec(
             gsMakeSortSpec(convert< atermpp::aterm_list >(s.m_sorts) +
                            convert< atermpp::aterm_list >(data_specification::aliases_const_range(s.m_aliases))),
             gsMakeConsSpec(convert< atermpp::aterm_list >(data_specification::constructors_const_range(s.m_constructors))),
             gsMakeMapSpec(convert< atermpp::aterm_list >(data_specification::constructors_const_range(s.m_mappings))),
             gsMakeDataEqnSpec(convert< atermpp::aterm_list >(s.m_equations)));
      }
    } // namespace detail
    /// \endcond


    template < typename Container, typename Sequence >
    void insert(Container& container, Sequence sequence)
    { container.insert(sequence.begin(), sequence.end());
    } 

    void data_specification::normalise_sorts() const
    { // Normalise the sorts of the constructors.
      m_normalised_sorts.clear();
      m_normalised_constructors.clear();
      m_normalised_mappings.clear();
      m_normalised_equations.clear();
      reconstruct_m_normalised_aliases();
      for(atermpp::set< sort_expression >::const_iterator i=m_sorts.begin();
           i!=m_sorts.end(); ++i)
      { m_normalised_sorts.insert(normalise_sorts(*i));
        import_system_defined_sort(*i);
      }

      for(atermpp::set< sort_expression >::const_iterator i=m_sorts_in_context.begin();
           i!=m_sorts_in_context.end(); ++i)
      { import_system_defined_sort(*i);
      }

      std::set< sort_expression > dependent_sorts;
      dependent_sorts.insert(sort_bool::bool_());

      // constructors
      insert(dependent_sorts, make_sort_range(constructors_const_range(m_constructors)));

      // mappings
      insert(dependent_sorts, make_sort_range(constructors_const_range(m_mappings)));

      // equations
      for (equations_const_range r(m_equations); !r.empty(); r.advance_begin(1))
      { // make function sort in case of constants to add the corresponding sort as needed
        insert(dependent_sorts, find_sort_expressions(r.front()));
      }

      // aliases, with both left and right hand sides.
      for(ltr_aliases_map::const_iterator i=m_aliases.begin();
                  i!=m_aliases.end(); ++i)
      { dependent_sorts.insert(i->first);
        insert(dependent_sorts,find_sort_expressions(i->second));
      } 

      for(atermpp::set< sort_expression >::const_iterator i=dependent_sorts.begin();
           i!=dependent_sorts.end(); ++i)
      { m_normalised_sorts.insert(normalise_sorts(*i));
        import_system_defined_sort(*i);
      }


      for(ltr_aliases_map ::const_iterator i=m_aliases.begin();  
           i!=m_aliases.end(); ++i)
      { m_normalised_sorts.insert(normalise_sorts(i->first));
        m_normalised_sorts.insert(normalise_sorts(i->second));
        import_system_defined_sort(i->first);
        import_system_defined_sort(i->second);
      }

      // sort_to_symbol_map new_constructors;
      for(sort_to_symbol_map::const_iterator i=m_constructors.begin();
           i!=m_constructors.end(); ++i)
      { const sort_expression normalised_sort=normalise_sorts(i->first);
        const function_symbol normalised_constructor=normalise_sorts(i->second);

        mappings_const_range range(m_normalised_constructors.equal_range(normalised_constructor.sort().target_sort()));
        if (std::find(range.begin(), range.end(), normalised_constructor) == range.end())
        { m_normalised_constructors.insert(std::pair<sort_expression, function_symbol>
                     (normalised_sort,normalised_constructor));
        }
        m_normalised_sorts.insert(normalised_sort);
      }

      // Normalise the sorts of the mappings.
      for(sort_to_symbol_map::const_iterator i=m_mappings.begin();
           i!=m_mappings.end(); ++i)
      { const sort_expression normalised_sort=normalise_sorts(i->first);
        const function_symbol normalised_mapping=normalise_sorts(i->second);

        mappings_const_range range(m_normalised_mappings.equal_range(normalised_mapping.sort().target_sort()));
        if (std::find(range.begin(), range.end(), normalised_mapping) == range.end())
        { m_normalised_mappings.insert((std::pair<sort_expression, function_symbol>
                      (normalised_sort,normalised_mapping)));
        }
        m_normalised_sorts.insert(normalised_sort);
      }
     
      // Normalise the sorts of the expressions and variables in equations.
      for(atermpp::set< data_equation >::const_iterator i=m_equations.begin();
           i!=m_equations.end(); ++i)
      { add_system_defined_equation(*i);
      }
    }

    /// \pre sort.is_system_defined()
    void data_specification::import_system_defined_sort(sort_expression const& sort) const
    { 
      const sort_expression normalised_sort=normalise_sorts(sort);
      // add sorts, constructors, mappings and equations
      if (normalised_sort == sort_bool::bool_())
      { // Add bool to the specification 
        add_system_defined_sort(sort_bool::bool_());

        function_symbol_vector f(sort_bool::bool_generate_constructors_code());
        std::for_each(f.begin(), f.end(), boost::bind(&data_specification::add_system_defined_constructor, this, _1));
        f = sort_bool::bool_generate_functions_code();
        std::for_each(f.begin(), f.end(), boost::bind(&data_specification::add_system_defined_mapping, this, _1));
        data_equation_vector e(sort_bool::bool_generate_equations_code());
        std::for_each(e.begin(), e.end(), boost::bind(&data_specification::add_system_defined_equation, this, _1));
      }
      else if (normalised_sort == sort_real::real_())
      { // Add Real to the specification
        add_system_defined_sort(sort_real::real_());

        function_symbol_vector f(sort_real::real_generate_constructors_code());
        std::for_each(f.begin(), f.end(), boost::bind(&data_specification::add_system_defined_constructor, this, _1));
        f = sort_real::real_generate_functions_code();
        std::for_each(f.begin(), f.end(), boost::bind(&data_specification::add_system_defined_mapping, this, _1));
        data_equation_vector e(sort_real::real_generate_equations_code());
        std::for_each(e.begin(), e.end(), boost::bind(&data_specification::add_system_defined_equation, this, _1));
 
        import_system_defined_sort(sort_int::int_()); // A full definition of Int is required
                                                      // as the rewrite rules of Real rely on it.
      }
      else if (normalised_sort == sort_int::int_())
      { // Add Int to the specification 
        add_system_defined_sort(sort_int::int_()); 

        function_symbol_vector f(sort_int::int_generate_constructors_code());
        std::for_each(f.begin(), f.end(), boost::bind(&data_specification::add_system_defined_constructor, this, _1));
        f = sort_int::int_generate_functions_code();
        std::for_each(f.begin(), f.end(), boost::bind(&data_specification::add_system_defined_mapping, this, _1));
        data_equation_vector e(sort_int::int_generate_equations_code());
        std::for_each(e.begin(), e.end(), boost::bind(&data_specification::add_system_defined_equation, this, _1));

        import_system_defined_sort(sort_nat::nat());  // See above, Int requires Nat.
      }
      else if (normalised_sort == sort_nat::nat())
      { // Add Nat to the specification
        add_system_defined_sort(sort_nat::natpair());
        add_system_defined_sort(sort_nat::nat());

        function_symbol_vector f(sort_nat::nat_generate_constructors_code());
        std::for_each(f.begin(), f.end(), boost::bind(&data_specification::add_system_defined_constructor, this, _1));
        f = sort_nat::nat_generate_functions_code();
        std::for_each(f.begin(), f.end(), boost::bind(&data_specification::add_system_defined_mapping, this, _1));
        data_equation_vector e(sort_nat::nat_generate_equations_code());
        std::for_each(e.begin(), e.end(), boost::bind(&data_specification::add_system_defined_equation, this, _1));

        import_system_defined_sort(sort_pos::pos());  // See above, Nat requires Pos.
      }
      else if (normalised_sort == sort_pos::pos())
      { // Add Pos to the specification
        add_system_defined_sort(sort_pos::pos());

        function_symbol_vector f(sort_pos::pos_generate_constructors_code());
        std::for_each(f.begin(), f.end(), boost::bind(&data_specification::add_system_defined_constructor, this, _1));
        f = sort_pos::pos_generate_functions_code();
        std::for_each(f.begin(), f.end(), boost::bind(&data_specification::add_system_defined_mapping, this, _1));
        data_equation_vector e(sort_pos::pos_generate_equations_code());
        std::for_each(e.begin(), e.end(), boost::bind(&data_specification::add_system_defined_equation, this, _1));
      }
      else
      { if (sort.is_container_sort())
        { sort_expression element_sort(container_sort(sort).element_sort());
          if (sort_list::is_list(sort))
          { import_system_defined_sort(sort_nat::nat()); // Required for lists.
               
            // Add a list to the specification.
            add_system_defined_sort(sort);

            function_symbol_vector f(sort_list::list_generate_constructors_code(element_sort));
            std::for_each(f.begin(), f.end(), boost::bind(&data_specification::add_system_defined_constructor, this, _1));
            f = sort_list::list_generate_functions_code(element_sort);
            std::for_each(f.begin(), f.end(), boost::bind(&data_specification::add_system_defined_mapping, this, _1));
            data_equation_vector e(sort_list::list_generate_equations_code(element_sort));
            std::for_each(e.begin(), e.end(), boost::bind(&data_specification::add_system_defined_equation, this, _1));
          }
          else if (sort_set::is_set(sort))
          {
            // Add a set to the specification.
            add_system_defined_sort(sort);
            function_symbol_vector f(sort_set::set_generate_constructors_code(element_sort));
            std::for_each(f.begin(), f.end(), boost::bind(&data_specification::add_system_defined_constructor, this, _1));
            f = sort_set::set_generate_functions_code(element_sort);
            std::for_each(f.begin(), f.end(), boost::bind(&data_specification::add_system_defined_mapping, this, _1));
            data_equation_vector e(sort_set::set_generate_equations_code(element_sort));
            std::for_each(e.begin(), e.end(), boost::bind(&data_specification::add_system_defined_equation, this, _1));

            // Add also the finite set specification.
            add_system_defined_sort(sort);
            f = sort_fset::fset_generate_constructors_code(element_sort);
            std::for_each(f.begin(), f.end(), boost::bind(&data_specification::add_system_defined_constructor, this, _1));
            f = sort_fset::fset_generate_functions_code(element_sort);
            std::for_each(f.begin(), f.end(), boost::bind(&data_specification::add_system_defined_mapping, this, _1));
            e = sort_fset::fset_generate_equations_code(element_sort);
            std::for_each(e.begin(), e.end(), boost::bind(&data_specification::add_system_defined_equation, this, _1));
          }
          else if (sort_bag::is_bag(sort))
          { import_system_defined_sort(sort_nat::nat()); // Required for bags.
            import_system_defined_sort(sort_set::set_(element_sort));
            // Add a bag to the specification.
            add_system_defined_sort(sort_fbag::fbag(element_sort));
            function_symbol_vector f(sort_bag::bag_generate_constructors_code(element_sort));
            std::for_each(f.begin(), f.end(), boost::bind(&data_specification::add_system_defined_constructor, this, _1));
            f = sort_bag::bag_generate_functions_code(element_sort);
            std::for_each(f.begin(), f.end(), boost::bind(&data_specification::add_system_defined_mapping, this, _1));
            data_equation_vector e(sort_bag::bag_generate_equations_code(element_sort));
            std::for_each(e.begin(), e.end(), boost::bind(&data_specification::add_system_defined_equation, this, _1));

            // Also add a finite bag to the specification
            add_system_defined_sort(sort);
            f = sort_fbag::fbag_generate_constructors_code(element_sort);
            std::for_each(f.begin(), f.end(), boost::bind(&data_specification::add_system_defined_constructor, this, _1));
            f = sort_fbag::fbag_generate_functions_code(element_sort);
            std::for_each(f.begin(), f.end(), boost::bind(&data_specification::add_system_defined_mapping, this, _1));
            e = sort_fbag::fbag_generate_equations_code(element_sort);
            std::for_each(e.begin(), e.end(), boost::bind(&data_specification::add_system_defined_equation, this, _1));
           }
        }
        else if (sort.is_structured_sort())
        { insert_mappings_constructors_for_structured_sort(sort);
        }
      }
      add_standard_mappings_and_equations(normalised_sort);
    }

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

                /*****************************************/
                /*                                       */
                /*  m_normalised_aliases_are_up_to_date  */
                /*                                       */
                /*****************************************/

    void data_specification::reconstruct_m_normalised_aliases() const
    { 
     // First reset the normalised aliases and the mappings and constructors that have been
     // inherited to basic sort aliases during a previous round of sort normalisation.
     m_normalised_aliases.clear(); 

     // Copy m_normalised_aliases. Simple aliases are stored from left to 
     // right. If the right hand side is non trivial (struct, list, set or bag)
     // the alias is stored from right to left.
     for(ltr_aliases_map::const_iterator i=m_aliases.begin();
               i!=m_aliases.end(); ++i)
     { assert(m_normalised_aliases.count(i->first)==0); // sort aliases have a unique left hand side.
       if (is_structured_sort(i->second) ||
           is_function_sort(i->second) ||
           is_container_sort(i->second))
       { // We deal here with a declaration of the shape sort A=ComplexType.
         // Rewrite every occurrence of ComplexType to A. Suppose that there are
         // two declarations of the shape sort A=ComplexType; B=ComplexType then
         // ComplexType is rewritten to A and B is also rewritten to A.
         const atermpp::map< sort_expression, sort_expression >::const_iterator j=m_normalised_aliases.find(i->second);
         if (j!=m_normalised_aliases.end())
         { m_normalised_aliases[i->first]=j->second;
         }
         else 
         { m_normalised_aliases[i->second]=i->first;
         }
       }
       else
       { // We are dealing with a sort declaration of the shape sort A=B.
         // Every occurrence of sort A is normalised to sort B.
         assert(is_basic_sort(i->first));
         m_normalised_aliases[i->first]=i->second;
       }
     }

     // Close the mapping m_normalised_aliases under itself. If a rewriting
     // loop is detected, throw a runtime error.

     for(atermpp::map< sort_expression, sort_expression >::iterator i=m_normalised_aliases.begin();
              i!=m_normalised_aliases.end(); i++)
     { std::set < sort_expression > sort_already_seen;
       sort_expression result_sort=i->second;

       std::set< sort_expression > all_sorts;
       if (is_container_sort(i->first) || is_function_sort(i->first))
       { find_sort_expressions(i->first, std::inserter(all_sorts, all_sorts.end()));
       }
       while (m_normalised_aliases.count(result_sort)>0)
       { sort_already_seen.insert(result_sort);
         result_sort= m_normalised_aliases.find(result_sort)->second;
         if (sort_already_seen.count(result_sort))
         { throw mcrl2::runtime_error("Sort alias " + pp(result_sort) + " is defined in terms of itself.");
         }

         for (std::set< sort_expression >::const_iterator j = all_sorts.begin(); j != all_sorts.end(); ++j)
         { if (*j==result_sort)
           { throw mcrl2::runtime_error("Sort alias " + pp(i->first) + " depends on sort" +
                                           pp(result_sort) + ", which is circularly defined.\n");
           }
         }
       }
       // So the normalised sort of i->first is result_sort. 
       i->second=result_sort;
     }
    }

    /* template <typename Object> Object data_specification::normalise_sorts(const Object& o) const
    { normalise_specification_if_required();
      std::cerr << "Object " << o << "\n";
      substitution < Object, sort_expression, Object > sigma(m_normalised_aliases);
      return sigma(o);
    } */

    sort_expression data_specification::normalise_sorts_helper(const sort_expression & e) const
    { // Check whether e has already a normalised sort
      // If yes return it.
      const atermpp::map< sort_expression, sort_expression >::const_iterator i=m_normalised_aliases.find(e);
      if (i!=m_normalised_aliases.end())
      { 
        return i->second;
      }

      if (e.is_basic_sort())
      { // Apparently, e is already a normalised sort.
        return e;
      }
      else if (e.is_function_sort())
      { 
        atermpp::vector< sort_expression > new_domain;
        for (boost::iterator_range< function_sort::domain_const_range::iterator > r(function_sort(e).domain()); 
                  !r.empty(); r.advance_begin(1))
        { new_domain.push_back(normalise_sorts_helper(r.front()));
        }
        return function_sort(new_domain, normalise_sorts_helper(function_sort(e).codomain()));
      }
      else if (e.is_container_sort())
      { return container_sort(container_sort(e).container_type(), normalise_sorts_helper(container_sort(e).element_sort()));
      }
      else if (e.is_structured_sort())
      { atermpp::vector< structured_sort_constructor > new_constructors;
        for (structured_sort::constructors_const_range r(structured_sort(e).struct_constructors()); !r.empty(); r.advance_begin(1))
        { atermpp::vector< structured_sort_constructor_argument > new_arguments;
          for (structured_sort_constructor::arguments_const_range ra(r.front().arguments()); !ra.empty(); ra.advance_begin(1))
          { new_arguments.push_back(structured_sort_constructor_argument(normalise_sorts_helper(ra.front().sort()), ra.front().name()));
          }
          new_constructors.push_back(structured_sort_constructor(r.front().name(), new_arguments, r.front().recogniser()));
        }
        return structured_sort(new_constructors);
      }
      return e;
    } 

    sort_expression data_specification::normalise_sorts(const sort_expression & e) const
    { normalise_specification_if_required();
      return normalise_sorts_helper(e);
    }

    function_symbol data_specification::normalise_sorts(function_symbol const& f) const
    { normalise_specification_if_required();
      return function_symbol(f.name(),normalise_sorts(f.sort()));
    }

    data_expression data_specification::normalise_sorts(data_expression const& e) const
    { normalise_specification_if_required();
      if (e.is_abstraction())
      { const abstraction a(e);
        const abstraction::variables_const_range variables=a.variables();
        variable_vector normalised_variables;
        for(abstraction::variables_const_range::const_iterator i=variables.begin();
              i!=variables.end(); ++i)
        { normalised_variables.push_back(variable(i->name(),normalise_sorts(i->sort())));
        }
        
        return abstraction(a.binding_operator(),normalised_variables,normalise_sorts(a.body()));
      }
      if (e.is_application())
      { const application a(e);
        const application::arguments_const_range args=a.arguments();
        data_expression_vector normalised_arguments;
        for(application::arguments_const_range::const_iterator i=args.begin();
            i!=args.end(); ++i)
        { normalised_arguments.push_back(normalise_sorts(*i));
        }
        return application(normalise_sorts(a.head()),normalised_arguments);
      }
      if (e.is_function_symbol())
      { return function_symbol(function_symbol(e).name(),normalise_sorts(e.sort()));
      }
      /* if (e.is_list_expression())
      { return COMMENTARIEER UIT
      }*/
      if (e.is_variable())
      { return variable(variable(e).name(),normalise_sorts(e.sort()));
      }
      assert(e.is_where_clause());
      const where_clause w(e);
      const where_clause::declarations_const_range decls=w.declarations();
      assignment_vector normalised_assignments;
      for(atermpp::term_list <assignment>::const_iterator i=decls.begin();
             i!=decls.end(); ++i)
      { const variable v=i->lhs();
        const data_expression exp=i->rhs();
        normalised_assignments.push_back(assignment(normalise_sorts(v),normalise_sorts(exp)));
      }
      
      return where_clause(normalise_sorts(w.body()),normalised_assignments);
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

      // Store the sorts and aliases.
      for (atermpp::term_list_iterator< atermpp::aterm_appl > i = term_sorts.begin(); i != term_sorts.end(); ++i)
      { if (data::is_alias(*i)) // Compatibility with legacy code
        { // if (!detail::has_legacy_name(alias(*i).name()))
          { add_alias(*i);
          }
        }
        else 
        { m_sorts.insert(*i);
        } 
      }

      // Store the constructors.
      for (atermpp::term_list_iterator< function_symbol > i = term_constructors.begin(); i != term_constructors.end(); ++i)
      { m_constructors.insert(sort_to_symbol_map::value_type(i->sort().target_sort(), *i));
      }

      // Store the mappings.
      for (atermpp::term_list_iterator< function_symbol > i = term_mappings.begin(); i != term_mappings.end(); ++i)
      { m_mappings.insert(sort_to_symbol_map::value_type(i->sort().target_sort(), *i));
      }

      // Store the equations.
      for (atermpp::term_list_iterator< data_equation > i = term_equations.begin(); i != term_equations.end(); ++i)
      { m_equations.insert(*i);
      }

      data_is_not_necessarily_normalised_anymore();
    }
  } // namespace data
} // namespace mcrl2

