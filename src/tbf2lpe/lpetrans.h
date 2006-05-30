#include <stdbool.h>
#include "aterm2.h"

ATermAppl translate(ATermAppl spec, bool convert_bools, bool convert_funcs);
//Pre: spec is an mCRL LPE
//Ret: an equivalent mCRL2 LPE, in which sort Bool is converted based on the
//     values of convert_bools and convert_funcs

bool is_mCRL_spec(ATermAppl spec);
//Ret: spec is a mCRL LPE
