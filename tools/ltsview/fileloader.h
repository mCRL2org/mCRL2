// Author(s): Bas Ploeger and Carst Tankink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file fileloader.h
/// \brief Add your file description here.

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
