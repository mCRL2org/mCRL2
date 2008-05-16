// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// LICENSE_1_0.txt)
//
/// \file source/basic_datatype.cpp

#include "tipi/basic_datatype.hpp"

#include <boost/xpressive/xpressive_static.hpp>

namespace tipi {

  namespace datatype {

    /************************************************************************
     * Implementation of Boolean
     ************************************************************************/

    const std::string boolean::true_string("true");

    const std::string boolean::false_string("false");

    /************************************************************************
     * Implementation of Integer
     ************************************************************************/

    /** \brief Implementation dependent limitation (minimum value) */
    const long int integer::implementation_minimum = LONG_MIN;

    /** \brief Implementation dependent limitation (maximum value) */
    const long int integer::implementation_maximum = LONG_MAX;

    /************************************************************************
     * Implementation of Real 
     ************************************************************************/

    /** \brief Implementation dependent limitation (minimum value) */
    const double real::implementation_minimum = DBL_MIN;

    /** \brief Implementation dependent limitation (maximum value) */
    const double real::implementation_maximum = DBL_MAX;

    /************************************************************************
     * Implementation of Enumeration
     ************************************************************************/

    /**
     * @param[in] s any string
     **/
    enumeration::enumeration(std::string const& s) : m_default_value(0) {
      using namespace boost::xpressive;

      assert(regex_match(s, sregex(+(set[range('0','9') | range('a','z') | range('A','Z') | '_' | '-' | '\\']))));

      m_values.push_back(s);
    }

    /**
     * @param[in] s any string
     * @param[in] b whether this element should now be marked as the default
     **/
    void enumeration::add_value(std::string const& s, bool b) {
      using namespace boost::xpressive;

      assert(regex_match(s, sregex(+(set[range('0','9') | range('a','z') | range('A','Z') | '_' | '-' | '\\']))));

      std::vector< std::string >::iterator i = std::find(m_values.begin(), m_values.end(), s);

      if (i == m_values.end()) {
        if (b) {
          m_default_value = m_values.size();
        }

        m_values.push_back(s);
      }
    }

    /************************************************************************
     * Implementation of String
     ************************************************************************/

    const unsigned int string::implementation_maximum_length = UINT_MAX;
  }
}

