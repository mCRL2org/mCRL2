<p align="center">
  <img src="https://mcrl2.org/web/_images/mcrl2.png" >
</p>

mCRL2 is a formal specification language with an associated toolset. The toolset
can be used for modelling, validation and verification of concurrent systems and
protocols. It can be run on Windows, Linux and macOS.

The toolset supports a collection of tools for linearisation, simulation,
state-space exploration and generation and tools to optimise and analyse
specifications. Moreover, state spaces can be manipulated, visualised and
analysed.

The mCRL2 toolset is developed at the department of Mathematics and Computer
Science of the [Technische Universiteit Eindhoven](https://fsa.win.tue.nl/), in
collaboration with the [University of Twente](http://fmt.cs.utwente.nl).

See our [website](https://mcrl2.org) for extensive documentation on the usage of
the toolset and downloads of the release and nightly versions.

## Build instructions
mCRL2 has the following minimum dependencies:
 - A C++17 compliant compiler, which can be one of the following:
   - GCC: 7.0
   - Clang: 5.0
   - AppleClang: 11.0
   - MSVC: 2019 v16.0
 - Qt (Linux/Windows: 5.9.0, MacOS: 5.10.0; for the GUI tools only)
 - Boost (Linux: 1.65.1, MacOS/Windows: 1.67.0)

Furthermore, sphinx and xsltproc are required to build the documentation.
Makefiles can be generated using cmake. To build the toolset in Release mode
using 4 threads, run the following commands (preferably in an empty directory):

```
git clone https://github.com/mCRL2org/mCRL2 src
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ../src
make -j4
```

When compilation is finished, the binaries can be found in `stage/bin/`.
Convenient front-ends for cmake are ccmake (macOS/Linux) and cmake-gui. Under
Windows and macOS, it is usually necessary to set the variables
`Boost_INCLUDE_DIR` and `Qt5_DIR` manually. More build instructions can be found
in the
[documentation](https://mcrl2.org/web/user_manual/build_instructions/index.html).

## Contributing
Report bugs in the [issue tracker](https://github.com/mCRL2org/mCRL2/issues).
Please include the version number from `mcrl22lps --version`, and a complete,
self-contained test case in each bug report.

Contributions in the form of a pull request are welcome. For more details, see
the [documentation](https://mcrl2.org/web/developer_manual/repository.html).

If you have questions about using the mCRL2 toolset which the documentation does
not answer, send a mail to <mcrl2-users@listserver.tue.nl> or open an issue.

## License
Copyright (C) 2005-2019 Eindhoven University of Technology  
mCRL2 is licensed under the [Boost
license](https://www.boost.org/LICENSE_1_0.txt). See the file COPYING for
detailed license information.
