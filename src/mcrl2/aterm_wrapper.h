///////////////////////////////////////////////////////////////////////////////
/// \file mcrl2/aterm_wrapper.h
/// Contains a wrapper for ATerms with pretty print functionality.

#ifndef MCRL2_ATERM_WRAPPER_H
#define MCRL2_ATERM_WRAPPER_H

#include <list>
#include <boost/iterator/iterator_facade.hpp>
#include "atermpp/aterm.h"
#include "mcrl2/pretty_print.h"

namespace mcrl2 {

using atermpp::aterm;
using atermpp::aterm_appl;

  //---------------------------------------------------------//
  //                     aterm_wrapper
  //---------------------------------------------------------//
  class aterm_wrapper
  {
    protected:
      aterm_appl m_term;

    public:
      aterm_wrapper()
      {}
      
      aterm_wrapper(aterm_appl term)
        : m_term(term)
      {}
      
      /// Two terms are considered equal if the underlying aterms are identical.
      /// 
      bool operator==(const aterm_wrapper& other) const
      {
        return m_term == other.m_term;
      }
      
      /// Returns a string representation of the term.
      ///
      std::string to_string() const
      {
        return m_term.to_string();
      }

      operator ATermAppl() const
      {
        return m_term;
      }

      /// Returns a pretty print representation of the term.
      ///
      std::string pp() const
      {
        return pretty_print(m_term);
      }
  };

} // namespace mcrl

#endif // MCRL2_ATERM_WRAPPER_H
