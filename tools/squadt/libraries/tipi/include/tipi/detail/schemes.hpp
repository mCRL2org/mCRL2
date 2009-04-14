// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file tipi/detail/schemes.hpp

#ifndef TIPI_SCHEMES_H
#define TIPI_SCHEMES_H

#include "tipi/detail/basic_messenger.hpp"

namespace tipi {

  namespace command_line_interface {
    class argument_extractor;
  }

  namespace messaging {

    /** \brief Abstract base class for container types for parsed information about schemes */
    class scheme {
      friend class command_line_interface::argument_extractor;

      public:

        /** \brief Builds a connection between a tool communicator and a controller communicator */
        virtual void connect(basic_messenger< tipi::message >&) const = 0;

        /** \brief Destructor */
        virtual ~scheme() = 0;
    };

    /** \brief Derived class for the traditional scheme */
    class traditional_scheme : public scheme {
      friend class command_line_interface::argument_extractor;

      protected:

        /** Builds a connection between a tool communicator and a controller communicator, using the traditional scheme */
        inline void connect(basic_messenger< tipi::message >&) const {
          throw std::runtime_error("Sorry direct connection is not yet implemented");
        }

      public:

        ~traditional_scheme() {
        }
    };

    /** \brief Derived class for the socket scheme */
    class socket_scheme : public scheme {
      friend class command_line_interface::argument_extractor;

      private:

        /** \brief IPv4 address of endpoint */
        std::string host_name;

        /** \brief Port number */
        long int    port;

      protected:

        /** Builds a connection between a tool communicator and a controller communicator, using the socket scheme */
        inline void connect(basic_messenger< tipi::message >& t) const {
          t.connect(host_name, static_cast < short int > (port));
        }

      public:

        ~socket_scheme() {
        }
    };

    /** \brief Destructor */
    inline scheme::~scheme() {
    }
  }
}
#endif
