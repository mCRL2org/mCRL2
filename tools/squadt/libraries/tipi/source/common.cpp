//  Copyright 2007 Jeroen van der Wulp. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
/// \file source/common.cpp

#define TIPI_IMPORT_STATIC_DEFINITIONS

#include <tipi/configuration.h>
#include <tipi/tool/category.h>
#include <tipi/detail/basic_messenger.h>
#include <tipi/common.h>

namespace tipi {
  namespace messaging {

    /** The type identifier for messages of which the type is not known */
    template < >
    const tipi::message_identifier_t message< tipi::message_identifier_t, tipi::message_unknown, tipi::message_any >::message_unknown = tipi::message_unknown;

    /** The type identifier for messages of any type */
    template < >
    const tipi::message_identifier_t message< tipi::message_identifier_t, tipi::message_unknown, tipi::message_any >::message_any     = tipi::message_any;
  }

  /** \brief textual identifiers for each element of message_identifier_t */
  template < >
  std::string as_string(message_identifier_t const& t) {
    static char const* message_identifier_strings[] = {
      "unknown",
      "any",
      "instance_identification",
      "controller_capabilities",
      "tool_capabilities",
      "configuration",
      "task_start",
      "task_done",
      "display_layout",
      "display_update",
      "termination",
      "report"
    };

    return (message_identifier_strings[t]);
  }
}

