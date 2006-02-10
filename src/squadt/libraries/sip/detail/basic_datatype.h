#ifndef SIP_BASIC_DATATYPE
#define SIP_BASIC_DATATYPE

#include <string>
#include <ostream>

#include <sip/detail/exception.h>

#include <xml2pp/xml_text_reader.h>

/* Interface classes for both the tool and the controller side of the Squadt Interaction Protocol */
namespace sip {

  namespace datatype {
    /** Basic datatypes used in the protocol (for validation purposes) */
    class basic_datatype {
      public:
        /** Reconstruct a type specification from XML stream */
        static basic_datatype* from_xml(xml2pp::text_reader&);

        /** Write from XML stream, using value */
        virtual void to_xml(std::ostream&, std::string value = "") const = 0;

        /** Establishes whether value is valid for an element of this type */
        virtual bool validate(std::string& value) const = 0;

        /** Pure virtual destructor */
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

    class string : public basic_datatype {
      private:
        size_t minimum_length;
        size_t maximum_length;

      public:
        /** Constructor */
        inline string(size_t minimum = 0, size_t maximum = 0); 

        /** Reconstruct a type from XML stream */
        inline static string* from_xml(xml2pp::text_reader&);

        /** Set the minimum length of a string of this type */
        inline void set_minimum_length(size_t);

        /** Set the maximum length of a string of this type */
        inline void set_maximum_length(size_t);

        /** Write from XML stream, using value */
        inline void to_xml(std::ostream&, std::string value = "") const;

        /** Establishes whether value is valid for an element of this type */
        inline bool validate(std::string& value) const;

        inline ~string();
    };

    /*************************************************************************
     * Implementation of basic_datatype
     ************************************************************************/
    inline basic_datatype::~basic_datatype() {
    }

    inline basic_datatype* basic_datatype::from_xml(xml2pp::text_reader& reader) {
      if (reader.is_element("enumeration")) {
//        return (enumeration::from_xml(reader));
      }
      else if (reader.is_element("integer")) {
//        return (integer::from_xml(reader));
      }
      else if (reader.is_element("real")) {
//        return (real::from_xml(reader));
      }
      else if (reader.is_element("uri")) {
//        return (uri::from_xml(reader));
      }
      else if (reader.is_element("string")) {
        return (string::from_xml(reader));
      }

      /* Unknown type in configuration */
      throw (new exception(exception::unknown_type_in_configuration));
    }

    /************************************************************************
     * Implementation of string
     ************************************************************************/
    inline string::string(size_t minimum, size_t maximum) : minimum_length(minimum), maximum_length(maximum_length) {
    }

    /* Precondition : the current element must be <string> */
    inline string* string::from_xml(xml2pp::text_reader& reader) {
      sip::datatype::string* new_string = new string;

      /* Current element must be <string> */
      assert(reader.is_element("string"));

      reader.get_attribute(&new_string->minimum_length, "minimum-length");
      reader.get_attribute(&new_string->maximum_length, "maximum-length");

      reader.read();

      return (new_string);
    }

    inline void string::set_maximum_length(size_t m) {
      maximum_length = m;
    }

    inline void string::set_minimum_length(size_t m) {
      minimum_length = m;
    }

    inline void string::to_xml(std::ostream& output, std::string value) const {
      assert(validate(value));

      output << "<string";

      if (minimum_length != 0) {
        output << " minimum-length=\"" << minimum_length << "\"";
      }
      if (maximum_length != 0) {
        output << " maximum-length=\"" << maximum_length << "\"";
      }

      if (value.empty()) {
        output << "/>";
      }
      else {
        output << ">" << value << "</string>";
      }
    }

    inline bool string::validate(std::string& s) const {
      return (minimum_length <= s.size() && (maximum_length <= minimum_length || s.size() <= maximum_length));
    }

    inline string::~string() {
    }

    /** Some commonly used instances */
//    static integer standard_integer;
//    static integer standard_natural;
//    static real    standard_real;
    static string standard_string;
  }
}

#endif

