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
#include "mcrl2/data/detail/data_utility.h"
#include "mcrl2/data/replace.h"

namespace mcrl2
{

namespace data
{
/// \cond INTERNAL_DOCS

namespace detail
{

/**
 * \param[in/\<aterm\>/aterm/g
 * :%s/\<aterm_int\>/aterm_int/g
 * :%s/\<aterm_appl\>/aterm_appl/g
 * :%s/\<aterm_list\>/aterm_list/g
 * :%s/\<function_symbol\>/function_symbol/g
 * :%s/\<atermpp\>/atermpp/g
 *  compatible whether the produced aterm is compatible with the `format after type checking'
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
             gsMakeSortSpec(atermpp::aterm_list(s.m_sorts.begin(),s.m_sorts.end()) +
                            atermpp::aterm_list(s.m_aliases.begin(),s.m_aliases.end())),
             gsMakeConsSpec(atermpp::aterm_list(s.m_constructors.begin(),s.m_constructors.end())),
             gsMakeMapSpec(atermpp::aterm_list(s.m_mappings.begin(),s.m_mappings.end())),
             gsMakeDataEqnSpec(atermpp::aterm_list(s.m_equations.begin(),s.m_equations.end())));
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
      function_symbol_vector constructors(m_specification.constructors(s));
      if(constructors.empty())
      {
        return false;
      }

      for (function_symbol_vector::const_iterator i = constructors.begin(); i != constructors.end(); ++i)
      {
        if (is_function_sort(i->sort()))
        {
          const function_sort f_sort(i->sort());
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

    bool is_finite(const alias&)
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
  const bool result=finiteness_helper(*this).is_finite(s);
  return result;
}


// The function below checks whether there is an alias loop, e.g. aliases
// of the form A=B; B=A; or more complex A=B->C; B=Set(D); D=List(A); Loops
// through structured sorts are allowed. If a loop is detected, an exception
// is thrown.
void data_specification::check_for_alias_loop(
  const sort_expression s,
  std::set < sort_expression > sorts_already_seen,
  const bool toplevel) const
{
  if (is_basic_sort(s))
  {
    if (sorts_already_seen.count(s)>0)
    {
      throw mcrl2::runtime_error("Sort alias " + s.to_string() + " is defined in terms of itself.");
    }
    for(alias_vector::const_iterator i = m_aliases.begin(); i != m_aliases.end(); ++i)
    {
      if(i->name() == s)
      {
        sorts_already_seen.insert(s);
        check_for_alias_loop(i->reference(),sorts_already_seen,true);
        sorts_already_seen.erase(s);
        return;
      }
    }
    return;
  }

  if (is_container_sort(s))
  {
    check_for_alias_loop(container_sort(s).element_sort(),sorts_already_seen,false);
    return;
  }

  if (is_function_sort(s))
  {
    sort_expression_list s_domain(function_sort(s).domain());
    for (sort_expression_list::const_iterator i = s_domain.begin(); i != s_domain.end(); ++i)
    {
      check_for_alias_loop(*i,sorts_already_seen,false);
    }

    check_for_alias_loop(function_sort(s).codomain(),sorts_already_seen,false);
    return;
  }

  // A sort declaration with a struct on toplevel can be recursive. Otherwise a
  // check needs to be made.
  if (is_structured_sort(s) && !toplevel)
  {
    const structured_sort ss(s);
    structured_sort_constructor_list constructors=ss.constructors();
    for (structured_sort_constructor_list::const_iterator i=constructors.begin();
         i!=constructors.end(); ++i)
    {
      structured_sort_constructor constructor=*i;
      structured_sort_constructor_argument_list ssca=constructor.arguments();
      for (structured_sort_constructor_argument_list::const_iterator j=ssca.begin();
           j!=ssca.end(); ++j)
      {
        check_for_alias_loop(j->sort(),sorts_already_seen,false);
      }
    }
  }

}


// This function returns the normal form of e, under the two maps map1 and map2.
// This normal form is obtained by repeatedly applying map1 and map2, until this
// is not possible anymore. It is assumed that this procedure terminates. There is
// no check for loops.
static
sort_expression find_normal_form(
  const sort_expression& e,
  const std::multimap< sort_expression, sort_expression >  &map1,
  const std::multimap< sort_expression, sort_expression >  &map2,
  std::set < sort_expression > sorts_already_seen = std::set < sort_expression >())
{
  assert(sorts_already_seen.find(e)==sorts_already_seen.end()); // e has not been seen already.
  assert(!is_unknown_sort(e));
  assert(!is_multiple_possible_sorts(e));

  if (is_function_sort(e))
  {
    const function_sort fs(e);
    const sort_expression normalised_codomain=
      find_normal_form(fs.codomain(),map1,map2,sorts_already_seen);
    const sort_expression_list domain=fs.domain();
    sort_expression_list normalised_domain;
    for (sort_expression_list::const_iterator i=domain.begin();
         i!=domain.end(); ++i)
    {
      normalised_domain.push_front(find_normal_form(*i,map1,map2,sorts_already_seen));
    }
    return function_sort(reverse(normalised_domain),normalised_codomain);
  }

  if (is_container_sort(e))
  {
    const container_sort cs(e);
    return container_sort(cs.container_name(),find_normal_form(cs.element_sort(),map1,map2,sorts_already_seen));
  }

  sort_expression result_sort;

  if (is_structured_sort(e))
  {
    const structured_sort ss(e);
    structured_sort_constructor_list constructors=ss.constructors();
    structured_sort_constructor_list normalised_constructors;
    for (structured_sort_constructor_list::const_iterator i=constructors.begin();
         i!=constructors.end(); ++i)
    {
      structured_sort_constructor constructor=*i;
      structured_sort_constructor_argument_list normalised_ssa;
      structured_sort_constructor_argument_list ssca=constructor.arguments();
      for (structured_sort_constructor_argument_list::const_iterator j=ssca.begin();
           j!=ssca.end(); ++j)
      {
        normalised_ssa.push_front(structured_sort_constructor_argument(j->name(),
                                      find_normal_form(j->sort(),map1,map2,sorts_already_seen)));
      }

      normalised_constructors.push_front(
                                structured_sort_constructor(
                                  constructor.name(),
                                  reverse(normalised_ssa),
                                  constructor.recogniser()));

    }
    result_sort=structured_sort(reverse(normalised_constructors));
  }

  if (is_basic_sort(e))
  {
    result_sort=e;
  }


  assert(is_basic_sort(result_sort) || is_structured_sort(result_sort));
  const std::multimap< sort_expression, sort_expression >::const_iterator i1=map1.find(result_sort);
  if (i1!=map1.end()) // found
  {
#ifndef NDEBUG
    sorts_already_seen.insert(result_sort);
#endif
   return find_normal_form(i1->second,map1,map2
                           ,sorts_already_seen
                          );
 }
 const std::multimap< sort_expression, sort_expression >::const_iterator i2=map2.find(result_sort);
 if (i2!=map2.end()) // found
 {
#ifndef NDEBUG
    sorts_already_seen.insert(result_sort);
#endif
    return find_normal_form(i2->second,map1,map2,
                            sorts_already_seen
                           );
  }
  return result_sort;
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
  for (alias_vector::const_iterator i=m_aliases.begin(); i!=m_aliases.end(); ++i)
  {
    std::set < sort_expression > sorts_already_seen; // Empty set.
    check_for_alias_loop(i->name(),sorts_already_seen,true);
  }

  // Copy m_normalised_aliases. All aliases are stored from left to right,
  // except structured sorts, which are stored from right to left. The reason is
  // that structured sorts can be recursive, and therefore, they cannot be
  // rewritten from left to right, as this can cause sorts to be infinitely rewritten.

  std::multimap< sort_expression, sort_expression > sort_aliases_to_be_investigated;
  for (alias_vector::const_iterator i=m_aliases.begin(); i!=m_aliases.end(); ++i)
  {
    if (is_structured_sort(i->reference()))
    {
      sort_aliases_to_be_investigated.insert(std::pair<sort_expression,sort_expression>(i->reference(),i->name()));
    }
    else
    {
      sort_aliases_to_be_investigated.insert(std::pair<sort_expression,sort_expression>(i->name(),i->reference()));
    }
  }

  // Apply Knuth-Bendix completion to the rules in m_normalised_aliases.

  std::multimap< sort_expression, sort_expression > resulting_normalized_sort_aliases;

  for (; !sort_aliases_to_be_investigated.empty() ;)
  {
    const std::multimap< sort_expression, sort_expression >::iterator p=sort_aliases_to_be_investigated.begin();
    const sort_expression lhs=p->first;
    const sort_expression rhs=p->second;
    sort_aliases_to_be_investigated.erase(p);

    for (std::multimap< sort_expression, sort_expression >::const_iterator
         i=resulting_normalized_sort_aliases.begin();
         i!=resulting_normalized_sort_aliases.end(); ++i)
    {
      const sort_expression s1=data::replace_sort_expressions(lhs,sort_expression_assignment(i->first,i->second), true);

      if (s1!=lhs)
      {
        // There is a conflict between the two sort rewrite rules.
        assert(is_basic_sort(rhs));
        // Choose the normal form on the basis of a lexicographical ordering. This guarantees
        // uniqueness of normal forms over different tools. Ordering on addresses (as used previously)
        // proved to be unstable over different tools.
        const bool rhs_to_s1 = is_basic_sort(s1) && basic_sort(s1).to_string()<=rhs.to_string();
        const sort_expression left_hand_side=(rhs_to_s1?rhs:s1);
        const sort_expression pre_normal_form=(rhs_to_s1?s1:rhs);
        assert(is_basic_sort(pre_normal_form));
        // const sort_expression e1=find_normal_form(pre_normal_form,resulting_normalized_sort_aliases,sort_aliases_to_be_investigated);
        const sort_expression e1=pre_normal_form;
        if (e1!=left_hand_side)
        {
          sort_aliases_to_be_investigated.insert(std::pair<sort_expression,sort_expression > (left_hand_side,e1));
        }
      }
      else
      {
        const sort_expression s2 = data::replace_sort_expressions(i->first,sort_expression_assignment(lhs,rhs), true);
        if (s2!=i->first)
        {
          assert(is_basic_sort(i->second));
          // Choose the normal form on the basis of a lexicographical ordering. This guarantees
          // uniqueness of normal forms over different tools.
          const bool i_second_to_s2 = is_basic_sort(s2) && basic_sort(s2).to_string()<=i->second.to_string();
          const sort_expression left_hand_side=(i_second_to_s2?i->second:s2);
          const sort_expression pre_normal_form=(i_second_to_s2?s2:i->second);
          assert(is_basic_sort(pre_normal_form));
          // const sort_expression e2=find_normal_form(pre_normal_form,resulting_normalized_sort_aliases,
          //             sort_aliases_to_be_investigated);
          const sort_expression e2=pre_normal_form;
          if (e2!=left_hand_side)
          {
            sort_aliases_to_be_investigated.insert(std::pair<sort_expression,sort_expression > (left_hand_side,e2));
          }
        }
      }
    }
    assert(lhs!=rhs);
    resulting_normalized_sort_aliases.insert(std::pair<sort_expression,sort_expression >(lhs,rhs));

  }
  // Copy resulting_normalized_sort_aliases into m_normalised_aliases, i.e. from multimap to map.
  // If there are rules with equal left hand side, only one is arbitrarily chosen. Rewrite the
  // right hand side to normal form.

  const std::multimap< sort_expression, sort_expression > empty_multimap;
  for (std::multimap< sort_expression, sort_expression >::const_iterator
       i=resulting_normalized_sort_aliases.begin();
       i!=resulting_normalized_sort_aliases.end(); ++i)
  {
    m_normalised_aliases.insert(std::pair< sort_expression,sort_expression>(i->first,
                                find_normal_form(i->second,resulting_normalized_sort_aliases,empty_multimap)));
    assert(i->first!=i->second);
  }
}

bool data_specification::is_well_typed() const
{
  // check 1)
  if (!detail::check_data_spec_sorts(constructors(), m_sorts))
  {
    std::clog << "data_specification::is_well_typed() failed: not all of the sorts appearing in the constructors "
              << data::pp(constructors()) << " are declared in " << data::pp(m_sorts) << std::endl;
    return false;
  }

  // check 2)
  if (!detail::check_data_spec_sorts(mappings(), m_sorts))
  {
    std::clog << "data_specification::is_well_typed() failed: not all of the sorts appearing in the mappings "
              << data::pp(mappings()) << " are declared in " << data::pp(m_sorts) << std::endl;
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
  {
    if (data::is_alias(*i)) // Compatibility with legacy code
    {
      add_alias(*i);
    }
    else
    {
      add_sort(*i);
    }
  }

  // Store the constructors.
  for (atermpp::term_list_iterator< function_symbol > i = term_constructors.begin(); i != term_constructors.end(); ++i)
  {
    add_constructor(*i);
  }

  // Store the mappings.
  for (atermpp::term_list_iterator< function_symbol > i = term_mappings.begin(); i != term_mappings.end(); ++i)
  {
    add_mapping(*i);
  }

  // Store the equations.
  for (atermpp::term_list_iterator< data_equation > i = term_equations.begin(); i != term_equations.end(); ++i)
  {
    add_equation(*i);
  }

  //data_is_not_necessarily_normalised_anymore();
}
} // namespace data
} // namespace mcrl2

