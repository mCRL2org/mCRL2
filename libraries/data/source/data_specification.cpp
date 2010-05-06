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
#include "mcrl2/data/function_update.h"
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
      atermpp::aterm_appl data_specification_to_aterm_data_spec(const data_specification& s)
      { using namespace core::detail;

        return gsMakeDataSpec(
             gsMakeSortSpec(atermpp::convert< atermpp::aterm_list >(s.m_sorts) +
                            atermpp::convert< atermpp::aterm_list >(data_specification::aliases_const_range(s.m_aliases))),
             gsMakeConsSpec(atermpp::convert< atermpp::aterm_list >(data_specification::constructors_const_range(s.m_constructors))),
             gsMakeMapSpec(atermpp::convert< atermpp::aterm_list >(data_specification::constructors_const_range(s.m_mappings))),
             gsMakeDataEqnSpec(atermpp::convert< atermpp::aterm_list >(s.m_equations)));
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
      std::set < sort_expression > sorts_already_added_to_m_normalised_sorts;
      reconstruct_m_normalised_aliases();
      for(atermpp::set< sort_expression >::const_iterator i=m_sorts.begin();
           i!=m_sorts.end(); ++i)
      { add_system_defined_sort(*i);
        import_system_defined_sort(*i,sorts_already_added_to_m_normalised_sorts);
      }

      for(atermpp::set< sort_expression >::const_iterator i=m_sorts_in_context.begin();
           i!=m_sorts_in_context.end(); ++i)
      { import_system_defined_sort(*i,sorts_already_added_to_m_normalised_sorts);
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
      { add_system_defined_sort(*i);
        import_system_defined_sort(*i,sorts_already_added_to_m_normalised_sorts);
      }


      for(ltr_aliases_map ::const_iterator i=m_aliases.begin();  
           i!=m_aliases.end(); ++i)
      { add_system_defined_sort(i->first);
        add_system_defined_sort(i->second);
        import_system_defined_sort(i->first,sorts_already_added_to_m_normalised_sorts);
        import_system_defined_sort(i->second,sorts_already_added_to_m_normalised_sorts);
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
        add_system_defined_sort(normalised_sort);
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
        add_system_defined_sort(normalised_sort);
      }
     
      // Normalise the sorts of the expressions and variables in equations.
      for(atermpp::set< data_equation >::const_iterator i=m_equations.begin();
           i!=m_equations.end(); ++i)
      { add_system_defined_equation(*i);
      }
    }

    /// The function normalise_sorts imports for the given sort_expression sort all sorts, constructors,
    /// mappings and equations that belong to this sort to the `normalised' sets in this
    /// data type. E.g. for the sort Nat of natural numbers, it is required that Pos 
    /// (positive numbers) are defined. 
    void data_specification::import_system_defined_sort(
                                      sort_expression const& sort,
                                      std::set <sort_expression> &sorts_already_added_to_m_normalised_sorts) const
    { 
      // First check whether sort has already been added. If yes, we can skip this step.
      if (sorts_already_added_to_m_normalised_sorts.count(sort)>0) return;
      sorts_already_added_to_m_normalised_sorts.insert(sort);

      // add sorts, constructors, mappings and equations
      if (sort == sort_bool::bool_())
      { // Add bool to the specification 
        add_system_defined_sort(sort_bool::bool_());

        function_symbol_vector f(sort_bool::bool_generate_constructors_code());
        std::for_each(f.begin(), f.end(), boost::bind(&data_specification::add_system_defined_constructor, this, _1));
        f = sort_bool::bool_generate_functions_code();
        std::for_each(f.begin(), f.end(), boost::bind(&data_specification::add_system_defined_mapping, this, _1));
        data_equation_vector e(sort_bool::bool_generate_equations_code());
        std::for_each(e.begin(), e.end(), boost::bind(&data_specification::add_system_defined_equation, this, _1));
      }
      else if (sort == sort_real::real_())
      { // Add Real to the specification
        add_system_defined_sort(sort_real::real_());

        function_symbol_vector f(sort_real::real_generate_constructors_code());
        std::for_each(f.begin(), f.end(), boost::bind(&data_specification::add_system_defined_constructor, this, _1));
        f = sort_real::real_generate_functions_code();
        std::for_each(f.begin(), f.end(), boost::bind(&data_specification::add_system_defined_mapping, this, _1));
        data_equation_vector e(sort_real::real_generate_equations_code());
        std::for_each(e.begin(), e.end(), boost::bind(&data_specification::add_system_defined_equation, this, _1));
 
        import_system_defined_sort(sort_int::int_(),sorts_already_added_to_m_normalised_sorts); 
                                                      // A full definition of Int is required
                                                      // as the rewrite rules of Real rely on it.
      }
      else if (sort == sort_int::int_())
      { // Add Int to the specification 
        add_system_defined_sort(sort_int::int_()); 

        function_symbol_vector f(sort_int::int_generate_constructors_code());
        std::for_each(f.begin(), f.end(), boost::bind(&data_specification::add_system_defined_constructor, this, _1));
        f = sort_int::int_generate_functions_code();
        std::for_each(f.begin(), f.end(), boost::bind(&data_specification::add_system_defined_mapping, this, _1));
        data_equation_vector e(sort_int::int_generate_equations_code());
        std::for_each(e.begin(), e.end(), boost::bind(&data_specification::add_system_defined_equation, this, _1));

        import_system_defined_sort(sort_nat::nat(),sorts_already_added_to_m_normalised_sorts);  
                                                       // See above, Int requires Nat.
      }
      else if (sort == sort_nat::nat())
      { // Add Nat to the specification
        add_system_defined_sort(sort_nat::natpair());
        add_system_defined_sort(sort_nat::nat());

        function_symbol_vector f(sort_nat::nat_generate_constructors_code());
        std::for_each(f.begin(), f.end(), boost::bind(&data_specification::add_system_defined_constructor, this, _1));
        f = sort_nat::nat_generate_functions_code();
        std::for_each(f.begin(), f.end(), boost::bind(&data_specification::add_system_defined_mapping, this, _1));
        data_equation_vector e(sort_nat::nat_generate_equations_code());
        std::for_each(e.begin(), e.end(), boost::bind(&data_specification::add_system_defined_equation, this, _1));

        import_system_defined_sort(sort_pos::pos(),sorts_already_added_to_m_normalised_sorts);  // See above, Nat requires Pos.
      }
      else if (sort == sort_pos::pos())
      { // Add Pos to the specification
        add_system_defined_sort(sort_pos::pos());

        function_symbol_vector f(sort_pos::pos_generate_constructors_code());
        std::for_each(f.begin(), f.end(), boost::bind(&data_specification::add_system_defined_constructor, this, _1));
        f = sort_pos::pos_generate_functions_code();
        std::for_each(f.begin(), f.end(), boost::bind(&data_specification::add_system_defined_mapping, this, _1));
        data_equation_vector e(sort_pos::pos_generate_equations_code());
        std::for_each(e.begin(), e.end(), boost::bind(&data_specification::add_system_defined_equation, this, _1));
      }
      else if (is_function_sort(sort))
      { const sort_expression t=function_sort(sort).codomain();
        import_system_defined_sort(t,sorts_already_added_to_m_normalised_sorts);
        const sort_expression_list &l=function_sort(sort).domain();
        for(sort_expression_list::const_iterator i=l.begin(); i!=l.end(); ++i)
        {
          import_system_defined_sort(*i,sorts_already_added_to_m_normalised_sorts);
        }
        if (l.size()==1)
        {  data_equation_vector e(function_update_generate_equations_code(l.front(),t));
           std::for_each(e.begin(), e.end(), boost::bind(&data_specification::add_system_defined_equation, this, _1));
        }
      }
      else if (is_container_sort(sort))
      {
        sort_expression element_sort(container_sort(sort).element_sort());
        // Import the element sort (which may be a complex sort also).
        import_system_defined_sort(element_sort,sorts_already_added_to_m_normalised_sorts);
        if (sort_list::is_list(sort))
        {
          import_system_defined_sort(sort_nat::nat(),sorts_already_added_to_m_normalised_sorts); // Required for lists.

          // Add a list to the specification.
          add_system_defined_sort(sort);

          function_symbol_vector f(sort_list::list_generate_constructors_code(element_sort));
          std::for_each(f.begin(), f.end(), boost::bind(&data_specification::add_system_defined_constructor, this, _1));
          f = sort_list::list_generate_functions_code(element_sort);
          std::for_each(f.begin(), f.end(), boost::bind(&data_specification::add_system_defined_mapping, this, _1));
          data_equation_vector e(sort_list::list_generate_equations_code(element_sort));
          std::for_each(e.begin(), e.end(), boost::bind(&data_specification::add_system_defined_equation, this, _1));
        }
        else if (sort_set::is_set(sort)||sort_fset::is_fset(sort))
        {
          // Add the function sort element_sort->Bool to the specification
          // const sort_expression_list l(element_sort);
          import_system_defined_sort(function_sort(push_front(sort_expression_list(),element_sort),sort_bool::bool_()),
                                           sorts_already_added_to_m_normalised_sorts);

          // Add a set to the specification.
          add_system_defined_sort(sort_set::set_(element_sort));
          function_symbol_vector f(sort_set::set_generate_constructors_code(element_sort));
          std::for_each(f.begin(), f.end(), boost::bind(&data_specification::add_system_defined_constructor, this, _1));
          f = sort_set::set_generate_functions_code(element_sort);
          std::for_each(f.begin(), f.end(), boost::bind(&data_specification::add_system_defined_mapping, this, _1));
          data_equation_vector e(sort_set::set_generate_equations_code(element_sort));
          std::for_each(e.begin(), e.end(), boost::bind(&data_specification::add_system_defined_equation, this, _1));

          // Add also the finite set specification.
          add_system_defined_sort(sort_fset::fset(element_sort));
          f = sort_fset::fset_generate_constructors_code(element_sort);
          std::for_each(f.begin(), f.end(), boost::bind(&data_specification::add_system_defined_constructor, this, _1));
          f = sort_fset::fset_generate_functions_code(element_sort);
          std::for_each(f.begin(), f.end(), boost::bind(&data_specification::add_system_defined_mapping, this, _1));
          e = sort_fset::fset_generate_equations_code(element_sort);
          std::for_each(e.begin(), e.end(), boost::bind(&data_specification::add_system_defined_equation, this, _1));
        }
        else if (sort_bag::is_bag(sort)||sort_fbag::is_fbag(sort))
        {
          // Add the sorts Nat and set_(element_sort) to the specification.
          import_system_defined_sort(sort_nat::nat(),sorts_already_added_to_m_normalised_sorts); // Required for bags.
          import_system_defined_sort(sort_set::set_(element_sort),sorts_already_added_to_m_normalised_sorts);

          // Add the function sort element_sort->Nat to the specification
          import_system_defined_sort(function_sort(push_front(sort_expression_list(),element_sort),sort_nat::nat()),
                                                    sorts_already_added_to_m_normalised_sorts);

          // Add a bag to the specification.
          add_system_defined_sort(sort_bag::bag(element_sort));
          function_symbol_vector f(sort_bag::bag_generate_constructors_code(element_sort));
          std::for_each(f.begin(), f.end(), boost::bind(&data_specification::add_system_defined_constructor, this, _1));
          f = sort_bag::bag_generate_functions_code(element_sort);
          std::for_each(f.begin(), f.end(), boost::bind(&data_specification::add_system_defined_mapping, this, _1));
          data_equation_vector e(sort_bag::bag_generate_equations_code(element_sort));
          std::for_each(e.begin(), e.end(), boost::bind(&data_specification::add_system_defined_equation, this, _1));

          // Also add a finite bag to the specification
          add_system_defined_sort(sort_fbag::fbag(element_sort));
          f = sort_fbag::fbag_generate_constructors_code(element_sort);
          std::for_each(f.begin(), f.end(), boost::bind(&data_specification::add_system_defined_constructor, this, _1));
          f = sort_fbag::fbag_generate_functions_code(element_sort);
          std::for_each(f.begin(), f.end(), boost::bind(&data_specification::add_system_defined_mapping, this, _1));
          e = sort_fbag::fbag_generate_equations_code(element_sort);
          std::for_each(e.begin(), e.end(), boost::bind(&data_specification::add_system_defined_equation, this, _1));
        }
      }
      else if (is_structured_sort(sort))
      {
        insert_mappings_constructors_for_structured_sort(sort);
      }
      const sort_expression normalised_sort=normalise_sorts(sort);
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

                /*****************************************/
                /*                                       */
                /*  m_normalised_aliases_are_up_to_date  */
                /*                                       */
                /*****************************************/

    // The function below checks whether there is an alias loop, e.g. aliases
    // of the form A=B; B=A; or more complex A=B->C; B=Set(D); D=List(A); Loops
    // through structured sorts are allowed. If a loop is detected, an exception
    // is thrown. 
    void data_specification::check_for_alias_loop(
                 const sort_expression s, 
                 std::set < sort_expression > sorts_already_seen) const
    { 
      if (is_basic_sort(s))
      { 
        if (sorts_already_seen.count(s)>0)
        { 
          throw mcrl2::runtime_error("Sort alias " + pp(s) + " is defined in terms of itself.");
        }
        const ltr_aliases_map::const_iterator i=m_aliases.find(s);
        if (i!=m_aliases.end())  // s is in m_aliases
        { 
          sorts_already_seen.insert(s);
          check_for_alias_loop(i->second,sorts_already_seen);
          sorts_already_seen.erase(s);
        }
        return;
      }
    
      if (is_container_sort(s))
      { check_for_alias_loop(container_sort(s).element_sort(),sorts_already_seen);
        return;
      }
      
      if (is_function_sort(s))
      { for (boost::iterator_range< sort_expression_list::iterator > r(function_sort(s).domain());
                  !r.empty(); r.advance_begin(1))
        { 
          check_for_alias_loop(r.front(),sorts_already_seen);
        }

        check_for_alias_loop(function_sort(s).codomain(),sorts_already_seen);
        return;
      }
      
      assert(is_structured_sort(s)); // In this case we do not need to carry out a check.

    }
    
    // This function returns the normal form of e, under the two maps map1 and map2.
    // This normal form is obtained by repeatedly applying map1 and map2, until this
    // is not possible anymore. It is assumed that this procedure terminates. There is
    // no check for loops. 
    static basic_sort find_normal_form(
                    const basic_sort &e, 
                    const atermpp::multimap< sort_expression, basic_sort >  &map1,
                    const atermpp::multimap< sort_expression, basic_sort >  &map2
#ifndef NDEBUG
                    , std::set < basic_sort > sorts_already_seen = std::set < basic_sort >()
#endif
      )
    { 
      assert(sorts_already_seen.find(e)==sorts_already_seen.end()); // e has not been seen already.
      const atermpp::multimap< sort_expression, basic_sort >::const_iterator i1=map1.find(e);
      if (i1!=map1.end()) // found
      { 
#ifndef NDEBUG
        sorts_already_seen.insert(i1->second);
#endif
        return find_normal_form(i1->second,map1,map2
#ifndef NDEBUG
                               ,sorts_already_seen
#endif
                               );
      }
      const atermpp::multimap< sort_expression, basic_sort >::const_iterator i2=map2.find(e);
      if (i2!=map2.end()) // found
      { 
#ifndef NDEBUG
        sorts_already_seen.insert(i2->second);
#endif
        return find_normal_form(i2->second,map1,map2
#ifndef NDEBUG
                               ,sorts_already_seen
#endif
                               );
      }
      return e;
    }

    // The function below recalculates m_normalised_aliases, such that 
    // it forms a confluent terminating rewriting system using which
    // sorts can be normalised. 
    void data_specification::reconstruct_m_normalised_aliases() const
    { 
      // First reset the normalised aliases and the mappings and constructors that have been
      // inherited to basic sort aliases during a previous round of sort normalisation.
      m_normalised_aliases.clear(); 

      // Check for loops in the aliases. The type checker should already have done this,
      // but we check it again here.
      for(ltr_aliases_map::const_iterator i=m_aliases.begin();
                i!=m_aliases.end(); ++i)
      { 
        std::set < sort_expression > sorts_already_seen; // Empty set.
        check_for_alias_loop(i->first,sorts_already_seen); 
      }

      // Copy m_normalised_aliases. All aliases are stored from right to left,
      // assuming that the alias is introduced with a reason. 
      atermpp::multimap< sort_expression, basic_sort > sort_aliases_to_be_investigated;
      for(ltr_aliases_map::const_iterator i=m_aliases.begin();
                i!=m_aliases.end(); ++i)
      { 
        sort_aliases_to_be_investigated.insert(std::pair<sort_expression,basic_sort>(i->second,i->first));
      }

      // Apply Knuth-Bendix completion on the rules in m_normalised_aliases.

      atermpp::multimap< sort_expression, basic_sort > resulting_normalized_sort_aliases;

      for( ; !sort_aliases_to_be_investigated.empty() ; )
      { 
        const atermpp::multimap< sort_expression, basic_sort >::iterator p=sort_aliases_to_be_investigated.begin();
        sort_aliases_to_be_investigated.erase(p);
        const sort_expression lhs=p->first;
        const sort_expression rhs=p->second;
     
        for(atermpp::multimap< sort_expression, basic_sort >::const_iterator 
                 i=resulting_normalized_sort_aliases.begin();
                 i!=resulting_normalized_sort_aliases.end(); ++i)
        { 
          const sort_expression s1=replace(lhs,i->first,i->second);
          if (s1!=lhs)
          { 
            assert(is_basic_sort(rhs));
            const basic_sort e1=find_normal_form(rhs,resulting_normalized_sort_aliases,sort_aliases_to_be_investigated);
            if (e1!=s1)
            {
              sort_aliases_to_be_investigated.insert(std::pair<sort_expression,basic_sort > (s1,e1));
            }
          }
          else 
          { 
            const sort_expression s2=replace(i->first,lhs,rhs);
            if (s2!=i->first)
            { 
              assert(is_basic_sort(i->second));
              const basic_sort e2=find_normal_form(i->second,resulting_normalized_sort_aliases,
                                                             sort_aliases_to_be_investigated);
              if (e2!=s2)
              { 
                sort_aliases_to_be_investigated.insert(std::pair<sort_expression,basic_sort > (s2,e2));
              }
            } 
          }
        }
        assert(lhs!=rhs);
        resulting_normalized_sort_aliases.insert(std::pair<sort_expression,basic_sort >(lhs,rhs));

      }
      // Copy resulting_normalized_sort_aliases into m_normalised_aliases, i.e. from multimap to map.
      // If there are rules with equal left hand side, only one is arbitrarily chosen.
      
      for(atermpp::multimap< sort_expression, basic_sort >::const_iterator 
                 i=resulting_normalized_sort_aliases.begin();
                 i!=resulting_normalized_sort_aliases.end(); ++i)
      { 
        m_normalised_aliases.insert(*i);
        assert(i->first!=i->second);
      }
    }

    sort_expression data_specification::normalise_sorts_helper(const sort_expression & e) const
    { // This routine takes the map m_normalised_aliases which contains pairs of sort expressions
      // <A,B> and takes all these pairs as rewrite rules, which are applied to e using an innermost
      // strategy.

      sort_expression new_sort; // This will be a placeholder for the sort of which all
                                // arguments will be normalised.
      if (is_basic_sort(e))
      { 
        new_sort=e;
      }
      else if (is_function_sort(e))
      { // Rewrite the arguments into normal form.
        atermpp::vector< sort_expression > new_domain;
        for (boost::iterator_range< sort_expression_list::iterator > r(function_sort(e).domain());
                  !r.empty(); r.advance_begin(1))
        { new_domain.push_back(normalise_sorts_helper(r.front()));
        }
        new_sort=function_sort(new_domain, normalise_sorts_helper(function_sort(e).codomain()));
      }
      else if (is_container_sort(e))
      { // Rewrite the argument of the container sort to normal form.
        new_sort=container_sort(
                          container_sort(e).container_name(), 
                          normalise_sorts_helper(container_sort(e).element_sort()));
        
      }
      else if (is_structured_sort(e))
      { // Rewrite the argument sorts to normal form.
        atermpp::vector< structured_sort_constructor > new_constructors;
        for (structured_sort::constructors_const_range r(structured_sort(e).struct_constructors()); 
                        !r.empty(); r.advance_begin(1))
        { 
          atermpp::vector< structured_sort_constructor_argument > new_arguments;
          for (structured_sort_constructor::arguments_const_range ra(r.front().arguments()); 
                    !ra.empty(); ra.advance_begin(1))
          { 
            new_arguments.push_back(structured_sort_constructor_argument(
                         normalise_sorts_helper(ra.front().sort()), ra.front().name()));
          }
          new_constructors.push_back(structured_sort_constructor(r.front().name(), new_arguments, r.front().recogniser()));
        }
        new_sort=structured_sort(new_constructors);
      }

      // The arguments of new_sort are now in normal form.
      // Rewrite it to normal form.
      const atermpp::map< sort_expression, sort_expression >::const_iterator i=m_normalised_aliases.find(new_sort);
      if (i==m_normalised_aliases.end())
      {
        return new_sort; // e is a normal form.
      }
      else
      {
        return normalise_sorts_helper(i->second); // rewrite the result until normal form.
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
      if (is_abstraction(e))
      { const abstraction a(e);
        const variable_list variables=a.variables();
        variable_vector normalised_variables;
        for(variable_list::const_iterator i=variables.begin();
              i!=variables.end(); ++i)
        { normalised_variables.push_back(variable(i->name(),normalise_sorts(i->sort())));
        }
        
        return abstraction(a.binding_operator(),normalised_variables,normalise_sorts(a.body()));
      }
      else if (is_application(e))
      { const application a(e);
        const data_expression_list args=a.arguments();
        data_expression_vector normalised_arguments;
        for(data_expression_list::const_iterator i=args.begin();
            i!=args.end(); ++i)
        { normalised_arguments.push_back(normalise_sorts(*i));
        }
        return application(normalise_sorts(a.head()),normalised_arguments);
      }
      else if (is_function_symbol(e))
      { return function_symbol(function_symbol(e).name(),normalise_sorts(e.sort()));
      }
      else if (is_variable(e))
      { return variable(variable(e).name(),normalise_sorts(e.sort()));
      }
      else if (is_where_clause(e))
      {
        const where_clause w(e);
        const assignment_list decls=w.declarations();
        assignment_vector normalised_assignments;
        for(atermpp::term_list <assignment>::const_iterator i=decls.begin();
               i!=decls.end(); ++i)
        { const variable v=i->lhs();
          const data_expression exp=i->rhs();
          normalised_assignments.push_back(assignment(normalise_sorts(v),normalise_sorts(exp)));
        }

        return where_clause(normalise_sorts(w.body()),normalised_assignments);
      }
      else
      {
        throw mcrl2::runtime_error("normalise_sorts: unexpected expression " + e.to_string() + " occurred.");
      }
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

