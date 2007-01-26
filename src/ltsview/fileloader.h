#ifndef FILELOADER_H
#define FILELOADER_H

#include <string>
#include "lts.h"

class FileLoader {
  public:
    static void loadFile(const std::string fileName,LTS* const lts);
  private:
    static void loadFSMFile(const std::string fileName,LTS* const lts);
};

#endif // LOADER_H
