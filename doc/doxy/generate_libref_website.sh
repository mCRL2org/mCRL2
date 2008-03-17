#!/bin/bash
#
# IMPORTANT: This script should ONLY be run from the trunk directory of the
# mCRL2 repository!

###################### SETTINGS ###############################################

# Whether this script is indeed called from the trunk directory will be checked
# using the following variable.
# It contains the path to this script from the trunk directory.
PATH_FROM_TRUNK=doc/doxy

# The file containing the global Doxygen configuration.
# Path is relative to the current directory, i.e. trunk.
DOXYMASTER=doc/doxy/Doxymaster

# The directory in which the website will be generated.
# Path is relative to the current directory, i.e. trunk.
OUTPUT_DIR=build/doxy

# The CSS file containing the style sheet of the website.
# Path is relative to the current directory, i.e. trunk.
STYLESHEET=doc/doxy/doxystyle.css

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
Data:data:libraries/data
LPS:lps:libraries/lps
LTS:lts:libraries/lts
PBES:pbes:libraries/pbes
Trace:trace:libraries/trace
Utilities:utilities:libraries/utilities
TIPi:tipi:tools/squadt/libraries/tipi"

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

# The names of the temporary PHP header and footer files.
DOXYHEADER=doxyheader.php
DOXYFOOTER=doxyfooter.php

# The names of the index and portlet PHP files
INDEX=index.php
PORTLET=portlet.php

# The text on the main page of the website (OUTPUT_DIR/index.php) in HTML
# syntax.
MAIN_TEXT="<h1>mCRL2 library reference manual</h1>
<p>
This website contains the reference manuals of every mCRL2 library.
These pages list and document the available data structures, methods and
interfaces.
</p>
<p>
The pages can be browsed using the navigation menu on the left.
They are generated automatically using <a
href=\"http://www.doxygen.org\">Doxygen</a> and are updated to the latest
revision of the <a href=\"https://svn.win.tue.nl/viewcvs/MCRL2\">mCRL2 SVN
repository</a> every night.
</p>
<p>
This reference manual is part of the <a
href=\"/wiki/index.php/Library_documentation\">mCRL2 library
documentation</a> which also includes <i>user manuals</i> for developers
who want to start using any of the libraries.
It is therefore recommended to read those pages first.
</p>"

# The text at the bottom of every generated HTML page.
# We use some of the predefined placeholders (starting with a '$') that Doxygen
# will replace by approriate text.
FOOTER_TEXT="<ul>
  <li>This page was generated on \$datetime by 
      <a href=\"http://www.doxygen.org\">doxygen</a> \$doxygenversion.
  </li>
</ul>"

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

function write_portlet {
  echo "<?php function mcrl2_libref_portlet(\$current) { ?>
  <div class=\"portlet\">
    <h5>Libraries</h5>
    <ul>" > $OUTPUT_DIR/$PORTLET

  OLDIFS1=$IFS
  IFS=$'\n'
  for L in $LIBRARY_LIST ; do
    IFS=$':'
    set -- $L
    echo "      <li <?php if (\$current == \"$2\") { ?> class=\"current\"<?php } ?>><a href=\"/libref/$2/\">$1</a></li>" >> $OUTPUT_DIR/$PORTLET
  done
  IFS=$OLDIFS1

  echo "    </ul>
  </div> <!-- portlet -->
<?php } ?>" >> $OUTPUT_DIR/$PORTLET
} # End of function write_portlet

