#ifndef XML_TEXT_READER_H
#define XML_TEXT_READER_H

#include <iosfwd>
#include <exception>
#include <string>
#include <cstdlib>

namespace xml2pp {

  extern "C" {
    #include <libxml/xmlreader.h>
  }

  /* Macro to convert constant C-type strings to const xmlChar* */
  #define TO_XML_STRING(c_string) reinterpret_cast < const unsigned char* > (c_string)

  class exception : std::exception {
    /* Errors :
     *
     *  - could not open input file
     *  - error loading XML schema file for validation
     */
  };

  /* A C++ wrapper around xmlTextReader from libxml2 */
  class text_reader {
    private:
      xmlTextReaderPtr reader;

    public:

      /** Constructor for reading from a file */
      text_reader(const char*, const char* schema_name = 0);

      /** Constructor for reading from an in memory document */
      text_reader(const std::string& document, const char* schema_name = 0);

      ~text_reader();

      /** Traverses of the XML document tree */
      void read() throw (int);

      /** Returns the name of the current element */
      inline std::string element_name();

      /** Get the value of an attribute as ... (second argument remains unchanged if the attribute is not present) */
      inline bool get_attribute(std::string* string, const char* attribute_name);

      /** Get the value of an attribute as ... (second argument remains unchanged if the attribute is not present) */
      template < typename it >
      inline bool get_attribute(it*, const char*);

      /** Returns whether the attribute is present or not */
      inline bool get_attribute(const char*);

      /** Get the value of an element as ... */
      inline bool get_value(std::string* astring);

      /** Get the value of an element as ... */
      template < typename it >
      inline bool get_value(it* aninteger);

      /** Whether the current element matches element_name */
      inline bool is_element(char* element_name);

      /** Whether the current element is an end of element tag */
      inline bool is_end_element();

      /** Whether the current element is empty */
      inline bool is_empty_element();
  };

  /****************************************************************************
   * Definitions for inline functions follow
   ****************************************************************************/
  inline text_reader::text_reader(const char* file_name, const char* schema_name) {
    reader = xmlNewTextReaderFilename(file_name);

    if (reader == 0) {
      /* Error opening file, abort ... */
      throw (new xml2pp::exception);
    }

#ifdef SCHEMA_VALIDATION_ENABLED
    if (schema_name != 0 && xmlTextReaderSchemaValidate(reader, schema_name) < 0) {
      /* Error schema file, abort ... */
      throw (new xml2pp::exception);
    }
#endif
  }

  inline text_reader::text_reader(const std::string& document, const char* schema_name) {
    reader = xmlReaderForMemory(document.c_str(), document.size(), "", 0, 0);

#ifdef SCHEMA_VALIDATION_ENABLED
    if (schema_name != 0 && xmlTextReaderSchemaValidate(reader, schema_name) < 0) {
      /* Error schema file, abort ... */
      throw (new xml2pp::exception);
    }
#endif
  }

  inline text_reader::~text_reader() {
    xmlTextReaderClose(reader);
    xmlFreeTextReader(reader);
  }

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
