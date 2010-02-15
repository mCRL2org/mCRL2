// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "tipi/configuration.hpp"
#include "tipi/tool/category.hpp"
#include "tipi/common.hpp"
#include "tipi/detail/basic_messenger.ipp"

namespace tipi {
  /// \cond INTERNAL_DOCS
  namespace messaging {

    /* Explicit instantiations for tipi::message */
    template basic_messenger< tipi::message >::basic_messenger(boost::shared_ptr < basic_messenger_impl< tipi::message > > const& c);
    template utility::logger& basic_messenger< tipi::message >::get_logger();
    template utility::logger& basic_messenger< tipi::message >::get_default_logger();
    template void basic_messenger< tipi::message >::disconnect();
    template void basic_messenger< tipi::message >::send_message(tipi::message const& m);
    template void basic_messenger< tipi::message >::add_handler(const tipi::message::message_type t, handler_type h);
    template void basic_messenger< tipi::message >::clear_handlers(const tipi::message::message_type t);
    template void basic_messenger< tipi::message >::remove_handler(const tipi::message::message_type t, handler_type h);
    template boost::shared_ptr< const tipi::message > basic_messenger< tipi::message >::await_message(tipi::message::message_type t);
    template boost::shared_ptr< const tipi::message > basic_messenger< tipi::message >::await_message(tipi::message::message_type t, long const& ts);
  }

  /** \brief textual identifiers for each element of message_identifier_t */
  template < >
  std::string as_string(message_identifier_t const& t) {
    static char const* message_identifier_strings[] = {
      "any",
      "capabilities",
      "configuration",
      "display_layout",
      "display_data",
      "identification",
      "report",
      "task",
      "termination",
      "unknown"
    };

    return message_identifier_strings[t];
  }
  /// \endcond
}

