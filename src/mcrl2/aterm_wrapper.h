///////////////////////////////////////////////////////////////////////////////
/// \file mcrl2/list_iterator.h
/// Contains a general aterm_wrapper iterator.

#ifndef MCRL2_ATERM_WRAPPER_H
#define MCRL2_ATERM_WRAPPER_H

#include <list>
#include <boost/iterator/iterator_facade.hpp>
#include "atermpp/aterm.h"
#include "mcrl2/gs_init.h"
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
      bool operator==(const aterm_wrapper& t) const
      {
        return m_term == t.m_term;
      }
      
      /// Returns a string representation of the term.
      ///
      std::string to_string() const
      {
        return m_term.to_string();
      }
      
      /// Conversion to ATerm
      ///
      ATerm to_ATerm() const
      {
        return m_term;
      }
      
      /// Conversion to ATermAppl
      ///
      ATermAppl to_ATermAppl() const
      {
        return m_term.to_ATermAppl();
      }
      
      /// Conversion to aterm
      ///
      aterm to_aterm() const
      {
        return m_term;
      }

      /// Conversion to aterm_appl
      ///
      aterm_appl to_appl() const
      {
        return m_term;
      }
      
      /// Returns a pretty print representation of the term.
      ///
      std::string pp() const
      {
        return pretty_print(to_ATerm());
      }
  };

} // namespace mcrl

#endif // MCRL2_ATERM_WRAPPER_H
