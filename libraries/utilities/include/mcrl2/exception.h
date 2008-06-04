// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/exception.h

#ifndef __MCRL2_EXCEPTION_H__
#define __MCRL2_EXCEPTION_H__

#include <stdexcept>

namespace mcrl2 {

  /**
   *
   **/
  class runtime_error : public std::runtime_error {

      std::string m_message;

    public:

      /// \brief Constructor
      /// \param[in] message the exception message
      runtime_error(std::string const& message) : std::runtime_error(message), m_message(std::string("error: ") + message) {
      }

      /// \brief error: to message
      const char* what() const throw () {
        return m_message.c_str();
      }

      ~runtime_error() throw () {
      }
  };
}

// forcing use of mcrl2::runtime_error over mcrl2::runtime_error when namespace
// qualifier is not present
using mcrl2::runtime_error;

#endif
