// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/exception.h
/// \brief Exception classes for use in libraries and tools

#ifndef __MCRL2_EXCEPTION_H__
#define __MCRL2_EXCEPTION_H__

#include <stdexcept>
#include <string>
#include <iostream>
#include <cassert>

namespace mcrl2 {

  /**
   * \brief Standard exception class for reporting runtime errors.
   **/
  class runtime_error : public std::runtime_error {

    private:

      std::string m_message;

    public:

      /// \brief Constructor
      /// \param[in] message the exception message
      runtime_error(std::string const& message) : std::runtime_error(message), m_message(std::string("error: ").append(message)) {
      }

      /// \brief error: to message
      const char* what() const throw () {
        return m_message.c_str();
      }

      ~runtime_error() throw () {
      }
  };

  /**
   * \brief Standard exception class for reporting runtime errors.
   **/
  class logic_error : public std::logic_error {

    private:

      std::string m_message;

    public:

      /// \brief Constructor
      /// \param[in] message the exception message
      logic_error(std::string const& message) : std::logic_error(message), m_message(std::string("error: ").append(message)) {
      }

      /// \brief error: to message
      const char* what() const throw () {
        return m_message.c_str();
      }

      ~logic_error() throw () {
      }
  };
}

#endif
