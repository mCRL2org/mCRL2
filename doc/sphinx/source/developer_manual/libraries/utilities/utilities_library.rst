Introduction
============

.. cpp:namespace:: mcrl2::utilities

This library holds functionality that does not (or not yet) fit in any of the other libraries. It mainly contains functionality that simplifies the use of other libraries or combinations thereof. The purpose of bundling this functionality is to encourage reuse.

Much of the current functionality should at some point be integrated in one of the other libraries. Please contact any of the developers when you think this is the case.

Structure
=========
The header files of the utilities library are roughly organised as depicted below.

.. figure:: img/layout.png
   :align: center

The top directory is mcrl2, containing a header file with toolset specific build information and the utilities directory.

The command line interfacing sublibrary standardises some more aspects of tool command line interfaces.

ext is the BDD implementation (on top of the ATerm library), consisting of bdd2dot.h, bdd_info.h and bdd_manipulator.h.
Reference

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

  -q, --quiet              do not display warning messages
  -v, --verbose            display short intermediate messages
  -d, --debug              display detailed intermediate messages
  -h, --help               display help information
      --version            display version information

* Every tool that utilises ''rewriting'' should additionally provide the
  following option::

  -rNAME, --rewriter=NAME  use rewrite strategy NAME:
                           'jitty' for jitty rewriting (default),
                           'jittyp' for jitty rewriting with prover,
                           'jittyc' for compiled jitty rewriting.

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

Use of the :cpp:class:`mcrl2::utilities::qt::qt_tool` class takes care of both by
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
                           
.. table:: Tool classes and their supported command line arguments

   ================================================================================  ===================================================================
   tool class                                                                        command line arguments
   ================================================================================  ===================================================================
   class :cpp:class:`mcrl2::utilities::tool`                                         handles =--quiet=, =--verbose=, =--debug=, =--help= and =--version=
   class :cpp:class:`mcrl2::utilities::input_tool`                                   in addition handles a positional input file argument
   class :cpp:class:`mcrl2::utilities::input_output_tool`                            in addition handles a positional output file argument
   template <typename Tool> class :cpp:class:`mcrl2::utilities::rewriter_tool`       extends a tool with a =--rewriter= option
   template <typename Tool> class :cpp:class:`mcrl2::utilities::pbes_rewriter_tool`  extends a tool with =--rewriter= and =--pbes-rewriter= options
   ================================================================================  ===================================================================
                                                                                 
The class :cpp:class:`mcrl2::utilities::rewriter_tool` makes strategies of the
data rewriter available to the user. The class
:cpp:class:`mcrl2::utilities::pbes_rewriter_tool` makes pbes rewriters available
to the user.
                                                           
Example
-------

Below an example is given for the pbesparelm tool. Since this is a tool that
takes a file as input and also writes output to a file, it derives from the
class :cpp:class:`mcrl2:utilities:input_output_tool`.

.. literalinclude:: ../../../../../../../tools/release/pbesparelm/pbesparelm.cpp
   :language: c++

using the :cpp:member:`execute` member function, that accepts the command line
arguments specified by the user.

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

  #. Override the :cpp:member:`run` member function
  
     The actual execution of the tool happens in the virtual member function :cpp:member:`run`.
     The developer has to override this function to add the behavior of the tool
     The :cpp:member:`run` function is called from the :cpp:member:`execute` member function, after the
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
     methods :cpp:member:`parse_options` and :cpp:member:`add_options`:

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
by overriding the method :cpp:member:`available_rewriters`.

.. _logging_library:

Logging Library
===============

Introduction
------------
Printing of logging and debug messages has been standardised throughout the
mCRL2 toolset through this logging library. The facilities provided by this
library should be used throughout the toolset. The library is inspired by the
description in `"Logging in C++" by P. Marginean <http://drdobbs.com/cpp/201804215>`_.

All code of this library can be found in the mcrl2::log namespace.

Concepts
--------

The logging library incorporates the concepts introduced in this section.

Log level
^^^^^^^^^

The type :cpp:type:`log_level_t` describes the various log levels that we identify.
The log level describes the severity of the message.

.. note::

   No message should ever be printed to the quiet log level. This level
   is meant to disable all messages.

Hint
^^^^

Hints can be used to distinguish between separate components in the toolset.
The logging library allows controlling logging statements with different hints
separately. One can e.g. change the log level for a specific hint, or attach
another output stream to a specific hint, allowing the library user to write
specific messages to a file.

