#ifndef IMPORTER_H
#define IMPORTER_H

#include <string>
#include "graph.h"

class Importer
{
  public:
    Importer();
    virtual ~Importer();
    virtual Graph* importFile(std::string file) = 0;

};

#endif //IMPORTER_H