function write_index {
  echo "<?php
  require('../common/mcrl2.php');
  require('$PORTLET');
  mcrl2_html_begin();
  mcrl2_head_begin();
?>
  <title>mCRL2 Library Reference</title>
  <link href=\"doxystyle.css\" rel=\"stylesheet\" type=\"text/css\">

<?php
  mcrl2_head_end();
  mcrl2_body_begin();
?>
<div id=\"main\">

<?php 
  mcrl2_portal_begin();
  mcrl2_libref_portlet(\"\");
  mcrl2_search_portlet(2);
  mcrl2_portal_end();
?>

<div id=\"contents-column\">
<div class=\"contents\">
  $MAIN_TEXT
</div> <!-- contents -->
</div> <!-- contents-column -->

</div> <!-- main -->

<?php 
  mcrl2_body_end();
  mcrl2_html_end();
?>" > $OUTPUT_DIR/$INDEX
} # End of function write_index


function write_doxyheader {
  # The name of the directory that will contain the HTML files that use this
  # header, is passed as an argument
  CURRENT=$1
  echo "<?php
  require('../../common/mcrl2.php');
  require('../$PORTLET');
  mcrl2_html_begin();
  mcrl2_head_begin();
?>
  <title>\$title</title>
  <link href=\"../doxystyle.css\" rel=\"stylesheet\" type=\"text/css\">

<?php
  mcrl2_head_end();
  mcrl2_body_begin();
?>
<div id=\"main\">

<?php 
  mcrl2_portal_begin();
  mcrl2_libref_portlet(\"$CURRENT\");
  mcrl2_search_portlet(2)
  mcrl2_portal_end();
?>

<div id=\"contents-column\">" > $DOXYHEADER
} # End of function write_doxyheader

function write_doxyfooter {
  echo "<!--sphider_noindex-->
  <div id=\"pageinfo\">
    $FOOTER_TEXT
  </div>
<!--/sphider_noindex-->
</div> <!-- contents-column -->

</div> <!-- main -->

<?php 
  mcrl2_body_end();
  mcrl2_html_end();
?>" > $DOXYFOOTER
} # End of function write_doxyfooter

function print_help {
  echo "Usage: `basename $0` [OPTION]"
  echo
  echo "where OPTION is one of:"
  echo "   -h            prints this help message"
  echo "   -i            lists the known libraries along with their IDs"
  echo "   -l ID[,ID]*   generates the websites for the libraries with the"
  echo "                 listed IDs only; use -i to see the IDs"
  echo
  echo "If no OPTION is passed, then the websites for all known libraries are"
  echo "generated."
  echo
  echo "Use of -l does generally not result in properly interlinked library"
  echo "websites."
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

# First we generate a tag file called for every library in its root directory.
TAGFILES=""
# LIBRARY_LIST is a newline-separated list, so define \n to be the internal
# field separator.
OLDIFS=$IFS
IFS=$'\n'

for L in $LIBRARY_LIST ; do
  # set variables $1, $2 and $3 to each of the colon-separated fields of $L
  # respectively
  IFS=$':'
  set -- $L
  IFS=$OLDIFS

  determine_input $3

  TAGFILES="$TAGFILES $3/$2.tag=../$2"
  DOXYCONFIG="
    GENERATE_TAGFILE = $3/$2.tag
    INPUT = $DOXYINPUT"
  if [ -e $3/doc/Doxyfile ] ; then
    ( cat $DOXYMASTER $3/doc/Doxyfile ; echo "$DOXYCONFIG" ) | doxygen -
  else
    ( cat $DOXYMASTER ; echo "$DOXYCONFIG" ) | doxygen -
  fi
done

# Create the HTML footer that will be used for all generated HTML files
write_doxyfooter

# Now we generate the HTML reference pages for each library. We use the
# generated tag files to allow for cross-library links on the pages.

IFS=$'\n'
for L in $LIBRARY_LIST ; do
  # set variables $1, $2 and $3 to each of the colon-separated fields of $L
  # respectively
  IFS=$':'
  set -- $L
  IFS=$OLDIFS
  
  # Create the HTML header for all pages that are generated for this library.
  write_doxyheader $2
  determine_input $3

  # This library's tag file must *not* be in the list of tag files that are
  # passed to Doxygen via the TAGFILES variable. Otherwise, the reference pages
  # will be rather empty. So we filter it out.
  TAGFILES_NOT_SELF=`echo "$TAGFILES" | sed -e "s|$3/$2.tag=../$2||"`
  DOXYCONFIG="
    PROJECT_NAME = \"$1\" 
    INPUT = $DOXYINPUT
    GENERATE_HTML = YES
    OUTPUT_DIRECTORY = $OUTPUT_DIR
    HTML_OUTPUT = $2
    TAGFILES = $TAGFILES_NOT_SELF
    HTML_HEADER = $DOXYHEADER
    HTML_FOOTER = $DOXYFOOTER
    STRIP_FROM_PATH = $TRUNK/$3
    STRIP_FROM_INC_PATH = $TRUNK/$3/include $TRUNK/$3/source"
  if [ -e $3/doc/Doxyfile ] ; then
    ( cat $DOXYMASTER $3/doc/Doxyfile ; echo "$DOXYCONFIG" ) | doxygen -
  else
    ( cat $DOXYMASTER ; echo "$DOXYCONFIG" ) | doxygen -
  fi
  
  # Remove unused files that Doxygen generated.
  rm -f $OUTPUT_DIR/$2/{tab_[blr].gif,*.css,doxygen.png,installdox}
done

# Create the website's main page and portlet file
write_index
write_portlet

# Copy the CSS style sheet to the output directory
cp $STYLESHEET $OUTPUT_DIR

# Clean up
for f in $TAGFILES ; do
  rm -f `echo "$f" | sed -e "s|=.*||"`
done
rm -f $DOXYHEADER $DOXYFOOTER
