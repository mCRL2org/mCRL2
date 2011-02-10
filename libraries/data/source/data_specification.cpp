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

#include "mcrl2/data/data_specification.h"
#include "mcrl2/data/print.h"
#include "mcrl2/data/detail/data_utility.h"

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
    { 
      protected:

        data_specification const& m_specification;
        std::set< sort_expression > m_visiting;

        bool is_finite_aux(const sort_expression s)
        {
          for (data_specification::constructors_const_range r(m_specification.constructors(s)); !r.empty(); r.advance_begin(1))
          {
// ATfprintf(stderr,"Constructor %t\n",(ATermAppl)r.front());
            if (is_function_sort(r.front().sort()))
            {
              const function_sort f_sort(r.front().sort());
              const sort_expression_list l=f_sort.domain();

              for (sort_expression_list::const_iterator i=l.begin(); i!=l.end(); ++i)
              {
                if (!is_finite(*i))
                {
                  return false;
                }
              }
            }
          }
          return true;
        }

      public:

        finiteness_helper(data_specification const& specification) : m_specification(specification)
        { }

        bool is_finite(const sort_expression& s)
        {
// ATfprintf(stderr,"Is finite %t\n",(ATermAppl)s);
          assert(s==normalize_sorts(s,m_specification));
          if (m_visiting.count(s)>0)
          {
            return false;
          }

          m_visiting.insert(s);

          bool result=false;
          if (is_basic_sort(s))
          {
            result=is_finite(basic_sort(s));
          }
          else if (is_container_sort(s))
          {
            result=is_finite(container_sort(s));
          }
          else if (is_function_sort(s))
          {
            result=is_finite(function_sort(s));
          }
          else if (is_structured_sort(s))
          {
            result=is_finite(structured_sort(s));
          }

          m_visiting.erase(s);
// ATfprintf(stderr,"IS finite result %d\n",result);
          return result;
        }

        bool is_finite(const basic_sort& s)
        {
          return is_finite_aux(s);
        }

        bool is_finite(const function_sort& s)
        {
          for (sort_expression_list::const_iterator i=s.domain().begin(); i!=s.domain().end(); ++i)
          {
            if (!is_finite(*i))
            {
              return false;
            }
          }

          return is_finite(s.codomain());
        }

        bool is_finite(const container_sort& s)
        {
          return (s.container_name() == set_container()) ? is_finite(s.element_sort()) : false;
        }

        bool is_finite(const alias& s)
        {
          assert(0);
          return false;
        }

        bool is_finite(const structured_sort& s)
        {
          return is_finite_aux(s);
        }
    };

    /// \brief Checks whether a sort is certainly finite.
    ///
    /// \param[in] s A sort expression
    /// \return true if s can be determined to be finite,
    ///      false otherwise.
    bool data_specification::is_certainly_finite(const sort_expression& s) const
    {
// ATfprintf(stderr,"Is certainly finite %t\n",(ATermAppl)s);
      const bool result=finiteness_helper(*this).is_finite(s);
// if (result) ATfprintf(stderr,"Yes\n"); else ATfprintf(stderr,"No\n");
      return result;
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

