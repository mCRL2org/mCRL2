#ifndef XML2PP_TEXT_READER_TCC
#define XML2PP_TEXT_READER_TCC

#include <cstring>
#include <cstdio>

#include <xml2pp/text_reader.h>

namespace xml2pp {

  extern "C" {
    #include <libxml/xmlreader.h>
  }

  /** \brief Helper class for wrapping the reader to contain libxml2 dependencies */
  class reader_wrapper {

    private:

      /** \brief Pointer to the instance of the XML text reader */
      xmlTextReaderPtr reader;

      /** \brief For libxml2 initialisation in multi-threaded applications*/
      static bool      initialised;

      /** \brief Whether the end-of-stream was reached */
      bool             past_end_of_stream;

    private:

      /** \brief Does some libxml initialisation */
      static bool initialise();

    public:

      /** \brief Constructor */
      inline reader_wrapper(xmlTextReaderPtr r);

      /** \brief Set schema for validation purposes */
      inline void set_schema(boost::filesystem::path const& n) throw ();

      /** \brief Proceeds to the next element (optionally skip a few of them) */
      inline void next_element(unsigned int n = 0) throw ();

      /** \brief Returns the value of a named attribute attribute as a string */
      inline std::string get_attribute_as_string(const char* n) const;

      /** \brief Get the value of an element as a string */
      inline std::string get_value_as_string() const;

      /** \brief Returns the name of the current element */
      inline std::string element_name() const;

      /** \brief Whether the current element matches element_name */
      inline bool is_element(char const* e) const;

      /** \brief Whether the current element is an end of element tag */
      inline bool is_end_element() const;

      /** \brief Whether the current element is empty, or not */
      inline bool is_empty_element() const;

      /** \brief Whether the reader currently points to a valid element */
      inline bool valid() const;

      /** \brief Destructor */
      ~reader_wrapper();
  };

  bool reader_wrapper::initialised = reader_wrapper::initialise();

  inline bool reader_wrapper::initialise() {
    xmlInitParser();

    return (true);
  }

  inline reader_wrapper::reader_wrapper(xmlTextReaderPtr r) : reader(r) {
    if (r == 0) {
      throw (exception::exception(exception::unable_to_initialise_reader));
    }
    else {
      /* Point to first element */
      past_end_of_stream = false;

      next_element();
    }
  }

  inline void reader_wrapper::set_schema(boost::filesystem::path const& n) throw () {
#ifdef SCHEMA_VALIDATION_ENABLED
    if (xmlTextReaderReadState(reader) != XML_TEXTREADER_MODE_INITIAL) {
      throw (exception::exception(exception::illegal_operation_after_first_read));
    }
    else if (xmlTextReaderSchemaValidate(reader, n.native_file_string().c_str()) < 0) {
      /* Error schema file, abort ... */
      throw (exception::exception(exception::unable_to_open_file, n.native_file_string()));
    }
#endif
  }

