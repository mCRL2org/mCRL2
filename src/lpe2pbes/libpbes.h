#include <aterm1.h>
#include "lpe/specification.h"

ATermAppl create_pbes(ATermAppl state_frm, lpe::specification lpe_spec);
//Pre: state_frm is a state formula that adheres to the internal format after
//     regular formula translation
//     lpe_spec represents an LPE
//Ret: a PBES resulting from combining state_frm and lpe_spec
