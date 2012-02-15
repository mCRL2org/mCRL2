Introduction
============
The __trace_library__ is a simple library to store, load and traverse traces. 

Concepts
========

A trace is a sequence of actions [^a1 a2 a3 ... an]. Between the actions there can
be states and the actions/states can have time tags. In the most extensive form
a trace is a sequence [^s1 t1 a1 s2 t2 a2 ... sn tn] an [^sn+1 tn+1] where [^si]
is state [^i], [^ti] is time tag [^i] and [^ai] is action [^i]. Traces can be
generated using a
simulation tool but they can also be the result of an analysis tool. E.g., an
analysis tool can generate one or more traces to a deadlock. Such a generated
trace can subsequently be inspected by a tool capable of reading a trace.

All states, traces and time tags are of type `ATermAppl`. This is the type of
a term consisting of a function application to terms in the `ATerm` library. In
practice, this easily allows to use the library for all kind of traces. If a
state or time tag is not defined, this is indicated by the `NULL` pointer.

Structure
=========
The trace library is based on the ATerm library and a few standard C++
libraries.

Tutorial
========
The following fragment of code shows how to read a trace from standard in and
print its contents to standard out. The function `ATfprintf` is the print
function from the ATerm library, that allows to nicely print terms of type
`ATermAppl`. Note that `trace.nextAction()` automatically moves the current
position in the trace to the next position.

.. code-block:: c++

  #include <iostream>
  #include <mcrl2/atermpp/atermpp.h>
  #include <mcrl2/trace/trace.h>
   
  using namespace std;
  using namespace mcrl2::trace;
   
  int main(int argc, char **argv)
  {
    MCRL2_ATERMPP_INIT(argc, argv) // initialise ATerm library
   
    Trace trace(cin); // read trace from stdin
   
    for(unsigned int i=0 ; i<trace.getLength() ; i++)
    { 
      if (trace.currentState()!=NULL)
      {
        ATfprintf(stdout,"State: %t\n",trace.currentState());
      }
   
      if (trace.currentTime()!=NULL)
      {
        ATfprintf(stdout,"Time: %t\n",trace.currentTime());
      }
   
      ATfprintf(stdout,"Action: %t\n",trace.nextAction());
    }
  }

Note that the above code does not print the final state and time. This can
easily be added by copying the two ifs after the for loop.
