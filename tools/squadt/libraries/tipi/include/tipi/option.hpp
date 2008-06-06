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

#ifndef TIPI_OPTION_H
#define TIPI_OPTION_H

#include <boost/shared_ptr.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/type_traits/is_enum.hpp>
#include <boost/range/iterator_range.hpp>

#ifndef NDEBUG
# include <typeinfo>
#endif

namespace tipi {

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

      /** \brief Convenience type to hide the shared pointer wrapping */
      typedef boost::shared_ptr < option >  sptr;

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

      /** \brief Append type and instance ... */
      template < typename S, typename T >
      void append_argument(boost::shared_ptr< S > const&, T const&);

      /** \brief Append type and instance ... */
      template < typename T >
      typename boost::enable_if_c< boost::is_enum< T >::value, void >::type append_argument(T const& d);

      /** \brief Special function to set/replace the value of an argument ... */
      template < unsigned int n, typename S, typename T >
      void set_argument_value(T const&, bool = true);

      /** \brief Special function to set/replace the value of an argument ... */
      template < unsigned int n, typename T >
      inline typename boost::enable_if_c< boost::is_enum< T >::value, void >::type set_argument_value(T const& t);

      /** \brief Replace an argument (type and instance) ... */
      template < typename T >
      void replace_argument(unsigned int const& n, boost::shared_ptr< datatype::basic_datatype >, T const&);

      /** \brief Assigns a value to the n-th argument of the option */
      void bind_argument(const size_t n, std::string const&);

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

    return (m_arguments[n].first->evaluate< T >(m_arguments[n].second));
  }

  /**
   * \param[in] t smart pointer to the data type definition
   * \param[in] d data that must be an instance of the chosen data type
   **/
  template < typename S, typename T >
  inline void configuration::option::append_argument(boost::shared_ptr< S > const& t, T const& d) {
    assert(t.get() != 0);

    append_argument(boost::static_pointer_cast< tipi::datatype::basic_datatype > (t), t->convert(d));
  }

  /**
   * \param[in] t smart pointer to the data type definition
   * \param[in] d data that must be an instance of the chosen data type
   **/
  template < >
  inline void configuration::option::append_argument(boost::shared_ptr< datatype::basic_datatype > const& t, std::string const& d) {
    assert(t.get() != 0);

    assert(t->validate(d));

    m_arguments.push_back(std::make_pair(t, d));
  }

  /**
   * \param[in] d data that must be an instance of the chosen data type
   **/
  template < typename T >
  inline typename boost::enable_if_c< boost::is_enum< T >::value, void >::type configuration::option::append_argument(T const& d) {
    boost::shared_ptr< datatype::basic_datatype > p(new datatype::enumeration< T >);

    append_argument(p, d);
  }

  /**
   * \param[in] t pointer to the data type definition
   * \param[in] b whether or not to add if the argument is already present
   **/
  template < unsigned int n, typename S, typename T >
  void configuration::option::set_argument_value(T const& t, bool b) {
    if (n < m_arguments.size()) {
      if (b) {
        assert(typeid(S) == typeid(*m_arguments[n].first));

        m_arguments[n].second = m_arguments[n].first->convert(t);
      }
    }
    else {
      append_argument(boost::shared_ptr< S >(new S), t);
    }
  }

  /**
   * \param[in] t pointer to the data type definition
   * \pre n <= m_arguments.size()
   **/
  template < unsigned int n, typename T >
  inline typename boost::enable_if_c< boost::is_enum< T >::value, void >::type
         configuration::option::set_argument_value(T const& t) {

    assert(n <= m_arguments.size());

    set_argument_value< n, datatype::enumeration< T >, T >(t, true);
  }

  /**
   * \param[in] n the index of the element to replace
   * \param[in] t pointer to the data type definition
   * \param[in] d data that is valid w.r.t. the data type
   **/
  template < typename T >
  inline void configuration::option::replace_argument(unsigned int const& n, boost::shared_ptr< datatype::basic_datatype > t, T const& d) {
    assert(t.get() != 0);

    replace_argument(n, t, t->convert(d));
  }

  /**
   * \param[in] n the index of the element to replace
   * \param[in] t pointer to the data type definition
   * \param[in] d data that is valid w.r.t. the data type
   * \note if the the current number of arguments is n - 1, then the type/value pair is appended
   **/
  template < >
  inline void configuration::option::replace_argument(unsigned int const& n, boost::shared_ptr< datatype::basic_datatype > t, std::string const& d) {
    assert(t.get() != 0);
    assert(0 <= n && (n < m_arguments.size() + 1));
    assert(t->validate(d));

    if (m_arguments.size() + 1 == n) {
      m_arguments.push_back(std::make_pair(t, d));
    }
    else {
      m_arguments[n] = std::make_pair(t, d);
    }
  }

  /**
   * \param[in] i the index of the element to replace
   * \param[in] d data that is valid w.r.t. the data type
   **/
  inline void configuration::option::bind_argument(const size_t i, std::string const& d) {
    assert(0 <= i && i < m_arguments.size());
    assert(m_arguments[i].first->validate(d));

    m_arguments[i].second = d;
  }

  inline void configuration::option::clear() {
    m_arguments.clear();
  }
}

#endif
