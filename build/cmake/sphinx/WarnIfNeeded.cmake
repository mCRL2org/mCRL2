#
# This include file is executed after the Sphinx build process has terminated
# via a custom command. Its task is to check whether the file ${LogPath} that
# stderr was redirected to during the Sphinx build contains anything. If so,
# then a warning is printed drawing the developer's eye to the file. Otherwise,
# the empty file is deleted to avoid any confusion.
#

set(CMAKE_MODULE_PATH ${IncludePath})
include(FileInformation)

if(EXISTS ${LogPath})
  file(SIZE ${LogPath} LOG_SIZE)
  if(LOG_SIZE GREATER 0)
    message(WARNING "Please check ${LogPath} for warnings and errors")
  else()
    file(REMOVE ${LogPath})
  endif()
endif()
