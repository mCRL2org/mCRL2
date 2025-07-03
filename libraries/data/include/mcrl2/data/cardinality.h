// Author: Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/cardinality.h
/// \brief This file provides a class that can determine the cardinality of a sort in a datatype

#ifndef MCRL2_DATA_CARDINALIY_H
#define MCRL2_DATA_CARDINALIY_H

#include "mcrl2/data/enumerator.h"

namespace mcrl2::data
{

class cardinality_calculator
{
  protected:

    const data_specification& m_specification;
    const rewriter& m_rewriter;

    // Determine whether f is a new unique element of sort s. If this cannot be determined,
    // the boolean determined is set to false, and the function yields false. 
    bool is_a_new_unique_element(const data_expression& t, 
                                 const std::vector<data_expression> already_found_elements,
                                 bool& determined) const
    {
      data_expression result;
      for(const data_expression& u: already_found_elements)
      {
        result=m_rewriter(equal_to(t,u));
        if (result==sort_bool::true_())
        {
          determined=true;
          return false;
        }
        else if (result!=sort_bool::false_())
        {
          determined=false;
          return false;
        }
        // else result==false and the element can still be unique.
      }
      determined=true;
      return true; 
    }
    
  public:

    cardinality_calculator(const data_specification& specification, const rewriter& r) 
      : m_specification(specification),
        m_rewriter(r)
    {}

    /// \brief Counts the number of elements in a sort. 
    /// \details The returned value is either the number of elements, or zero if
    ///          the number of elements cannot be determined, or is infinite.
    /// \param s The sort expression to be determined.
    std::size_t operator()(const sort_expression& s) const
    {
      if (s==sort_bool::bool_()) // Special case that occurs often. 
      {
        return 2;
      }
      /* if (!m_specification.is_certainly_finite(s))
      {
        return 0;
      } */
      std::vector<data_expression> found_elements(enumerate_expressions(s, m_specification, m_rewriter));
      std::vector<data_expression> unique_found_elements;  
     
      for(const data_expression& t: found_elements)
      {
        bool determined;
        if (is_a_new_unique_element(t, unique_found_elements, determined))
        {
          unique_found_elements.push_back(t);
        }
        else if (!determined)
        {
          // It is not possible to determine whether the found element is equal to any earlier element. 
          return 0;
        }
      }
      return unique_found_elements.size();
    }
};

} // namespace mcrl2::data

      
#endif // MCRL2_DATA_CARDINALITY_H
