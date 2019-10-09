# mCRL2 documentation/website

We will assume that you build the documentation out of source, in the same
directory where you built/configured mCRL2.

    $ cd path/to/your/build/directory/


## Python virtual environment (optional)

If you don't want to install Python libraries system wide, you can set up a
Python virtual environment (needs to be done only once).

    $ python3 -m venv sphinx-venv

Activate it.

    $ source sphinx-venv/bin/activate

Install Sphinx (currently version 2.2.0) and other dependencies.

    $ pip install Sphinx wheel dparser

Ensure that CMake uses the right Python binary.

    $ cmake [other options] \
          -DPYTHON_EXECUTABLE=absolute/path/to/sphinx-venv/bin/python \
          path/to/mCRL2/src


## Building the documentation

Generate the documentation (can also use Ninja etc.).

    $ make doc

Later, you may want to build the documentation without clearing the cache.

    $ make fastdoc

The build process will write its output to `sphinx`, and any warnings
generated during the build process are written to
`sphinx/sphinx-build-warnings.log` - *not* to stderr! This file will contain
ANSI escape sequences, so use for example `less -R ...` to open it.
