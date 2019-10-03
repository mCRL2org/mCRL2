# Building the documentation

Set up a Python virtual environment (needs to be done only once).

    $ python3 -m venv sphinx-venv

Activate it.

    $ source sphinx-venv/bin/activate

Install Sphinx (currently version 2.2.0).

    $ pip install Sphinx

Patch `sphinx-venv/lib/python3.7/site-packages/sphinx/domains/cpp.py` (or the
equivalent path for your Python version), line 4261, which contains
`assert len(withDecl) <= 1`: comment that out. It will error out unnecessarily.
This is a [known issue](https://github.com/sphinx-doc/sphinx/issues/5496) in
Sphinx.

Generate the documentation using the Makefile.

    $ make html

Later, you may want to clear the cached Doxygen and PDF files by first cleaning
the build environment.

    $ make clean html

The build process will write its output to `_build/html`, and any warnings
generated during the build process are written to
`_build/sphinx-build-warnings.log` - *not* to stderr! This file will contain
ANSI escape sequences, so use for example `less -R ...` to open it.

## Integration with mCRL2 CMake

You may need to pass an additional flag to CMake to let it use the right version
of Python. This can be done as follows:

    cmake [other options] -DPYTHON_EXECUTABLE=[absolute path to sphinx-venv]/bin/python ../src

The above should not be necessary if Sphinx is installed system wide.

Once done with the CMake process, you can `make doc` to generate the documentation.
