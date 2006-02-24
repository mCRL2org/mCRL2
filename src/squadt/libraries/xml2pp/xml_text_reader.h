#ifndef XML2PP_TEXT_READER_H
#define XML2PP_TEXT_READER_H

#include <iosfwd>
#include <exception>
#include <string>
#include <cstdlib>
#include <iostream>

#include <xml2pp/detail/exception.h>

#ifdef SCHEMA_VALIDATION_ENABLED
 #define XML2PP_SCHEMA(x) , x
#else
 #define XML2PP_SCHEMA(x)
#endif

namespace xml2pp {

  extern "C" {
    #include <libxml/xmlreader.h>
  }

  /* Macro to convert constant C-type strings to const xmlChar* */
  #define TO_XML_STRING(c_string) reinterpret_cast < const unsigned char* > (c_string)

  /* A C++ wrapper around xmlTextReader from libxml2 */
  class text_reader {
    private:
      /** The XML text reader from libxml2 */
      xmlTextReaderPtr   reader;

      const std::string& _document;

    public:

      /** \brief Constructor for reading from a file */
      text_reader(const char*, const char* schema_name = 0);

      /** \brief Constructor for reading an in memory document */
      text_reader(const std::string& document, const char* schema_name = 0);

      /** \brief Constructor for reading part of an in memory document */
      text_reader(const std::string& document, const size_t prefix_length, const char* schema_name = 0);

      /** \brief descructor */
      ~text_reader();

      /** \brief Set schema for validation purposes */
      inline void set_schema(const char*);

      /** \brief Traverses of the XML document tree */
      void read() throw ();

      /** \brief Returns the name of the current element */
      inline std::string element_name();

      /** \brief Get the value of an attribute as ... (second argument remains unchanged if the attribute is not present) */
      inline bool get_attribute(std::string* string, const char* attribute_name);

      /** \brief Get the value of an attribute as ... (second argument remains unchanged if the attribute is not present) */
      template < typename it >
      inline bool get_attribute(it*, const char*);

      /** \brief Returns whether the attribute is present or not */
      inline bool get_attribute(const char*);

      /** \brief Get the value of an element as ... */
      inline bool get_value(std::string* astring);

      /** \brief Get the value of an element as ... */
      template < typename it >
      inline bool get_value(it* aninteger);

      /** \brief Whether the current element matches element_name */
      inline bool is_element(char* element_name);

      /** \brief Whether the current element is an end of element tag */
      inline bool is_end_element();

      /** \brief Whether the current element is empty */
      inline bool is_empty_element();
  };

  /****************************************************************************
   * Definitions for inline functions follow
   ****************************************************************************/
  inline text_reader::text_reader(const char* file_name, const char* schema_name) : _document(0) {
std::cerr << "died on exception\n";
    reader = xmlNewTextReaderFilename(file_name);

    if (reader == 0) {
      /* Error opening file, abort ... */
      throw (xml2pp::exception(exception::unable_to_open_input_file));
    }

    set_schema(schema_name);
  }

  inline text_reader::text_reader(const std::string& document, const char* schema_name) : _document(document) {
    reader = xmlReaderForMemory(document.c_str(), document.size(), "", 0, 0);

    set_schema(schema_name);
  }

  inline text_reader::text_reader(const std::string& document, const size_t prefix_length, const char* schema_name) : _document(document) {
    reader = xmlReaderForMemory(document.c_str(), prefix_length, "", 0, 0);

    set_schema(schema_name);
  }

  inline text_reader::~text_reader() {
    xmlTextReaderClose(reader);
    xmlFreeTextReader(reader);
  }

#ifdef SCHEMA_VALIDATION_ENABLED
  /** \attention throws if the first read() has already occured */
  inline void text_reader::set_schema(const char* schema_name) {
    if (xmlTextReaderReadState(reader) != XML_TEXTREADER_MODE_INITIAL) {
      throw (exception(illegal_operation_after_first_read));
    }
    else if (schema_name != 0 && xmlTextReaderSchemaValidate(reader, schema_name) < 0) {
      /* Error schema file, abort ... */
      throw (xml2pp::exception(unable_to_open_schema_file));
    }
  }
#else
  inline void text_reader::set_schema(const char*) {
  }
#endif

  /* Returns the name of the current element */
  inline std::string text_reader::element_name() {
    char*       temporary = (char*) xmlTextReaderName(reader);
    std::string name      = std::string(temporary);

    xmlFree(temporary);

    return (name);
  }

  /* Get the value of an attribute as ... */
  inline bool text_reader::get_attribute(std::string* astring, const char* attribute_name) {
    char* temporary = (char*) xmlTextReaderGetAttribute(reader, TO_XML_STRING(attribute_name));
    bool  return_value = temporary != NULL;

    *astring = (return_value) ? std::string(temporary) : "";

    xmlFree(temporary);

    return (return_value);
  }

  template < typename it >
  inline bool text_reader::get_attribute(it* aninteger, const char* attribute_name) {
    char* temporary    = (char*) xmlTextReaderGetAttribute(reader, TO_XML_STRING(attribute_name));
    bool  return_value = temporary != NULL;

    *aninteger = static_cast < it > ((return_value) ? atoi(temporary) : 0);

    xmlFree(temporary);

    return (return_value);
  }

  inline bool text_reader::get_attribute(const char* attribute_name) {
    char* temporary    = (char*) xmlTextReaderGetAttribute(reader, TO_XML_STRING(attribute_name));
    bool  return_value = temporary != NULL;

    xmlFree(temporary);

    return (return_value);
  }

  /* Get the value of an attribute as ... */
  inline bool text_reader::get_value(std::string* astring) {
    char* temporary    = (char*) xmlTextReaderValue(reader);
    bool  return_value = temporary != NULL;

    *astring = (return_value) ? std::string(temporary) : "";

    xmlFree(temporary);

    return (return_value);
  }

  template < typename it >
  inline bool text_reader::get_value(it* aninteger) {
    char* temporary    = (char*) xmlTextReaderValue(reader);
    bool  return_value = temporary != NULL;

    *aninteger = static_cast < it > ((return_value) ? atoi(temporary) : 0);

    xmlFree(temporary);

    return (return_value);
  }

  /* Whether the current element matches element_name */
  inline bool text_reader::is_element(char* element_name) {
    xmlChar* temporary    = xmlTextReaderName(reader);
    bool     return_value = xmlStrEqual(temporary, TO_XML_STRING(element_name)) != 0;

    xmlFree(temporary);

    return (return_value);
  }

  /* Whether the current element is an end of element tag */
  inline bool text_reader::is_end_element() {
    return (xmlTextReaderNodeType(reader) == XML_READER_TYPE_END_ELEMENT);
  }

  /* Whether the current element is empty */
  inline bool text_reader::is_empty_element() {
    return (xmlTextReaderIsEmptyElement(reader));
  }
}

#endif
