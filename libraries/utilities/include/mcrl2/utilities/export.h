// Author(s): Sjoerd Cranen
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/utilities/export.h
/// \brief Defines annotations to force exports.

#ifdef _MSC_VER
    #define MCRL2_EXPORT __declspec(dllexport)
#else 
    #ifdef __GNUC__
        #define MCRL2_EXPORT __attribute__ ((dllexport))
    #else
        #define MCRL2_EXPORT
    #endif // __GNUC__
#endif // _MSC_VER
