#ifndef __LIBLTS_DOT_H
#define __LIBLTS_DOT_H

#include <string>
#include <ostream>
#include "liblts.h"

namespace mcrl2
{
namespace lts
{

bool write_lts_to_dot(lts &l, std::ostream &os, std::string &name, bool print_states);
bool write_lts_to_dot(lts &l, std::string &filename, std::string &name, bool print_states);

}
}

#endif
