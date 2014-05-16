// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/rewriters/enumerate_quantifiers_rewriter.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_REWRITERS_ENUMERATE_QUANTIFIERS_REWRITER_H
#define MCRL2_PBES_REWRITERS_ENUMERATE_QUANTIFIERS_REWRITER_H

#ifdef MCRL2_USE_ALTERNATIVE_PBES_ENUMERATOR
#define enumerate_quantifiers_alternative_rewriter enumerate_quantifiers_rewriter
#include "mcrl2/pbes/rewriters/enumerate_quantifiers_alternative_rewriter.h"
#else
#include "mcrl2/pbes/rewriters/enumerate_quantifiers_rewriter_new.h"
#endif

#endif // MCRL2_PBES_REWRITERS_ENUMERATE_QUANTIFIERS_REWRITER_H

