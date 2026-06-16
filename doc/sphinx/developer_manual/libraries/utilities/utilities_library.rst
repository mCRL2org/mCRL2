Introduction
============

.. cpp:namespace:: mcrl2::utilities

This library holds functionality that does not (or not yet) fit in any of the other libraries. It mainly contains functionality that simplifies the use of other libraries or combinations thereof. The purpose of bundling this functionality is to encourage reuse.

Much of the current functionality should at some point be integrated in one of the other libraries. Please contact any of the developers when you think this is the case.

Structure
=========
The header files of the utilities library are roughly organised as depicted below.

.. figure:: /_static/utilities/layout.png
   :align: center

The top directory is mcrl2, containing a header file with toolset specific build information and the utilities directory.

The command line interfacing sublibrary standardises some more aspects of tool command line interfaces.

.. _cli_library:

CLI Library
===========

Introduction
------------

A set of user interface guidelines and conventions has been compiled to standardise user interfaces across the tools in the mCRL2 toolset. The purpose of this library is to simplify creation and maintenance of standard conforming command line user interfaces of tools in the mCRL2 toolset.

Concepts
--------

Here we introduce the set of concepts involved.

Command line interface
^^^^^^^^^^^^^^^^^^^^^^

A command line interface is an interaction mechanism for software systems based on textual commands. The system awaits the next command, at which point it interprets this command and starts waiting again for the next command.

A shell is an example of a command line interface that provides a user with access to services typically services provided by the operating system kernel. The programs we would like to consider, mCRL2 tools, are typically started by feeding a command to a shell. The command that starts a tool program is an integral part of the command line interface of that program.

Command
^^^^^^^

A command is a sequence of characters (also called string) that is used to invoke tools.

Command Arguments
^^^^^^^^^^^^^^^^^

Commands consist of a part that identifies a tool and optional arguments that affect the behaviour of that tool. We distinguish two types of arguments options and non-option arguments.

An option is a special command argument that is used to trigger optional behaviour in a tool. Every option has a long identifier and optionally a single-character short identifier. Additional structure is imposed on options, for one they need to be distinguishable from the arguments. To disambiguate between arguments and option identifiers (to specify options) the short and long option identifiers are prefixed with '-' and '--' respectively. An extension taken from the getopt library (a well-known C-style library command line parsing) is the chaining of short option identifiers. For example -bc2 is treated the same as -b -c2 (under context conditions stated below).

