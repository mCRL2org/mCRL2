#ifndef TRANSPORT_EXCEPTION_H
#define TRANSPORT_EXCEPTION_H

#include <utility/exception.h>

namespace transport {

  /** \brief The common exception type */
  class exception : public std::exception {

    private:

      /** \brief The message to display */
      std::string  message;

    public:

      /** \brief Constructor */
      exception(std::string const&);

      /** \brief A description for the exception */
      const char* what() const throw ();

      /** \brief Destructor */
      ~exception() throw ();
  };

  inline exception::exception(std::string const& m) : message(m) {
  }

  inline const char* exception::what() const throw () {
    return (message.c_str());
  }

  inline exception::~exception() throw () {
  }
}

#endif

