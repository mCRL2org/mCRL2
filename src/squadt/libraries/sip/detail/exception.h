#ifndef SIP_EXCEPTION
#define SIP_EXCEPTION

#include <exception/exception.h>

namespace sip {
  namespace exception {

    /** \brief Type for exception identification */
    enum values {
      not_yet_implemented,                ///< \brief the requested functionality is not, or only partly implemented
      message_expected_start,             ///< \brief the start of a message sip_messenger::open_tag was expected 
      message_expected_end,               ///< \brief the end of a message was expected 
      message_forbidden_content,          ///< \brief message contains a sequence that is not allowed 
      message_context_invalid,            ///< \brief the message cannot be send in this phase of the protocol 
      message_missing_required_attribute, ///< \brief a required attribute is missing
      message_unknown_type,               ///< \brief the type encountered in a configuration specification is unknown 
      controller_capabilities_unknown,    ///< \brief the controller capabilities are not known yet
      cli_parse_error,                    ///< \brief a parse error occurred on a command line argument
      cli_parse_error_expected,           ///< \brief a parse error occurred on a command line argument (message with expected clause)
      unknown_layout_element              ///< \brief a layout description contained an unknown layout element
    };

    /** \brief The common exception type */
    typedef ::exception::exception < values > exception;
  }
}

#endif
