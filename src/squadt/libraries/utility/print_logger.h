#ifndef PRINT_LOGGER_H_
#define PRINT_LOGGER_H_

#include <iostream>

#include "logger.h"

namespace utility {

  /** \brief Simple logger that prints to a standard stream */
  class print_logger : public logger {

    private:

      /** \brief The stream to which the output should be printed */
      std::ostream& stream;

    private:

      /** \brief Adds a log message with a string and a log level */
      inline void actually_log(std::string const&);

      /** \brief Adds a log message with a format object and a log level */
      inline void actually_log(boost::format const&);

    public:
     
      /** \brief Constructor */
      inline print_logger(std::ostream&, log_level = 1);

      /** \brief Destructor */
      inline ~print_logger();
  };

  /**
   * @param[in,out] s the stream on which to print during the lifetime of the object
   * @param[in] l the log level below which all messages will be printed
   **/
  inline print_logger::print_logger(std::ostream& s, log_level l) : logger(l), stream(s) {
  }

  /**
   * @param[in] m the message content
   **/
  inline void print_logger::actually_log(std::string const& m) {
    stream << m;
  }

  /**
   * @param[in] m the message content
   **/
  inline void print_logger::actually_log(boost::format const& m) {
    stream << m;
  }

  inline print_logger::~print_logger() {
  }
}

#endif
