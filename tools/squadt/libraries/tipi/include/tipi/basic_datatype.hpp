// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file tipi/basic_datatype.hpp
/// \brief Data types for input validation

#ifndef TIPI_BASIC_DATATYPE
#define TIPI_BASIC_DATATYPE

#include <string>
#include <utility>
#include <sstream>
#include <limits>
#include <map>

#include <boost/any.hpp>
#include <boost/integer_traits.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/type_traits/is_enum.hpp>
#include <boost/type_traits/is_pod.hpp>

#include "tipi/detail/visitors.hpp"

namespace tipi {

  namespace datatype {

    /** \brief Base class for classes that specify types */
    class basic_datatype : public ::utility::visitable {

      protected:

        /** \brief Converts to underlying type */
        virtual boost::any specialised_evaluate(std::string const&) const = 0;

        /** \brief Converts from the underlying implementation type */
        virtual std::string specialised_convert(boost::any const&) const = 0;

      public:

        /** \brief Converts a value of an arbitrary non-pod-type to its string representation */
        template < typename T >
        inline typename boost::disable_if< typename boost::is_pod< T >::type, std::string >::type convert(T const& t) const {
          return specialised_convert(boost::any(t));
        }

        /** \brief Converts a value of an arbitrary pod-type to its string representation */
        template < typename T >
        inline typename boost::enable_if< typename boost::is_pod< T >::type, std::string >::type convert(T t) const {
          return specialised_convert(boost::any(t));
        }

        /** \brief Specialisation for constant character arrays */
        template < typename T, size_t s >
        inline std::string convert(T (&t)[s]) const {
          return specialised_convert(boost::any(std::string(t)));
        }

        /** \brief Converts to underlying type */
        template < typename T >
        inline typename boost::disable_if< typename boost::is_enum< T >::type, T >::type evaluate(std::string const& s) const {
          return boost::any_cast< T >(specialised_evaluate(s));
        }

        /** \brief Converts to underlying type */
        template < typename T >
        inline typename boost::enable_if< typename boost::is_enum< T >::type, T >::type evaluate(std::string const& s) const {
          boost::any result(specialised_evaluate(s));

          if (result.type() == typeid(size_t)) {
            return static_cast< T > (boost::any_cast< size_t >(result));
          }

          return boost::any_cast< T >(result);
        }

        /** \brief Establishes whether value is valid for an element of this type */
        virtual bool validate(std::string const& value) const = 0;

        /** \brief Pure virtual destructor */
        virtual ~basic_datatype() {
        }
    };

    /** \brief Specialisation for C strings */
    template < >
    inline boost::enable_if< boost::is_pod< char* >::type, std::string >::type
    basic_datatype::convert(char* t) const {
      return specialised_convert(boost::any(std::string(t)));
    }

    template < typename T >
    class enumeration;

    /// \cond INTERNAL_DOCS
    /**
     * \brief Derived data type specifier for enumerations
     *
     * An enumeration is a finite set of entities.
     **/
    class basic_enumeration : public basic_datatype {
      template < typename R, typename S >
      friend class ::utility::visitor;

      public:

        /// Iterator range for traversing the elements of the type
        typedef std::pair< std::map < size_t, std::string >::const_iterator,
                 std::map < size_t, std::string >::const_iterator > const_iterator_range;
        
      public:
        
        /** \brief Add value **/
        virtual basic_enumeration& add(const size_t v, std::string const& s) = 0;

        /** \brief Establishes whether value is valid for an element of this type **/
        virtual bool validate(std::string const& s) const = 0;

        /** \brief Gets an iterator over the possible elements of the type */
        virtual const_iterator_range values() const = 0;

        /** \brief Constructor */
        virtual ~basic_enumeration() { }
    };
    /// \endcond

    template < typename C = size_t >
    class enumeration;

    /**
     * \brief Derived data type specifier for enumerations
     *
     * An enumeration is a finite set of alternatives whose implementation is
     * based on the type of a standard enumerated type. A subset of values of
     * the enumerated type are given names and represent the new enumeration.
     *
     * This is the specialisation in which template parameter C is size_t,
     * instead of an enumerated type. The alternate implementations (with
     * enumerated types for C) all use instances of this type.
     **/
    template < >
    class enumeration< size_t > : public basic_enumeration {
      template < typename R, typename S >
      friend class ::utility::visitor;

      template < typename T >
      friend class enumeration;

      public:

        /** \brief POD type used for implementation */
        typedef std::string implementation_type;

      private:
        
        /** \brief The possible values in the domain */
        std::map < size_t, std::string > m_values;

      protected:

