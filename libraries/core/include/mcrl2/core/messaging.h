// Author(s): Aad Mathijssen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/core/messaging.h
/// \brief Controlling and printing of messages.

#ifndef __MCRL2_CORE_MESSAGING_H__
#define __MCRL2_CORE_MESSAGING_H__

#include <cstdarg>
#include <cassert>
#include "boost/current_function.hpp"
#include "mcrl2/utilities/logger.h"

namespace mcrl2
{
/// \brief The main namespace for the Core library.
namespace core
{

/// \brief Function for printing verbose messages.
void gsVerboseMsg(const char* Format, ...);
/// \brief Function for printing debug messages.
void gsDebugMsg(const char* Format, ...);
/// \brief Function for printing debug messages.
void gsDebugMsgFunc(const char* FuncName, const char* Format, ...);

/// \brief Function for printing debug messages.
#define gsDebugMsg(...)        gsDebugMsgFunc(BOOST_CURRENT_FUNCTION, __VA_ARGS__)

}
}

#endif
