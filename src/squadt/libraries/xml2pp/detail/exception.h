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

      /** Exception identification type*/
      enum type {
        unable_to_open_input_file          /// \brief Unable to read the specified input file!
       ,unable_to_open_schema_file         /// \brief Unable to read the specified schema file!
       ,illegal_operation_after_first_read /// \brief Operation not supported after first read!
       ,error_while_parsing_document             /// \brief Unexpected end of file, schema error or parse error!
      };

    private:

      /** The type identifier for this exception */
      type _type;

    public:
      /** \brief Constructor */
      inline exception(type t);

      inline ~exception() throw ();
  };

  inline exception::exception(exception::type t) : _type(t) {
  }

  inline exception::~exception() throw () {
  }
}

#endif
