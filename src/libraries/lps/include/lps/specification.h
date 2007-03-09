///////////////////////////////////////////////////////////////////////////////
/// \file specification.h
/// 
/// Contains LPS data structures for the LPS Library.

#ifndef LPS_SPECIFICATION_H
#define LPS_SPECIFICATION_H

#include <iostream>
#include <vector>
#include <utility>
#include <cassert>
#include <iterator>
#include <algorithm>

#include "atermpp/aterm.h"
#include "lps/function.h"
#include "lps/linear_process.h"
#include "lps/pretty_print.h"
#include "lps/data_specification.h"
//#include "lps/detail/specification_utility.h"

namespace lps {

using atermpp::aterm_appl;
using atermpp::read_from_named_file;
using atermpp::aterm_traits;

///////////////////////////////////////////////////////////////////////////////
// Specification
/// \brief mCRL2 specification
///
// sort ...;
// 
// cons ...;
// 
// map ...;
// 
// eqn ...;
// 
// proc P(b: Bool, n: Nat) = a(b).P() + sum c: Bool. b -\> e@1.P(b := c);
// 
// init P(true, 0);
// 
//<Spec>         ::= SpecV1(<DataSpec>, <ActSpec>, <ProcEqnSpec>, <Init>)
class specification: public aterm_appl
{
  protected:
    data_specification   m_data;
    action_label_list    m_action_labels;
    linear_process       m_process;
    process_initializer  m_initial_process;

    //data_variable_list   m_initial_free_variables;
    //data_assignment_list m_initial_assignments;   

    /// Returns true if the action labels in the specification are included in m_action_labels.
    bool has_proper_action_labels() const
    {
      // find all action labels that occur in the LPS
      std::set<action_label> labels;
      atermpp::find_all_if(*this, is_action_label, std::inserter(labels, labels.end()));
      
      // put the elements of m_action_labels in a set
      std::set<action_label> cached_labels;
      for (action_label_list::iterator i = m_action_labels.begin(); i != m_action_labels.end(); ++i)
      {
        cached_labels.insert(*i);
      }
      
      return std::includes(cached_labels.begin(), cached_labels.end(), labels.begin(), labels.end());
    }

    /// Initialize the specification with an aterm_appl.
    ///
    void init_term(aterm_appl t)
    {
      m_term = aterm_traits<aterm_appl>::term(t);
      aterm_appl::iterator i = t.begin();
      m_data            = aterm_appl(*i++);
      m_action_labels   = aterm_appl(*i++)(0);
      m_process         = aterm_appl(*i++);
      m_initial_process = aterm_appl(*i);
    }

  public:
    specification()
      : aterm_appl(detail::constructSpec())
    {
      assert(is_well_typed());
    }

    specification(aterm_appl t)
      : aterm_appl(t)
    {
      assert(detail::check_rule_Spec(m_term));
      init_term(t);
      assert(is_well_typed());
    }

    specification(
        data_specification  data         ,
        action_label_list   action_labels,
        linear_process      lps          ,
        process_initializer initial_process
       )
      :
        m_data(data),
        m_action_labels(action_labels),
        m_process(lps),
        m_initial_process(initial_process)
    {
      m_term = reinterpret_cast<ATerm>(
        gsMakeSpecV1(
          data,
          gsMakeActSpec(action_labels),
          lps,
          initial_process
        )
      );        
      assert(has_proper_action_labels());
      assert(is_well_typed());
    }

    /// Reads the LPS from file. Returns true if the operation succeeded.
    ///
    bool load(const std::string& filename)
    {
      aterm_appl t = atermpp::read_from_named_file(filename);
      if (!t)
        return false;
      init_term(t);
      return true;
    }

    /// Writes the LPS to file. Returns true if the operation succeeded.
    ///
    bool save(const std::string& filename, bool binary = true)
    {
      if (binary)
      {
        return atermpp::write_to_named_binary_file(m_term, filename);
      }
      else
      {
        return atermpp::write_to_named_text_file(m_term, filename);
      }
    }

    /// Returns the LPS of the specification.
    ///
    linear_process process() const
    {
      return m_process;
    }

    /// Returns the data specification.
    ///
    data_specification data() const
    { return m_data; }

    /// Returns a sequence of action labels. This sequence includes all
    /// action labels occurring in this specification, but it can have more.
    ///
    action_label_list action_labels() const
    { return m_action_labels; }

    /// Returns the initial process.
    ///
    process_initializer initial_process() const
    {
      return m_initial_process;
    }
    
    /// Returns true if
    /// <ul>
    /// <li>the process is well typed</li>
    /// <li>the sorts occurring in summation variables are declared in the data specification</li>
    /// <li>the sorts occurring in process parameters are declared in the data specification</li>
    /// <li>the sorts occurring in the free variables are declared in the data specification</li>
    /// <li>the sorts occurring in the action labels are declared in the data specification</li>
    /// <li>the labels occurring in the actions of the summands are contained in the action labels</li>
    /// </ul>
    bool is_well_typed() const
    {
      // check 1)
      if (!process().is_well_typed())
      {
        return false;
      }
      
      std::set<lps::sort> sorts = detail::make_set(data().sorts());

      // check 2)
      for (summand_list::iterator i = process().summands().begin(); i != process().summands().end(); ++i)
      {
        if (!(detail::check_variable_sorts(i->summation_variables(), sorts)))
          return false;
      }

      // check 3)
      if (!(detail::check_variable_sorts(process().process_parameters(), sorts)))
      {
        return false;
      }

      // check 4)
      if (!(detail::check_variable_sorts(process().free_variables(), sorts)))
      {
        return false;
      }

      // check 5)
      if (!(detail::check_action_label_sorts(action_labels(), sorts)))
      {
        return false;
      }

      std::set<action_label> labels = detail::make_set(action_labels());

      // check 6)
      for (summand_list::iterator i = process().summands().begin(); i != process().summands().end(); ++i)
      {
        if (!(detail::check_action_labels(i->actions(), labels)))
          return false;
      }
      
      return true;
    }    
};


inline
specification set_data_specification(specification spec, data_specification data)
{
  return specification(data,
                       spec.action_labels(),
                       spec.process(),
                       spec.initial_process()
                      );
}

inline
specification set_action_labels(specification spec, action_label_list action_labels)
{
  return specification(spec.data(),
                       action_labels,
                       spec.process(),
                       spec.initial_process()
                      );
}

inline
specification set_lps(specification spec, linear_process lps)
{
  return specification(spec.data(),
                       spec.action_labels(),
                       lps,
                       spec.initial_process()
                      );
}

inline
specification set_initial_process(specification spec, process_initializer initial_process)
{
  return specification(spec.data(),
                       spec.action_labels(),
                       spec.process(),
                       initial_process
                      );
}

} // namespace lps

/// INTERNAL ONLY
namespace atermpp
{
using lps::specification;

template<>
struct aterm_traits<specification>
{
  typedef ATermAppl aterm_type;
  static void protect(lps::specification t)   { t.protect(); }
  static void unprotect(lps::specification t) { t.unprotect(); }
  static void mark(lps::specification t)      { t.mark(); }
  static ATerm term(lps::specification t)     { return t.term(); }
  static ATerm* ptr(lps::specification& t)    { return &t.term(); }
};

} // namespace atermpp

#endif // LPS_SPECIFICATION_H
