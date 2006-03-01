#ifndef BASIC_EXCEPTION_H
#define BASIC_EXCEPTION_H

#include <exception>

#include <boost/format.hpp>

namespace exception {

  /* Exception class for protocol specific exceptions. */
  template < typename T >
  class exception : public std::exception {

    private:
      /** \brief The optional argument for an error description */
      boost::format            _message;

      /** Description messages for every possible value in T */
      static const char* const descriptions[];

    public:

      /** \brief Constructor */
      inline exception(T t);

      /** \brief Constructor for messages with one argument. */
      template < typename T1 >
      inline exception(T t, T1);

      /** \brief Constructor for messages with two arguments. */
      template < typename T1, typename T2 >
      inline exception(T t, T1, T2);

      /** \brief Returns */
      inline boost::format& message();

      /** \brief A description for the exception */
      inline const char* what() const throw ();

      /** \brief Destructor */
      inline ~exception() throw ();
  };

  /**
   * @param[in] t the type identifier of the exception
   **/
  template < typename T >
  inline exception< T >::exception(const T t) : _message(descriptions[t]) {
  }

  /**
   * @param[in] t the type identifier of the exception
   * @param[in] a an argument of which the value should be substituted in the message 
   **/
  template < typename T >
  template < typename T1 >
  inline exception< T >::exception(const T t, T1 a1) : _message(descriptions[t]) {
    _message % a1;
  }

  /**
   * @param[in] t the type identifier of the exception
   * @param[in] a1 an argument of which the value should be substituted in the message 
   * @param[in] a2 an argument of which the value should be substituted in the message 
   **/
  template < typename T >
  template < typename T1, typename T2 >
  inline exception< T >::exception(const T t, T1 a1, T2 a2) : _message(descriptions[t]) {
    _message % a1 % a2;
  }

  template < typename T >
  inline boost::format& exception< T >::message() {
    return (_message); 
  }

  template < typename T >
  inline const char* exception< T >::what() const throw () {
    return (boost::str(_message).c_str());
  }

  template < typename T >
  inline exception< T >::~exception() throw () {
  }
}

#endif

