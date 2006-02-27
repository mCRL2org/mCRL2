#ifndef SIP_EXCEPTION
#define SIP_EXCEPTION

#include <exception/exception.h>

namespace sip {

  namespace exception_identifier {

    /** Type for exception identification */
    enum values {
       expected_message_start          /// \brief the start of a message sip_messenger::open_tag was expected 
      ,expected_message_end            /// \brief the end of a message was expected 
      ,forbidden_message_content       /// \brief message contains a sequence that is not allowed 
      ,message_context_invalid         /// \brief the message cannot be send in this phase of the protocol 
      ,unknown_type_in_configuration   /// \brief the type encountered in a configuration specification is unknown 
      ,controller_capabilities_unknown /// \brief the controller capabilities are not known yet
      ,cli_missing_arguments_separator /// \brief a parse error occurred on a command line argument
    };
  }

  typedef exception::exception < exception_identifier::values > exception;
}

#endif
