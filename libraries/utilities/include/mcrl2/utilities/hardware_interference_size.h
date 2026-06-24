// Author(s): Maurice Laveaux
//
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/utilities/hardware_interference_size.h
/// \brief Provides the hardware constructive and destructive interference sizes.

#ifndef MCRL2_UTILITIES_HARDWARE_INTERFERENCE_SIZE_H
#define MCRL2_UTILITIES_HARDWARE_INTERFERENCE_SIZE_H

#include <new>

namespace mcrl2::utilities
{

#ifdef __cpp_lib_hardware_interference_size
    using std::hardware_constructive_interference_size;
    using std::hardware_destructive_interference_size;
#else
    // 64 bytes on x86-64
    constexpr std::size_t hardware_constructive_interference_size = 64;
    constexpr std::size_t hardware_destructive_interference_size = 64;
#endif

} // namespace mcrl2::utilities

#endif // MCRL2_UTILITIES_HARDWARE_INTERFERENCE_SIZE_H
