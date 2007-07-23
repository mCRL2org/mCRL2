//  Copyright 2007 Jeroen van der Wulp. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
/// \file source/common.cpp

#define TIPI_IMPORT_STATIC_DEFINITIONS

#include "tipi/configuration.hpp"
#include "tipi/tool/category.hpp"
#include "tipi/detail/basic_messenger.hpp"
#include "tipi/common.hpp"

namespace tipi {
  namespace messaging {

    /** The type identifier for messages of which the type is not known */
    template < >
    const tipi::message_identifier_t message< tipi::message_identifier_t, tipi::message_unknown, tipi::message_any >::message_unknown = tipi::message_unknown;

    /** The type identifier for messages of any type */
    template < >
    const tipi::message_identifier_t message< tipi::message_identifier_t, tipi::message_unknown, tipi::message_any >::message_any     = tipi::message_any;

    static char const* message_identifier_strings[] = {
      "any",
      "capabilities",
      "configuration",
      "display_layout",
      "display_data",
      "identification",
      "report",
      "task_start",
      "task_done",
      "termination",
      "unknown"
    };
  }

  /** \brief textual identifiers for each element of message_identifier_t */
  template < >
  std::string as_string(message_identifier_t const& t) {
    return (messaging::message_identifier_strings[t]);
  }
}

