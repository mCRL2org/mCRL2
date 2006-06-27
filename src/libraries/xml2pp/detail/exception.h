/*======================================================================
//
// Copyright (c) 2005 2006 TU/e
//
// author     : Jeroen van der Wulp <J.v.d.Wulp@tue.nl>
//
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
//=====================================================================*/

#ifndef XML2PP_EXCEPTION
#define XML2PP_EXCEPTION

#include <exception>

#include <boost/format.hpp>

namespace xml2pp {

  /* Exception class for protocol specific exceptions. */
  class exception : public std::exception {

    public:

      /** The exception type */
      enum type_identifier {
        unable_to_open_file,                ///< \brief Unable to read the specified input file!
        unable_to_initialise_reader,        ///< \brief The libXML reader could not be initialised!
        illegal_operation_after_first_read, ///< \brief Operation not supported after first read!
        error_while_parsing_document,       ///< \brief Unexpected end of file, schema error or parse error!
        end_of_stream                       ///< \brief Tried to read past the end of the stream
      };

    private:

      /** The exception type */
      type_identifier          type;

      /** \brief The optional argument for an error description */
      boost::format            _message;

      /** Description messages for every possible value in T */
      static const char* const descriptions[];

    public:

      /** \brief Constructor */
      inline exception(type_identifier t);

      /** \brief Constructor for messages with one argument. */
      template < typename T1 >
      inline exception(type_identifier, T1);

      /** \brief Constructor for messages with two arguments. */
      template < typename T1, typename T2 >
      inline exception(type_identifier, T1, T2);

      /** \brief Get the type of the exception */
      inline type_identifier get_type() const;

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
  inline exception::exception(const type_identifier t) : type(t), _message(descriptions[t]) {
  }

  /**
   * @param[in] t the type identifier of the exception
   * @param[in] a an argument of which the value should be substituted in the message 
   **/
  template < typename T1 >
  inline exception::exception(const type_identifier t, T1 a) : type(t), _message(descriptions[t]) {
    _message % a;
  }

  /**
   * @param[in] t the type identifier of the exception
   * @param[in] a1 an argument of which the value should be substituted in the message 
   * @param[in] a2 an argument of which the value should be substituted in the message 
   **/
  template < typename T1, typename T2 >
  inline exception::exception(const type_identifier t, T1 a1, T2 a2) : type(t), _message(descriptions[t]) {
    _message % a1 % a2;
  }

  inline exception::type_identifier exception::get_type() const {
    return (type); 
  }

  inline boost::format& exception::message() {
    return (_message); 
  }

  inline const char* exception::what() const throw () {
    return (boost::str(_message).c_str());
  }

  inline exception::~exception() throw () {
  }
}

#endif
