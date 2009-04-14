// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file tipi/detail/visitors.hpp
/// \brief Visitor types and functions for (de)serialisation

#ifndef TIPI_VISITORS_H__
#define TIPI_VISITORS_H__

#include <iosfwd>
#include <sstream>

#include <boost/mpl/or.hpp>
#include <boost/mpl/not.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/filesystem/path.hpp>

#include "tipi/detail/utility/generic_visitor.hpp"

namespace tipi {

  /// \cond INTERNAL_DOCS
  class store_visitor_impl;
  class restore_visitor_impl;

  /** \brief Visitor type for storing object hierarchies */
  class store_visitor : public ::utility::visitor_interface< tipi::store_visitor_impl > {

    public:

      /** \brief Constructor to write to file */
      explicit store_visitor(boost::filesystem::path const&);

      /** \brief Constructor to writes to stream */
      store_visitor(std::ostream&);
  };

  /** \brief Visitor type for restoring object hierarchies */
  class restore_visitor : public ::utility::visitor_interface< tipi::restore_visitor_impl > {

    public:

      /** \brief Constructor to read from string */
      restore_visitor(std::string const&);

      /** \brief Constructor to read from file */
      restore_visitor(boost::filesystem::path const&);

      /** \brief Constructor to read from stream */
      template < typename T >
      restore_visitor(T&);
  };
  /// \endcond

  /**
   * \brief Operations on hierarchies using visitors
   **/
  class visitors {

    private:

      /** \cond INTERNAL_DOCS
       *
       * Any type that is not std::string or boost::filesystem::path
       */
      template < typename U >
      struct not_string_or_path : public boost::enable_if_c < boost::mpl::not_<
                                           boost::mpl::or_< boost::is_same < U, std::string >,
                                              boost::is_same < U, boost::filesystem::path > > >::value, void > {
      };
      /// \endcond

    public:

      /** \brief Writes to string */
      template < typename T >
      static std::string store(T const&);

      /** \brief Writes to string */
      template < typename T, typename U >
      static std::string store(T const&, U&);

      /** \brief Writes to file */
      template < typename T >
      static void store(T const&, boost::filesystem::path const&);

      /** \brief Writes to stream */
      template < typename T >
      static void store(T const&, std::ostream&);

      /** \brief Reads from string */
      template < typename T >
      static void restore(T&, std::string const&);

      /** \brief Reads from string */
      template < typename T, typename U >
      static void restore(T&, U&, std::string const&);

      /** \brief Reads from file */
      template < typename T >
      static void restore(T&, boost::filesystem::path const&);

      /** \brief Reads from stream */
      template < typename T, typename U >
      static typename not_string_or_path< U >::type restore(T&, U&);
  };

  template < typename T >
  std::string visitors::store(T const& t) {
    std::ostringstream out;

    tipi::store_visitor(out).visit(t);

    return out.str();
  }

  template < typename T, typename U >
  inline std::string visitors::store(T const& t, U& u) {
    std::ostringstream out;

    tipi::store_visitor v(out);

    v.visit(t, u);

    return out.str();
  }

  template < typename T >
  inline void visitors::store(T const& t, boost::filesystem::path const& p) {
    tipi::store_visitor  v(p);

    v.visit(t);
  }

  template < typename T >
  inline void visitors::store(T const& t, std::ostream& o) {
    tipi::store_visitor  v(o);

    v.visit(t);
  }

  template < typename T >
  inline void visitors::restore(T& t, std::string const& s) {
    tipi::restore_visitor  v(s);

    v.visit(t);
  }

  template < typename T, typename U >
  inline void visitors::restore(T& t, U& u, std::string const& s) {
    tipi::restore_visitor  v(s);

    v.visit(t, u);
  }

  template < typename T >
  inline void visitors::restore(T& t, boost::filesystem::path const& p) {
    tipi::restore_visitor  v(p);

    v.visit(t);
  }

  template < typename T, typename U >
  inline typename visitors::not_string_or_path< U >::type visitors::restore(T& t, U& s) {
    tipi::restore_visitor  v(s);

    v.visit(t);
  }
}

#endif