        /** \brief Converts to underlying type */
        inline boost::any specialised_evaluate(std::string const& s) const {
          return evaluate(s);
        }

        /** \brief Converts from the underlying implementation type */
        inline std::string specialised_convert(boost::any const& v) const {
          return convert(boost::any_cast< size_t >(v));
        }

        /** \brief Returns an iterator over the values of the enumerated type */
        inline basic_enumeration::const_iterator_range values() const {
          return std::make_pair(m_values.begin(), m_values.end());
        }

      public:
        
        /** \brief Add value
         * \param[in] v value of the chosen carrier type
         * \param[in] s any string
         * \return *this
         **/
        enumeration< size_t >& add(const size_t v, std::string const& s);

        /// \brief converts a value of the enumerated type to its associated string
        std::string convert(size_t const& s) const;

        /// \brief validates whether a string is a value of the enumerated type
        bool validate(std::string const& s) const;

        /** \brief Converts a string to an index representation
         * \param[in] s the string to evaluate
         **/
        size_t evaluate(std::string const& s) const;
    };

    /**
     * \brief Derived data type specifier for enumerations
     *
     * An enumeration is a finite set of alternatives whose implementation is
     * based on the type of a standard enumerated type. A subset of values of
     * the enumerated type are given names and represent the new enumeration.
     **/
    template < typename C >
    class enumeration : public basic_enumeration {
      template < typename R, typename S >
      friend class ::utility::visitor;

      public:

        /** \brief POD type used for implementation */
        typedef std::string implementation_type;

      private:
        
        /** \brief The possible values in the domain */
        std::map < size_t, std::string > m_values;

      private:

        enumeration< size_t >& get_single_instance() const {
          static std::auto_ptr< enumeration< size_t > > instance(new enumeration< size_t >);

          return *instance;
        }

      protected:

        /** \brief Converts to underlying type */
        boost::any specialised_evaluate(std::string const& s) const {
          return evaluate(s);
        }

        /** \brief Converts from the underlying implementation type */
        std::string specialised_convert(boost::any const& v) const {
          return get_single_instance().convert(static_cast< size_t >(boost::any_cast< C >(v)));
        }

        /** \brief Returns an iterator over the values of the enumerated type */
        basic_enumeration::const_iterator_range values() const {
          return get_single_instance().values();
        }

      public:
        
        /** \brief Add value
         * \param[in] v value of the chosen carrier type
         * \param[in] s any string
         * \return *this
         **/
        enumeration< C >& add(const size_t v, std::string const& s) {
          get_single_instance().add(static_cast< const size_t > (v), s);

          return *this;
        }

        /// \brief converts a value of the enumerated type to its associated string
        std::string convert(C const& s) const {
          return get_single_instance().convert(static_cast< const size_t > (s));
        }

        /**
         * \brief validates whether a string is a value of the enumerated type
         **/
        bool validate(std::string const& s) const {
          return get_single_instance().validate(s);
        }

        /** \brief Converts a string to an index representation
         * \param[in] s the string to evaluate
         **/
        C evaluate(std::string const& s) const {
          return static_cast < C > (get_single_instance().evaluate(s));
        }
    };

    /// \cond INTERNAL_DOCS
    /**
     * \brief Base class for ranges of integers
     **/
    class basic_integer_range :  public basic_datatype {
      template < typename R, typename S >
      friend class ::utility::visitor;

      friend std::ostream& operator<<(std::ostream&, basic_integer_range const&);

      protected:

        /// \brief prints range
        virtual std::ostream& print(std::ostream&) const = 0;

        /// \brief reconstructs a range from a string
        static std::auto_ptr < basic_integer_range > reconstruct(std::string const&);
    };

    inline std::ostream& operator<<(std::ostream& o, tipi::datatype::basic_integer_range const& e) {
      return e.print(o);
    }
    /// \endcond

    /**
     * \brief Derived data type specifier for integer number ranges (finite using long int)
     * 
     * The range is specified by a minimum and a maximum. The minimum, of
     * course, must be smaller than the maximum.
     **/
    template < typename C >
    class integer_range : public basic_integer_range {
      template < typename R, typename S >
      friend class ::utility::visitor;

      protected:

        /** \brief Maximum value that specifies the lower bound of a range */
        C m_minimum;

        /** \brief Maximum value that specifies the upper bound of a range */
        C m_maximum;

      protected:

        /** \brief Converts to underlying type */
        boost::any specialised_evaluate(std::string const& s) const {
          return evaluate(s);
        }

        /** \brief Converts from the underlying implementation type */
        std::string specialised_convert(boost::any const& v) const {
          return convert(boost::any_cast< C >(v));
        }

