#ifndef SIP_VISITORS_H__
#define SIP_VISITORS_H__

#include <boost/mpl/or.hpp>
#include <boost/mpl/not.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/filesystem/path.hpp>

#include <utility/visitor.h>

namespace sip {

  class store_visitor_impl;

  class store_visitor : public utility::visitor_interface< sip::store_visitor_impl > {

    public:

      /** \brief Constructor to write to string */
      store_visitor(std::string&);

      /** \brief Constructor to write to file */
      store_visitor(boost::filesystem::path const&);

      /** \brief Constructor to writes to stream */
      store_visitor(std::ostream&);
  };

  class restore_visitor_impl;

  class restore_visitor : public utility::visitor_interface< sip::restore_visitor_impl, false > {

    public:

      /** \brief Constructor to read from string */
      restore_visitor(std::string const&);

      /** \brief Constructor to read from file */
      restore_visitor(boost::filesystem::path const&);

      /** \brief Constructor to read from stream */
      template < typename T >
      restore_visitor(T&);
  };

  class visitors {

    private:

      template < typename U >
      struct not_string_or_path : public boost::enable_if_c < boost::mpl::not_<
                                           boost::mpl::or_< boost::is_same < U, std::string >,
                                              boost::is_same < U, boost::filesystem::path > > >::value, void > {
      };

    public:

      /** \brief Writes to string */
      template < typename T >
      static std::string store(T const&);

      /** \brief Writes to string */
      template < typename T >
      static void store(T const&, std::string&);

      /** \brief Writes to file */
      template < typename T >
      static void store(T const&, boost::filesystem::path const&);

      /** \brief Writes to stream */
      template < typename T >
      static void store(T const&, std::ostream&);

      /** \brief Reads from string */
      template < typename T >
      static void restore(T&, std::string const&);

      /** \brief Reads from file */
      template < typename T >
      static void restore(T&, boost::filesystem::path const&);

      /** \brief Reads from stream */
      template < typename T, typename U >
      static typename not_string_or_path< U >::type restore(T&, U&);
  };

  template < typename T >
  std::string visitors::store(T const& t) {
    std::string output;

    store(t, output);

    return (output);
  }

  template < typename T >
  inline void visitors::store(T const& t, std::string& s) {
    sip::store_visitor  v(s);

    v.do_accept(t);
  }

  template < typename T >
  inline void visitors::store(T const& t, boost::filesystem::path const& p) {
    sip::store_visitor  v(p);

    v.do_accept(t);
  }

  template < typename T >
  inline void visitors::store(T const& t, std::ostream& o) {
    sip::store_visitor  v(o);

    v.do_accept(t);
  }

  template < typename T >
  inline void visitors::restore(T& t, std::string const& s) {
    sip::restore_visitor  v(s);

    v.do_accept(t);
  }

  template < typename T >
  inline void visitors::restore(T& t, boost::filesystem::path const& p) {
    sip::restore_visitor  v(p);

    v.do_accept(t);
  }

  template < typename T, typename U >
  inline typename visitors::not_string_or_path< U >::type visitors::restore(T& t, U& s) {
    sip::restore_visitor  v(s);

    v.do_accept(t);
  }
}

#endif
