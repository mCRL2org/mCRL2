# mCRL2
mCRL2 is a formal specification language with an associated toolset. The toolset can be used for modelling, validation and verification of concurrent systems and protocols. It can be run on Windows, Linux and macOS.

The toolset supports a collection of tools for linearisation, simulation, state-space exploration and generation and tools to optimise and analyse specifications. Moreover, state spaces can be manipulated, visualised and analysed.

The mCRL2 toolset is developed at the department of Mathematics and Computer Science of the [Technische Universiteit Eindhoven](https://fsa.win.tue.nl/), in collaboration with [CWI](http://www.cwi.nl) and the [University of Twente](http://fmt.cs.utwente.nl).

See our [website](https://mcrl2.org) for extensive documentation on the usage of the toolset.

## Build instructions
mCRL2 has the following dependencies:
 - Qt (5.5.0 or higher, only for the GUI tools)
 - Boost (1.48 or higher)

Furthermore, sphinx and xsltproc are required to build the documentation. Makefiles can be generated using cmake. To build the toolset in Release mode using 4 threads, run the following commands (preferably in a separate directory):

    cmake -DCMAKE_BUILD_TYPE=Release <SOURCE_LOCATION>
    make -j4

Convenient front-ends for cmake are ccmake (macOS/Linux) and cmake-gui (Windows). Under Windows, it is usually necessary to set the variables `Boost_INCLUDE_DIR` and `Qt5_DIR` manually. More build instructions can be found in the [documentation](https://mcrl2.org/web/user_manual/build_instructions/index.html).

## Contributing
Report bugs in the [issue tracker](https://github.com/mCRL2org/mCRL2/issues). Please include the version number from `mcrl22lps --version`, and a complete, self-contained test case in each bug report.

Contributions in the form of a pull request are welcome. For more details, see the [documentation](https://mcrl2.org/web/developer_manual/repository.html).

If you have questions about using the mCRL2 toolset which the documentation does not answer, send a mail to <mcrl2-users@listserver.tue.nl> or open an issue.

## License
Copyright (C) 2005-2018 Eindhoven University of Technology
mCRL2 is licensed under the [Boost license](https://www.boost.org/LICENSE_1_0.txt). See the file COPYING for license information.
