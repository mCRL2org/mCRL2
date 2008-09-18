#ifndef XML_IMPORTER_H
#define XML_IMPORTER_H

#include "importer.h"

class XMLImporter: public Importer
{
  public:
    XMLImporter() {};
    ~XMLImporter() {};
    
    Graph* importFile(std::string file);
};

#endif // XML_IMPORTER_H
