#ifndef XML_TEXT_READER_H
#define XML_TEXT_READER_H

#include <iostream>
#include <stdlib.h>

extern "C" {
 #include <libxml/xmlreader.h>
}

/* Macro to convert constant C-type strings to const xmlChar* */
#define TO_XML_STRING(c_string) reinterpret_cast < const unsigned char* > (c_string)

/* A C++ wrapper around xmlTextReader from libxml2 */
class XMLTextReader {
  private:
    xmlTextReaderPtr reader;

  public:

    XMLTextReader(const char* file_name);

    void Close();

    void Destroy();

    /* Traverses of the XML document tree */
    void Read() throw (int);

#if defined(PARSER_SCHEMA_VALIDATION)
    bool SetSchemaForValidation(const char* file_name);
#endif

    /* Returns the name of the current element */
    inline std::string ElementName();

    /* Get the value of an attribute as ... */
    inline bool GetAttribute(std::string* string, char* attribute_name);
    inline bool GetAttribute(unsigned int* integer, char* attribute_name);

    /* Get the value of an element as ... */
    inline bool GetValue(std::string* astring);
    inline bool GetValue(unsigned int* aninteger);

    /* Whether the current element matches element_name */
    inline bool IsElement(char* element_name);

    /* Whether the current element is an end of element tag */
    inline bool IsEndElement();

    /* Whether the current element is empty */
    inline bool IsEmptyElement();

    /* Stream is opened */
    inline bool StreamIsOpened();
};

/****************************************************************************
 * Definitions for inline functions follow
 ****************************************************************************/

/* Returns the name of the current element */
inline std::string XMLTextReader::ElementName() {
  char*       temporary = (char*) xmlTextReaderName(reader);
  std::string name      = std::string(temporary);

  xmlFree(temporary);

  return (name);
}

/* Get the value of an attribute as ... */
inline bool XMLTextReader::GetAttribute(std::string* astring, char* attribute_name) {
  char* temporary = (char*) xmlTextReaderGetAttribute(reader, TO_XML_STRING(attribute_name));
  bool  return_value = temporary != NULL;

  if (return_value) {
    *astring = std::string(temporary);
  }

  xmlFree(temporary);

  return (return_value);
}

inline bool XMLTextReader::GetAttribute(unsigned int* aninteger, char* attribute_name) {
  char* temporary    = (char*) xmlTextReaderGetAttribute(reader, TO_XML_STRING(attribute_name));
  bool  return_value = temporary != NULL;

  if (return_value) {
    *aninteger = atoi(temporary);
  }

  xmlFree(temporary);

  return (return_value);
}

/* Get the value of an attribute as ... */
inline bool XMLTextReader::GetValue(std::string* astring) {
  char* temporary    = (char*) xmlTextReaderValue(reader);
  bool  return_value = temporary != NULL;

  if (return_value) {
    *astring = std::string(temporary);
  }

  xmlFree(temporary);

  return (return_value);
}

inline bool XMLTextReader::GetValue(unsigned int* aninteger) {
  char* temporary    = (char*) xmlTextReaderValue(reader);
  bool  return_value = temporary != NULL;

  if (return_value) {
    *aninteger = atoi(temporary);
  }

  xmlFree(temporary);

  return (return_value);
}

/* Whether the current element matches element_name */
inline bool XMLTextReader::IsElement(char* element_name) {
  xmlChar* temporary    = xmlTextReaderName(reader);
  bool     return_value = xmlStrEqual(temporary, TO_XML_STRING(element_name)) != 0;

  xmlFree(temporary);

  return (return_value);
}

/* Whether the current element is an end of element tag */
inline bool XMLTextReader::IsEndElement() {
  return (xmlTextReaderNodeType(reader) == XML_READER_TYPE_END_ELEMENT);
}

/* Whether the current element is empty */
inline bool XMLTextReader::IsEmptyElement() {
  return (xmlTextReaderIsEmptyElement(reader));
}

/* Whether reader is a valid pointer to an XMLTextReader object */
inline bool XMLTextReader::StreamIsOpened() {
  return (reader != 0);
}

#endif
