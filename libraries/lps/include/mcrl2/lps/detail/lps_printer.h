// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/detail/lps_printer.h
/// \brief add your file description here.

#ifndef MCRL2_LPS_DETAIL_LPS_REPLACER_H
#define MCRL2_LPS_DETAIL_LPS_REPLACER_H

#include <vector>
#include "mcrl2/data/replace.h"
#include "mcrl2/lps/specification.h"

namespace mcrl2 {

namespace lps {

namespace detail {

  /// \brief Function object for pretty printing LPS data types to a stream object.
  template <typename Stream>
  struct lps_printer
  {
    mutable Stream& out;
  
    /// \brief Constructor
    lps_printer(Stream& out_)
      : out(out_)
    {}

    /// \brief Applies the pretty printer to the elements of a term list
    template <typename TermList>
    void print_list(TermList& l) const
    {
      out << core::pp(l);
    }

    /// \brief Applies the pretty printer to the elements of a container
    template <typename Container>
    void print_container(const Container& c) const
    {
      for (typename Container::const_iterator i = c.begin(); i != c.end(); ++i)
      {
        if (i != c.begin())
        {
          out << ", ";
        }
        print(*i);
      }
    }   

    /// \brief Applies the pretty printer to a data expression
    /// \param d A data expression
    void print(const data::data_expression& d) const    
    {                                         
      out << pp(d);
    } 
  
    /// \brief Applies the pretty printer to an assignment
    /// \param a An assignment
    void print(const data::assignment& a) const
    {
      out << pp(d);
    } 
  
    /// \brief Applies the pretty printer to an action
    /// \param a An action
    void print(const action& a) const
    {
      out << core::pp(a);
    }
    
    /// \brief Applies the pretty printer to a deadlock
    /// \param d A deadlock
    void print(const deadlock& d) const
    {
      out << d.to_string();
    } 
  
    /// \brief Applies the pretty printer to a multi-action
    /// \param a A multi-action
    void print(const multi_action& a) const
    {
      out << a.to_string();
    } 
  
    /// \brief Applies the pretty printer to a summand
    /// \param s A summand
    void print(const action_summand& s) const
    {
      out << core::pp(action_summand_to_aterm(s)) << std::endl;
    }

    /// \brief Applies the pretty printer to a summand
    /// \param s A summand
    void print(const deadlock_summand& s) const
    {
      out << core::pp(deadlock_summand_to_aterm(s)) << std::endl;
    }
    
    /// \brief Applies the pretty printer to a process_initializer
    /// \param s A process_initializer
    void print(const process_initializer& i) const
    {
      out << core::pp(i);
    }
  
    /// \brief Applies the pretty printer to a linear_process
    /// \param s A linear_process
    void print(const linear_process& p) const
    {
      out << core::pp(linear_process_to_aterm(p));
    }
  
    /// \brief Applies the pretty printer to a linear process specification
    /// \param spec A linear process specification
    void print(const specification& spec) const
    {
      out << core::pp(specification_to_aterm(spec));
    }
    
    template <typename Term>
    void operator()(Term& t)
    {
      print(t);
    }
  };

  /// \brief Utility function to create an lps_printer.
  template <typename Stream>    
  lps_printer<Stream> make_lps_printer(const Stream& out)        
  {
    return lps_printer<Stream>(out);
  } 

} // namespace detail

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_DETAIL_LPS_REPLACER_H
