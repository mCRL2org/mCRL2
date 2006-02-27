#ifndef BASIC_EXCEPTION_H
#define BASIC_EXCEPTION_H

#include <exception>

#include <boost/format.hpp>

namespace exception {

  /* Exception class for protocol specific exceptions. */
  template < typename T >
  class exception : public std::exception {

    private:
      /** Exception type identifier */
      T                  identifier;

      /** \brief The optional argument for an error description */
      const std::string  argument;

      /** Description messages for every possible value in T */
      static const char* const descriptions[];

    public:

      /** \brief Constructor */
      template < typename A >
      inline exception(T t, const A a = "");

      /** \brief Constructor */
      inline exception(T t);

      /** \brief A description for the exception */
      inline const char* what() const throw ();

      /** \brief Destructor */
      inline ~exception() throw ();
  };

  /**
   * @param[in] t the type identifier of the exception
   * @param[in] a an argument for the error
   **/
  template < typename T >
  template < typename A >
  inline exception< T >::exception(const T t, const A a) : identifier(t), argument(a) {
  }

  /**
   * @param[in] t the type identifier of the exception
   **/
  template < typename T >
  inline exception< T >::exception(const T t) : identifier(t), argument() {
  }

  template < typename T >
  inline const char* exception< T >::what() const throw () {
    return (boost::str(boost::format(descriptions[identifier]) % argument).c_str());
  }

  template < typename T >
  inline exception< T >::~exception() throw () {
  }
}

#endif

