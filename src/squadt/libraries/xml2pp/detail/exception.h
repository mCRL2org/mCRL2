#ifndef XML2PP_EXCEPTION
#define XML2PP_EXCEPTION

#include <exception>

namespace xml2pp {

  class exception : std::exception {
    /* Errors :
     *
     *  - could not open input file
     *  - error loading XML schema file for validation
     */
    public:

      enum type {
        unable_to_open_input_file,
        unable_to_open_schema_file,
        illegal_operation_after_first_read
      };

    private:
      type _type;

    public:
      exception(type t);
  };

  inline exception::exception(exception::type t) : _type(t) {
  }
}

#endif
