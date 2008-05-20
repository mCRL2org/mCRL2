// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING).
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file tipi/report.hpp

#ifndef TIPI_REPORT_H
#define TIPI_REPORT_H

#include <string>

#include <boost/shared_ptr.hpp>

#include "tipi/detail/utility/generic_visitor.hpp"
#include "tipi/configuration.hpp"

namespace tipi {

  class report;

  /** \brief Describes a report of tool operation */
  class report : public ::utility::visitable {
    template < typename R, typename S >
    friend class ::utility::visitor;

    public:

      /** \brief Type alias for convenience */
      typedef boost::shared_ptr < report > sptr;

      /** \brief The message class */
      enum type {
        notice,  //< like a warning but even less urgent
        warning, //< warning of some kind
        error    //< fatal error occurred
      };

    private:

      /** \brief The type of the report */
      type        m_report_type;

      /** \brief Room for errors (any error here implies unsuccessful termination) */
      std::string description;

    public:

      /** \brief Constructor */
      inline report(type const& = notice, std::string const& = "");

      /** \brief Gets the description */
      inline std::string get_description();
  };

  /**
   * \param[in] t the type of the report
   * \param[in] d a description
   **/
  inline report::report(type const& t, std::string const& d) : m_report_type(t), description(d) {
  }

  inline std::string report::get_description() {
    return (description);
  }
}

#endif
