// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file tipi/common.hpp
/// \brief Collection of commonly used declarations

#ifndef TIPI_COMMON_H_
#define TIPI_COMMON_H_

#include "tipi/detail/message.hpp"
#include "tipi/detail/basic_messenger.hpp"

/**
 * \brief Interface implementation for a protocol for interactive tool integration
 *
 * This namespace contains interfaces and functionality that implements a
 * communication protocol for interactive tool integration. The partners in
 * this communication are a controller and a single tool.
 *
 * The implementation offers two distinct interfaces, targeted specifically at
 * one of the communication partners. Additionally there is a common set of
 * classes and functions that are used in both interfaces.
 **/
namespace tipi {

  /**
   * \brief Utility functionality
   *
   * This namespace harbours functionality that does not really belong anywhere
   * else in the protocol interface. For example logging functionality is
   * something that is completely unrelated to the protocol or any part of its
   * interfaces.
   **/
  namespace utility {
  }

  class visitors;
  class report;
  class configuration;
  class store_visitor;
  class store_visitor_impl;
  class restore_visitor;
  class restore_visitor_impl;

  /**
   * \brief Specific controller-side functionality
   *
   * The protocol specifies a set of rules for communication between two
   * partners, the controller and the tool. The partners are not equal and
   * therefore each partner uses it's own interface. This namespace contains
   * the interface-specific functionality for the controller side.
   **/
  namespace controller {
    class communicator_impl;
  }

  /**
   * \brief Specific tool-side functionality
   *
   * The protocol specifies a set of rules for communication between two
   * partners, the controller and the tool. The partners are not equal and
   * therefore each partner uses it's own interface. This namespace contains
   * the interface-specific functionality for the tool side.
   **/
  namespace tool {
    class communicator_impl;
  }

  /**
   * \brief Layout specific functionality
   *
   * The protocol prescribes a `tool display', which is a service offered to
   * each tool by the controller. The tool display is a method to communicate
   * directly with the user (think of a window with graphical user interface
   * controls). To make use of it a tool needs to communicate layout
   * information.
   *
   * The layout is represented by a hierarchy of classes used by both the
   * controller and a tool. User interaction with the tool display on the
   * controller side, and manipulation of the class structure on the tool
   * side get communicated automatically.
   **/
  namespace layout {
    /**
     * \brief Layout specific functionality
     *
     * Contains classes for the user interface controls available for the tool display.
     **/
    namespace elements {
    }
  }

  /**
   * \brief Type specification functionality
   *
   * At several places in the interfaces arbitrary data is communicated as a
   * string whereas it should be a very specific string. The data types specify
   * what the string should look like and can convert safely to an internal
   * representation (say using C++ POD-type int). To verify whether data has
   * the expected type, one must first have some sort of type-checker. This is
   * the kind of functionality contained in this namespace. Every available
   * type has a class with checking and conversion functionality.
   *
   * Some examples on the use of this functionality are for instance a text
   * field control on the tool display that is meant to contain an integer
   * value but the user can type any value. Another example is an argument to
   * an option inside a configuration must be an integer.
   **/
  namespace datatype {
  }

  /** \brief Type for tipi protocol message identification */
  enum message_identifier_t {
     message_any,                              ///< \brief matches any of the types below
     message_capabilities,                     ///< \brief request/response of tool/controller capabilities
     message_configuration,                    ///< \brief offer/accept tool configuration
     message_display_layout,                   ///< \brief the controller a layout description for the display
     message_display_data,                     ///< \brief the controller a data to be displayed using the current display layout
     message_identification,                   ///< \brief the unique identifier assigned to a tool instance
     message_report,                           ///< \brief send the controller a report of a tools operation
     message_task,                             ///< \brief signal for task execution
     message_termination,                      ///< \brief request/response of tool termination
     message_unknown                           ///< \brief unspecified or unknown (or should be derived from content)
  };

  /// \cond INTERNAL_DOCS
  /**
   * \brief Command line part for tool interface
   *
   * The communication protocol determines the structure of communication
   * between controller and tool once both are in a position to communicate
   * (i.e. when a communication connection has been established). Before that
   * time the tool and controller have no information about each other so
   * creating a connection is impossible or would take outside help. This is
   * where the command line interface comes in.
   *
   * The command line interface parses the command line that is available to a
   * tool after execution begins. The controller starts a tool with on the
   * command line a set of arguments that carry the information on how the tool
   * can build a connection with the controller. The command line interface of
   * the protocol takes care of parsing the command line and if connection
   * information is found building a connection with the controller.
   **/
  namespace command_line_interface {
  }

  /// Helper function to invoke the as_string method a type T */
  template < typename T > std::string as_string(T const& t) {
    return (as_string(t));
  }

  template <> std::string as_string(message_identifier_t const&);
  /// \endcond

  /** \brief A message type for communication of tipi protocol messages */
  typedef messaging::message < message_identifier_t, message_unknown, message_any > message;

  /** \brief A messenger type for communication of tipi protocol messages */
  typedef messaging::basic_messenger < tipi::message >                              messenger;

  /** \brief Type for protocol version */
  struct version {
    unsigned char major; ///< the major number
    unsigned char minor; ///< the minor number
  };

  /** \brief Protocol version {major,minor} */
  const version default_protocol_version = {1,0};

  /** \brief Type for TCP port specification */
  typedef unsigned short int tcp_port;
}

#endif
