/*
 * MSVC workaround header for mCRL2.
 *
 * Windows/MSVC does not provide a POSIX <unistd.h>. This shim forwards the
 * subset of declarations that mCRL2 actually uses (e.g. getpid()) to the
 * corresponding MSVC headers <io.h> and <process.h>. It is only on the include
 * path for MSVC builds via cmake/ConfigureMSVC.cmake, but the guard below makes
 * it a no-op should the path ever leak into a non-Windows toolchain.
 */

#if defined(_MSC_VER) || defined(_WIN32)

#include <io.h>
#include <process.h>

#endif
