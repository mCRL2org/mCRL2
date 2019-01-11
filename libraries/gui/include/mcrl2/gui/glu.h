// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \brief Includes <OpenGL/glu.h> on MAC and <GL/glu.h> on other platforms.

#ifndef MCRL2_GUI_GLU_H_
#define MCRL2_GUI_GLU_H_

#include "mcrl2/utilities/platform.h"

#ifdef MCRL2_PLATFORM_WINDOWS
#include <windows.h>
#endif

#ifdef MCRL2_PLATFORM_MAC
#include <OpenGL/glu.h>
#else
#include <GL/glu.h>
#endif

#endif // MCRL2_GUI_GLU_H_