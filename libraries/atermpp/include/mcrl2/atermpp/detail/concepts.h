// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#pragma once

#include "mcrl2/atermpp/aterm_core.h"

/// Concept that can be used to indicate that T is of an ATerm type.
template<typename T>
concept IsATerm = requires(T t)
{
    std::is_base_of<atermpp::unprotected_aterm_core, T>::value;
};

