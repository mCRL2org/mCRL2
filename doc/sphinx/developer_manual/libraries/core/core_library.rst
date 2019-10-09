Introduction
============
The Core library provides a low level interfaces to the internally used data
structures. Using this library internal data structures can be manipulated
directly, parsed from and printed to human readable textual descriptions. 

Concepts
========
The low level data structures are represented as ATerms (short for Annotated
Terms). ATerms can be used to represent tree-like data structures which have
distinguishing features as: maximal subterm sharing, annotations, automatic
garbage collection and compressed binary exchange formats. A more elaborate
description can be found `here <http://www.cwi.nl/htbin/sen1/twiki/bin/view/Meta-Environment/ATerms>`_. 

Structure
=========
The Core library is a collection of methods and functions in the core name space.
In fact it is a collection of small sub-libraries, each with their own functionality.

The alphabet reduction library transforms the mCRL2 specification to an
equivalent specification where renaming operations (allow, hide, rename, block
and communication) are distributed over the other operations. The reduction is
performed to reduce the number of multi-actions which are generated during
intermediate steps in the mCRL2 linearization procedure.

The messaging library is a library for controlling and displaying messages.
The messages are printed for logging purposes. It is possible to distinct four
different logging levels, namely normal, verbose, debug and quite. If logging
level has not explicitly been set, the logging level is by default set to
normal. In the normal logging all warning and error messages are displayed. If
the logging level is set to verbose, additional verbose messages are printed.
When a more detailed logging is required, the logging level can be set to the
debug level. If the logging level is set to quite, no errors and warnings are
displayed. Next to the printing of regular messages (like the C++ printf
function), these functions also allow the printing of ATerms.

The internal mCRL2 ATerm is a complex structure, especially when the
specifications grow large. To improve the readability on the internal ATerm
format, the print library provides the opportunity to pretty print ATerms in
a more human readable format.

The struct library contains a set of operations defined on the ATerm structure.
With these operations it is possible to:

  * check for a certain ATerm data structure,
  * create a predefined ATerm data structure, or
  * retrieve a value from the ATerm data structure. 

Tutorial
========
This tutorial shows the usage of the core library by creating a mCRL2 action
rename program. In the program all actions 'a' are substituted by a fresh
action 'b'. The basics are explained first, before getting into detail. 

Dependencies
------------
For the tutorial the following core and core/detail headers need to be included:

.. code-block:: c++

   // Core libraries 
   #include "mcrl2/core/messaging.h"           // Messaging library
   #include "mcrl2/core/parse.h"               // Parse library
   #include "mcrl2/core/typecheck.h"           // Type check library
   #include "mcrl2/core/data_implementation.h" // Data implementation library
   #include "mcrl2/core/alpha.h"               // Alpha reduction library
 
   // Core/detail libraries 
   #include "mcrl2/core/detail/struct.h"       // ATerm building blocks

To address the methods and members in the libraries, the following namespaces are used:

.. code-block:: c++

   using namespace mcrl2::core;                // core namespace
   using namespace mcrl2::core::detail;        // core::detail namespace

The specification
-----------------
For the tutorial, we consider the following specification, which is stored in
the `std::string` variable `spec`. 

.. code-block:: mcrl2

   act a: Bool;
  
   init a(true);
  
The program
-----------
First we need to initialize the ATerm Library via `MCRL2_ATERM_INIT(argc, argv)`.
This to prohibits the ATerm data structure from being removed on a run of the
garbage collector. To display verbose information during the execution of the
code, the method `gsSetVerboseMsg()` is called. After calling this method, all
instances of `gsVerboseMsg(...)` will display show their output, which is supplied
as an argument. The initialisation of the program is as follows:

.. code-block:: c++

   MCRL2_ATERM_INIT(argc, argv)   // initialise ATerm library
   gsSetVerboseMsg();             // enable Verbose logging 
   istringstream istr( spec );    // convert string to stringstream

To ensure that our specification is correct, we need to parse and type check
the specification. To parse the specification we call the parse_spec method
from the `core::detail` namespace. The function `parse_spec` reads the `stringstream`
variable
`istr` and stores the parsed specification in the `ATermAppl` variable `parsed_spec`.
If the parser cannot parse the specification, it will return empty output. To
ensure parsing went well, we check for the `parsed_spec` being empty. If parsing
succeeded, the outcome is displayed via a `gsVerboseMsg(...)` method. If parsing
failed, the outcome is shown via a `gsErrorMsg(...)` method. Type checking is
performed in an analogue way.

.. code-block:: c++

   ATermAppl parsed_spec = parse_proc_spec( istr );
   if (parsed_spec == NULL) 
   {
     gsErrorMsg("parsing failed\n");
     return 1;
   }
   gsVerboseMsg("parsing succeeded!\n");
 
   ATermAppl typed_checked_parsed_spec = type_check_proc_spec( parsed_spec );
   if (typed_checked_parsed_spec == NULL) 
   {
     gsErrorMsg("type checking failed!\n");
     return 1;
   }
   gsVerboseMsg("type checking succeeded!\n");

