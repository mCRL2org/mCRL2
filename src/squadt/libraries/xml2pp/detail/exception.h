#ifndef XML2PP_EXCEPTION
#define XML2PP_EXCEPTION

#include <exception/exception.h>

namespace xml2pp {

  namespace exception_identifier {

    /** Type for exception identification */
    enum values {
       unable_to_open_file                /// \brief Unable to read the specified input file!
      ,unable_to_initialise_reader        /// \brief The libXML reader could not be initialised!
      ,illegal_operation_after_first_read /// \brief Operation not supported after first read!
      ,error_while_parsing_document       /// \brief Unexpected end of file, schema error or parse error!
    };
  }

  /** \brief The common exception type */
  typedef exception::exception < exception_identifier::values > exception;
}

#endif
