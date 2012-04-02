.. index:: pbesabsinthe

.. _tool-pbesabsinthe:

pbesabsinthe
============

Apply abstraction to the data domain of a PBES, based on auser defined mappings.

Besides a PBES, the user must supply two input files, containing the following:

* A partial data specification, that contains sorts and equations for the added
  abstractions:

  .. code-block:: mcrl2
  
     sort
       AbsList = struct empty | one | more;
       AbsNat  = struct nul | een | meer;
       AbsPos  = struct aap ;

     var a:AbsNat;
     eqn
       Abstail(empty) = {empty};
       Abstail(one)   = {empty};
       Abstail(more)  = {one,more};

       Absconc(a,empty) = {one};
       Absconc(a,one)   = {more};
       Absconc(a,more)  = {more};

       Abslen(empty)    = {nul};
       Abslen(one)      = {een};
       Abslen(more)     = {meer};

       Absge(nul,nul)   = {true};
       Absge(nul,een)   = {false};
       Absge(nul,meer)  = {false};
       Absge(een,nul)   = {true};
       Absge(een,een)   = {true};
       Absge(een,meer)  = {false};
       Absge(meer,nul)  = {true};
       Absge(meer,een)  = {true};
       Absge(meer,meer) = {false,true};

       Abscapacity      = meer;

* An abstraction mapping of sorts, and an abstraction mapping of functions,
  separated by an empty line. Note that the symbol `:=` is used to separate the
  left hand side and the right hand sides of the mappings.

  .. code-block:: mcrl2
  
     Nat       := AbsNat
     List(Nat) := AbsList

     tail: List(Nat) -> List(Nat)     := Abstail    : AbsList -> Set(AbsList)
     |>: Nat # List(Nat) -> List(Nat) := Absconc    : AbsNat # AbsList -> Set(AbsList)
     #: List(Nat) -> Nat              := Abslen     : AbsList -> Set(AbsNat)
     >=: Nat # Nat -> Bool            := Absge      : AbsNat # AbsNat -> Set(Bool)
     capacity: Pos                    := Abscapacity: AbsNat
     []: List(Nat)                    := empty      : AbsList
     @cNat: Pos -> Nat                := AbscNat    : Pos -> Set(AbsNat)</nowiki>

These files are passed to the tool using the options :option:`-D` and
:option:`-m`. The tool attempts to automatically generate abstractions of
functions that were not specified by the user. The data specification and the
right hand sides of the function mapping are merged with the data specification
of the input PBES.