Once we know that our specification is correct, we show how the methods defined
in the messaging header can be used to print ATerms. The first case shows how
ATerms can be pretty printed. The second case show normal printing of ATerms.

.. code-block:: c++

   gsVerboseMsg("pretty printed specification:\n%P\n", typed_checked_parsed_spec );
   gsVerboseMsg("textual ATerm representation of the specification\n%T\n", typed_checked_parsed_spec );

The output of the first case looks similar to the specification provided.
The second case shows output which looks like:

.. code-block:: none

   SpecV1(
    DataSpec(SortSpec([]),
      ConsSpec([]),
      MapSpec([]),
      DataEqnSpec([])
    ),
    ActSpec([ActId("a",[SortId("Bool")])]),
    ProcEqnSpec([]),
    ProcessInit([],
      Action(
        ActId("a",[SortId("Bool")]),
        [OpId("true",SortId("Bool"))]
      )
    )
   )

It is possible log the output, but it is not always needed to show the logging.
Therefore it is desirable to only log the output in debug mode. This can be
accomplished by the following line.

.. code-block:: c++

   gsDebugMsg("pretty printed specification:\n%P\n", data_impl_typed_checked_parsed_spec );

To ensure that the acquired specification is correct, the following line is
added.

.. code-block:: c++

   assert( gsIsSpecV1( data_impl_typed_checked_parsed_spec ) );
  
Manipulating the specification
------------------------------
In this part of the tutorial, the initial specification is manipulated on a low
level of the internal data structure. The methods that start with `gs...` and
`ATA...` are defined in the mCRL2 core library. Methods starting with `AT...` are
defined in the ATerm Library.

We perform the following construction and manipulation of the specification.
First the new action b is created. In this tutorial we take the action b as
void action. The method `gsMakeActId` provides the means to create a Action
Identifier. The method requires two arguments, namely a label and a type. To
create a label and type, a string is converted to a `ATermAppl` and an empty
`ATermList` is created, respectively. To redefine the action specification,
the method `gsMakeActSpec(...)` needs to be called with a list of action
identifiers. Via an `ATmakeList1(...)` it is possible to create a list of length
one. The task can be accomplished with the following piece of code:

.. code-block:: c++

   ATermAppl actIdB = gsMakeActSpec( ATmakeList1( (ATerm) gsMakeActId( gsString2ATermAppl("b"), ATmakeList0() ) ) );

The variable `actIdB` is used to derive the action transistion b in the
initialisation. To solely acquire the action transition, the `actIdB`, we are taking
the first argument of `ActSpec` first . This leaves us with the the list of
Actions. To acquire the first argument from a list, the head function
`ATgetFirst(...)` is used. To create an action the function `gsMakeAction` is used.
This function requires two arguments. The first argument specifies an `ATermAppl`
Action. The second argument specifies the time conditions. While we do not
consider time, an empty list is supplied.

.. warning::

   Be careful when casting `ATerm`, `ATermAppl` and `ATermList`. These formats are interchangeable.

.. code-block:: c++

   ATermAppl actB = gsMakeAction( (ATermAppl) ATgetFirst( ATgetArgument(actIdB, 0 ) ), ATmakeList0() );

To create the initialisation we take the variable `actB`:

.. code-block:: c++

   ATermAppl init = gsMakeProcessInit( ATmakeList0(), actB );

To reconstruct the new specification we take the first and third argument of
the original specification, because their are untouched. The second and the
fourth argument need to be adjusted, therefore the appropriate variables
`actIdB` and `init` are substituted. To verify that the new specification is
correct, addition logging information is printed.


.. code-block:: c++

   ATermAppl new_spec = gsMakeSpecV1( 
                            ATAgetArgument(data_impl_typed_checked_parsed_spec, 0 )
                          , actIdB 
                          , ATAgetArgument(data_impl_typed_checked_parsed_spec, 2 )
                          , init 
                         ); 
 
   gsVerboseMsg("pretty printed version of the specification:\n%P", new_spec );

Alphabet reduction
------------------
To finalize the tutorial we apply an alphabet reduction on the specification.
For this small example the alphabet reduction does not have any effect. However
it is only here to illustrate its usage. Alphabet reduction can be applied by
using the method `gsAlpha(...)`. To assert the validity of the specification,
an addition check is performed.

.. code-block:: c++

   ATermAppl new_alpha_spec = gsAlpha(new_spec);
 
   assert( gsIsSpecV1( new_alpha_spec ) );

References
----------

.. [ATerm] M.G.J. van den Brand and P. Klint (2007). "ATerms for manipulation and
   exchange of structured data: It's all about sharing." Information and Software
   Technology 49:55--64.

