#ifndef SIP_VISITORS_H__
#define SIP_VISITORS_H__

#include <boost/filesystem/path.hpp>
#include <boost/shared_ptr.hpp>

namespace sip {

  class store_visitor_impl;

  class store_visitor {

    private:

      boost::shared_ptr < store_visitor_impl > impl;

    public:

      /** \brief Constructor to write to string */
      store_visitor(std::string&);

      /** \brief Constructor to write to file */
      store_visitor(boost::filesystem::path const&);

      /** \brief Constructor to writes to stream */
      store_visitor(std::ostream&);

      /** \brief Start storage procedure */
      template < typename T >
      void visit(T const&);
  };

  class restore_visitor_impl;

  class restore_visitor {

    private:

      boost::shared_ptr < restore_visitor_impl > impl;

    public:

      /** \brief Constructor to read from string */
      restore_visitor(std::string const&);

      /** \brief Constructor to read from file */
      restore_visitor(boost::filesystem::path const&);

      /** \brief Constructor to read from stream */
      template < typename T >
      restore_visitor(T&);

      /** \brief Start storage procedure */
      template < typename T >
      void visit(T&);
  };

  class visitors {

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
      static void restore(T&, U&);
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

    v.visit(t);
  }

  template < typename T >
  inline void visitors::store(T const& t, boost::filesystem::path const& p) {
    sip::store_visitor  v(p);

    v.visit(t);
  }

  template < typename T >
  inline void visitors::store(T const& t, std::ostream& o) {
    sip::store_visitor  v(o);

    v.visit(t);
  }

  template < typename T >
  inline void visitors::restore(T& t, std::string const& s) {
    sip::restore_visitor  v(s);

    v.visit(t);
  }

  template < typename T >
  inline void visitors::restore(T& t, boost::filesystem::path const& p) {
    sip::restore_visitor  v(p);

    v.visit(t);
  }

  template < typename T, typename U >
  inline void visitors::restore(T& t, U& s) {
    sip::restore_visitor  v(s);

    v.visit(t);
  }
}

#endif
