// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "tipi/basic_datatype.hpp"

#include <boost/xpressive/xpressive_static.hpp>

namespace tipi {

  namespace datatype {

    class integer_range< char >;
    class integer_range< unsigned char >;
    class integer_range< short >;
    class integer_range< unsigned short >;
    class integer_range< int >;
    class integer_range< unsigned int >;
    class integer_range< long int >;
    class integer_range< unsigned long int >;
    class integer_range< long long int >;
    class integer_range< unsigned long long int >;
    class real_range< float, false, false >;
    class real_range< float, false, true >;
    class real_range< float, true, false >;
    class real_range< float, true, true>;
    class real_range< double, false, false >;
    class real_range< double, false, true >;
    class real_range< double, true, false >;
    class real_range< double, true, true >;
    class real_range< long double, false, false >;
    class real_range< long double, false, true >;
    class real_range< long double, true, false >;
    class real_range< long double, true, true >;

    /*
     * \param[in] v value of the chosen carrier type
     * \param[in] s any string
     * \return *this
     **/
    enumeration< size_t >& enumeration< size_t >::add(const size_t v, std::string const& s) {
      using namespace boost::xpressive;
 
      assert(regex_match(s, sregex(+(set[range('0','9') | range('a','z') | range('A','Z') | '_' | '-' | '\\']))));
 
      if (m_values.find(v) == m_values.end()) {
        m_values[v] = s;
      }
 
      return *this;
    }
 
    /**
     * \param[in] s the string to evaluate
     **/
    size_t enumeration< size_t >::evaluate(std::string const& s) const {
      for (std::map< size_t, std::string >::const_iterator i = m_values.begin(); i != m_values.end(); ++i) {
        if (i->second == s) {
          return i->first;
        }
      }
 
      return m_values.end()->first;
    }
 
    /** \brief Converts to a string representation
     * \param[in] s the string to convert (value must be in the domain)
     **/
    std::string enumeration< size_t >::convert(size_t const& s) const {
      std::map< size_t, std::string >::const_iterator i =
         m_values.find(static_cast < const size_t > (s));

      if (i == m_values.end()) { 
        throw std::runtime_error("invalid operand to conversion");
      }

      return i->second;
    }

    /**
     * \param[in] s any string to be checked as a valid instance of this type
     **/
    bool enumeration< size_t >::validate(std::string const& s) const {
      for (std::map< size_t, std::string >::const_iterator i = m_values.begin(); i != m_values.end(); ++i) {
        if (i->second == s) {
          return true;
        }
      }

      return false;
    }
 
    std::auto_ptr < basic_integer_range > basic_integer_range::reconstruct(std::string const&) {
      std::auto_ptr < basic_integer_range > new_range;
 
      return new_range;
    }

    std::auto_ptr < basic_real_range > basic_real_range::reconstruct(std::string const& s) {
      using namespace boost::xpressive;
 
      smatch results;
 
      regex_match(s, results, sregex((as_xpr('[') | '(') >> (s1= +_d) >> "..." >> (s2= +_d) >> (as_xpr(']') | ')')));
 
      std::cerr << results[1] << " " << results[2] << std::endl;
 
      std::auto_ptr < basic_real_range > new_range;
 
      return new_range;
    }
  }
}

