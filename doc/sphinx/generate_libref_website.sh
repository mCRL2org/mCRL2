#!/bin/bash
#
# IMPORTANT: This script should ONLY be run from the trunk directory of the
# mCRL2 repository!

###################### SETTINGS ###############################################

# Whether this script is indeed called from the trunk directory will be checked
# using the following variable.
# It contains the path to this script from the trunk directory.
PATH_FROM_TRUNK=doc/sphinx

# The file containing the global Doxygen configuration.
# Path is relative to the current directory, i.e. trunk.
DOXYMASTER=doc/sphinx/Doxymaster

# The directory in which the website will be generated.
# Path is relative to the current directory, i.e. trunk.
OUTPUT_DIR=doc/sphinx/doxy

# The CSS file containing the style sheet of the website.
# Path is relative to the current directory, i.e. trunk.
STYLESHEET=doc/sphinx/doxystyle.css

# The variable LIBRARY_LIST contains the list of libraries for which
# reference pages will be generated.
# Each entry is on a separate line and is a colon-separated list of the
# following form:
# [libname]:[dirname]:[path]
# where
# [libname] is the name of the library, e.g. "ATerm++" or "My Library"
#           (omitting the quotes). This name will appear in the navigation bar
#           on the website and on various reference pages.
# [dirname] is the name of the subdirectory of OUTPUT_DIR in which the reference
#           pages of the library will be stored. The directory will be created
#           if it does not already exist. Typically, [dirname] is a simplified
#           version of [libname] using lowercase alphanumerical characters
#           and underscores only, e.g. "atermpp" or "my_library".
# [path]    is the path to the library's root directory, relative to the current
#           directory (i.e. trunk).
#
# If you want to have documentation generated for your library, simply add an
# entry to LIBRARY_LIST below on a new line.
LIBRARY_LIST="
ATerm++:atermpp:libraries/atermpp
Core:core:libraries/core
BES:bes:libraries/bes
Data:data:libraries/data
LPS:lps:libraries/lps
LTS:lts:libraries/lts
PBES:pbes:libraries/pbes
Process:process:libraries/process
Trace:trace:libraries/trace
Utilities:utilities:libraries/utilities"

# Doxygen will extract documentation from every file/directory within your
# library's directory that is listed in the INPUT_LIST variable (if and only if
# it exists). This is a space-separated list of paths that are relative to a
# library's root directory.
INPUT_LIST="include source doc/Mainpage"

# Optionally, a Doxygen configuration file called 'Doxyfile' may be added to the
# 'doc' subdirectory of your library's root directory. If such a file exists,
# the settings in that file override the global settings in the DOXYMASTER file.
#
# For example, you can prevent specific files or directories from being
# processed by including the following line in that Doxyfile (note the use of
# '+=' as explained below):
#
# EXCLUDE_PATTERNS      += [patlist]
#
# where [patlist] is a space-separated list of file name patterns that will be
# matched against the absolute path of a file. See the manual on the Doxygen
# website for more information on these patterns.
#
# IMPORTANT: Please use '+=' instead of '=' on the line mentioned above!
# This is because the DOXYMASTER file already assigns some common exclude
# patterns, like "*/.svn/*" and "*/detail/*", to the EXCLUDE_PATTERNS variable.
# Using a '=' would assign a new list of patterns to this variable, thereby
# overwriting the patterns assigned to it in the DOXYMASTER file. A '+=' would
# merely append patterns to this list.
#
# For a list of all variables that can be set in your Doxyfile, please refer to
# the online Doxygen manual. Note that the values of some variables should not
# be overridden if they are part of mCRL2 policy. In case of uncertainty, please
# check with the other mCRL2 library developers before overriding specific
# variable values.

###################### END OF SETTINGS ########################################



###################### FUNCTIONS ##############################################

# Determines the Doxygen input files/directories for the library of which the
# location is passed as an argument
function determine_input {
  DOXYINPUT=""
  for f in $INPUT_LIST ; do
    if [ -e $1/$f ] ; then
      DOXYINPUT="$DOXYINPUT $1/$f"
    fi
  done
} # End of function determine_input

function print_help {
  echo "Usage: `basename $0` [OPTION]"
  echo
  echo "where OPTION is one of:"
  echo "   -h            prints this help message"
  echo "   -i            lists the known libraries along with their IDs"
  echo "   -l ID[,ID]*   generates the websites for the libraries with the"
  echo "                 listed IDs only; use -i to see the IDs"
  echo
  echo "If -l is passed, the generated webpages are suitable for offline"
  echo "viewing. They contain normal HTML and use the default Doxygen"
  echo "layout. Use of -l does generally not result in properly interlinked"
  echo "library websites."
  echo
  echo "If no OPTION is passed, then the websites for all known libraries are"
  echo "generated. In this case the output is NOT suitable for offline"
  echo "viewing as it contains PHP commands that are defined at mcrl2.org."
  echo
  echo "In any case, the webpages are written to the following subdirectory"
  echo "of the mCRL2 trunk directory:"
  echo "  $OUTPUT_DIR"
  echo
  echo "If this script is not called from the trunk directory of the mCRL2"
  echo "repository, it will abort with an error message before generating any"
  echo "website."
} # End of function print_help

