// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "boost/cstdint.hpp"
#include "tipi/basic_datatype.hpp"

#include "boost/xpressive/xpressive_static.hpp"

namespace tipi {

  namespace datatype {

    template class integer_range< boost::int8_t >;
    template class integer_range< boost::uint8_t >;
    template class integer_range< boost::int16_t >;
    template class integer_range< boost::uint16_t >;
    template class integer_range< boost::int32_t >;
    template class integer_range< boost::uint32_t >;
    template class integer_range< boost::int64_t >;
    template class integer_range< boost::uint64_t >;
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

    /// \cond INTERNAL_DOCS
    enumeration< size_t >& enumeration< size_t >::do_add(const size_t v, std::string const& s) {
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

    size_t enumeration< size_t >::do_evaluate(std::string const& s) const {
      for (std::map< size_t, std::string >::const_iterator i = m_values.begin(); i != m_values.end(); ++i) {
        if (i->second == s) {
          return i->first;
        }
      }

      return m_values.end()->first;
    }

    bool enumeration< size_t >::do_validate(std::string const& s) const {
      for (std::map< size_t, std::string >::const_iterator i = m_values.begin(); i != m_values.end(); ++i) {
        if (i->second == s) {
          return true;
        }
      }

      return false;
    }

    std::auto_ptr < basic_integer_range > basic_integer_range::reconstruct(std::string const&) {
      std::auto_ptr < basic_integer_range > new_range;
 
      throw mcrl2::runtime_error( "Accessing function with unpredictable behaviour.\n" );

      return new_range;
    }

    std::auto_ptr < basic_real_range > basic_real_range::reconstruct(std::string const& s) {
      using namespace boost::xpressive;

      smatch results;

      regex_match(s, results, sregex((as_xpr('[') | '(') >> (s1= +_d) >> "..." >> (s2= +_d) >> (as_xpr(']') | ')')));

      std::cerr << results[1] << " " << results[2] << std::endl;

      std::auto_ptr < basic_real_range > new_range;

      throw mcrl2::runtime_error( "Accessing function with unpredictable behaviour.\n" );

      return new_range;
    }
    /// \endcond
  }
}

