#ifndef SIP_BASIC_DATATYPE
#define SIP_BASIC_DATATYPE

#include <cstdio>
#include <climits>
#include <cfloat>
#include <string>
#include <ostream>
#include <utility>
#include <iostream>

#include <boost/any.hpp>
#include <boost/lexical_cast.hpp>

#include <sip/detail/exception.h>

#include <xml2pp/text_reader.h>

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

        /** \brief Initialises static members */
        static bool initialise();
    };

    /**
     * \brief Derived datatype specifier for enumerations
     *
     * An enumeration is a finite set of alternatives.
     **/
    class enumeration : public basic_datatype {

      private:
        
        /** \brief The possible values in the domain */
        std::vector < std::string > values;

        /** \brief Index into values of the default value for elements of the specified type */
        size_t default_value;

      private:
        
        /** \brief Constructor */
        enumeration();

        /** \brief Constructor */
        enumeration(std::string const& s);

        /** \brief Write method that does all the work */
        void private_write(std::ostream& o, std::string const& s) const;

      public:

        /** \brief Add value */
        void add_value(std::string const&, bool = false);

        /** \brief Convenience function for shared pointer instances */
        static boost::shared_ptr < enumeration > create(std::string const& s);

        /** \brief Reconstruct a type from XML stream */
        static std::pair < basic_datatype::sptr, std::string > read(xml2pp::text_reader&);

        /** \brief Write from XML stream, using value */
        inline void write(std::ostream&, std::string const&) const;

        /** \brief Write from XML stream, using value */
        inline void write(std::ostream&, std::string) const;

        /** \brief Converts a long int to a string representation */
        static std::string convert(size_t const&);

        /** \brief Converts a string to a long int representation */
        boost::any evaluate(std::string const&);

        /** \brief Establishes whether value is valid for an element of this type */
        inline bool validate(std::string const& value) const;

        /** \brief Initialises static members */
        static void initialise();

        /** \brief Convenience method for adding values */
        enumeration& operator% (std::string const&);
    };

    /**
     * \brief Derived datatype specifier for integer number ranges (finite using long int)
     * 
     * The range is specified by a minimum and a maximum. The minimum, of
     * course, must be smaller than the maximum. The default value is taken to
     * be the minimum, unless it is specified at construction time.
     **/
    class integer : public basic_datatype {

      private:

        /** \brief The minimum integer value in the range */
        long int minimum;

        /** \brief The maximum integer value in the range */
        long int maximum;

        /** \brief The default value for elements of the specified type */
        const long int default_value;

      public:

        /** The set of integers, bounded by implementation limits only */
        static basic_datatype::sptr standard;

        /** The set of natural numbers, bounded by implementation limits only */
        static basic_datatype::sptr naturals;

        /** The set of positive numbers, bounded by implementation limits only */
        static basic_datatype::sptr positives;

        /** \brief Implementation dependent limitation (minimum value) */
        static const long int implementation_minimum;

        /** \brief Implementation dependent limitation (maximum value) */
        static const long int implementation_maximum;

      private:

        /** \brief Constructor */
        integer(long int = implementation_minimum, long int = implementation_minimum, long int = implementation_maximum);

        /** \brief Write method that does all the work */
        void private_write(std::ostream& o, std::string const& s) const;

      public:

        /** \brief Initialises static members */
        static void initialise();

        /** \brief Convenience function for shared pointer instances */
        static basic_datatype::sptr create(long int d = implementation_minimum, long int = implementation_minimum, long int = implementation_maximum);

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

    /**
     * \brief Derived datatype specifier for real number ranges (finite using double)
     *
     * The range is specified by a minimum and a maximum. The minimum, of
     * course, must be smaller than the maximum. The default value is taken to
     * be the minimum, unless it is specified at construction time.
     **/
    class real : public basic_datatype {

      private:

        /** \brief The minimum integer value in the range */
        double minimum;

        /** \brief The maximum integer value in the range */
        double maximum;

        /** \brief The default value for elements of the specified type */
        const double default_value;

      public:

        /** The set of real numbers, bounded by implementation limits only */
        static basic_datatype::sptr standard;

        /** The set of real numbers [0 ... 1], bounded by implementation limits only */
        static basic_datatype::sptr probability;

        /** The set of non negative real numbers [0 ... ), bounded by implementation limits only */
        static basic_datatype::sptr non_negatives;

        /** The set of non negative real numbers (0 ... ), bounded by implementation limits only */
        static basic_datatype::sptr positives;

        /** \brief Implementation dependent limitation (minimum value) */
        static const double implementation_minimum;

        /** \brief Implementation dependent limitation (maximum value) */
        static const double implementation_maximum;

      private:

        /** \brief Constructor */
        real(double d = implementation_minimum, double = implementation_minimum, double = implementation_maximum);

        /** \brief Write method that does all the work */
        void private_write(std::ostream& o, std::string const& s) const;

      public:

        /** \brief Initialises static members */
        static void initialise();

        /** \brief Convenience function for shared pointer instances */
        static basic_datatype::sptr create(double d = implementation_minimum, double minimum = implementation_minimum, double maximum = implementation_maximum);

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

      public:

        /** \brief The string that represents true */
        static const std::string true_string;

        /** \brief The string that represents false */
        static const std::string false_string;

        /** \brief Instance of a Boolean */
        static basic_datatype::sptr standard;

      private:

        /** \brief Constructor */
        boolean();

        /** \brief Write method that does all the work */
        void private_write(std::ostream& o, std::string const& s) const;

      public:

        /** \brief Initialises static members */
        static void initialise();

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

        /** \brief The default value for elements of the specified type */
        const std::string default_value;

      public:

        /** \brief Instance of a string (without limitations except the implementation limits) */
        static basic_datatype::sptr standard;

        /** \brief The empty string */
        static const std::string empty;

        /** \brief The maximum length a string may have */
        static const unsigned int implementation_maximum_length;

      private:

        /** \brief Constructor */
        string();

        /** \brief Constructor */
        inline string(std::string const&, unsigned int minimum = 0, unsigned int maximum = implementation_maximum_length); 

        /** \brief Write method that does all the work */
        void private_write(std::ostream& o, std::string const& s) const;

      public:

        /** \brief Initialises static members */
        static void initialise();

        /** \brief Convenience function for shared pointer instances */
        static basic_datatype::sptr create(std::string const& = empty, unsigned int = 0, unsigned int = implementation_maximum_length);

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
      throw (sip::exception(sip::message_unknown_type, r.element_name()));
    }

    inline bool basic_datatype::initialise() {
      string::initialise();
      enumeration::initialise();
      boolean::initialise();
      integer::initialise();
      real::initialise();

      return (true);
    }

    /** \brief Converts a boolean */
    template <>
    inline std::string basic_datatype::convert(bool const& s) {
      return (boolean::convert(s));
    }

    /** \brief Converts a long int */
    template < typename T >
    inline std::string basic_datatype::convert(T const& s) {
      return (integer::convert(s));
    }

    /** \brief Converts a double */
    template <>
    inline std::string basic_datatype::convert(double const& s) {
      return (real::convert(s));
    }

    /** \brief Converts a string */
    template <>
    inline std::string basic_datatype::convert(std::string const& s) {
      return (s);
    }

    /************************************************************************
     * Implementation of Boolean
     ************************************************************************/

#ifdef SIP_IMPORT_STATIC_DEFINITIONS
    basic_datatype::sptr boolean::standard;

    const std::string boolean::true_string;

    const std::string boolean::false_string;
#endif

    inline void boolean::initialise() {
      standard   = basic_datatype::sptr(new boolean);

      std::string* m = const_cast < std::string* > (&true_string);

      m->assign("true");
      m = const_cast < std::string* > (&false_string);
      m->assign("false");
    }

    inline boolean::boolean() {
    }

    /**
     * @param[in] r the xml2pp text reader from which to read
     *
     * \pre The current element must be \<boolean\>
     **/
    inline std::pair < basic_datatype::sptr, std::string > boolean::read(xml2pp::text_reader& r) {
      /* Current element must be <string> */
      assert(r.is_element("boolean"));

      bool b = r.get_attribute("value");

      r.next_element();
      r.skip_end_element("boolean");

      return (make_pair(boolean::standard, (b) ? true_string : false_string));
    }

    inline basic_datatype::sptr boolean::create() {
      return (boolean::standard);
    }

    /**
     * @param[out] o the stream to which to write the result to
     * @param[in] v an optional (valid) instance
     **/
    inline void boolean::private_write(std::ostream& o, std::string const& v) const {
      o << "<boolean";
      
      if (v == true_string) {
        o << " value=\"" << v << "\"";
      }

      o << "/>";
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
     * @param[in] s any string
     **/
    inline bool boolean::validate(std::string const& s) const {
      return (s == true_string || s == false_string);
    }

    /************************************************************************
     * Implementation of Integer
     ************************************************************************/

#ifdef SIP_IMPORT_STATIC_DEFINITIONS
    basic_datatype::sptr integer::standard;
    basic_datatype::sptr integer::naturals;
    basic_datatype::sptr integer::positives;

    /** \brief Implementation dependent limitation (minimum value) */
    const long int integer::implementation_minimum = LONG_MIN;

    /** \brief Implementation dependent limitation (maximum value) */
    const long int integer::implementation_maximum = LONG_MAX;
#endif

    inline void integer::initialise() {
      standard   = basic_datatype::sptr(new integer);
      naturals   = basic_datatype::sptr(new integer(0));
      positives  = basic_datatype::sptr(new integer(1));
    }
    /**
     * @param[in] min the minimum value in the domain
     * @param[in] max the maximum value in the domain
     * @param[in] d the default value in the domain
     **/
    inline integer::integer(long int d, long int min, long int max) : minimum(min), maximum(max), default_value(d) {
    }

    /**
     * @param[in] min the minimum value in the domain
     * @param[in] max the maximum value in the domain
     * @param[in] d the default value in the domain
     **/
    inline basic_datatype::sptr integer::create(long int d, long int min, long int max) {
      return (basic_datatype::sptr(new integer(d, min, max)));
    }

    /**
     * @param[in] r the xml2pp text reader from which to read
     *
     * \pre The current element must be \<integer\>
     **/
    inline std::pair < basic_datatype::sptr, std::string > integer::read(xml2pp::text_reader& r) {
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
    inline void integer::private_write(std::ostream& o, std::string const& v) const {
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
     * @param[in] s any string
     **/
    inline bool integer::validate(std::string const& s) const {
      long int b;

      return (sscanf(s.c_str(), "%ld", &b) == 1);
    }

    /************************************************************************
     * Implementation of Real 
     ************************************************************************/

#ifdef SIP_IMPORT_STATIC_DEFINITIONS
    basic_datatype::sptr real::standard;
    basic_datatype::sptr real::probability;
    basic_datatype::sptr real::non_negatives;
    basic_datatype::sptr real::positives;

    /** \brief Implementation dependent limitation (minimum value) */
    const double real::implementation_minimum = DBL_MIN;

    /** \brief Implementation dependent limitation (maximum value) */
    const double real::implementation_maximum = DBL_MAX;
#endif

    inline void real::initialise() {
      standard      = basic_datatype::sptr(new real);
      probability   = basic_datatype::sptr(new real(0,1));
      non_negatives = basic_datatype::sptr(new real(0));
      positives     = basic_datatype::sptr(new real(1));
    }

    /**
     * @param[in] min the minimum value in the domain
     * @param[in] max the maximum value in the domain
     * @param[in] d the default value in the domain
     **/
    inline real::real(double d, double min, double max) : minimum(min), maximum(max), default_value(d) {
    }

    /**
     * @param[in] min the minimum value in the domain
     * @param[in] max the maximum value in the domain
     * @param[in] d the default value in the domain
     **/
    inline basic_datatype::sptr real::create(double d, double min, double max) {
      return (basic_datatype::sptr(new real(d, min, max)));
    }

    /**
     * @param[in] r the xml2pp text reader from which to read
     *
     * \pre The current element must be \<real\>
     **/
    inline std::pair < basic_datatype::sptr, std::string > real::read(xml2pp::text_reader& r) {
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
    inline void real::private_write(std::ostream& o, std::string const& v) const {
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
     * @param[in] s any string
     **/
    inline bool real::validate(std::string const& s) const {
      double b;

      return (sscanf(s.c_str(), "%lf", &b) == 1);
    }

    /************************************************************************
     * Implementation of Enumeration
     ************************************************************************/

    inline void enumeration::initialise() {
    }

    /**
     * @param[in] s any string
     **/
    inline enumeration::enumeration(std::string const& s) : default_value(0) {
      values.push_back(s);
    }

    inline enumeration::enumeration() : default_value(0) {
    }

    /**
     * @param[in] s any string
     * @param[in] b whether this element should now be marked as the default
     **/
    inline void enumeration::add_value(std::string const& s, bool b) {
      if (std::find(values.begin(), values.end(), s) == values.end()) {
        values.push_back(s);

        if (b) {
          default_value = values.size() - 1;
        }
      }
    }

    /**
     * @param[in] s any string
     **/
    inline enumeration& enumeration::operator% (std::string const& s) {
      add_value(s);

      return (*this);
    }

    /**
     * @param[in] s the first (default) element
     **/
    inline boost::shared_ptr < enumeration > enumeration::create(std::string const& s) {
      return (boost::shared_ptr < enumeration >(new enumeration(s)));
    }

    /**
     * @param[in] r the xml2pp text reader from which to read
     *
     * \pre The current element must be \<enumeration\>
     **/
    inline std::pair < basic_datatype::sptr, std::string > enumeration::read(xml2pp::text_reader& r) {
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
    inline void enumeration::private_write(std::ostream& o, std::string const& v) const {
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
    inline std::string enumeration::convert(size_t const& s) {
      return (boost::lexical_cast < std::string > (s));
    }

    /**
     * @param[in] s the string to convert
     *
     * \pre the string should be parsable as one of the values
     **/
    inline boost::any enumeration::evaluate(std::string const& s) {
      return (boost::lexical_cast < size_t > (s));
    }

    /**
     * @param[in] s any string
     **/
    inline bool enumeration::validate(std::string const& s) const {
      return (std::find(values.begin(), values.end(), s) != values.end());
    }

    /************************************************************************
     * Implementation of String
     ************************************************************************/

#ifdef SIP_IMPORT_STATIC_DEFINITIONS
    const std::string  string::empty;

    const unsigned int string::implementation_maximum_length = UINT_MAX;

    basic_datatype::sptr string::standard;
#endif

    inline void string::initialise() {
      standard = basic_datatype::sptr(new string);
    }

    inline string::string() : minimum_length(0), maximum_length(UINT_MAX), default_value(empty) {
    }

    /**
     * @param[in] minimum the minimum length
     * @param[in] maximum the maximum length
     * @param[in] d the default value
     **/
    inline string::string(std::string const& d, unsigned int minimum, unsigned int maximum) :
                minimum_length(minimum), maximum_length(maximum_length), default_value(d) {
    }

    /**
     * @param[in] minimum the minimum length
     * @param[in] maximum the maximum length
     * @param[in] d the default value
     **/
    inline basic_datatype::sptr string::create(std::string const& d, unsigned int minimum, unsigned int maximum) {
      if (d.empty() && minimum == 0 && maximum == implementation_maximum_length) {
        return (string::standard);
      }
      else {
        return (basic_datatype::sptr(new string(d, minimum, maximum)));
      }
    }

    /** \pre The current element must be \<string\>  */
    inline std::pair < basic_datatype::sptr, std::string > string::read(xml2pp::text_reader& r) {
      unsigned int minimum = 0;
      unsigned int maximum = implementation_maximum_length;
      std::string  default_value;

      /* Current element must be <string> */
      assert(r.is_element("string"));

      r.get_attribute(&default_value, "default");

      r.get_attribute(&minimum, "minimum-length");
      r.get_attribute(&maximum, "maximum-length");

      std::string value = default_value;

      r.get_attribute(&value, "value");

      r.next_element();
      r.skip_end_element("string");

      return (std::make_pair(string::create(default_value, minimum, maximum), value));
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
      o << "<string";

      if (minimum_length != 0) {
        o << " minimum-length=\"" << minimum_length << "\"";
      }
      if (maximum_length != implementation_maximum_length) {
        o << " maximum-length=\"" << maximum_length << "\"";
      }

      if (!default_value.empty()) {
        o << "\" default=\"" << default_value;
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

