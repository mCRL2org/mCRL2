#ifndef _LIBLTS_BISIM_H
#define _LIBLTS_BISIM_H
#include "lts/liblts.h"

namespace mcrl2
{
namespace lts
{

bool bisimulation_reduce(lts &l, bool branching = false, bool add_class_to_state = false, std::vector<std::string> *tau_actions = NULL);
bool bisimulation_compare(lts &l1, lts &l2, bool branching = false, std::vector<std::string> *tau_actions = NULL);

}
}

#endif
