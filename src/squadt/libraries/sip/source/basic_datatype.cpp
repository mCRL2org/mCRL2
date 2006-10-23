#include <sip/detail/basic_datatype.h>

#include <boost/regex.hpp>

namespace sip {

  namespace datatype {

    basic_datatype::sptr boolean::standard;

    const std::string boolean::true_string;

    const std::string boolean::false_string;

    std::pair < basic_datatype::sptr, std::string > basic_datatype::read(xml2pp::text_reader& r) {
      if (r.is_element("enumeration")) {
        return (enumeration::read(r));
      }
      else if (r.is_element("boolean")) {
        return (boolean::read(r));
      }
      else if (r.is_element("integer")) {
        return (integer::read(r));
      }
      else if (r.is_element("real")) {
        return (real::read(r));
      }
      else if (r.is_element("uri")) {
//        return (uri::read(r));
      }
      else if (r.is_element("string")) {
        return (string::read(r));
      }

      /* Unknown type in configuration */
      throw (sip::exception(sip::message_unknown_type, r.element_name()));
    }

    bool basic_datatype::initialise() {
      string::initialise();
      enumeration::initialise();
      boolean::initialise();
      integer::initialise();
      real::initialise();

      return (true);
    }

    /************************************************************************
     * Implementation of Boolean
     ************************************************************************/

    void boolean::initialise() {
      standard   = basic_datatype::sptr(new boolean);

      std::string* m = const_cast < std::string* > (&true_string);

      m->assign("true");
      m = const_cast < std::string* > (&false_string);
      m->assign("false");
    }

    /**
     * @param[in] r the xml2pp text reader from which to read
     *
     * \pre The current element must be \<boolean\>
     **/
    std::pair < basic_datatype::sptr, std::string > boolean::read(xml2pp::text_reader& r) {
      /* Current element must be <string> */
      assert(r.is_element("boolean"));

      bool b = r.get_attribute("value");

      r.next_element();
      r.skip_end_element("boolean");

      return (make_pair(boolean::standard, (b) ? true_string : false_string));
    }

    /**
     * @param[out] o the stream to which to write the result to
     * @param[in] v an optional (valid) instance
     **/
    void boolean::private_write(std::ostream& o, std::string const& v) const {
      o << "<boolean";
      
      if (v == true_string) {
        o << " value=\"" << v << "\"";
      }

      o << "/>";
    }

    /************************************************************************
     * Implementation of Integer
     ************************************************************************/

    basic_datatype::sptr integer::standard;
    basic_datatype::sptr integer::naturals;
    basic_datatype::sptr integer::positives;

    /** \brief Implementation dependent limitation (minimum value) */
    const long int integer::implementation_minimum = LONG_MIN;

    /** \brief Implementation dependent limitation (maximum value) */
    const long int integer::implementation_maximum = LONG_MAX;

    void integer::initialise() {
      standard   = basic_datatype::sptr(new integer);
      naturals   = basic_datatype::sptr(new integer(0));
      positives  = basic_datatype::sptr(new integer(1));
    }

    /**
     * @param[in] r the xml2pp text reader from which to read
     *
     * \pre The current element must be \<integer\>
     **/
    std::pair < basic_datatype::sptr, std::string > integer::read(xml2pp::text_reader& r) {
      long int minimum = implementation_minimum;
      long int maximum = implementation_maximum;

      /* Current element must be <integer> */
      assert(r.is_element("integer"));

      r.get_attribute(&minimum, "minimum");
      r.get_attribute(&minimum, "maximum");

      long int default_value = minimum;

      r.get_attribute(&default_value, "default");

      std::string p = r.get_attribute_as_string("value");

      r.next_element();
      r.skip_end_element("integer");

      return (make_pair(integer::create(default_value, minimum, maximum), p));
    }

    /**
     * @param[out] o the stream to which to write the result to
     * @param[in] v an optional (valid) instance
     **/
    void integer::private_write(std::ostream& o, std::string const& v) const {
      o << "<integer";

      if (!v.empty()) {
        o << " value=\"" << std::dec << v << "\"";
      }

      if (minimum != implementation_minimum) {
        o << " minimum=\"" << minimum << "\"";
      }
        
      if (maximum != implementation_maximum) {
        o << " maximum=\"" << maximum << "\"";
      }

      if (default_value != minimum) {
        o << " default=\"" << default_value << "\"";
      }

      o << "/>";
    }

    /************************************************************************
     * Implementation of Real 
     ************************************************************************/

    basic_datatype::sptr real::standard;
    basic_datatype::sptr real::probability;
    basic_datatype::sptr real::non_negatives;
    basic_datatype::sptr real::positives;

    /** \brief Implementation dependent limitation (minimum value) */
    const double real::implementation_minimum = DBL_MIN;

    /** \brief Implementation dependent limitation (maximum value) */
    const double real::implementation_maximum = DBL_MAX;

    void real::initialise() {
      standard      = basic_datatype::sptr(new real);
      probability   = basic_datatype::sptr(new real(0,1));
      non_negatives = basic_datatype::sptr(new real(0));
      positives     = basic_datatype::sptr(new real(1));
    }

