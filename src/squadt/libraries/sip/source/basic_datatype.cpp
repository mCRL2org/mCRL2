#include <sip/detail/basic_datatype.h>

#include <boost/regex.hpp>

namespace sip {

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
      assert(boost::regex_search(s, boost::regex("\\`[A-Za-z0-9_\\-]+\\'")));

      m_values.push_back(s);
    }

    /**
     * @param[in] s any string
     * @param[in] b whether this element should now be marked as the default
     **/
    void enumeration::add_value(std::string const& s, bool b) {
      assert(boost::regex_search(s, boost::regex("\\`[A-Za-z0-9_\\-]+\\'")));

      if (std::find(m_values.begin(), m_values.end(), s) == m_values.end()) {
        m_values.push_back(s);

        if (b) {
          m_default_value = m_values.size() - 1;
        }
      }
    }

    /************************************************************************
     * Implementation of String
     ************************************************************************/

    const unsigned int string::implementation_maximum_length = UINT_MAX;
  }
}

