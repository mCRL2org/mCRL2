#ifndef SIP_BASIC_DATATYPE
#define SIP_BASIC_DATATYPE

#include <cstdio>
#include <climits>
#include <cfloat>
#include <string>
#include <ostream>
#include <utility>

#include <boost/any.hpp>

#include <sip/detail/exception.h>

#include <xml2pp/detail/text_reader.tcc>

namespace sip {

  namespace datatype {

    /**
     * \brief Basic datatypes used by configurations and simple form validation
     *
     * The idea of using these data types is to lift some of the burden of input
     * validation of back of the tool developer.  The data types presented here
     * are just a hand full of examples used for simple validation purposes. In
     * the future regular expression based descriptions or automaton recipes
     * are a possible extension, should there be a need for it.
     **/
    class basic_datatype {

      public:

        /** \brief Boost shared pointer type alias */
        typedef boost::shared_ptr < basic_datatype > sptr;

      public:

        /** \brief Reconstruct a type specification from XML stream */
        static std::pair < basic_datatype::sptr, std::string > read(xml2pp::text_reader&);

        /** \brief Write from XML stream, using value */
        virtual void write(std::ostream&, std::string const& value = "") const = 0;

        /** \brief Converts a boolean to a string representation */
        template < typename T >
        static std::string convert(T const&);

        /** \brief Converts a boolean to a string representation */
        virtual boost::any evaluate(std::string const&) = 0;

        /** \brief Establishes whether value is valid for an element of this type */
        virtual bool validate(std::string const& value) const = 0;

        /** \brief Pure virtual destructor */
        virtual ~basic_datatype() = 0;
    };

    /** \brief Derived datatype specifier for enumerations */
    class enumeration : public basic_datatype {

      private:
        
        /** \brief The possible values in the domain */
        std::vector < std::string > values;

      private:
        
        /** \brief Write method that does all the work */
        void private_write(std::ostream& o, std::string const& s) const;

      public:

        /** \brief Add value */
        void add_value(std::string);

        /** \brief Convenience function for shared pointer instances */
        static basic_datatype::sptr create();

        /** \brief Reconstruct a type from XML stream */
        static std::pair < basic_datatype::sptr, std::string > read(xml2pp::text_reader&);

        /** \brief Write from XML stream, using value */
        inline void write(std::ostream&, std::string const&) const;

        /** \brief Write from XML stream, using value */
        inline void write(std::ostream&, std::string) const;

        /** \brief Converts a long int to a string representation */
        static std::string convert(std::string const&);

        /** \brief Converts a string to a long int representation */
        boost::any evaluate(std::string const&);

        /** \brief Establishes whether value is valid for an element of this type */
        inline bool validate(std::string const& value) const;
    };

    /** \brief Derived datatype specifier for integers (finite using long int) */
    class integer : public basic_datatype {

      private:

        /** The minimum integer value */
        long int minimum;

        /** The maximum integer value */
        long int maximum;

      private:

        /** \brief Constructor */
        integer(long int minimum = LONG_MIN, long int maximum = LONG_MAX);

        /** \brief Write method that does all the work */
        void private_write(std::ostream& o, std::string const& s) const;

      public:

        /** \brief Convenience function for shared pointer instances */
        static basic_datatype::sptr create(long int minimum = LONG_MIN, long int maximum = LONG_MAX);

        /** \brief Reconstruct a type from XML stream */
        static std::pair < basic_datatype::sptr, std::string > read(xml2pp::text_reader&);

        /** \brief Write from XML stream, using value */
        inline void write(std::ostream&, std::string const&) const;

        /** \brief Write from XML stream, using value */
        inline void write(std::ostream&, std::string) const;

        /** \brief Converts a long int to a string representation */
        static std::string convert(long int const&);

        /** \brief Converts a string to a long int representation */
        boost::any evaluate(std::string const&);

        /** \brief Establishes whether value is valid for an element of this type */
        inline bool validate(std::string const& value) const;
    };

    /** \brief Derived datatype specifier for integers (finite using double) */
    class real : public basic_datatype {

      private:

        /** The minimum integer value */
        double minimum;

        /** The maximum integer value */
        double maximum;

      private:

        /** \brief Constructor */
        real(double minimum = DBL_MIN, double maximum = DBL_MAX);

        /** \brief Write method that does all the work */
        void private_write(std::ostream& o, std::string const& s) const;

      public:

