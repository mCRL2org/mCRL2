// Author(s): Maurice Laveaux.
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "mcrl2/atermpp/detail/global_aterm_pool.h"

#include "mcrl2/atermpp/aterm.h"
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/atermpp/detail/aterm_int.h"

namespace atermpp
{
namespace detail
{
  _aterm_int::_aterm_int(std::size_t val) :
    _aterm(g_term_pool().get_symbol_pool().as_int()),
    m_value(val)
  {}

  global_aterm_pool* g_aterm_pool_instance;

  aterm g_is_empty_list = aterm_appl(g_as_empty_list);
}
}