      public:

        /** \brief POD type used for implementation */
        typedef C implementation_type;

      public:

        /** \brief Constructor
         * \param[in] minimum the minimum value that specifies the range
         * \param[in] maximum the maximum value that specifies the range
         **/
        integer_range(C minimum = boost::integer_traits< C >::const_min, C maximum = boost::integer_traits< C >::const_max) : m_minimum(minimum), m_maximum(maximum) {
          assert(m_minimum < m_maximum);
        }

        /** \brief Converts a value to a string representation
         * \param[in] v the value to convert
         **/
        static std::string convert(const C v) {
          std::ostringstream temporary;

	  temporary << v;

          return temporary.str();
        }

        /** \brief Converts a string to a value of the chosen numeric type
         * \param[in] s the string to evaluate
         **/
        static C evaluate(std::string const& s) {
          C v;
          
          std::istringstream temporary(s);
           
          temporary >> v;

          return v;
        }

        /** \brief Establishes whether value is valid for an element of this type
         * \param[in] s the string to evaluate
         **/
        inline bool validate(std::string const& s) const {
          C v = evaluate(s);
      
          return m_minimum < v && v < m_maximum;
        }

        /// \brief prints range
        std::ostream& print(std::ostream& o) const {
          return o << "[" << m_minimum << "..." << m_maximum << "]";
        }
    };

    /// \cond INTERNAL_DOCS
    /**
     * \brief Base class for ranges of reals
     **/
    class basic_real_range : public basic_datatype {
      template < typename R, typename S >
      friend class ::utility::visitor;

      friend std::ostream& operator<<(std::ostream&, basic_real_range const&);

      protected:

        /// \brief prints range
        virtual std::ostream& print(std::ostream&) const = 0;

        /// \brief reconstructs a range from a string
        static std::auto_ptr < basic_real_range > reconstruct(std::string const& s);
    };

    inline std::ostream& operator<<(std::ostream& o, tipi::datatype::basic_real_range const& e) {
      return e.print(o);
    }
    /// \endcond

    /**
     * \brief Derived data type specifier for real number ranges
     *
     * The range is specified by a minimum and a maximum. The minimum, of
     * course, must be smaller than the maximum.
     *
     * \note The current implementation is based on commonly used finite
     * representations (double).
     **/
    template < typename C, bool minimum_included = true, bool maximum_included = true >
    class real_range : public basic_real_range {
      template < typename R, typename S >
      friend class ::utility::visitor;

      protected:

        /** \brief Maximum value that specifies the lower bound of a range */
        C m_minimum;

        /** \brief Maximum value that specifies the upper bound of a range */
        C m_maximum;

      protected:

        /** \brief Converts to underlying type */
        boost::any specialised_evaluate(std::string const& s) const {
          return evaluate(s);
        }

        /** \brief Converts from the underlying implementation type */
        std::string specialised_convert(boost::any const& v) const {
          return convert(boost::any_cast< C >(v));
        }

      public:

        /** \brief POD type used for implementation */
        typedef C implementation_type;

      public:

        /** \brief Constructor
         * \param[in] minimum the minimum value that specifies the range
         * \param[in] maximum the maximum value that specifies the range
         **/
        real_range(C minimum = (std::numeric_limits< C >::min)(), C maximum = (std::numeric_limits< C >::max)())
	       			: m_minimum(minimum), m_maximum(maximum) {
          assert(m_minimum < m_maximum);
        }

        /** \brief Converts a value to a string representation
         * \param[in] v the value to convert
         **/
        static std::string convert(const C v) {
          std::ostringstream temporary;

	  temporary << v;

          return temporary.str();
        }

        /** \brief Converts a string to a value of the chosen type
         * \param[in] s the string to evaluate
         **/
        static C evaluate(std::string const& s) {
          C v;
          
          std::istringstream(s) >> v;

          return v;
        }

        /** \brief Establishes whether value is valid for an element of this type
         * \param[in] s the string to evaluate
         **/
        bool validate(std::string const& s) const {
          C v(evaluate(s));
          
          return (m_minimum <= v) && (v <= m_maximum);
        }

        /// \brief prints range
        std::ostream& print(std::ostream& o) const {
          return o << "[" << m_minimum << "..." << m_maximum << "]";
        }
    };

    /// \cond INTERNAL
    /**
     * Specialisation for ranges with the maximum value not part of the range
     **/
    template < typename C >
    class real_range< C, true, false > : public real_range< C, true, true > {

        using real_range< C, true, true >::m_minimum; 
        using real_range< C, true, true >::m_maximum; 

      public:

