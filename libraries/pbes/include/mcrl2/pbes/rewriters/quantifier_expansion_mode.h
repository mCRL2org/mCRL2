// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/rewriters/detail/quantifier_expansion_mode
/// \brief Expansion modes for quantifiers in pbes expressions. 

#ifndef MCRL2_PBES_REWRITERS_QUANTIFIER_EXPANSION_MODE_H
#define MCRL2_PBES_REWRITERS_QUANTIFIER_EXPANSION_MODE_H

namespace mcrl2::pbes_system
{

// expand_finite_sorts means only replace quantifiers over finite sorts.
// expand_infinite_sorts means replace quantifiers over finite and infinite sorts. This may not terminate.
// expand_infinite_sorts_and_use_data_rewriter expands infinite sorts in PBESs, and uses the term rewriter to deal with 
//        quantifiers in data expressions. This last mode is typically used to instantiate a PBES to a BES. 
enum enumerate_quantifiers_mode { expand_finite_sorts, expand_infinite_sorts, expand_infinite_sorts_and_use_data_rewriter };

} // mcrl2::pbes_system

#endif // MCRL2_PBES_REWRITERS_QUANTIFIER_EXPANSION_MODE_H
