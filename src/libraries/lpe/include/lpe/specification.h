///////////////////////////////////////////////////////////////////////////////
/// \file specification.h
/// 
/// Contains LPE data structures for the LPE Library.

#ifndef LPE_SPECIFICATION_H
#define LPE_SPECIFICATION_H

#include <iostream>
#include <vector>
#include <utility>
#include <cassert>

#include "atermpp/aterm.h"
#include "lpe/function.h"
#include "lpe/linear_process.h"
#include "lpe/pretty_print.h"
#include "lpe/data_specification.h"
//#include "lpe/detail/specification_utility.h"

namespace lpe {

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
      // find all action labels that occur in the LPE
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
    {}

    specification(aterm_appl t)
      : aterm_appl(t)
    {
      assert(check_rule_Spec(m_term));
      init_term(t);
    }

    specification(
        data_specification  data         ,
        action_label_list   action_labels,
        linear_process      lpe          ,
        process_initializer initial_process
       )
      :
        m_data(data),
        m_action_labels(action_labels),
        m_process(lpe),
        m_initial_process(initial_process)
    {
      m_term = reinterpret_cast<ATerm>(
        gsMakeSpecV1(
          data,
          gsMakeActSpec(action_labels),
          lpe,
          initial_process
        )
      );        
      assert(has_proper_action_labels());
    }

    /// Reads the LPE from file. Returns true if the operation succeeded.
    ///
    bool load(const std::string& filename)
    {
      aterm_appl t = atermpp::read_from_named_file(filename);
      if (!t)
        return false;
      init_term(t);
      return true;
    }

    /// Writes the LPE to file. Returns true if the operation succeeded.
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

    /// Returns the LPE of the specification.
    ///
    linear_process lpe() const
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
    // data_assignment_list initial_assignments() const
};


inline
specification set_data_specification(specification spec, data_specification data)
{
  return specification(data,
                       spec.action_labels(),
                       spec.lpe(),
                       spec.initial_process()
                      );
}

inline
specification set_action_labels(specification spec, action_label_list action_labels)
{
  return specification(spec.data(),
                       action_labels,
                       spec.lpe(),
                       spec.initial_process()
                      );
}

inline
specification set_lpe(specification spec, linear_process lpe)
{
  return specification(spec.data(),
                       spec.action_labels(),
                       lpe,
                       spec.initial_process()
                      );
}

inline
specification set_initial_process(specification spec, process_initializer initial_process)
{
  return specification(spec.data(),
                       spec.action_labels(),
                       spec.lpe(),
                       initial_process
                      );
}

} // namespace lpe

/// INTERNAL ONLY
namespace atermpp
{
using lpe::specification;

template<>
struct aterm_traits<specification>
{
  typedef ATermAppl aterm_type;
  static void protect(lpe::specification t)   { t.protect(); }
  static void unprotect(lpe::specification t) { t.unprotect(); }
  static void mark(lpe::specification t)      { t.mark(); }
  static ATerm term(lpe::specification t)     { return t.term(); }
  static ATerm* ptr(lpe::specification& t)    { return &t.term(); }
};

} // namespace atermpp

#endif // LPE_SPECIFICATION_H
