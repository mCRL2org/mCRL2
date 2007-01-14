#ifndef SIP_OPTION_H
#define SIP_OPTION_H

#include <vector>
#include <string>

#include <boost/any.hpp>
#include <boost/utility.hpp>
#include <boost/shared_ptr.hpp>

#include <sip/detail/basic_datatype.h>
#include <sip/parameter.h>
#include <sip/exception.h>

namespace sip {

  class configuration;

  /** \brief Describes a single option (or option instance) the basic building block of a tool configuration */
  class option : public sip::parameter, public utility::visitable < option > {
    friend class sip::configuration;
    friend class sip::restore_visitor_impl;
    friend class sip::store_visitor_impl;

    private:

      /** \brief Type for argument to value mapping */
      typedef std::pair < datatype::basic_datatype::sptr, std::string > type_value_pair;

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

          /** \brief Constructor */
          inline argument_iterator(type_value_list::const_iterator, type_value_list::const_iterator);

          /** \brief Whether the iterator has moved past the end of the sequence */
          inline bool valid() const;

          /** \brief Advances to the next element */
          inline void operator++();

          /** \brief Returs a functor that, when invoked, returns a value */
          inline boost::any operator*() const;
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
      boost::any get_value(size_t const&) const;

      /** \brief Gets an iterator that in order of appearance returns the values for each argument */
      boost::iterator_range< type_value_list::const_iterator > get_value_iterator() const;

      /** \brief Gets an iterator that in order of appearance returns the values for each argument */
      boost::iterator_range< type_value_list::iterator > get_value_iterator();

      /** \brief Append to the type (option takes an additional argument of the specified type) */
      void append_type(datatype::basic_datatype::sptr const&);

      /** \brief Append to the type (option takes an additional argument of the specified type) */
      template < typename S >
      void append_type();

      /** \brief Append type and instance ... */
      template < typename S, typename T >
      void append_argument(S const&, T const&);

      /** \brief Append type and instance ... */
      template < typename S, typename T >
      void append_argument(T const&);

      /** \brief Replace an argument (type and instance) ... */
      template < typename T >
      void replace_argument(const size_t i, datatype::basic_datatype::sptr, T const&);

      /** \brief Assigns a value to the n-th argument of the option */
      void bind_argument(const size_t n, std::string const&);

      /** \brief Clears the list of arguments */
      void clear();
  };

  /**
   * \param b the iterator from which to start
   * \param e the iterator with which to end
   **/
  inline option::argument_iterator::argument_iterator
          (type_value_list::const_iterator b, type_value_list::const_iterator e) : iterator(b), end(e) {
  }

  inline bool option::argument_iterator::valid() const {
    return (iterator != end);
  }

  inline void option::argument_iterator::operator++() {
    ++iterator;
  }

  inline boost::any option::argument_iterator::operator*() const {
    sip::datatype::basic_datatype* p = (*iterator).first.get();

    return (p->evaluate((*iterator).second));
  }

  inline option::option() {
  }

  inline bool option::takes_arguments() const {
    return (m_arguments.size() != 0);
  }

  /**
   * \param[in] n the argument of which to return the value
   * \pre the option must have at least n arguments
   **/
  inline boost::any option::get_value(size_t const& n) const {
    assert(n < m_arguments.size());

    return (m_arguments[n].first->evaluate(m_arguments[0].second));
  }

  inline boost::iterator_range < option::type_value_list::const_iterator > option::get_value_iterator() const {
    return (boost::make_iterator_range(m_arguments.begin(), m_arguments.end()));
  }

  inline boost::iterator_range < option::type_value_list::iterator > option::get_value_iterator() {
    return (boost::make_iterator_range(m_arguments.begin(), m_arguments.end()));
  }

  inline void option::append_type(datatype::basic_datatype::sptr const& t) {
    assert(t.get() != 0);

    m_arguments.push_back(std::make_pair(t, ""));
  }

  template < typename S >
  inline void option::append_type() {
    boost::shared_ptr < S > p(new S);

    m_arguments.push_back(std::make_pair(p, ""));
  }

  /**
   * \param[in] t smart pointer to the data type definition
   * \param[in] d data that must be an instance of the chosen data type
   **/
  template < typename S, typename T >
  inline void option::append_argument(S const& t, T const& d) {
    assert(t.get() != 0);

    append_argument(boost::static_pointer_cast < sip::datatype::basic_datatype > (t), t->convert(d));
  }

  /**
   * \param[in] t smart pointer to the data type definition
   * \param[in] d data that must be an instance of the chosen data type
   **/
  template < >
  inline void option::append_argument(datatype::basic_datatype::sptr const& t, std::string const& d) {
    assert(t.get() != 0);

    assert(t->validate(d));

    m_arguments.push_back(std::make_pair(t, d));
  }

  /**
   * \param[in] t pointer to the data type definition
   * \param[in] d data that must be an instance of the chosen data type
   **/
  template < typename S, typename T >
  inline void option::append_argument(T const& d) {
    boost::shared_ptr < S > p(new S);

    append_argument(p, d);
  }

  /**
   * \param[in] i the index of the element to replace
   * \param[in] t pointer to the data type definition
   * \param[in] d data that is valid w.r.t. the data type
   **/
  template < typename T >
  inline void option::replace_argument(const size_t i, datatype::basic_datatype::sptr t, T const& d) {
    assert(t.get() != 0);

    replace_argument(i, t, t.get()->convert(d));
  }

  /**
   * \param[in] i the index of the element to replace
   * \param[in] t pointer to the data type definition
   * \param[in] d data that is valid w.r.t. the data type
   **/
  template < >
  inline void option::replace_argument(const size_t i, datatype::basic_datatype::sptr t, std::string const& d) {
    assert(t.get() != 0);
    assert(0 <= i && i < m_arguments.size());
    assert(t->validate(d));

    m_arguments[i] = std::make_pair(t, d);
  }

  /**
   * \param[in] i the index of the element to replace
   * \param[in] d data that is valid w.r.t. the data type
   **/
  inline void option::bind_argument(const size_t i, std::string const& d) {
    assert(0 <= i && i < m_arguments.size());
    assert(m_arguments[i].first->validate(d));

    m_arguments[i].second = d;
  }

  inline void option::clear() {
    m_arguments.clear();
  }
}

#endif
