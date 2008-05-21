// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file source/common.cpp

#define TIPI_IMPORT_STATIC_DEFINITIONS

#include "tipi/configuration.hpp"
#include "tipi/tool/category.hpp"
#include "tipi/detail/basic_messenger.ipp"
#include "tipi/common.hpp"

namespace tipi {
  /// \cond INTERNAL_DOCS
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

    /* Explicit instantiations for tipi::message */
    template basic_messenger< tipi::message >::basic_messenger();
    template basic_messenger< tipi::message >::basic_messenger(boost::shared_ptr < utility::logger > l);
    template basic_messenger< tipi::message >::basic_messenger(boost::shared_ptr < basic_messenger_impl< tipi::message > > const& c);
    template utility::logger& basic_messenger< tipi::message >::get_logger();
    template utility::logger& basic_messenger< tipi::message >::get_default_logger();
    template void basic_messenger< tipi::message >::disconnect();
    template void basic_messenger< tipi::message >::send_message(tipi::message const& m);
    template void basic_messenger< tipi::message >::add_handler(const tipi::message::type_identifier_t t, handler_type h);
    template void basic_messenger< tipi::message >::clear_handlers(const tipi::message::type_identifier_t t);
    template void basic_messenger< tipi::message >::remove_handler(const tipi::message::type_identifier_t t, handler_type h);
    template boost::shared_ptr< const tipi::message > basic_messenger< tipi::message >::await_message(tipi::message::type_identifier_t t);
    template boost::shared_ptr< const tipi::message > basic_messenger< tipi::message >::await_message(tipi::message::type_identifier_t t, long const& ts);
  }

  /** \brief textual identifiers for each element of message_identifier_t */
  template < >
  std::string as_string(message_identifier_t const& t) {
    return (messaging::message_identifier_strings[t]);
  }
  /// \endcond
}

