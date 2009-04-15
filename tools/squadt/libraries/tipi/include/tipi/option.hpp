// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file tipi/option.hpp
/// \brief Type used to represent an option in a configuration (protocol concept)

#ifndef _TIPI_CONFIGURATION_OPTION_HPP__
#define _TIPI_CONFIGURATION_OPTION_HPP__

#include <boost/shared_ptr.hpp>
#include <boost/call_traits.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/type_traits/is_enum.hpp>
#include <boost/type_traits/is_pod.hpp>
#include <boost/type_traits/is_floating_point.hpp>
#include <boost/type_traits/is_integral.hpp>
#include <boost/type_traits/remove_const.hpp>
#include <boost/type_traits/remove_reference.hpp>
#include <boost/range/iterator_range.hpp>

#include "tipi/basic_datatype.hpp"

#ifndef NDEBUG
# include <typeinfo>
#endif

namespace tipi {

  /// \cond INTERNAL_DOCS
  namespace detail {
    template < typename S, typename E = void >
    struct guess_type {
    };

    template < typename S >
    struct guess_type< S, typename boost::enable_if< typename boost::is_enum< S >::type >::type > {
      typedef datatype::enumeration< S > type;
    };

    template < typename S >
    struct guess_type< S, typename boost::enable_if< typename boost::is_floating_point< S >::type >::type > {
      typedef datatype::real_range< S, true, true > type;
    };

    template < typename S >
    struct guess_type< S, typename boost::enable_if< typename boost::is_integral< S >::type >::type > {
      typedef datatype::integer_range< S > type;
    };

    template < >
    struct guess_type< bool, void > {
      typedef datatype::boolean type;
    };

    template < >
    struct guess_type< std::string, void > {
      typedef datatype::string type;
    };

    template < >
    struct guess_type< const char*, void > {
      typedef datatype::string type;
    };

    template < >
    struct guess_type< char*, void > {
      typedef datatype::string type;
    };

    template < >
    struct guess_type< char* const, void > {
      typedef datatype::string type;
    };
  }
  /// \endcond

  /** \brief Describes a single option (or option instance) the basic building block of a tool configuration */
  class configuration::option : public configuration::parameter {
    friend class tipi::configuration;

    template < typename R, typename S >
    friend class ::utility::visitor;

    private:

      /** \brief Type for argument to value mapping */
      typedef std::pair < boost::shared_ptr< datatype::basic_datatype >, std::string > type_value_pair;

      /** \brief Container for lists of arguments to lists of value mapping */
      typedef std::vector < type_value_pair >                           type_value_list;

    public:

      /** \brief Iterator over the argument values */
      class argument_iterator {

        private:

          /** \brief the start of the sequence */
          type_value_list::const_iterator       iterator;

          /** \brief the end of the sequence */
          type_value_list::const_iterator const end;

        public:

          /** \brief Constructor
           * \param b the iterator from which to start
           * \param e the iterator with which to end
           **/
          inline argument_iterator(type_value_list::const_iterator& b, type_value_list::const_iterator& e) : iterator(b), end(e) {
          }

          /** \brief Whether the iterator has moved past the end of the sequence */
          inline bool valid() const {
            return (iterator != end);
          }

          /** \brief Advances to the next element */
          inline void operator++() {
            ++iterator;
          }

          /** \brief Returs a functor that, when invoked, returns a value */
          template < typename T >
          inline T operator*() const {
            tipi::datatype::basic_datatype* p = (*iterator).first.get();

            return (p->evaluate< T >((*iterator).second));
          }
      };

    private:

      /** \brief List of (type, default value) */
      type_value_list m_arguments;

    private:

      /** \brief Constructor */
      option();

    public:

      /** \brief Whether the option takes arguments */
      bool takes_arguments() const;

      /** \brief Returns the value of the first argument */
      template < typename T >
      T get_value(size_t const&) const;

      /** \brief Gets an iterator that in order of appearance returns the values for each argument */
      inline boost::iterator_range< type_value_list::const_iterator > get_value_iterator() const {
        return (boost::make_iterator_range(m_arguments.begin(), m_arguments.end()));
      }

      /** \brief Gets an iterator that in order of appearance returns the values for each argument */
      inline boost::iterator_range< type_value_list::iterator > get_value_iterator() {
        return (boost::make_iterator_range(m_arguments.begin(), m_arguments.end()));
      }

