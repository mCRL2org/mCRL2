#ifndef LOGGER_H_
#define LOGGER_H_

#include <string>

#include <boost/format.hpp>

#ifdef _MSC_VER
# include <substitutes.h>
#endif // _MSC_VER

namespace utility {

  /** \brief Minimal interface for a logger component */
  class logger {

    public:

      /** \brief Type for log level specification */
      typedef unsigned char log_level;

    protected:

      /** \brief The log level below which all messages are printed */
      log_level     filter_level;

    private:
     
      /** \brief Adds a log message with a string and a log level */
      virtual void actually_log(std::string const& m) = 0;

      /** \brief Adds a log message with a format object and a log level */
      virtual void actually_log(boost::format const&) = 0;

    public:

      inline logger(log_level);

      /** \brief Adds a log message with a string and a log level */
      inline void log(log_level, std::string const&);

      /** \brief Adds a log message with a format object and a log level */
      inline void log(log_level, boost::format const&);

      /** \brief Sets filter level below which messages are logged */
      inline void set_filter_level(log_level l);

      /** \brief Returns filter level below which messages are logged */
      inline log_level get_filter_level();

      /** \brief Destructor */
      virtual ~logger() = 0;
  };

  inline logger::logger(log_level l) : filter_level(l) {
  }

  inline void logger::set_filter_level(log_level l) {
    filter_level = l;
  }

  inline logger::log_level logger::get_filter_level() {
    return (filter_level);
  }

  /**
   * @param[in] l the log level of the message
   * @param[in] m the message content
   **/
  inline void logger::log(log_level l, std::string const& m) {
    if (l < filter_level) {
      actually_log(m);
    }
  }

  /**
   * @param[in] l the log level of the message
   * @param[in] m the message content
   **/
  inline void logger::log(log_level l, boost::format const& m) {
    if (l < filter_level) {
      actually_log(m);
    }
  }

  inline logger::~logger() {
  }
}

#endif
