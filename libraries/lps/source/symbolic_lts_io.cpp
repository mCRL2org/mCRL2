// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "mcrl2/lps/symbolic_lts_io.h"

#include "mcrl2/atermpp/aterm_io_binary.h"
#include "mcrl2/data/detail/io.h"
#include "mcrl2/data/data_io.h"

using namespace mcrl2;
using namespace mcrl2::lps;

static atermpp::aterm symbolic_labelled_transition_system_mark()
{
  return atermpp::aterm_appl(atermpp::function_symbol("symbolic_labelled_transition_system", 0));
}

std::ostream& operator<<(std::ostream& stream, const symbolic_lts& lts)
{
  atermpp::binary_aterm_ostream aterm_stream(stream);

  // We set the transformer for all other elements (transitions, state labels and the initial state).
  aterm_stream << data::detail::remove_index_impl;

  aterm_stream << symbolic_labelled_transition_system_mark();
  aterm_stream << lts.data_spec;

  return stream;
}

std::istream& operator>>(std::istream& stream, symbolic_lts& lts)
{
  atermpp::binary_aterm_istream aterm_stream(stream);

  // We set the transformer for all other elements (transitions, state labels and the initial state).
  aterm_stream >> data::detail::add_index_impl;

  return stream;
}
