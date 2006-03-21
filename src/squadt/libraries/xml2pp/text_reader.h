#ifndef XML2PP_TEXT_READER_H
#define XML2PP_TEXT_READER_H

#include <iosfwd>
#include <exception>
#include <string>
#include <cstdlib>
#include <iostream>

#include <boost/noncopyable.hpp>

#include <xml2pp/detail/exception.h>

namespace xml2pp {

  extern "C" {
    #include <libxml/xmlreader.h>
  }

  /** \brief A C++ wrapper around xmlTextReader from libxml2 */
  class text_reader : public boost::noncopyable {
    private:
      /** \brief The XML text reader from libxml2 */
      xmlTextReaderPtr   reader;

    public:

      /** \brief Wrapper class to allow distinction between input strings and file names */
      template < typename T >
      class file_name {
        /** \brief The name of the file */
        const T& name;

        public:
          /** \brief Constructor */
          inline file_name(const T& n);

          /** \brief Returns a C string that represents the file name */
          inline const char* get() const;
      };

      /** \brief Constructor for reading from a file */
      template < typename T >
      text_reader(const file_name< T >&) throw ();

      /** \brief Constructor for reading an in memory document */
      template < typename T >
      text_reader(const T& document) throw ();

      /** \brief Constructor for reading part of an in memory document */
      template < typename T >
      text_reader(const T& document, const size_t prefix_length) throw ();

      /** \brief descructor */
      ~text_reader();

      /** \brief Set schema for validation purposes */
      template < typename T >
      inline void set_schema(const file_name< T >&) throw ();

      /** \brief Traverses of the XML document tree */
      inline void read(unsigned int = 0) throw ();

      /** \brief Returns the name of the current element */
      inline std::string element_name();

      /** \brief Get the value of an attribute as ... (second argument remains unchanged if the attribute is not present) */
      inline bool get_attribute(std::string* string, const char* attribute_name);

      /** \brief Get the value of an attribute as ... (second argument remains unchanged if the attribute is not present) */
      template < typename T >
      inline bool get_attribute(T*, const char*);

      /** \brief Returns whether the attribute is present or not */
      inline bool get_attribute(const char*);

      /** \brief Get the value of an element as ... */
      inline bool get_value(std::string*);

      /** \brief Get the value of an element as ... */
      template < typename T >
      inline bool get_value(T*);

      /** \brief Whether the current element matches element_name */
      inline bool is_element(const char*);

      /** \brief Whether the current element is an end of element tag */
      inline bool is_end_element();

      /** \brief Skips to the next position if the current element is an end of element tag */
      inline bool skip_end_element(const char*);

      /** \brief Whether the current element is empty */
      inline bool is_empty_element();
  };
}

#endif