        /** \brief Convenience function for shared pointer instances */
        static basic_datatype::sptr create(double minimum = DBL_MIN, double maximum = DBL_MAX);

        /** \brief Reconstruct a type from XML stream */
        static std::pair < basic_datatype::sptr, std::string > read(xml2pp::text_reader&);

        /** \brief Write from XML stream, using value */
        inline void write(std::ostream&, std::string const&) const;

        /** \brief Write from XML stream, using value */
        inline void write(std::ostream&, std::string) const;

        /** \brief Converts a double to a string representation */
        static std::string convert(double const&);

        /** \brief Converts a string to a long int representation */
        boost::any evaluate(std::string const&);

        /** \brief Establishes whether value is valid for an element of this type */
        inline bool validate(std::string const& value) const;
    };

//    class uri : public basic_datatype {
//    };

    /** \brief Derived datatype specifier for booleans */
    class boolean : public basic_datatype {

      private:

        /** \brief The string that represents true */
        static std::string true_string;

        /** \brief The string that represents false */
        static std::string false_string;

      private:

        /** \brief Write method that does all the work */
        void private_write(std::ostream& o, std::string const& s) const;

      public:

        /** \brief Convenience function for shared pointer instances */
        static basic_datatype::sptr create();

        /** \brief Reconstruct a type from XML stream */
        static std::pair < basic_datatype::sptr, std::string > read(xml2pp::text_reader&);

        /** \brief Write from XML stream, using value */
        inline void write(std::ostream&, std::string const&) const;

        /** \brief Write from XML stream, using value */
        inline void write(std::ostream&, const std::string) const;

        /** \brief Write from XML stream, using value */
        inline void write(std::ostream&, bool = false) const;

        /** \brief Converts a boolean to a string representation */
        static std::string convert(bool const&);

        /** \brief Converts a string to a boolean representation */
        boost::any evaluate(std::string const&);

        /** \brief Establishes whether value is valid for an element of this type */
        inline bool validate(std::string const& value) const;
    };

    /** \brief Derived datatype for strings */
    class string : public basic_datatype {

      private:

        /** \brief The minimum length a string of this type has */
        unsigned int minimum_length;

        /** \brief The maximum length a string of this type has */
        unsigned int maximum_length;

      private:

        /** \brief Write method that does all the work */
        void private_write(std::ostream& o, std::string const& s) const;

      public:

        /** \brief Constructor */
        inline string(unsigned int minimum = 0, unsigned int maximum = 0); 

        /** \brief Convenience function for shared pointer instances */
        static basic_datatype::sptr create(unsigned int = 0, unsigned int = 0);

        /** \brief Reconstruct a type from XML stream */
        static std::pair < basic_datatype::sptr, std::string > read(xml2pp::text_reader&);

        /** \brief Set the minimum length of a string of this type */
        inline void set_minimum_length(unsigned int);

        /** \brief Set the maximum length of a string of this type */
        inline void set_maximum_length(unsigned int);

        /** \brief Write from XML stream, using value */
        inline void write(std::ostream&, std::string const& value = "") const;

        /** \brief Write from XML stream, using value */
        inline void write(std::ostream&, const std::string) const;

        /** \brief Converts a string to a string representation (copy) */
        inline std::string convert(std::string const& s);

        /** \brief Converts a string to a string representation (copy) */
        boost::any evaluate(std::string const&);

        /** \brief Establishes whether value is valid for an element of this type */
        inline bool validate(std::string const& value) const;
    };

    /*************************************************************************
     * Implementation of basic_datatype
     ************************************************************************/
    inline basic_datatype::~basic_datatype() {
    }

    inline std::pair < basic_datatype::sptr, std::string > basic_datatype::read(xml2pp::text_reader& r) {
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
      throw (sip::exception::exception(exception::message_unknown_type, r.element_name()));
    }

    template <>
    inline std::string basic_datatype::convert(bool const& s) {
      return (boolean::convert(s));
    }

    template <>
    inline std::string basic_datatype::convert(std::string const& s) {
      return (s);
    }

    /************************************************************************
     * Implementation of Boolean
     ************************************************************************/

    /** Instance of a Boolean */
    static basic_datatype::sptr standard_boolean(new boolean());

    /**
     * @param[in] r the xml2pp text reader from which to read
     *
     * \pre The current element must be \<boolean\>
     **/
    inline std::pair < basic_datatype::sptr, std::string > boolean::read(xml2pp::text_reader& r) {
      /* Current element must be <string> */
      assert(r.is_element("boolean"));

      std::string p = (r.get_attribute_string_value("value") == true_string) ? true_string : false_string;

      r.read();
      r.skip_end_element("boolean");

      return (make_pair(boolean::create(), p));
    }

