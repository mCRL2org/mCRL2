// Author(s): Bas Ploeger and Carst Tankink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING).
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file 
/// \brief Declaration of the FileLoader for loading FSM files

#ifndef FILELOADER_H
#define FILELOADER_H

#include <map>
#include <string>
#include <vector>
#include "lts.h"

namespace ltsview {

class FileLoader {
  public:
    FileLoader(LTS* _lts);
    // Global variables
    std::string filename;                /* Name of input file */
    LTS* lts;                      /* Points to LTS structure */
    unsigned int par_index;              /* Current parameter index */
    unsigned int num_pars;               /* Number of specified parameters */
    std::string par_name;                /* Name of current parameter */
    std::string par_type;                /* Type of current parameter */ 
    std::vector<std::string> par_values; /* Value domain of current parameter */
    std::vector<bool> ignore_par;        /* Records which parameters are ignored */
    int state_id;                        /* Current state index */
    std::vector<int> state_vector;       /* State vector of current state */
    std::map<std::string,int> labels;    /* Records index for every label string */

    // Function declarations
    void error(const class location& l, const std::string& m);
    void error(const std::string& m);
    bool parse_file(const std::string& _filename);
    class LTSViewFSMLexer* lexer;
};

}

#endif // FILELOADER_H
