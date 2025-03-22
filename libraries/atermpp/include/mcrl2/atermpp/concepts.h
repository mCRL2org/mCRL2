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

namespace atermpp {

/// Concept that can be used to indicate that T is of an ATerm type.
template<typename T>
concept IsATerm = requires(T t)
{
    /// Term must be derived from the base aterm.
    requires std::is_base_of<atermpp::unprotected_aterm_core, T>::value;

    /// aterm cast cannot be applied types derived from aterms where extra fields are added.
    requires sizeof(T) == sizeof(atermpp::unprotected_aterm_core);
};

template<typename T>
concept IsTermConverter = requires(T t)
{
    /// Calling the function with an aterm must yield an aterm.
    std::is_convertible<T, atermpp::unprotected_aterm_core>::value;
    std::is_invocable<T, atermpp::unprotected_aterm_core>::value;

};

} // namespace atermpp