  /**
   * @param[in] n the number times to skip a succesful read
   **/
  inline void reader_wrapper::next_element(unsigned int n) throw () {
    int status = xmlTextReaderRead(reader);

    do {
      if (status <= 0) {
        /* Process error, or end of file */
        if (status < 0) {
          throw (exception::exception(exception::error_while_parsing_document));
        }
        else {
          past_end_of_stream = true;

          break;
        }
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

  /**
   * @param[in] n the name of the attribute which value to get
   *
   * \attention the result is undefined if no attribute with that name is found
   **/
  inline std::string reader_wrapper::get_attribute_as_string(const char* n) const {
    std::string return_value;

    char* temporary = (char*) xmlTextReaderGetAttribute(reader, reinterpret_cast < const xmlChar* > (n));

    if (temporary != 0) {
      return_value.append(temporary);

      xmlFree(temporary);
    }

    return (return_value);
  }

  inline std::string reader_wrapper::get_value_as_string() const {
    std::string return_value;

    if (!xmlTextReaderIsEmptyElement(reader)) {
      char const* temporary = reinterpret_cast < char const* > (xmlTextReaderConstValue(reader));

      if (temporary != 0) {
        return_value = temporary;
      }
    }

    return (return_value);
  }

  /**
   * \pre valid() must be true
   **/
  inline std::string reader_wrapper::element_name() const {
    std::string temporary = (char*) xmlTextReaderConstLocalName(reader);

    return (temporary);
  }

  /**
   * @param[out] e the name of an element
   *
   * \return whether the name of the current element matches e
   **/
  inline bool reader_wrapper::is_element(char const* e) const {
    return (strcmp(reinterpret_cast < char const* > (xmlTextReaderConstLocalName(reader)), e) == 0);
  }

  inline bool reader_wrapper::is_end_element() const {
    return (xmlTextReaderNodeType(reader) == XML_READER_TYPE_END_ELEMENT);
  }

  inline bool reader_wrapper::is_empty_element() const {
    return (xmlTextReaderIsEmptyElement(reader));
  }

  inline bool reader_wrapper::valid() const {
    return (!past_end_of_stream);
  }

  reader_wrapper::~reader_wrapper() {
    xmlTextReaderClose(reader);

    xmlFreeTextReader(reader);
  }

  text_reader::text_reader(boost::filesystem::path const& p) {
    wrapped_reader.reset(new reader_wrapper(xmlReaderForFile(p.native_file_string().c_str(), "", 0)));
  }

  /**
   * @param[in] d the document that is to be parsed
   **/
  text_reader::text_reader(const char* d) {
    wrapped_reader.reset(new reader_wrapper(xmlReaderForDoc(reinterpret_cast < xmlChar const* > (d), "", 0, 0)));
  }

  /**
   * @param[in] d the document that is to be parsed
   * @param[in] l the length of a prefix of d that is to be taken as the document
   **/
  text_reader::text_reader(const char* d, const size_t l) {
    wrapped_reader.reset(new reader_wrapper(xmlReaderForDoc(reinterpret_cast < xmlChar const* > (d), "", 0, 0)));
  }

  /**
   * @param[in] d the document that is to be parsed
   **/
  text_reader::text_reader(std::string const& d) {
    wrapped_reader.reset(new reader_wrapper(xmlReaderForMemory(d.c_str(), d.size(), "", 0, 0)));
  }

  /**
   * @param[in] d the document that is to be parsed
   * @param[in] l the length of a prefix of d that is to be taken as the document
   **/
  text_reader::text_reader(std::string const& d, const size_t l) {
    wrapped_reader.reset(new reader_wrapper(xmlReaderForMemory(d.c_str(), l, "", 0, 0)));
  }

#ifdef SCHEMA_VALIDATION_ENABLED
  /**
   * @param[in] m the path to the schema file
   * \attention throws if the first read() has already occured
   **/
  void text_reader::set_schema(boost::filesystem::path const& n) {
    wrapped_reader->set_schema();
  }
#else
  void text_reader::set_schema(boost::filesystem::path const&) {
  }
#endif

  /**
   * @param[in] n the number times to skip a succesful read
   **/
  void text_reader::next_element(unsigned int n) {
    wrapped_reader->next_element();
  }

  /**
   * @param[in] n the name of the attribute which value to get
   *
   * \attention the result is undefined if no attribute with that name is found
   **/
  std::string text_reader::get_attribute_as_string(const char* n) const {
    return (wrapped_reader->get_attribute_as_string(n));
  }

  /**
   * @param[in] n the name of the attribute which value to get
   *
   * \return the value as a boolean
   **/
  bool text_reader::get_attribute(const char* n) {
    std::string v = get_attribute_as_string(n);

    if (!v.empty()) {
      if (v == "true" || v == "1" || v == "yes") {
        return (true);
      }
    }

    return (false);
  }

  /**
   * @param[out] d the variable that should hold the result 
   * @param[in] n the name of the attribute which value to get
   *
   * \return whether the attribute was present
   **/
  bool text_reader::get_attribute(std::string* d, const char* n) {
    d->assign(get_attribute_as_string(n));

    return (!d->empty());
  }

  /**
   * @param[out] d the variable that should hold the result 
   * @param[in] n the name of the attribute which value to get
   *
   * \return whether the attribute was present
   **/
  bool text_reader::get_attribute(char* d, const char* n) {
    return (0 < sscanf(get_attribute_as_string(n).c_str(), "%c", d));
  }

  /**
   * @param[out] d the variable that should hold the result 
   * @param[in] n the name of the attribute which value to get
   *
   * \return whether the attribute was present
   **/
  bool text_reader::get_attribute(unsigned char* d, const char* n) {
    return (0 < sscanf(get_attribute_as_string(n).c_str(), "%c", d));
  }

  /**
   * @param[out] d the variable that should hold the result 
   * @param[in] n the name of the attribute which value to get
   *
   * \return whether the attribute was present
   **/
  bool text_reader::get_attribute(short int* d, const char* n) {
    return (0 < sscanf(get_attribute_as_string(n).c_str(), "%hd", d));
  }

  /**
   * @param[out] d the variable that should hold the result 
   * @param[in] n the name of the attribute which value to get
   *
   * \return whether the attribute was present
   **/
  bool text_reader::get_attribute(short unsigned int* d, const char* n) {
    return (0 < sscanf(get_attribute_as_string(n).c_str(), "%hu", d));
  }

  /**
   * @param[out] d the variable that should hold the result 
   * @param[in] n the name of the attribute which value to get
   *
   * \return whether the attribute was present
   **/
  bool text_reader::get_attribute(int* d, const char* n) {
    return (0 < sscanf(get_attribute_as_string(n).c_str(), "%d", d));
  }

  /**
   * @param[out] d the variable that should hold the result 
   * @param[in] n the name of the attribute which value to get
   *
   * \return whether the attribute was present
   **/
  bool text_reader::get_attribute(unsigned int* d, const char* n) {
    return (0 < sscanf(get_attribute_as_string(n).c_str(), "%u", d));
  }

  /**
   * @param[out] d the variable that should hold the result 
   * @param[in] n the name of the attribute which value to get
   *
   * \return whether the attribute was present
   **/
  bool text_reader::get_attribute(long int* d, const char* n) {
    return (0 < sscanf(get_attribute_as_string(n).c_str(), "%ld", d));
  }

  /**
   * @param[out] d the variable that should hold the result 
   * @param[in] n the name of the attribute which value to get
   *
   * \return whether the attribute was present
   **/
  bool text_reader::get_attribute(long unsigned int* d, const char* n) {
    return (0 < sscanf(get_attribute_as_string(n).c_str(), "%lu", d));
  }

  /**
   * @param[out] d the variable that should hold the result 
   * @param[in] n the name of the attribute which value to get
   *
   * \return whether the attribute was present
   **/
  bool text_reader::get_attribute(float* d, const char* n) {
    return (0 < sscanf(get_attribute_as_string(n).c_str(), "%f", d));
  }

  /**
   * @param[out] d the variable that should hold the result 
   * @param[in] n the name of the attribute which value to get
   *
   * \return whether the attribute was present
   **/
  bool text_reader::get_attribute(double* d, const char* n) {
    return (0 < sscanf(get_attribute_as_string(n).c_str(), "%lf", d));
  }

  std::string text_reader::get_value_as_string() const {
    return (wrapped_reader->get_value_as_string());
  }

  /**
   * @param[out] d the variable that should hold the result 
   **/
  void text_reader::get_value(std::string* d) const {
    d->assign(wrapped_reader->get_value_as_string());
  }

  /**
   * @param[out] d the variable that should hold the result 
   **/
  void text_reader::get_value(char* d) const {
    sscanf(wrapped_reader->get_value_as_string().c_str(), "%c", d);
  }

  /**
   * @param[out] d the variable that should hold the result 
   **/
  void text_reader::get_value(unsigned char* d) const {
    sscanf(wrapped_reader->get_value_as_string().c_str(), "%c", d);
  }

  /**
   * @param[out] d the variable that should hold the result 
   **/
  void text_reader::get_value(short int* d) const {
    sscanf(wrapped_reader->get_value_as_string().c_str(), "%hd", d);
  }

  /**
   * @param[out] d the variable that should hold the result 
   **/
  void text_reader::get_value(short unsigned int* d) const {
    sscanf(wrapped_reader->get_value_as_string().c_str(), "%hu", d);
  }

  /**
   * @param[out] d the variable that should hold the result 
   **/
  void text_reader::get_value(int* d) const {
    sscanf(wrapped_reader->get_value_as_string().c_str(), "%d", d);
  }

  /**
   * @param[out] d the variable that should hold the result 
   **/
  void text_reader::get_value(unsigned int* d) const {
    sscanf(wrapped_reader->get_value_as_string().c_str(), "%u", d);
  }

  /**
   * @param[out] d the variable that should hold the result 
   **/
  void text_reader::get_value(long int* d) const {
    sscanf(wrapped_reader->get_value_as_string().c_str(), "%ld", d);
  }

  /**
   * @param[out] d the variable that should hold the result 
   **/
  void text_reader::get_value(long unsigned int* d) const {
    sscanf(wrapped_reader->get_value_as_string().c_str(), "%lu", d);
  }

  /**
   * @param[out] d the variable that should hold the result 
   **/
  void text_reader::get_value(float* d) const {
    sscanf(wrapped_reader->get_value_as_string().c_str(), "%f", d);
  }

  /**
   * @param[out] d the variable that should hold the result 
   **/
  void text_reader::get_value(double* d) const {
    sscanf(wrapped_reader->get_value_as_string().c_str(), "%lf", d);
  }

  /**
   * \pre valid() must be true
   **/
  std::string text_reader::element_name() const {
    return (wrapped_reader->element_name());
  }

  /**
   * @param[out] e the name of an element
   *
   * \return whether the name of the current element matches e
   **/
  bool text_reader::is_element(const char* e) const {
    return (wrapped_reader->is_element(e));
  }

  bool text_reader::is_end_element() const {
    return (wrapped_reader->is_end_element());
  }

  /**
   * @param e the end element that has to be matched
   *
   * \return whether the position was changed
   **/
  bool text_reader::is_end_element(const char* e) const {
    return (wrapped_reader->is_element(e) && wrapped_reader->is_end_element());
  }

  /**
   * @param e the end element that has to be matched
   *
   * \return whether the position was changed
   **/
  bool text_reader::skip_end_element(const char* e) {
    if (wrapped_reader->is_element(e) && wrapped_reader->is_end_element()) {

      wrapped_reader->next_element();

      return (true);
    }

    return (false);
  }

  /* Whether the current element is empty */
  bool text_reader::is_empty_element() const {
    return (wrapped_reader->is_empty_element());
  }

  bool text_reader::valid() const {
    return (wrapped_reader->valid());
  }
}
#endif

