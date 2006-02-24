#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <exception>
#include <string>

#include <boost/format.hpp>

namespace squadt {

  /** Exception class for protocol specific exceptions. */
  class exception : public std::exception {
    public:
      /** Type for distinguising exceptions */
      enum exception_type {
        cannot_access_user_settings_directory /// \brief the directory where user settings must be stored is not accessible
       ,cannot_load_tool_configuration        /// \brief the file that contains the information about known tools cannot be read
      };

    private:
      /** \brief Exception type identifier */
      exception_type     type;

      /** \brief The optional argument for an error description */
      const std::string  argument;

      /** \brief A list of textual descriptions for each type */
      static const char* const descriptions[];

    public:

      /** \brief Constructor */
      template < typename T >
      inline exception(exception_type t, const T a = "");

      /** \brief A description for the exception */
      inline const char* what() const throw ();

      /** \brief Destructor */
      inline ~exception() throw ();
  };
}

#endif

