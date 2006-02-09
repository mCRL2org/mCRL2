#ifndef SIP_EXCEPTION_H
#define SIP_EXCEPTION_H

#include <exception>

namespace sip {

  /* Exception class for protocol specific exceptions. */
  class exception : public std::exception {
    public:
      enum exception_type {
        forbidden_message_content,
        message_context_invalid,
        unknown_type_in_configuration
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

