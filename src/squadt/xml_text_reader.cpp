#include "xml_text_reader.h"

XMLTextReader::XMLTextReader(const char* file_name) {
  reader = xmlNewTextReaderFilename(file_name);
}

#if defined(PARSER_SCHEMA_VALIDATION)
bool XMLTextReader::SetSchemaForValidation(const char* file_name) {
  return (0 <= xmlTextReaderSchemaValidate(reader, file_name));
}
#endif

void XMLTextReader::Close() {
  xmlTextReaderClose(reader);
}

void XMLTextReader::Destroy() {
  xmlFreeTextReader(reader);
}

void XMLTextReader::Read() throw (int) {
  int status = xmlTextReaderRead(reader);

  do {
    if (status <= 0) {
      /* Process error, or end of file */
      throw (status);
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