OutputPolicy
^^^^^^^^^^^^

The output policy controls the way messages are output.
By default the file_output policy is used, which writes a message to the
file related to the hint of the current message.

Library interface
-----------------
The main routine in the library is :cpp:func:`mCRL2log(level, hint)`, where level is a
loglevel, and hint is a (optional) string hint. The routine returns an output
stream to which a single log message may be printed. Printing defaults
to stderr.

Maximal log level (compile time)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
The library includes a compile time variable :c:macro:`MCRL2_MAX_LOG_LEVEL`, which,
if not set, defaults to debug. All log messages with a log level
higher than :c:macro:`MCRL2_MAX_LOG_LEVEL` will be disabled during compile-time,
meaning they will not be in the generated executable.

Maximal log level (runtime)
^^^^^^^^^^^^^^^^^^^^^^^^^^^
The maximal reporting level can be set using
:cpp:member:`mcrl2_logger::set_reporting_level(level)`, by default info is assumed.

Setting output stream
^^^^^^^^^^^^^^^^^^^^^
The output stream of the logger can be set to be any file using
:cpp:member:`mcrl2_logger::output_policy_t::set_stream(file_pointer)`. Note that
file_pointer in this case can also be stderr or stdout. The default
output stream is stderr.

Incorporating hints
^^^^^^^^^^^^^^^^^^^
For both the reporting level and the stream, the routines to change them have
an optional hint argument that can be used to override the defaults for a
specific hint. To set a reporting level for a specific hint "hint" one can
use :cpp:member:`mcrl2_logger::set_reporting_level(level, "hint")`, likewise, for a stream
one can use :cpp:member:`mcrl2_logger::output_policy_t::set_stream(file_pointer, "hint")`.
In order to remove specific treatment of a hint, the routines
:cpp:member:`mcrl2_logger::clear_reporting_level("hint")` an
:cpp:member:`mcrl2_logger::output_policy_t::clear_stream("hint")` can be used.

Formatting the output
^^^^^^^^^^^^^^^^^^^^^
By default each line in the output is prefixed with a fixed string,
including a timestamp, the log level and, if provided, a hint. Furthermore,
the user of the library can control indentation (at a global level) using
the routines :cpp:member:`mcrl2_logger::indent()` and
:cpp:member:`mcrl2_logger::unindent()`.

Tutorial
--------
In this section we describe a typical use case of the logging library.

To enable logging, first include the header file.

.. code-block:: c++

  #include "mcrl2/utilities/logger.h"

If you want to control the log levels that are compiled into the code, you
should set the following macro *before the first include* of logger.h, or
you should provide it as a compiler flag.

.. code-block:: c++

  #define MCRL2_MAX_LOG_LEVEL debug

this only compiles logging statements up to and including debug
(and is actually the default).

Now let's start out main routine as usual

.. code-block:: c++

  using namespace mcrl2;
  int main(int argc, char** argv)
  {

We only allow reporting of messages up to verbose, so we do not print
messages of level debug or higher.

.. code-block:: c++

    log::mcrl2_logger::set_reporting_level(log::verbose);

We want this information to be printed to stderr, which is the default.
Let's do some logging.

.. code-block:: c++

    mCRL2log(log::info) << "This shows the way info messages are printed, using the default messages" << std::endl;
    mCRL2log(log::debug) << "This line is not printed, and the function " << my_function() << " is not evaluated" << std::endl;

Now we call an algorithm :cpp:func:`my_algorithm`, which we will define later.
The algorithm uses "my_algorithm" as hint for logging, and we want to write
its output to a file. First we create a file logger_test_file.txt to which
we log, and assign it to the hint "my_algorithm".

.. code-block:: c++

    FILE* plogfile;
    plogfile = fopen("logger_test_file.txt" , "w");
    if(plogfile == NULL)
    {
      throw mcrl2::runtime_error("Cannot open logfile for writing");
    }
    log::mcrl2_logger::output_policy_t::set_stream(plogfile, "my_algorithm");
    log::mcrl2_logger::set_reporting_level(log::debug3, "my_algorithm");

    // Execute algorithm
    my_algorithm();

    // Do not forget to close the file.
    fclose(plogfile);
  }

Let's take a look at an implementation of =my_algorithm()=.

