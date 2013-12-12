// Copyright (c) 2009-2013 University of Twente
// Copyright (c) 2009-2013 Michael Weber <michaelw@cs.utwente.nl>
// Copyright (c) 2009-2013 Maks Verver <maksverver@geocities.com>
// Copyright (c) 2009-2013 Eindhoven University of Technology
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef COMPATIBILITY_H_INCLUDED
#define COMPATIBILITY_H_INCLUDED

/*! \file compatibility.h
    \brief Cross-platform compatibility functions.

    This file declares functions that are used to achieve compatibility with
    platforms/compilers that do not completely support the POSIX, C99 or C++
    standards.

    Contrary to common conventions, this file should be included last in source
    files that require it, so any platform-specific definitions can take
    precedence.
*/

/* Figure out which hashtable implementation to use: */
#if (__clang_major__ >= 5 && __apple_build_version__ >= 5000279)
#include <unordered_set>
#include <unordered_map>
#define HASH_SET(k) std::unordered_set<k>
#define HASH_MAP(k,v) std::unordered_map<k, v>
#elif (__cplusplus > 199711L || __GNUC__ >= 4)  /* C++ TR1 supported (GCC 4) */
#include <tr1/unordered_set>
#include <tr1/unordered_map>
#define HASH_SET(k) std::tr1::unordered_set<k>
#define HASH_MAP(k,v) std::tr1::unordered_map<k, v>
#elif (__GNUC__ >= 3)  /* GCC 3 hash tables (untested) */
#include <ext/hash_set>
#include <ext/hash_map>
#define HASH_SET(k) __gnu_cxx::hash_set<k>
#define HASH_MAP(k,v) __gnu_cxx::hash_map<k, v>
#elif defined(_MSC_VER) /* Microsoft Visual C/C++ compiler */
#include <hash_set>
#include <hash_map>
#define HASH_SET(k) stdext::hash_set<k>
#define HASH_MAP(k,v) stdext::hash_map<k, v>
#else  /* generic/old C++ compiler */
#include <map>
#include <set>
#define HASH_SET(k) std::set<k>
#define HASH_MAP(k,v) std::map<k, v>
#endif

#endif /* ndef COMPATIBILITY_H_INCLUDED */
