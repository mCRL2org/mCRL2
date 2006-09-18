#ifndef SQUADT_EXCEPTION_H
#define SQUADT_EXCEPTION_H

#include <utility/exception.h>

namespace squadt {
  namespace exception {

    /** \brief Type for exception identification */
    enum values {
      cannot_access_user_settings_directory,    ///< \brief the directory where user settings must be stored is not accessible
      failed_loading_object,                    ///< \brief failed to load information
      required_attributes_missing,              ///< \brief input from file failed due to errors in input
      requested_tool_unavailable,               ///< \brief requested tool is not available
      program_execution_failed,                 ///< \brief program execution failed
      unexpected_instance_identifier,           ///< \brief a connection was accepted of which the peer provided an unexpected instance identifier
      missing_object_descriptor,                ///< \brief processor dependency graph has dangling edges
      cannot_build                              ///< \brief for some reason a file cannot be (re)build
    };

    /** \brief A basic type for exceptions derived from that provided by the standard library */
    typedef ::utility::exception < values > exception;
  }
}

#ifdef SQUADT_IMPORT_STATIC_DEFINITIONS
namespace utility {
  /** \brief Descriptions for error messages */
  template < >
  const char* const exception< squadt::exception::values >::descriptions[] = {
    "Cannot write to user settings directory ('%s').",
    "Failed to load %s from `%s'.",
    "Values for required attributes are missing in element `%s'.",
    "Requested tool `%s' is not available!",
    "Execution of program `%s' failed.",
    "Peer provided invalid or unexpected identifier!",
    "Dependency on object that no longer exists.",
    "Cannot not (re)build %s"
  };
}
#endif

#endif
