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

#include <xml2pp/text_reader.h>

#include <sip/detail/exception.h>

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
        static basic_datatype::sptr create(std::string const& = "", unsigned int = 0, unsigned int = implementation_maximum_length);

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

    inline boolean::boolean() {
    }

    inline basic_datatype::sptr boolean::create() {
      return (boolean::standard);
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

    inline enumeration::enumeration() : default_value(0) {
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
      return (boost::lexical_cast < size_t > (s) < values.size());
    }

    /************************************************************************
     * Implementation of String
     ************************************************************************/

    inline void string::initialise() {
      standard = basic_datatype::sptr(new datatype::string);
    }

    inline string::string() : minimum_length(0), maximum_length(UINT_MAX), default_value("") {
    }

    /**
     * @param[in] minimum the minimum length
     * @param[in] maximum the maximum length
     * @param[in] d the default value
     **/
    inline string::string(std::string const& d, unsigned int minimum, unsigned int maximum) :
                minimum_length(minimum), maximum_length(maximum_length), default_value(d) {
    }

    inline void string::set_maximum_length(unsigned int m) {
      maximum_length = m;
    }

    inline void string::set_minimum_length(unsigned int m) {
      minimum_length = m;
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

