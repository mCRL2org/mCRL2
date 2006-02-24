#ifndef XML2PP_EXCEPTION
#define XML2PP_EXCEPTION

#include <exception>

#include <boost/format.hpp>

namespace xml2pp {

  class exception : public std::exception {
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
       ,unable_to_initialise_reader        /// \brief The libXML reader could not be initialised!
       ,illegal_operation_after_first_read /// \brief Operation not supported after first read!
       ,error_while_parsing_document       /// \brief Unexpected end of file, schema error or parse error!
      };

    private:

      /** The type identifier for this exception */
      type _type;

    public:
      /** \brief Constructor */
      inline exception(type t);

      inline const char* what() const throw ();

      inline ~exception() throw ();
  };

  inline exception::exception(exception::type t) : _type(t) {
  }

  inline exception::~exception() throw () {
  }

  inline const char* exception::what() const throw () {
    return (str(boost::format("Fatal : %u\n") % (unsigned int) _type).c_str());
  }
}

#endif