.. code-block:: c++

  void do_something_special()
  {
    mCRL2log(log::debug3, "my_algorithm") << "doing something special" << std::endl;
  }

  std::string my_algorithm()
  {
    mCRL2log(log::debug, "my_algorithm") << "Starting my_algorithm" << std::endl;
    int iterations = 3;
    mCRL2log(log::debug1, "my_algorithm") << "A loop with " << iterations << " iterations" << std::endl;
    log::mcrl2_logger::indent();
    for(int i = 0; i < iterations; ++i)
    {
      mCRL2log(log::debug2, "my_algorithm") << "Iteration " << i << std::endl;
      if(i >= 2)
      {
        log::mcrl2_logger::indent();
        mCRL2log(log::debug3, "my_algorithm") << "iteration number >= 2, treating specially" << std::endl;
        do_something_special();
        log::mcrl2_logger::unindent();
      }
    }
    log::mcrl2_logger::unindent();
    return "my_algorithm";
  }

Note that, with the settings so far, only the first debug statement in
:cpp:func:`my_algorithm` will be printed, the other log messages are compiled away due
to the setting of :c:macro:`MCRL2_MAX_LOG_LEVEL`. To overcome this, the define before
the include of `logger.h' must allow for more debug levels, e.g. by setting
it as follows

.. code-block:: c++

  #define MCRL2_MAX_LOG_LEVEL log::debug3

This does not yet suffice; setting this only made sure that the logging
statements of all levels up to and including debug3 are actually compiled
into the code. We still have to enable the logging statements at run-time,
because so far we have only allowed logging of messages up to verbose level.
Therefore we should add the following anywhere before the execution of
the second debug print in :cpp:func:`my_algorithm`

.. code-block:: c++

  log::mcrl2_logger::set_reporting_level(log::debug3, "my_algorithm");

The complete code now looks as follows:

.. code-block:: c++

  #define MCRL2_MAX_LOG_LEVEL mcrl2::log::debug3
  #include "mcrl2/utilities/logger.h"

  using namespace mcrl2;

  void do_something_special()
  {
    mCRL2log(log::debug3, "my_algorithm") << "doing something special" << std::endl;
  }

  std::string my_algorithm()
  {
    mCRL2log(log::debug, "my_algorithm") << "Starting my_algorithm" << std::endl;
    int iterations = 3;
    mCRL2log(log::debug1, "my_algorithm") << "A loop with " << iterations << " iterations" << std::endl;
    log::mcrl2_logger::indent();
    for(int i = 0; i < iterations; ++i)
    {
      mCRL2log(log::debug2, "my_algorithm") << "Iteration " << i << std::endl;
      if(i >= 2)
      {
        log::mcrl2_logger::indent();
        mCRL2log(log::debug3, "my_algorithm") << "iteration number >= 2, treating specially" << std::endl;
        do_something_special();
        log::mcrl2_logger::unindent();
      }
    }
    log::mcrl2_logger::unindent();
    return "my_algorithm";
  }

  int main(int argc, char** argv)
  {
    log::mcrl2_logger::set_reporting_level(log::verbose);

    mCRL2log(log::info) << "This shows the way info messages are printed, using the default messages" << std::endl;
    mCRL2log(log::debug) << "This line is not printed, and the function " << my_algorithm() << " is not evaluated" << std::endl;

    FILE* plogfile;
    plogfile = fopen("logger_test_file.txt" , "w");
    if(plogfile == NULL)
    {
      throw std::runtime_error("Cannot open logfile for writing");
    }
    log::mcrl2_logger::output_policy_t::set_stream(plogfile, "my_algorithm");
    log::mcrl2_logger::set_reporting_level(log::debug3, "my_algorithm");

    // Execute algorithm
    my_algorithm();

    // Do not forget to close the file.
    fclose(plogfile);
  }

Note that in this code, the logging of :cpp:func:`my_algorithm` is done to the file
logger_test_file.txt, whereas the other log messages are printed to stderr.

After execution, stderr looks as follows::

  [11:51:02.639 info]    This shows the way info messages are printed, using the default messages

The file logger_test_file.txt contains the following::

  [11:52:35.381 my_algorithm::debug]   Starting my_algorithm
  [11:52:35.381 my_algorithm::debug]   A loop with 3 iterations
  [11:52:35.381 my_algorithm::debug]     Iteration 0
  [11:52:35.381 my_algorithm::debug]     Iteration 1
  [11:52:35.381 my_algorithm::debug]     Iteration 2
  [11:52:35.381 my_algorithm::debug]       iteration number >= 2, treating specially
  [11:52:35.381 my_algorithm::debug]       doing something special

