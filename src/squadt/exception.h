#ifndef SQUADT_EXCEPTION_H
#define SQUADT_EXCEPTION_H

#include <exception/exception.h>

namespace squadt {

  namespace exception_identifier {

    enum values {
      cannot_access_user_settings_directory /// \brief the directory where user settings must be stored is not accessible
     ,cannot_load_tool_configuration        /// \brief the file that contains the information about known tools cannot be read
     ,program_execution_failed              /// \brief program execution failed
    };
  }

  typedef exception::exception < exception_identifier::values > exception;
}

#endif
