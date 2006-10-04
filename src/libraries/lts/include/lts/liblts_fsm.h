#ifndef __LIBLTS_FSM_H
#define __LIBLTS_FSM_H

#include <string>
#include <iostream>
#include <aterm2.h>
#include "liblts.h"
#include "liblts_fsm.h"
#include "lpe/lpe.h"

namespace mcrl2
{
namespace lts
{

bool write_lts_to_fsm(lts &l, std::string const& filename, ATerm lpe = NULL);
bool write_lts_to_fsm(lts &l, std::string const& filename, lpe::LPE &lpe);
bool write_lts_to_fsm(lts &l, std::ostream& os, ATerm lpe = NULL);
bool write_lts_to_fsm(lts &l, std::ostream& os, lpe::LPE &lpe);

}
}

#endif
