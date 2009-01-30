// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file tipi/report.hpp
/// \brief Type used to represent a report (protocol concept)

#ifndef _TIPI_REPORT_HPP__
#define _TIPI_REPORT_HPP__

#include <string>

#include "tipi/detail/utility/generic_visitor.hpp"

namespace tipi {

  class report;

  /** \brief Describes a report of tool operation */
  class report : public ::utility::visitable {
    template < typename R, typename S >
    friend class ::utility::visitor;

    public:

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
      std::string m_description;

    public:

      /** \brief Constructor
       * \param[in] t the type of the report
       * \param[in] d a description
       **/
      inline report(type const& t = notice, std::string const& d = "") :
                                        m_report_type(t), m_description(d) {
      }

      /** \brief Gets the description */
      inline std::string description() {
        return m_description;
      }
  };
}

#endif