        /** \brief Establishes whether value is valid for an element of this type
         * \param[in] s the string to evaluate
         **/
        bool validate(std::string const& s) const {
          C v(real_range< C, true, true >::evaluate(s));
          
          return (m_minimum <= v) && (v < m_maximum);
        }

        /// \brief prints range
        std::ostream& print(std::ostream& o) const {
          return o << "[" << m_minimum << "..." << m_maximum << ")";
        }
    };

    /**
     * Specialisation for ranges with the minimum value not part of the range
     **/
    template < typename C >
    class real_range< C, false, true > : public real_range< C, true, true > {

        using real_range< C, true, true >::m_minimum; 
        using real_range< C, true, true >::m_maximum; 

      public:

        /** \brief Establishes whether value is valid for an element of this type
         * \param[in] s the string to evaluate
         **/
        bool validate(std::string const& s) const {
          C v(real_range< C, true, true >::evaluate(s));
          
          return (m_minimum < v) && (v <= m_maximum);
        }

        /// \brief prints range
        std::ostream& print(std::ostream& o) const {
          return o << "(" << m_minimum << "..." << m_maximum << "]";
        }
    };

    /**
     * Specialisation for ranges with the minimum value not part of the range
     **/
    template < typename C >
    class real_range< C, false, false > : public real_range< C, true, true > {

        using real_range< C, true, true >::m_minimum; 
        using real_range< C, true, true >::m_maximum; 

      public:

        /** \brief Establishes whether value is valid for an element of this type
         * \param[in] s the string to evaluate
         **/
        bool validate(std::string const& s) const {
          C v(real_range< C, true, true >::evaluate(s));
          
          return (m_minimum < v) && (v < m_maximum);
        }

        /// \brief prints range
        std::ostream& print(std::ostream& o) const {
          return o << "(" << m_minimum << "..." << m_maximum << ")";
        }
    };
    /// \endcond INTERNAL

    /** \brief Derived data type specifier for booleans */
    class boolean : public basic_datatype {
      template < typename R, typename S >
      friend class ::utility::visitor;

      protected:

        /** \brief Converts to underlying type */
        boost::any specialised_evaluate(std::string const& s) const {
          return evaluate(s);
        }

        /** \brief Converts from the underlying implementation type */
        std::string specialised_convert(boost::any const& v) const {
          return convert(boost::any_cast< bool >(v));
        }

      public:

        /** \brief POD type used for implementation */
        typedef bool implementation_type;

      public:

        /** \brief Constructor */
        boolean() {
        }

        /** \brief Converts a boolean to a string representation
         * \param[in] v the boolean to convert
         **/
        static std::string convert(const bool v) {
          return ((v) ? "true" : "false");
        }

        /** \brief Converts a string to a boolean representation
         * \param[in] s the string to evaluate
         **/
        static bool evaluate(std::string const& s) {
          return s.compare("true") == 0;
        }

        /** \brief Establishes whether value is valid for an element of this type
         * \param[in] s the string to evaluate
         **/
        bool validate(std::string const& s) const {
          return (s == "true" || s == "false");
        }
    };

    /** \brief Derived data type for strings */
    class string : public basic_datatype {
      template < typename R, typename S >
      friend class ::utility::visitor;

      protected:

        /** \brief Maximum value that specifies the lower bound of a range */
        size_t m_minimum_length;

        /** \brief Maximum value that specifies the upper bound of a range */
        size_t m_maximum_length;

      protected:

        /** \brief Converts to underlying type */
        boost::any specialised_evaluate(std::string const& s) const {
          return evaluate(s);
        }

        /** \brief Converts from the underlying implementation type */
        std::string specialised_convert(boost::any const& v) const {
          return convert(boost::any_cast< std::string >(v));
        }

      public:

        /** \brief type used for implementation */
        typedef std::string implementation_type;

      public:

        /** \brief Constructor */
        string(size_t minimum = 0, size_t maximum = boost::integer_traits< size_t >::const_max) :
                                                m_minimum_length(minimum), m_maximum_length(maximum) {
          assert(m_minimum_length < m_maximum_length);
        }

        /** \brief Converts a string to a string representation (copy) */
        static std::string convert(std::string const& s) {
          return s;
        }

        /** \brief Converts a string to a string representation (copy) */
        static std::string evaluate(std::string const& s) {
          return s;
        }

        /** \brief Establishes whether value is valid for an element of this type */
        bool validate(std::string const& v) const {
          return (m_minimum_length <= v.size() && v.size() <= m_maximum_length);
        }
    };

//    class uri : public basic_datatype {
//    };
  }
}

#endif