    inline basic_datatype::sptr boolean::create() {
      return (standard_boolean);
    }

    /**
     * @param[out] o the stream to which to write the result to
     * @param[in] v an optional (valid) instance
     **/
    inline void boolean::private_write(std::ostream& o, std::string const& v) const {
      o << "<boolean value=\"" << v << "\"/>";
    }

    /**
     * @param[out] o the stream to which to write the result to
     * @param[in] v an optional (valid) instance
     **/
    inline void boolean::write(std::ostream& o, std::string const& v) const {
      private_write(o, v);
    }

    /**
     * @param[out] o the stream to which to write the result to
     * @param[in] v an optional (valid) instance
     **/
    inline void boolean::write(std::ostream& o, const std::string v) const {
      private_write(o, v);
    }

    /**
     * @param[out] o the stream to which to write the result to
     * @param[in] v an optional (valid) instance
     **/
    inline void boolean::write(std::ostream& o, bool v) const {
      private_write(o, (v ? true_string : false_string));
    }

    /**
     * @param[in] s the boolean to convert
     **/
    inline std::string boolean::convert(bool const& s) {
      return ((s) ? boolean::true_string : boolean::false_string);
    }

    /**
     * @param[in] s the string to convert
     **/
    inline boost::any boolean::evaluate(std::string const& s) {
      return (boost::any(s == boolean::true_string));
    }

    /**
     * @param[in] any string s
     **/
    inline bool boolean::validate(std::string const& s) const {
      return (s == true_string || s == false_string);
    }

    /************************************************************************
     * Implementation of Integer
     ************************************************************************/

    /** Instance of an integer datatype */
    static basic_datatype::sptr standard_integer  = integer::create();
    static basic_datatype::sptr standard_natural  = integer::create(0);
    static basic_datatype::sptr standard_positive = integer::create(1);

    /**
     * @param[in] min the minimum value in the domain
     * @param[in] max the maximum value in the domain
     **/
    inline integer::integer(long int min, long int max) : minimum(min), maximum(max) {
    }

    /**
     * @param[in] min the minimum value in the domain
     * @param[in] max the maximum value in the domain
     **/
    inline basic_datatype::sptr integer::create(long int min, long int max) {
      return (basic_datatype::sptr(new integer(min, max)));
    }

    /**
     * @param[in] r the xml2pp text reader from which to read
     *
     * \pre The current element must be \<integer\>
     **/
    inline std::pair < basic_datatype::sptr, std::string > integer::read(xml2pp::text_reader& r) {
      long int minimum = LONG_MIN;
      long int maximum = LONG_MAX;

      /* Current element must be <integer> */
      assert(r.is_element("integer"));

      r.get_attribute(&minimum, "minimum");
      r.get_attribute(&minimum, "maximum");

      std::string p = r.get_attribute_string_value("value");

      r.read();
      r.skip_end_element("integer");

      return (make_pair(integer::create(minimum, maximum), p));
    }

    /**
     * @param[out] o the stream to which to write the result to
     * @param[in] v an optional (valid) instance
     **/
    inline void integer::private_write(std::ostream& o, std::string const& v) const {
      assert(validate(v));

      o << "<integer value=\"" << std::dec << v;

      if (minimum != LONG_MIN) {
        o << "\" minimum=\"" << minimum;
      }
        
      if (maximum != LONG_MAX) {
        o << "\" maximum=\"" << maximum;
      }

      o << "/>";
    }

    /**
     * @param[out] o the stream to which to write the result to
     * @param[in] v an optional (valid) instance
     **/
    inline void integer::write(std::ostream& o, std::string const& v) const {
      private_write(o, v);
    }

    /**
     * @param[out] o the stream to which to write the result to
     * @param[in] v an optional (valid) instance
     **/
    inline void integer::write(std::ostream& o, const std::string v) const {
      private_write(o, v);
    }

    /**
     * @param[in] s the integer to convert
     **/
    inline std::string integer::convert(long int const& s) {
      boost::format f("%ld");

      return ((f % s).str());
    }

    /**
     * @param[in] s the string to convert
     *
     * \pre the string should be parsable as long int
     **/
    inline boost::any integer::evaluate(std::string const& s) {
      long int b;

      sscanf(s.c_str(), "%ld", &b);

      return (b);
    }

