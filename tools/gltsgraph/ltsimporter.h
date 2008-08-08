#ifndef LTS_IMPORTER_H
#define LTS_IMPORTER_H

#include "importer.h"
#include <string>

class LTSImporter: public Importer
{
  public:
    Graph* importFile(std::string fn);
  
};

#endif //LTS_IMPORTER_H
