// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING.
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
    template < typename M >
    class scheme : public boost::noncopyable {
      public:

        /** \brief Destructor */
        virtual ~scheme() = 0;
 
        /** \brief Builds a connection between a tool communicator and a controller communicator */
        virtual void connect(basic_messenger< M >*) const = 0;
    };
 
    /** \brief Derived class for the traditional scheme */
    template < typename M >
    class traditional_scheme : public scheme< M > {
      friend class command_line_interface::argument_extractor;
 
      public:
        inline traditional_scheme();
 
        /** Builds a connection between a tool communicator and a controller communicator, using the traditional scheme */
        inline void connect(basic_messenger< M >*) const;
    };
 
    /** \brief Derived class for the socket scheme */
    template < typename M >
    class socket_scheme : public scheme< M > {
      friend class command_line_interface::argument_extractor;
 
      private:
        /** \brief IPv4 address of endpoint */
        std::string host_name;
 
        /** \brief Port number */
        long int    port;
 
      public:
        inline socket_scheme();
 
        /** Builds a connection between a tool communicator and a controller communicator, using the socket scheme */
        inline void connect(basic_messenger< M >*) const;
    };

    /** \brief Destructor */
    template < typename M >
    inline scheme< M >::~scheme() {
    }
 
    /** \brief Constructor */
    template < typename M >
    inline traditional_scheme< M >::traditional_scheme() {
    }
 
    /** \brief Constructor */
    template < typename M >
    inline socket_scheme< M >::socket_scheme() {
    }
  }
}
#endif