      /**
       * \brief Append type and instance ...
       * \param[in] t smart pointer to the data type definition
       * \param[in] d data that must be an instance of the chosen data type
       **/
      template < typename S, typename T >
      inline void append_argument(boost::shared_ptr< S > const& t, typename boost::call_traits< const T >::param_type d) {
        m_arguments.push_back(std::make_pair(t, convert(*t, d)));
      }

      /**
       * \brief Append type and instance ...
       * \param[in] d data that must be an instance of the chosen data type
       **/
      template < typename T >
      inline void append_argument(typename boost::enable_if< typename boost::is_enum< T >::type, const T >::type d) {
        boost::shared_ptr< datatype::basic_datatype > p(new datatype::enumeration< T >);

        append_argument< datatype::basic_datatype, T >(p, d);
      }

      /**
       * \brief Special function to set/replace the value of an argument ...
       * \param[in] t value to set
       * \param[in] b whether or not to append if the argument is not present
       **/
      template < unsigned int n, typename S, typename T >
      inline typename boost::enable_if< typename boost::is_pod< T >::type, void >::type
      set_argument_value(const T t, bool b = true) {
        if (n < m_arguments.size()) {
          if (b) {
            if (dynamic_cast< S const* > (m_arguments[n].first.get())) {
              m_arguments[n].second = convert(*m_arguments[n].first,t);
            }
            else {
              boost::shared_ptr< S > a_type(new S);

              m_arguments[n] = std::make_pair(a_type, convert(*a_type,t));
            }
          }
        }
        else {
          append_argument< S, T >(boost::shared_ptr< S >(new S), t);
        }
      }

      /**
       * \brief Special function to set/replace the value of an argument ...
       * \param[in] t value to set
       * \param[in] b whether or not to append if the argument is not present
       **/
      template < unsigned int n, typename T >
      inline void set_argument_value(const T t, bool b = true) {
        typedef typename detail::guess_type< typename boost::remove_const<
                typename boost::remove_reference< T >::type >::type >::type guessed_type;

        if (n < m_arguments.size()) {
          if (b) {
            if (dynamic_cast< guessed_type const* > (m_arguments[n].first.get())) {
              m_arguments[n].second = convert(*m_arguments[n].first, t);
            }
            else {
              boost::shared_ptr< guessed_type > a_type(new guessed_type);

              m_arguments[n] = std::make_pair(a_type, convert(*a_type,t));
            }
          }
        }
        else {
          append_argument< guessed_type, T >(boost::shared_ptr< guessed_type >(new guessed_type), t);
        }
      }

      /** \brief Special function to set/replace the value of an argument ... */
      template < unsigned int n, typename S, typename T >
      inline typename boost::disable_if< typename boost::is_pod< T >::type, void >::type
      set_argument_value(T const&, bool = true);

      /** \brief Clears the list of arguments */
      void clear();
  };

  inline configuration::option::option() {
  }

  inline bool configuration::option::takes_arguments() const {
    return (m_arguments.size() != 0);
  }

  /**
   * \param[in] n the argument of which to return the value
   * \pre the option must have at least n arguments
   **/
  template < typename T >
  inline T configuration::option::get_value(size_t const& n) const {
    assert(n < m_arguments.size());

    return tipi::datatype::convert< T >(*m_arguments[n].first, m_arguments[n].second);
  }

  /**
   * \param[in] t smart pointer to the data type definition
   * \param[in] d data that must be an instance of the chosen data type
   **/
  template < >
  inline void configuration::option::append_argument< datatype::basic_datatype, const std::string >(
    boost::shared_ptr< datatype::basic_datatype > const& t, boost::call_traits< const std::string >::param_type d) {

    assert(t->validate(d));

    m_arguments.push_back(std::make_pair(t, d));
  }

  template < unsigned int n, typename S, typename T >
  inline typename boost::disable_if< typename boost::is_pod< T >::type, void >::type
  configuration::option::set_argument_value(T const& t, bool b) {
    if (n < m_arguments.size()) {
      if (b) {
        assert(typeid(S) == typeid(*m_arguments[n].first));

        m_arguments[n].second = convert(*m_arguments[n].first, t);
      }
    }
    else {
      append_argument< S, T >(boost::shared_ptr< S >(new S), t);
    }
  }

  inline void configuration::option::clear() {
    m_arguments.clear();
  }
}

#endif
