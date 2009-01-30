// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file visitors.h
/// \brief Add your file description here.

#include <ostream>

#include <boost/filesystem/path.hpp>

#include "tipi/detail/utility/generic_visitor.hpp"

namespace squadt {
  class store_visitor_impl;

  /**
   * \brief Writes the current state of components to file
   **/
  class store_visitor : public utility::visitor_interface< squadt::store_visitor_impl > {

    public:

      /** \brief Constructor to write to string */
      store_visitor(std::string&);

      /** \brief Constructor to write to file */
      store_visitor(boost::filesystem::path const&);

      /** \brief Constructor to writes to stream */
      store_visitor(std::ostream&);

      /** \brief Stores elements using the visitor */
      template < typename T >
      void store(T const& t) {
        visit(t);
      }
  };

  class restore_visitor_impl;

  /**
   * \brief Reads preferences from file and updates the state of components accordingly
   **/
  class restore_visitor : public utility::visitor_interface< squadt::restore_visitor_impl > {

    public:

      /** \brief Constructor to read from string */
      restore_visitor(std::string const&);

      /** \brief Constructor to read from file */
      restore_visitor(boost::filesystem::path const&);

      /** \brief Constructor to read from stream */
      template < typename T >
      restore_visitor(T&);

      /** \brief Restores elements using the visitor */
      template < typename T >
      void restore(T& t) {
        visit(t);
      }
  };

  class visitors {

    public:

      /** \brief Writes to string */
      template < typename T >
      static std::string store(T const& t) {
        std::string output;

        store(t, output);

        return (output);
      }

      /** \brief Writes to string */
      template < typename T >
      static void store(T const& t, std::string& s) {
        squadt::store_visitor  v(s);

        v.visit(t);
      }

      /** \brief Writes to file */
      template < typename T >
      static void store(T const& t, boost::filesystem::path const& p) {
        squadt::store_visitor  v(p);

        v.visit(t);
      }

      /** \brief Writes to stream */
      template < typename T >
      static void store(T const& t, std::ostream& o) {
        squadt::store_visitor  v(o);

        v.visit(t);
      }

      /** \brief Reads from stream */
      template < typename T, typename U >
      static void restore(T& t, U const& s) {
        squadt::restore_visitor v(s);

        v.visit(t);
      }
  };
}
