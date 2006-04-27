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
   * @param[in] d the document that is to be parsed
   **/
  template < typename T >
  inline text_reader::text_reader(const T& d) throw () {
    reader = xmlReaderForDoc(reinterpret_cast < const xmlChar* const > (d), "", 0, 0);

    if (reader == 0) {
      throw (exception(exception_identifier::unable_to_initialise_reader));
    }
  }

  /**
   * @param[in] d the document that is to be parsed
   **/
  template < >
  inline text_reader::text_reader(const std::string& d) throw () {
    reader = xmlReaderForMemory(d.c_str(), d.size(), "", 0, 0);

    if (reader == 0) {
      throw (exception(exception_identifier::unable_to_initialise_reader));
    }
  }

  /**
   * @param[in] d the document that is to be parsed
   * @param[in] l the length of a prefix of d that is to be taken as the document
   **/
  template < typename T >
  inline text_reader::text_reader(const T& d, const size_t l) throw () {
    reader = xmlReaderForDoc(d, "", 0, 0);

    if (reader == 0) {
      throw (exception(exception_identifier::unable_to_initialise_reader));
    }
  }

  /**
   * @param[in] d the document that is to be parsed
   * @param[in] l the length of a prefix of d that is to be taken as the document
   **/
  template < >
  inline text_reader::text_reader(const std::string& d, const size_t l) throw () {
    reader = xmlReaderForMemory(d.c_str(), l, "", 0, 0);

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

  /**
   * @param[out] d the variable that should hold the result 
   * @param[in] n the name of the attribute which value to get
   *
   * \return whether the attribute was present
   **/
  inline bool text_reader::get_attribute(std::string* d, const char* n) {
    char* temporary = (char*) xmlTextReaderGetAttribute(reader, reinterpret_cast < const xmlChar* > (n));
    bool  return_value = temporary != NULL;

    if (return_value) {
      d->assign(temporary);
    }

    xmlFree(temporary);

    return (return_value);
  }

  /**
   * @param[out] d the variable that should hold the result 
   * @param[in] n the name of the attribute which value to get
   *
   * \return whether the attribute was present
   **/
  template < typename T >
  inline bool text_reader::get_attribute(T* d, const char* n) {
    char* temporary    = (char*) xmlTextReaderGetAttribute(reader, reinterpret_cast < const xmlChar* > (n));
    bool  return_value = temporary != NULL;

    if (return_value) {
      *d= static_cast < T > (atol(temporary));
    }

    xmlFree(temporary);

    return (return_value);
  }

  /**
   * @param[in] n the name of the attribute which value to get
   *
   * \return the value as a boolean
   **/
  inline bool text_reader::get_attribute(const char* n) {
    char* temporary    = (char*) xmlTextReaderGetAttribute(reader, reinterpret_cast < const xmlChar* > (n));
    bool  return_value = temporary != NULL;

    xmlFree(temporary);

    return (return_value);
  }

  /**
   * @param[out] d the variable that should hold the result 
   *
   * \return whether the node is empty
   **/
  inline bool text_reader::get_value(std::string* d) {
    char* temporary    = (char*) xmlTextReaderValue(reader);
    bool  return_value = temporary != NULL;

    *d = (return_value) ? std::string(temporary) : "";

    xmlFree(temporary);

    return (return_value);
  }

  /**
   * @param[out] d the variable that should hold the result 
   *
   * \return whether the node is empty
   **/
  template < typename T >
  inline bool text_reader::get_value(T* d) {
    char* temporary    = (char*) xmlTextReaderValue(reader);
    bool  return_value = temporary != NULL;

    *d = static_cast < T > ((return_value) ? atoi(temporary) : 0);

    xmlFree(temporary);

    return (return_value);
  }

  /**
   * @param[out] e the name of an element
   *
   * \return whether the name of the current element matches e
   **/
  inline bool text_reader::is_element(const char* e) {
    xmlChar* temporary    = xmlTextReaderName(reader);
    bool     return_value = xmlStrEqual(temporary, reinterpret_cast < const xmlChar* > (e)) != 0;

    xmlFree(temporary);

    return (return_value);
  }

  inline bool text_reader::is_end_element() {
    return (xmlTextReaderNodeType(reader) == XML_READER_TYPE_END_ELEMENT);
  }

  /**
   * @param e the end element that has to be matched
   *
   * \return whether the position was changed
   **/
  inline bool text_reader::is_end_element(const char* e) {
    return (is_element(e) && xmlTextReaderNodeType(reader) == XML_READER_TYPE_END_ELEMENT);
  }

  /**
   * @param e the end element that has to be matched
   *
   * \return whether the position was changed
   **/
  inline bool text_reader::skip_end_element(const char* e) {
    if (xmlTextReaderNodeType(reader) == XML_READER_TYPE_END_ELEMENT && is_element(e)) {
      read();

      return (true);
    }

    return (false);
  }

  /* Whether the current element is empty */
  inline bool text_reader::is_empty_element() {
    return (xmlTextReaderIsEmptyElement(reader));
  }

  /**
   * @param[in] n the number times to skip a succesful read
   **/
  inline void text_reader::read(unsigned int n) throw () {
    int status = xmlTextReaderRead(reader);

    do {
      if (status <= 0) {
        /* Process error, or end of file */
        throw (exception(exception_identifier::error_while_parsing_document));
      }
      else if (xmlTextReaderNodeType(reader) == XML_READER_TYPE_SIGNIFICANT_WHITESPACE) {
        /* Skip whitespace */
        status = xmlTextReaderRead(reader);
      }
      else if (1 < n) {
        status = xmlTextReaderRead(reader);

        --n;
      }
      else {
        break;
      }
    }
    while (1);
  }
}
#endif

