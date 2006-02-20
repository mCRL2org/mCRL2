#ifndef SIP_EXCEPTION_H
#define SIP_EXCEPTION_H

#include <exception>

namespace sip {

  /* Exception class for protocol specific exceptions. */
  class exception : public std::exception {
    public:
      enum exception_type {
        expected_message_start           /// \brief the start of a message <message> was expected 
        ,expected_message_end            /// \brief the end of a message </message> was expected 
        ,forbidden_message_content       /// \brief message contains a sequence that is not allowed 
        ,message_context_invalid         /// \brief the message cannot be send in this phase of the protocol 
        ,unknown_type_in_configuration   /// \brief the type encountered in a configuration specification is unknown 
        ,controller_capabilities_unknown /// \brief the controller capabilities are not known yet
      };

    private:
      exception_type     type;

      static const char* descriptions[];

    public:

      /** Constructor */
      inline exception(exception_type t);

      /** A description for the exception */
      inline const char* what();

      /** Desctructor */
      inline ~exception() throw ();
  };

  inline exception::exception(exception_type t) : type(t) {
  }

  inline const char* exception::what() {
    return (descriptions[type]);
  }

  inline exception::~exception() throw () {
  }
}

#endif

