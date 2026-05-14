// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MCRL2_DATA_CONCEPTS_H
#define MCRL2_DATA_CONCEPTS_H

#include <mcrl2/atermpp/concepts.h>
#include <mcrl2/data/variable.h>

namespace mcrl2 {
namespace data {

/// Concept that indicates that T is a substitution with an operator () on
/// variables, and a static constant is_identity_substitution that indicates
/// whether the substitution applied to a variable only delivers that variable.
template <typename T>
concept IsSubstitution = requires(T sigma, variable v)
{
  T::is_identity_substitution;
  sigma(variable());
  requires atermpp::IsATerm<std::remove_cvref_t<decltype(sigma(v))>>;
};

} // namespace data
} // namespace mcrl2

#endif // MCRL2_DATA_CONCEPTS_H