For the sake of completeness, the following is a full EBNF(-like) grammar for commands::

    command ::= white-space * tool-name ( white-space+ ( option | argument ) ) * white-space *
    white-space ::= [ \t\n\r]
    argument    ::= [^ \t\n\r-] + | '"' [^"]* '"' | "'" [^']* "'"
    option      ::= ("--" long-option [ "=" argument ] ) | ("-" short-options [ white-space * argument ])
    long-option   ::= alpha-low ( long-option-character ) *
    short-options ::= ( short-option-character ) +
    long-option-character  ::= '-' | digit | alpha-low
    short-option-character ::= digit | alpha-low | alpha-high
    alpha-low             ::= 'a' | 'b' | 'c' | ... | 'z'
    alpha-high            ::= 'A' | 'B' | 'C' | ... | 'Z'
    digit                 ::= '0' | '1' | '2' | ... | '9'

Context conditions
^^^^^^^^^^^^^^^^^^

An option argument is called optional (otherwise mandatory) if a default value is available that is assumed present as argument if the option was found without argument. The option identified by a short or long identifier is associated with the information that it either expects no arguments, an optional argument or a mandatory argument. A default value must be associated with every optional argument. Finally no white-space is allowed between a short option identifier and the optional argument. For example '-oA' (and not '-o A') for option o with argument A and '-o ' specifies the the default value for option o.

For the chaining of short options it is required that all options except the last in the chain take no arguments (so not even an optional argument). The reason is that there is no reliable way to disambiguate between option argument and option identifiers. All that follows the first option in the chain that takes an optional or mandatory argument is taken as argument.

Library interface
-----------------

The public library interface consists of two classes, one for the specification of a command line interface and the other for the actual parsing of a command using an interface specification.

Objects of class mcrl2::utilities::interface_description contain an interface specification and a description of this interface. The specification part consist of a set of option specifications each containing a long identifier, a description of the option, and optionally an argument specification and short identifier. The argument specification describes whether the argument is mandatory or optional (in the latter case case it also specifies a default value). The descriptive part consists of some general interface information and every option and option argument is equipped with a textual description of its use.

Up to here functionality focusses on specifying input. The user interface conventions also mention standardised output. Formatting functionality is available for:

  * printing a textual interface description (for use with -h or --help option),
  * printing a copyright message,
  * printing a man page,
  * version information (--version option),
  * error reporting for command line parsing.

Especially the error reporting functionality can be useful for tool developers in situations where problems arise during processing the results of command line parsing.

Parsing commands against an interface specification and accessing the results can be done using an mcrl2::utilities::command_line_parser object. The output of parsing is the set of options and arguments associated to options that were part of the input command. When parsing finishes without problems parse results are available for inspection. On a parse error an exception is thrown, with a properly formatted error description as message.
Important usability notes

The interface conventions specify a number of standard options:

  #. for messaging \-\-verbose (-v), \-\-quiet (-q), \-\-debug (-d), and
  #. for strategy selection for rewriting using the rewrite library

If the tool uses the core messaging layer, it is necessary to include mcrl2/core/messaging.h prior to the header file of this library in order to activate automatic handling of messaging options on the command line. Similarly if a tool uses the rewriter library, it is necessary to include mcrl2/data/rewriter.h prior to header files of this library to activate handling of rewriter options.

Tutorial
--------

There is no tutorial for the use of this library, the reference documentation contains a number of small examples on the use of this library.

The command line interfacing library is part of the mCRL2 utilities library. It contains only infrastructure functionality for the construction of tools that provide the doorway to the core functionality of the mCRL2 toolset. The references pages are part of the utilities library reference pages.

.. _tool_classes:

Tool classes
============
To simplify the creation of a tool, a number of tool classes is available in the
Utilities Library. They all inherit from the class `tool`, and they can be found
in the namespace `utilities::tools`. The main purpose of the tool classes is to
standardize the behavior of tools. Tool classes use the :ref:`cli_library` for
handling command line arguments.

Using the tool classes ensure that all tools adhere to the following
guidelines

Tool interface guidelines
-------------------------

Command line interface
^^^^^^^^^^^^^^^^^^^^^^
The command line interface of each tool should adhere to the following guidelines.

Options
"""""""

Options can be provided in the following two forms:

* a long form (mandatory): ``--option``, where ``option`` is a string of the
  form ``[a-z][a-z0-9\-]*``;
* a short form (strongly recommended): ``-o``, where ``o`` is a
  character of the form ``[a-zA-Z0-9]``. Furthermore, the options should
  adhere to the following:

  * Options may take arguments, either mandatory or optionally;
    the mandatory argument of an option must be accepted as ``--option=ARG``
    for long forms and as ``-oARG`` or ``-o␣ARG`` for short forms, where
    ``␣`` stands for one or more whitespace characters.
    The optional argument of an option must be accepted as ``--option=ARG`` for
    long forms and as ``-oARG`` for short forms.

* Short forms of options may be concatenated, where the last option in the chain
  may take an argument. For instance, given options ``-o`` and
  ``-p`` where the latter takes an argument ``ARG``, the chain
  ``-opARG`` is valid (but ``-pARGo`` is not).
* Users should not be allowed to specify an option more than once.
* Every tool should provide the following standard options::

    |  -q, --quiet              do not display warning messages
    |  -v, --verbose            display short intermediate messages
    |  -d, --debug              display detailed intermediate messages
    |  -h, --help               display help information
    |      --version            display version information

* Every tool that utilises ''rewriting'' should additionally provide the
  following option::

    | -rNAME, --rewriter=NAME  use rewrite strategy NAME:
    |                         'jitty' for jitty rewriting (default),
    |                         'jittyp' for jitty rewriting with prover,
    |                         'jittyc' for compiled jitty rewriting.

Input and output files
""""""""""""""""""""""
Some tools require input and/or output files; these include
transformation and conversion tools (but not GUI tools). The most important
input file and the most important output file (if any) should be accepted as
optional command line arguments, in the following way:

* the first argument is treated as the input file, the second argument is
  treated as the output file (if present);
* when the input file is not supplied, input is read from ``stdin``;
* when the output file is not supplied, output is written to ``stdout``.

It is only allowed to deviate from these rules if it is technically
infeasible to read from ``stdin`` or write to ``stdout``.

Furthermore, the following features are not allowed:

* designate the input file without its extension, e.g.

  * wrong: ``mcrl22lps abp``
  * right: ``mcrl22lps abp.mcrl2``

* option ``-`` to indicate input should be read from ``stdin``, e.g.

  * wrong: ``... | lpsrewr - abp.rewr.lps``
  * right: ``... | lpsrewr > abp.rewr.lps``

Exit codes
""""""""""
The command line interface should have an exit code of ``0`` upon
successful termination, and non-zero upon unsuccessful termination. Success here
means that during executing of the tool, no errors have occurred.
No special meaning may be assigned to specific non-zero exit codes.

Handling interface errors
"""""""""""""""""""""""""
When parsing the command line, errors may be encountered, for instance due to an
invalid number of arguments, unrecognised options or illegal arguments to
options. When such errors are encountered the following actions should be taken,
depending on whether the tool has a GUI or not:

A tool that does not have a GUI should print the following message to ``stderr``::

  TOOL: ERROR_MSG
  Try `TOOL --help' for more information.

where:

* ``TOOL`` stands for the name of the tool that the user called, i.e. ``argv[0]``;
* ``ERROR_MSG`` stands for the error message corresponding to the first
  error that is encountered when parsing the command line. After that, the tool
  should terminate with exit code ``1``.

A tool that has a GUI should show an error message dialog containing the error
message corresponding to the first error that is encountered when parsing
the command line.

Exceptions
""""""""""
It is not allowed for tools to pass unhandled exceptions to the operating system.

Graphical user interface
^^^^^^^^^^^^^^^^^^^^^^^^
Every tool that has a graphical user interface tool should provide a help
menu containing the following menu items:
* Contents: a link to the tool user manual;
* About: a message dialog containing the tool version information.

Use of the :mcrl2:`mcrl2::utilities::qt::qt_tool` class takes care of both by
default. This class must be used for all QT tools to get the correct
command line interface behaviour.

Help and version information
^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Tool help and version information should adhere to the following guidelines.

Help information
""""""""""""""""
Help information should be provided by the command line option ``-h, --help``.

It basically is a condensed version of the tool user manual in plain text with a
maximum width of 80 characters.

Version information
"""""""""""""""""""
Version information should be provided by:

* the command line option ``--version``;
* the ``About`` menu item.

Available tool classes
----------------------

The table below gives an overview of the
available tool classes, and the command line options that they handle.

.. list-table:: Tool classes and their supported command line arguments
   :header-rows: 1

   * - tool class
     - command line arguments
   * - class :mcrl2:`mcrl2::utilities::tool`
     - handles ``--quiet``, ``--verbose``, ``--debug``, ``--help`` and ``--version``
   * - class :mcrl2:`mcrl2::utilities::input_tool`
     - in addition handles a positional input file argument
   * - class :mcrl2:`mcrl2::utilities::input_output_tool`
     - in addition handles a positional output file argument
   * - template <typename Tool> class :mcrl2:`mcrl2::data::tools::rewriter_tool`
     - extends a tool with a ``--rewriter`` option
   * - template <typename Tool> class :mcrl2:`mcrl2::pbes_system::tools::pbes_rewriter_tool`
     - extends a tool with ``--rewriter`` and ``--pbes-rewriter`` options

The class :mcrl2:`mcrl2::data::tools::rewriter_tool` makes strategies of the
data rewriter available to the user. The class
:mcrl2:`mcrl2::pbes_system::tools::pbes_rewriter_tool` makes pbes rewriters available
to the user.

Example
-------

A good example to look at is the pbesparelm tool. Since this is a tool that
takes a file as input and also writes output to a file, it derives from the
class :mcrl2:`mcrl2::utilities::input_output_tool`. It can be found in
the directory ``tools/release/pbesparelm/pbesparelm.cpp``.

In the constructor a few settings are provided.

This is enough to create a tool with the follow help message::

  Usage: pbesparelm [OPTION]... [INFILE [OUTFILE]]
  Reads a file containing a PBES, and applies parameter elimination to it. If
  OUTFILE is not present, standard output is used. If INFILE is not present,
  standard input is used.

Tool properties
^^^^^^^^^^^^^^^

.. table:: Tool properties

   ========  ==============================
   Property  Meaning
   ========  ==============================
   synopsis  Summary of command-line syntax
   what is   don't know
   ========  ==============================


Creating a new tool
^^^^^^^^^^^^^^^^^^^
To create a new tool, the following needs to be done:

  #. Override the :mcrl2:`run` member function

     The actual execution of the tool happens in the virtual member function :mcrl2:`run`.
     The developer has to override this function to add the behavior of the tool
     The :mcrl2:`run` function is called from the :mcrl2:`execute` member function, after the
     command line parameters have been parsed.

  #. Set some parameters in the constructor

     In the constructor of a tool, one has to supply a name for the tool,
     an author and a description:

     .. code-block:: c++

        class my_tool: public input_tool
        {
          public:
            my_tool()
              : input_tool(
                  "mytool",
                  "John Doe",
                  "Reads a file and processes it"
                )
            {}
        };

  #. Optionally add additional command line arguments]
     Additional command line arguments can be specified by overriding the virtual
     methods :mcrl2:`parse_options` and :mcrl2:`add_options`:

     .. code-block:: c++

        class pbes_constelm_tool: public filter_tool_with_pbes_rewriter
        {
          protected:
            bool m_compute_conditions;

            void parse_options(const command_line_parser& parser)
            {
              m_compute_conditions = parser.options.count("compute-conditions") > 0;
            }

            void add_options(interface_description& clinterface)
            {
              clinterface.add_option("compute-conditions", "compute propagation conditions", 'c');
            }
          ...
        };

One can change this selection
by overriding the method :mcrl2:`available_rewriters`.

.. _logging_library:

Logging Library
===============

Introduction
------------
Printing of logging and debug messages has been standardised throughout the
mCRL2 toolset through this logging library. The facilities provided by this
library should be used throughout the toolset. The library is inspired by the
description in `"Logging in C++" by P. Marginean <http://drdobbs.com/cpp/201804215>`_.

All code of this library can be found in the ``mcrl2::log`` namespace.

Concepts
--------

The logging library incorporates the concepts introduced in this section.

Log level
^^^^^^^^^

The type :mcrl2:`log_level_t <mcrl2::log::log_level_t>` describes the various
log levels that we identify. The log level describes the severity or verbosity
of a message. From lowest to highest verbosity the levels are:

.. table:: Log levels

   =========  ================================================================
   Level      Meaning
   =========  ================================================================
   quiet      No output at all; used to disable all messages
   error      Error messages only
   warning    Warnings and errors
   info       Informational messages (default reporting level)
   status      Progress messages that overwrite the previous status line
   verbose    Additional detail beyond info
   debug      Debug-level output
   trace      Fine-grained trace output
   =========  ================================================================

.. note::

   No message should ever be printed *to* the ``quiet`` log level. This level
   is meant only to suppress all output.

OutputPolicy
^^^^^^^^^^^^

The output policy controls the way messages are written out.
The abstract base class is ``mcrl2::log::output_policy``; the default
implementation ``mcrl2::log::file_output`` writes messages to stderr.
Custom policies can be registered and unregistered on the
``mcrl2::log::logger`` class.

Library interface
-----------------
The main entry point is the macro ``mCRL2log(LEVEL)``, where ``LEVEL`` is one
of the ``mcrl2::log::log_level_t`` values. It returns an output stream to
which a single log message can be written. Output defaults to stderr.

.. code-block:: c++

  mCRL2log(mcrl2::log::verbose) << "This message is printed at verbose level." << std::endl;

Maximal log level (runtime)
^^^^^^^^^^^^^^^^^^^^^^^^^^^
The maximal reporting level can be set and queried using the static methods of
``mcrl2::log::logger``:

.. code-block:: c++

  mcrl2::log::logger::set_reporting_level(mcrl2::log::verbose);
  mcrl2::log::log_level_t current = mcrl2::log::logger::get_reporting_level();

The default reporting level is ``info``.

Custom output policies
^^^^^^^^^^^^^^^^^^^^^^
To redirect log output (e.g. to a file), implement ``mcrl2::log::output_policy``
and register it:

.. code-block:: c++

  class my_file_output : public mcrl2::log::output_policy
  {
  public:
    void output(mcrl2::log::log_level_t level, time_t timestamp,
                const std::string& msg, bool print_time_information) override
    {
      // write msg to custom destination
    }
  };

  my_file_output my_policy;
  mcrl2::log::logger::register_output_policy(my_policy);
  // ... use logging ...
  mcrl2::log::logger::unregister_output_policy(my_policy);

Tutorial
--------
In this section we describe a typical use case of the logging library.

To enable logging, include the header file:

.. code-block:: c++

  #include "mcrl2/utilities/logger.h"

A minimal example:

.. code-block:: c++

  #include "mcrl2/utilities/logger.h"

  using namespace mcrl2;

  std::string my_algorithm()
  {
    mCRL2log(log::verbose) << "Starting my_algorithm" << std::endl;
    int iterations = 3;
    for (int i = 0; i < iterations; ++i)
    {
      mCRL2log(log::debug) << "Iteration " << i << std::endl;
    }
    return "my_algorithm";
  }

  int main(int argc, char** argv)
  {
    // Only report messages up to and including verbose level (debug messages
    // will be suppressed).
    log::logger::set_reporting_level(log::verbose);

    mCRL2log(log::info) << "Starting tool" << std::endl;
    mCRL2log(log::debug) << "This line is not printed because debug > verbose" << std::endl;

    my_algorithm();
  }

After execution, stderr looks as follows::

  [info]    Starting tool
  [verbose] Starting my_algorithm
