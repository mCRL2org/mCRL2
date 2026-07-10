/*
 * MSVC workaround header for mCRL2.
 *
 * Windows/MSVC does not provide a POSIX <dirent.h> and mCRL2 does not implement
 * a shim for it. This file intentionally provides no declarations: it exists
 * only so that including <dirent.h> resolves to an actionable diagnostic rather
 * than silently succeeding with an empty header (which would surface later as
 * confusing "undeclared identifier" errors on DIR/opendir/readdir/closedir).
 *
 * Any use of the POSIX directory API must be guarded with #if !defined(WIN32)
 * (as in 3rd-party/dparser/d.h) or replaced with std::filesystem.
 */
#error \
  "dirent.h is not available on MSVC. Guard POSIX directory usage with #if !defined(WIN32) or use std::filesystem instead."