    /**
     * @param[in] r the xml2pp text reader from which to read
     *
     * \pre The current element must be \<real\>
     **/
    std::pair < basic_datatype::sptr, std::string > real::read(xml2pp::text_reader& r) {
      double minimum = implementation_minimum;
      double maximum = implementation_maximum;

      /* Current element must be <integer> */
      assert(r.is_element("real"));

      r.get_attribute(&minimum, "minimum");
      r.get_attribute(&maximum, "maximum");

      double default_value = minimum;

      r.get_attribute(&default_value, "default");

      std::string p = r.get_attribute_as_string("value");

      r.next_element();
      r.skip_end_element("real");

      return (make_pair(real::create(default_value, minimum, maximum), p));
    }

    /**
     * @param[out] o the stream to which to write the result to
     * @param[in] v an optional (valid) instance
     **/
    void real::private_write(std::ostream& o, std::string const& v) const {
      o << "<real";

      if (!v.empty()) {
        o << " value=\"" << std::dec << v << "\"";
      }

      if (minimum != implementation_minimum) {
        o << " minimum=\"" << minimum << "\"";
      }
        
      if (maximum != implementation_maximum) {
        o << " maximum=\"" << maximum << "\"";
      }

      if (default_value != minimum) {
        o << " default=\"" << default_value << "\"";
      }

      o << "/>";
    }

    /************************************************************************
     * Implementation of Enumeration
     ************************************************************************/

    /**
     * @param[in] s any string
     **/
    enumeration::enumeration(std::string const& s) : default_value(0) {
      assert(boost::regex_search(s, boost::regex("\\`[A-Za-z0-9_\\-]+\\'")));

      values.push_back(s);
    }

    /**
     * @param[in] s any string
     * @param[in] b whether this element should now be marked as the default
     **/
    void enumeration::add_value(std::string const& s, bool b) {
      assert(boost::regex_search(s, boost::regex("\\`[A-Za-z0-9_\\-]+\\'")));

      if (std::find(values.begin(), values.end(), s) == values.end()) {
        values.push_back(s);

        if (b) {
          default_value = values.size() - 1;
        }
      }
    }

    /**
     * @param[in] r the xml2pp text reader from which to read
     *
     * \pre The current element must be \<enumeration\>
     **/
    std::pair < basic_datatype::sptr, std::string > enumeration::read(xml2pp::text_reader& r) {
      /* Current element must be <enumeration> */
      assert(r.is_element("enumeration"));

      boost::shared_ptr < enumeration > new_enumeration(new enumeration);
      
      r.get_attribute(&new_enumeration->default_value, "default");

      std::string p = r.get_attribute_as_string("value");

      r.next_element();

      while (!r.is_end_element("enumeration")) {
        /* Assume element */
        new_enumeration->add_value(r.get_attribute_as_string("value"));

        r.next_element();
        r.skip_end_element("element");
      }

      r.skip_end_element("enumeration");

      if (p.empty()) {
        p = new_enumeration->values[new_enumeration->default_value];
      }

      return (make_pair(boost::static_pointer_cast < basic_datatype, enumeration > (new_enumeration), p));
    }

    /**
     * @param[out] o the stream to which to write the result to
     * @param[in] v an optional (valid) instance
     **/
    void enumeration::private_write(std::ostream& o, std::string const& v) const {
      o << "<enumeration value=\"" << v;

      if (default_value != 0) {
        o << "\" default=\"" << default_value;
      }

      o << "\">";

      for (std::vector < std::string >::const_iterator i = values.begin(); i != values.end(); ++i) {
        o << "<element value=\"" << *i << "\"/>";
      }

      o << "</enumeration>";
    }

    /************************************************************************
     * Implementation of String
     ************************************************************************/

    const std::string  string::empty;

    const unsigned int string::implementation_maximum_length = UINT_MAX;

    basic_datatype::sptr string::standard;

    /**
     * @param[in] minimum the minimum length
     * @param[in] maximum the maximum length
     * @param[in] d the default value
     **/
    basic_datatype::sptr string::create(std::string const& d, unsigned int minimum, unsigned int maximum) {
      if (d.empty() && minimum == 0 && maximum == implementation_maximum_length) {
        return (string::standard);
      }
      else {
        return (basic_datatype::sptr(new string(d, minimum, maximum)));
      }
    }

    /** \pre The current element must be \<string\>  */
    std::pair < basic_datatype::sptr, std::string > string::read(xml2pp::text_reader& r) {
      unsigned int minimum = 0;
      unsigned int maximum = implementation_maximum_length;
      std::string  default_value;

      /* Current element must be <string> */
      assert(r.is_element("string"));

      r.get_attribute(&default_value, "default");

      r.get_attribute(&minimum, "minimum-length");
      r.get_attribute(&maximum, "maximum-length");

      std::string value = default_value;

      if (!r.is_empty_element()) {
        r.next_element();

        r.get_value(&value);
      }

      r.next_element();
      r.skip_end_element("string");

      return (std::make_pair(string::create(default_value, minimum, maximum), value));
    }

    /**
     * @param[out] o the stream to which to write the result to
     * @param[in] s an optional (valid) instance
     **/
    void string::private_write(std::ostream& o, std::string const& s) const {
      assert(!boost::regex_search(s, boost::regex("]]>")));

      o << "<string";

      if (minimum_length != 0) {
        o << " minimum-length=\"" << minimum_length << "\"";
      }
      if (maximum_length != implementation_maximum_length) {
        o << " maximum-length=\"" << maximum_length << "\"";
      }

      if (!default_value.empty()) {
        o << " default=\"" << default_value << "\"";
      }

      if (!s.empty()) {
        o << "><![CDATA[" << s << "]]></string>";
      }
      else {
        o << "/>";
      }
    }
  }
}

