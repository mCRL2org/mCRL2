#ifndef XML2PP_TEXT_READER_TCC
#define XML2PP_TEXT_READER_TCC

#include <xml2pp/text_reader.h>

namespace xml2pp {

  /**
   * @param[in] n the name of a file
   **/
  template < typename T >
  inline text_reader::file_name< T >::file_name(const T& n) : name(n) {
  }

  template < typename T >
  inline const char* text_reader::file_name< T >::get() const {
    return (name);
  }

  template < >
  inline const char* text_reader::file_name< std::string >::get() const {
    return (name.c_str());
  }

  /**
   * @param[in] f the wrapped filename to read the document from that should be parsed
   **/
  template < typename T >
  inline text_reader::text_reader(const file_name< T >& f) throw () {
    reader = xmlReaderForFile(f.get(), "", 0);

    if (reader == 0) {
      throw (exception(exception_identifier::unable_to_open_file, f.get()));
    }
  }

  /**
   * @param d[in] the document that is to be parsed
   **/
  template < typename T >
  inline text_reader::text_reader(const T& d) throw () {
    reader = xmlReaderForDoc(reinterpret_cast < const xmlChar* const > (d), "", 0, 0);

    if (reader == 0) {
      throw (exception(exception_identifier::unable_to_initialise_reader));
    }
  }

  /**
   * @param d[in] the document that is to be parsed
   **/
  template < >
  inline text_reader::text_reader(const std::string& d) throw () {
    reader = xmlReaderForMemory(d.c_str(), d.size(), "", 0, 0);

    if (reader == 0) {
      throw (exception(exception_identifier::unable_to_initialise_reader));
    }
  }

  /**
   * @param d[in] the document that is to be parsed
   **/
  template < typename T >
  inline text_reader::text_reader(const T& d, const size_t prefix_length) throw () {
    reader = xmlReaderForDoc(d, "", 0, 0);

    if (reader == 0) {
      throw (exception(exception_identifier::unable_to_initialise_reader));
    }
  }

  /**
   * @param d[in] the document that is to be parsed
   **/
  template < >
  inline text_reader::text_reader(const std::string& d, const size_t prefix_length) throw () {
    reader = xmlReaderForMemory(d.c_str(), prefix_length, "", 0, 0);

    if (reader == 0) {
      throw (exception(exception_identifier::unable_to_initialise_reader));
    }
  }

  inline text_reader::~text_reader() {
    xmlTextReaderClose(reader);
    xmlFreeTextReader(reader);
  }

#ifdef SCHEMA_VALIDATION_ENABLED
  /** \attention throws if the first read() has already occured */
  template < typename T >
  inline void text_reader::set_schema(const file_name< T >& schema_name) throw () {
    if (xmlTextReaderReadState(reader) != XML_TEXTREADER_MODE_INITIAL) {
      throw (exception(exception::illegal_operation_after_first_read));
    }
    else if (xmlTextReaderSchemaValidate(reader, schema_name.get()) < 0) {
      /* Error schema file, abort ... */
      throw (exception(exception_identifier::unable_to_open_file, f.get()));
    }
  }
#else
  template < typename T >
  inline void text_reader::set_schema(const file_name< T >&) throw () {
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
    char* temporary = (char*) xmlTextReaderGetAttribute(reader, reinterpret_cast < const xmlChar* > (attribute_name));
    bool  return_value = temporary != NULL;

    if (return_value) {
      astring->assign(temporary);
    }

    xmlFree(temporary);

    return (return_value);
  }

  template < typename T >
  inline bool text_reader::get_attribute(T* aninteger, const char* attribute_name) {
    char* temporary    = (char*) xmlTextReaderGetAttribute(reader, reinterpret_cast < const xmlChar* > (attribute_name));
    bool  return_value = temporary != NULL;

    if (return_value) {
      *aninteger = static_cast < T > (atoi(temporary));
    }

    xmlFree(temporary);

    return (return_value);
  }

  inline bool text_reader::get_attribute(const char* attribute_name) {
    char* temporary    = (char*) xmlTextReaderGetAttribute(reader, reinterpret_cast < const xmlChar* > (attribute_name));
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

  template < typename T >
  inline bool text_reader::get_value(T* aninteger) {
    char* temporary    = (char*) xmlTextReaderValue(reader);
    bool  return_value = temporary != NULL;

    *aninteger = static_cast < T > ((return_value) ? atoi(temporary) : 0);

    xmlFree(temporary);

    return (return_value);
  }

  /* Whether the current element matches element_name */
  inline bool text_reader::is_element(char* element_name) {
    xmlChar* temporary    = xmlTextReaderName(reader);
    bool     return_value = xmlStrEqual(temporary, reinterpret_cast < const xmlChar* > (element_name)) != 0;

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

  inline void text_reader::read() throw () {
    int status = xmlTextReaderRead(reader);

    do {
      if (status <= 0) {
        /* Process error, or end of file */
        throw (exception(exception_identifier::error_while_parsing_document));
      }
      else {
        /* Skip white space */
        if (xmlTextReaderNodeType(reader) == XML_READER_TYPE_SIGNIFICANT_WHITESPACE) {
          /* Text is no profile, skip */
          status = xmlTextReaderRead(reader);

          continue;
        }
      }

      break;
    }
    while (1);
  }
}
#endif