function print_liblist {
  OLDIFS=$IFS
  IFS=$'\n'
  echo "This is the list of known libraries along with their IDs:"
  I=1
  for L in $LIBRARY_LIST ; do
    IFS=$':'
    set -- $L
    IFS=$OLDIFS
    echo -e "$I\t$1"
    let I++
  done
} # End of function print_liblist

###################### END OF FUNCTIONS #######################################



# Variables and functions are set, here comes the script!

# First check if we're called from the trunk directory
if [ ! -e "$PATH_FROM_TRUNK/`basename $0`" ] ; then
  echo "error: script is not called from the trunk directory of the mCRL2 repository"
  exit 1
fi
# Ok, apparently this holds:
TRUNK=`pwd`

# This variable indicates whether the webpages generated by the script
# will be viewed offline by the user on his local machine. If so, then
# we cannot generate the PHP code that is placed on the mCRL2 website.
OFFLINE=

# Process the command line arguments
CMDARGS=":hil:"
while getopts $CMDARGS OPT
do
  if [ "$OPT" == "h" ] ; then
    print_help
    exit 0
  fi
  if [ "$OPT" == "i" ] ; then
    print_liblist
    exit 0
  fi
  if [ "$OPT" == "l" ] ; then
    LIBID_LIST=$OPTARG
    OFFLINE="yes"
    continue
  fi
  if [ "$OPT" == ":" ] ; then
    echo "error: missing argument after -$OPTARG option"
    exit 1
  fi
  if [ "$OPT" == "?" ] ; then
    echo "error: unknown option -$OPTARG"
    exit 1
  fi
  # this should never happen
  echo "error: unknown error"
  exit 1
done
if [ "$OPTIND" -ne "`expr $# + 1`" ] ; then
  echo "error: too many arguments"
  exit 1
fi

if [ -n "$LIBID_LIST" ] ; then
  # We process only the requested libraries
  # Construct the list of libraries that have to be processed
  OLDIFS=$IFS
  IFS=$','
  for I in $LIBID_LIST ; do
    # set a value at place $I in array A
    A[$I]="yes"
  done

  IFS=$'\n'
  I=1
  for L in $LIBRARY_LIST ; do
    if [ "${A[$I]}" == "yes" ] ; then
      NEW_LIB_LIST=`echo -e "$NEW_LIB_LIST\n$L"`
    fi
    let I++
  done
  IFS=$OLDIFS

  if [ -z "$NEW_LIB_LIST" ] ; then
    echo "no libraries to process; exiting"
    exit 0
  fi
  LIBRARY_LIST="$NEW_LIB_LIST"
fi


# Sort LIBRARY_LIST alphabetically by library name
LIBRARY_LIST=`echo "$LIBRARY_LIST" | sort`

# Now we generate the reference pages for each library. We use the
# generated tag files to allow for cross-library links on the pages.

IFS=$'\n'
for L in $LIBRARY_LIST ; do
  # set variables $1, $2 and $3 to each of the colon-separated fields of
  # $L respectively
  IFS=$':'
  set -- $L
  IFS=$OLDIFS

  determine_input $3

  DOXYCONFIG="
    PROJECT_NAME = \"$1\"
    INPUT = $DOXYINPUT
    GENERATE_HTML = YES
    GENERATE_XML = YES
    OUTPUT_DIRECTORY = $OUTPUT_DIR
    XML_OUTPUT = $TRUNK/doc/sphinx/$3/xml
    HTML_OUTPUT = $2
    STRIP_FROM_PATH = $TRUNK/$3
    STRIP_FROM_INC_PATH = $TRUNK/$3/include $TRUNK/$3/source"

  # Configuration is all set, run Doxygen!
  if [ -e $3/doc/Doxyfile ] ; then
    ( cat $DOXYMASTER $3/doc/Doxyfile ; echo "$DOXYCONFIG" ) | doxygen -
  else
    ( cat $DOXYMASTER ; echo "$DOXYCONFIG" ) | doxygen -
  fi

done

# Clean up
for f in $TAGFILES ; do
  rm -f `echo "$f" | sed -e "s|=.*||"`
done
rm -f $DOXYHEADER $DOXYFOOTER
