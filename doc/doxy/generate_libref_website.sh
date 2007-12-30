#!/bin/bash

# IMPORTANT: This script should ONLY be run from the trunk directory of the
# mCRL2 repository!

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
#
# By default, this script will process all source code files in the 'include'
# and 'source' subdirectories of your library's directory. See below for a
# way of changing this behaviour.
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

# The names of the temporary HTML header and footer files.
DOXYHEADER=doxyheader.html
DOXYFOOTER=doxyfooter.html

# The text on the main page of the website (OUTPUT_DIR/index.html) in HTML
# syntax.
MAIN_TEXT="Welcome to the mCRL2 Library Reference.
<p>
You can choose a library from the navigation bar to browse its reference pages."

# The text at the bottom of every generated HTML page.
# We use some of the predefined placeholders (starting with a '$') that Doxygen
# will replace by approriate text.
FOOTER_TEXT="This page was generated on \$datetime by <a
href=\"http://www.doxygen.org\">doxygen</a> \$doxygenversion."



# End of variables section; below is the script



function write_index {
  echo "
<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\">
<html>

<head>
  <meta http-equiv=\"Content-Type\" content=\"text/html;charset=iso-8859-1\">
  <title>mCRL2 Library Reference</title>
  <link href=\"http://www.mcrl2.org/common/mcrl2.css\" rel=\"stylesheet\" type=\"text/css\">
  <link href=\"doxystyle.css\" rel=\"stylesheet\" type=\"text/css\">
</head>

<body>
<div id=\"globalWrapper\">
  <div id=\"header\">
    <a href=\"http://www.mcrl2.org\"><img
      src=\"http://www.mcrl2.org/common/mcrl2.png\"/></a>
  </div>
  <div id=\"navbar\">
    <div id=\"p-menu\">
      <ul>
        <li><a href=\"http://www.mcrl2.org/wiki/index.php/Home\">Home</a></li>
        <li><a href=\"http://www.mcrl2.org/wiki/index.php/About\">About</a></li>
        <li><a href=\"http://www.mcrl2.org/wiki/index.php/Download\">Download</a></li>
        <li><a href=\"http://www.mcrl2.org/wiki/index.php/Documentation\">Documentation</a></li>
        <li><a href=\"http://www.mcrl2.org/wiki/index.php/Showcases\">Showcases</a></li>
        <li><a href=\"http://www.mcrl2.org/wiki/index.php/Development\">Development</a></li>
      </ul>
    </div>
  </div>
  <div id=\"libbar\">
    <div id=\"libmenu\">
    <ul>" > $OUTPUT_DIR/index.html

  OLDIFS1=$IFS
  IFS=$'\n'
  for L in $LIBRARY_LIST ; do
    IFS=$':'
    set -- $L
    echo "<li><a href=\"$2/index.html\">$1</a></li>" >> $OUTPUT_DIR/index.html
  done
  IFS=$OLDIFS1

  echo "
      </ul>
    </div>
  </div>
  <div id=\"main\">
    <div class=\"contents\" id=\"notabs\">
    $MAIN_TEXT
    </div>
  </div>
</div>
</body>

</html>" >> $OUTPUT_DIR/index.html
} # End of function write_index


function write_doxyheader {
  # The name of the directory that will contain the HTML files that use this
  # header, is passed as an argument
  CURRENT=$1
  echo "
<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\">
<html>

<head>
  <meta http-equiv=\"Content-Type\" content=\"text/html;charset=iso-8859-1\">
  <title>\$title</title>
  <link href=\"http://www.mcrl2.org/common/mcrl2.css\" rel=\"stylesheet\" type=\"text/css\">
  <link href=\"../doxystyle.css\" rel=\"stylesheet\" type=\"text/css\">
</head>

<body>
<div id=\"globalWrapper\">
  <div id=\"header\">
    <a href=\"http://www.mcrl2.org\"><img
      src=\"http://www.mcrl2.org/common/mcrl2.png\"/></a>
  </div>
  <div id=\"navbar\">
    <div id=\"p-menu\">
      <ul>
        <li><a href=\"http://www.mcrl2.org/wiki/index.php/Home\">Home</a></li>
        <li><a href=\"http://www.mcrl2.org/wiki/index.php/About\">About</a></li>
        <li><a href=\"http://www.mcrl2.org/wiki/index.php/Download\">Download</a></li>
        <li><a href=\"http://www.mcrl2.org/wiki/index.php/Documentation\">Documentation</a></li>
        <li><a href=\"http://www.mcrl2.org/wiki/index.php/Showcases\">Showcases</a></li>
        <li><a href=\"http://www.mcrl2.org/wiki/index.php/Development\">Development</a></li>
      </ul>
    </div>
  </div>
  <div id=\"libbar\">
    <div id=\"libmenu\">
    <ul>" > $DOXYHEADER

  OLDIFS1=$IFS
  IFS=$'\n'
  for L in $LIBRARY_LIST ; do
    IFS=$':'
    set -- $L
    if [ "$2" == "$CURRENT" ] ; then
      echo "<li class=\"current\"><a href=\"../$2/index.html\">$1</a></li>" >> $DOXYHEADER
    else
      echo "<li><a href=\"../$2/index.html\">$1</a></li>" >> $DOXYHEADER
    fi
  done
  IFS=$OLDIFS1

  echo "
      </ul>
    </div>
  </div>
  <div id=\"main\">
    <div class=\"contents\">" >> $DOXYHEADER
} # End of function write_doxyheader

function write_doxyfooter {
  echo "
    </div>
    <div id=\"pageinfo\">
      $FOOTER_TEXT
    </div>
  </div>
</div>
</body>

</html>" > $DOXYFOOTER
} # End of function write_doxyfooter



TRUNK=`pwd`

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

  TAGFILES="$TAGFILES $3/$2.tag=../$2"
  DOXYCONFIG="
    GENERATE_TAGFILE = $3/$2.tag
    INPUT = $3/include $3/source"
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

  # This library's tag file must *not* be in the list of tag files that are
  # passed to Doxygen via the TAGFILES variable. Otherwise, the reference pages
  # will be rather empty. So we filter it out.
  TAGFILES_NOT_SELF=`echo "$TAGFILES" | sed -e "s|$3/$2.tag=../$2||"`
  DOXYCONFIG="
    PROJECT_NAME = \"$1\" 
    INPUT = $3/include $3/source
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

# Create the website's main page (index.html)
write_index

# Copy the CSS style sheet to the output directory
cp $STYLESHEET $OUTPUT_DIR

# Clean up
for f in $TAGFILES ; do
  rm -f `echo "$f" | sed -e "s|=.*||"`
done
rm -f $DOXYHEADER $DOXYFOOTER
