// Author(s): Willem Rietdijk
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lts_combine.h
/// \brief The combine_lts function combines two LTSs and applies 
/// the comm, block, allow and hide operators in that order.

#ifndef MCRL2_LTS_COMBINE_H_
#define MCRL2_LTS_COMBINE_H_

#include "../../../libraries/lts/include/mcrl2/lts/lts_lts.h"

namespace mcrl2
{

/// \brief Combine two LTSs and apply the comm, block, allow and hide operators.
void combine_lts(std::vector<lts::lts_lts_t>& ltss,
    std::vector<core::identifier_string_list>& syncs,
    std::vector<core::identifier_string>& resulting_actions,
    std::vector<core::identifier_string>& blocks,
    std::vector<core::identifier_string>& hiden,
    std::vector<core::identifier_string_list>& allow,
    std::string filename,
    bool save_at_end,
    std::size_t nr_of_threads = 1);
} // namespace mcrl2

#endif // MCRL2_LTS_COMBINE_H_