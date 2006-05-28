#ifndef SIP_BASIC_DATATYPE
#define SIP_BASIC_DATATYPE

#include <string>
#include <ostream>

#include <boost/any.hpp>

#include <sip/detail/exception.h>

#include <xml2pp/detail/text_reader.tcc>

/* Interface classes for both the tool and the controller side of the Squadt Interaction Protocol */
namespace sip {

  namespace datatype {
    /** \brief Basic datatypes used in the protocol (for validation purposes) */
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

//    class enumeration : public basic_datatype {
//    };

//    class integer : public basic_datatype {
//    };

//    class real : public basic_datatype {
//    };

//    class uri : public basic_datatype {
//    };

    /** \brief Derived datatype specifier for booleans */
    class boolean : public basic_datatype {

      private:

        /** \brief The string that represents true */
        static std::string true_string;

        /** \brief The string that represents false */
        static std::string false_string;

      public:

        /** \brief Convenience function for shared pointer instances */
        static basic_datatype::sptr create();

        /** \brief Reconstruct a type from XML stream */
        static std::pair < basic_datatype::sptr, std::string > read(xml2pp::text_reader&);

        /** \brief Write from XML stream, using value */
        inline void write(std::ostream&, std::string const&) const;

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
//        return (enumeration::read(r));
      }
      else if (r.is_element("boolean")) {
        return (boolean::read(r));
      }
      else if (r.is_element("integer")) {
//        return (integer::read(r));
      }
      else if (r.is_element("real")) {
//        return (real::read(r));
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

//    static integer standard_integer;
//    static integer standard_natural;
//    static real    standard_real;

    /************************************************************************
     * Implementation of boolean
     ************************************************************************/

    /** Instance of a boolean */
    static basic_datatype::sptr standard_boolean(new boolean());

    /** \pre The current element must be \<string\>  */
    inline std::pair < basic_datatype::sptr, std::string > boolean::read(xml2pp::text_reader& r) {
      /* Current element must be <string> */
      assert(r.is_element("boolean"));

      std::string p = (r.get_attribute_string_value("value") == true_string) ? true_string : false_string;

      r.read();
      r.skip_end_element("string");

      return (make_pair(boolean::create(), p));
    }

    inline basic_datatype::sptr boolean::create() {
      return (standard_boolean);
    }

    /**
     * @param[out] o the stream to which to write the result to
     * @param[in] v an optional (valid) instance
     **/
    inline void boolean::write(std::ostream& o, std::string const& s) const {
      assert(validate(s));

      o << "<boolean value=\"" << s << "\"/>";
    }

    /**
     * @param[out] o the stream to which to write the result to
     * @param[in] v an optional (valid) instance
     **/
    inline void boolean::write(std::ostream& o, bool v) const {
      o << "<boolean value=\"" << (v ? true_string : false_string) << "\"/>";
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
     * Implementation of string
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

    inline void string::write(std::ostream& output, std::string const& value) const {
      assert(validate(value));

      output << "<string";

      if (minimum_length != 0) {
        output << " minimum-length=\"" << minimum_length << "\"";
      }
      if (maximum_length != 0) {
        output << " maximum-length=\"" << maximum_length << "\"";
      }

      if (!value.empty()) {
        output << " value=\"" << value << "\"";
      }

      output << "/>";
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