    /**
     * @param[in] any string s
     **/
    inline bool integer::validate(std::string const& s) const {
      long int b;

      return (sscanf(s.c_str(), "%ld", &b) != 1);
    }

    /************************************************************************
     * Implementation of Real 
     ************************************************************************/

    static basic_datatype::sptr standard_real              = real::create();
    static basic_datatype::sptr standard_real_probability  = real::create(0,1);
    static basic_datatype::sptr standard_non_negative_real = real::create(0);
    static basic_datatype::sptr standard_positive_real     = real::create(1);

    /**
     * @param[in] min the minimum value in the domain
     * @param[in] max the maximum value in the domain
     **/
    inline real::real(double min, double max) : minimum(min), maximum(max) {
    }

    /**
     * @param[in] min the minimum value in the domain
     * @param[in] max the maximum value in the domain
     **/
    inline basic_datatype::sptr real::create(double min, double max) {
      return (basic_datatype::sptr(new real(min, max)));
    }

    /**
     * @param[in] r the xml2pp text reader from which to read
     *
     * \pre The current element must be \<real\>
     **/
    inline std::pair < basic_datatype::sptr, std::string > real::read(xml2pp::text_reader& r) {
      double minimum = DBL_MIN;
      double maximum = DBL_MAX;

      /* Current element must be <integer> */
      assert(r.is_element("real"));

      r.get_attribute(&minimum, "minimum");
      r.get_attribute(&minimum, "maximum");

      std::string p = r.get_attribute_string_value("value");

      r.read();
      r.skip_end_element("real");

      return (make_pair(real::create(minimum, maximum), p));
    }

    /**
     * @param[out] o the stream to which to write the result to
     * @param[in] v an optional (valid) instance
     **/
    inline void real::private_write(std::ostream& o, std::string const& v) const {
      assert(validate(v));

      o << "<real value=\"" << std::dec << v;

      if (minimum != DBL_MIN) {
        o << "\" minimum=\"" << minimum;
      }
        
      if (maximum != DBL_MAX) {
        o << "\" maximum=\"" << maximum;
      }

      o << "/>";
    }

    /**
     * @param[out] o the stream to which to write the result to
     * @param[in] v an optional (valid) instance
     **/
    inline void real::write(std::ostream& o, std::string const& v) const {
      private_write(o, v);
    }

    /**
     * @param[out] o the stream to which to write the result to
     * @param[in] v an optional (valid) instance
     **/
    inline void real::write(std::ostream& o, const std::string v) const {
      private_write(o, v);
    }

    /**
     * @param[in] s the double to convert
     **/
    inline std::string real::convert(double const& s) {
      boost::format f("%lf");

      return ((f % s).str());
    }

    /**
     * @param[in] s the string to convert
     *
     * \pre the string should be parsable as long int
     **/
    inline boost::any real::evaluate(std::string const& s) {
      double b;

      sscanf(s.c_str(), "%lf", &b);

      return (b);
    }

    /**
     * @param[in] any string s
     **/
    inline bool real::validate(std::string const& s) const {
      double b;

      return (sscanf(s.c_str(), "%lf", &b) != 1);
    }

    /************************************************************************
     * Implementation of Enumeration
     ************************************************************************/

    /**
     * @param[in] any string s
     **/
    inline void enumeration::add_value(std::string s) {
      if (std::find(values.begin(), values.end(), s) == values.end()) {
        values.push_back(s);
      }
    }

    /**
     * @param[in] min the minimum value in the domain
     * @param[in] max the maximum value in the domain
     **/
    inline basic_datatype::sptr enumeration::create() {
      return (basic_datatype::sptr(new enumeration));
    }

    /**
     * @param[in] r the xml2pp text reader from which to read
     *
     * \pre The current element must be \<enumeration\>
     **/
    inline std::pair < basic_datatype::sptr, std::string > enumeration::read(xml2pp::text_reader& r) {
      /* Current element must be <enumeration> */
      assert(r.is_element("enumeration"));

      std::string p = r.get_attribute_string_value("value");

      r.read();

      boost::shared_ptr < enumeration > new_enumeration(new enumeration);
      
      while (!r.is_end_element("enumeration")) {
        /* Assume element */
        new_enumeration->add_value(r.get_attribute_string_value("value"));

        r.read();
        r.skip_end_element("element");
      }

      r.skip_end_element("enumeration");

      return (make_pair(boost::static_pointer_cast < basic_datatype, enumeration > (new_enumeration), p));
    }

