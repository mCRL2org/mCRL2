// Author(s): Jore Booy
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/utilities/boost_process.h
/// \brief A single place for the version-dependent includes and namespace alias
///        of Boost.Process.

#ifndef MCRL2_UTILITIES_BOOST_PROCESS_H
#define MCRL2_UTILITIES_BOOST_PROCESS_H

// Use std::filesystem instead of boost::filesystem, since the latter is not
// header-only and we do not link against the compiled Boost libraries.
#ifndef BOOST_PROCESS_USE_STD_FS
#define BOOST_PROCESS_USE_STD_FS
#endif

#include <boost/version.hpp>
// Boost 1.88
#if BOOST_VERSION >= 108800
#include <boost/process/v1/child.hpp>
#include <boost/process/v1/io.hpp>
#include <boost/process/v1/pipe.hpp>
namespace bp = boost::process::v1;
#else
#include <boost/process/child.hpp>
#include <boost/process/io.hpp>
#include <boost/process/pipe.hpp>
namespace bp = boost::process;
#endif

#endif // MCRL2_UTILITIES_BOOST_PROCESS_H
