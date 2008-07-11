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

    template class integer_range< char >;
    template class integer_range< unsigned char >;
    template class integer_range< short >;
    template class integer_range< unsigned short >;
    template class integer_range< int >;
    template class integer_range< unsigned int >;
    template class integer_range< long int >;
    template class integer_range< unsigned long int >;
    template class integer_range< long long int >;
    template class integer_range< unsigned long long int >;
    template class real_range< float, false, false >;
    template class real_range< float, false, true >;
    template class real_range< float, true, false >;
    template class real_range< float, true, true>;
    template class real_range< double, false, false >;
    template class real_range< double, false, true >;
    template class real_range< double, true, false >;
    template class real_range< double, true, true >;
    template class real_range< long double, false, false >;
    template class real_range< long double, false, true >;
    template class real_range< long double, true, false >;
    template class real_range< long double, true, true >;

    /*
     * \param[in] v value of the chosen carrier type
     * \param[in] s any string
     * \return *this
     * \throws std::runtime_error if s contains characters other than those in [0-9a-zA-Z_\\-]
     **/
    enumeration< size_t >& enumeration< size_t >::add(const size_t v, std::string const& s) {
      using namespace boost::xpressive;
 
      if (!regex_match(s, sregex(+(set[range('0','9') | range('a','z') | range('A','Z') | '_' | '-' | '\\'])))) {
        throw std::runtime_error(std::string("malformed value `").
                        append(s).append("' for enumerated type."));
      }
 
      if (m_values.find(v) == m_values.end()) {
        m_values[v] = s;
      }
 
      return *this;
    }
 
    /**
     * \param[in] s the string to evaluate
     * \return the associated value of the enumerated type
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
     * \return the string representation of the value of the enumerated type
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
     * \return whether s represents a string associated to a value of the enumerated type
     **/
    bool enumeration< size_t >::validate(std::string const& s) const {
      for (std::map< size_t, std::string >::const_iterator i = m_values.begin(); i != m_values.end(); ++i) {
        if (i->second == s) {
          return true;
        }
      }

      return false;
    }
 
    /// \cond INTERNAL_DOCS
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
    /// \endcond
  }
}