    /**
     * @param[out] o the stream to which to write the result to
     * @param[in] v an optional (valid) instance
     **/
    inline void enumeration::private_write(std::ostream& o, std::string const& v) const {
      assert(validate(v));

      o << "<enumeration value=\"" << v << "\"/>";

      for (std::vector < std::string >::const_iterator i = values.begin(); i != values.end(); ++i) {
        o << "<element value=\"" << *i << "\"/>";
      }

      o << "</enumeration>";
    }

    /**
     * @param[out] o the stream to which to write the result to
     * @param[in] v an optional (valid) instance
     **/
    inline void enumeration::write(std::ostream& o, std::string const& v) const {
      private_write(o, v);
    }

    /**
     * @param[out] o the stream to which to write the result to
     * @param[in] v an optional (valid) instance
     **/
    inline void enumeration::write(std::ostream& o, const std::string v) const {
      private_write(o, v);
    }

    /**
     * @param[in] s the string to convert (value must be in the domain)
     **/
    inline std::string enumeration::convert(std::string const& s) {
      return (s);
    }

    /**
     * @param[in] s the string to convert
     *
     * \pre the string should be parsable as one of the values
     **/
    inline boost::any enumeration::evaluate(std::string const& s) {
      return (s);
    }

    /**
     * @param[in] any string s
     **/
    inline bool enumeration::validate(std::string const& s) const {
      return (std::find(values.begin(), values.end(), s) != values.end());
    }

    /************************************************************************
     * Implementation of String
     ************************************************************************/

    /** Instance of a string (without limitations) */
    static basic_datatype::sptr standard_string(new string());

    /**
     * @param[in] minimum the minimum length
     * @param[in] maximum the maximum length
     **/
    inline string::string(unsigned int minimum, unsigned int maximum) : minimum_length(minimum), maximum_length(maximum_length) {
    }

    /**
     * @param[in] minimum the minimum length
     * @param[in] maximum the maximum length
     **/
    inline basic_datatype::sptr string::create(unsigned int minimum, unsigned maximum) {
      return (standard_boolean);
    }

    /** \pre The current element must be \<string\>  */
    inline std::pair < basic_datatype::sptr, std::string > string::read(xml2pp::text_reader& r) {
      unsigned int minimum = 0;
      unsigned int maximum = 0;

      /* Current element must be <string> */
      assert(r.is_element("string"));

      r.get_attribute(&minimum, "minimum-length");
      r.get_attribute(&maximum, "maximum-length");

      r.read();
      r.skip_end_element("string");

      return (std::make_pair((minimum == 0 && maximum == 0) ? standard_string : string::create(minimum, maximum), r.get_attribute_string_value("value")));
    }

    inline void string::set_maximum_length(unsigned int m) {
      maximum_length = m;
    }

    inline void string::set_minimum_length(unsigned int m) {
      minimum_length = m;
    }

    /**
     * @param[out] o the stream to which to write the result to
     * @param[in] s an optional (valid) instance
     **/
    inline void string::private_write(std::ostream& o, std::string const& s) const {
      assert(validate(s));

      o << "<string";

      if (minimum_length != 0) {
        o << " minimum-length=\"" << minimum_length << "\"";
      }
      if (maximum_length != 0) {
        o << " maximum-length=\"" << maximum_length << "\"";
      }

      if (!s.empty()) {
        o << " value=\"" << s << "\"";
      }

      o << "/>";
    }

    /**
     * @param[out] o the stream to which to write the result to
     * @param[in] v an optional (valid) instance
     **/
    inline void string::write(std::ostream& o, std::string const& v) const {
      private_write(o, v);
    }

    /**
     * @param[out] o the stream to which to write the result to
     * @param[in] v an optional (valid) instance
     **/
    inline void string::write(std::ostream& o, const std::string v) const {
      private_write(o, v);
    }

    /**
     * @param[in] s the string to convert
     **/
    inline std::string string::convert(std::string const& s) {
      return (s);
    }

    /**
     * @param[in] s the string to convert
     **/
    inline boost::any string::evaluate(std::string const& s) {
      return (s);
    }

    inline bool string::validate(std::string const& s) const {
      return (minimum_length <= s.size() && (maximum_length <= minimum_length || s.size() <= maximum_length));
    }
  }
}

#endif

