.. index:: pbesabsinthe

.. _tool-pbesabsinthe:

pbesabsinthe
============

Apply abstraction to the data domain of a PBES, based on user defined mappings.

Besides a PBES, the user must supply an input file, containing the following:

* A partial data specification, that contains sorts and equations for the added
  abstractions.

* An abstraction mapping of sorts, and an abstraction mapping of functions.
  The keywords absmap and absfunc are used for this. The symbol `:=` is used to separate
  the left hand and the right hand sides of the mappings.

  .. code-block:: mcrl2

     sort
       AbsBit  = struct arbitrary;

     var
       b:Bit;

     eqn
       h(b) = arbitrary;
       abseq(arbitrary,arbitrary) = {true,false};
       absinv(arbitrary) = {arbitrary};

     absmap
       h: Bit -> AbsBit;

     absfunc
       ==: Bit # Bit -> Bool        := abseq : AbsBit # AbsBit -> Set(Bool);
       inv: Bit -> Bit              := absinv : AbsBit -> Set(AbsBit);

This file is passed to the tool using the option :option:`-a`.
The tool attempts to automatically generate abstractions of functions that were
not specified by the user. The data specification and the right hand sides of the
function mapping are merged with the data specification of the input PBES.

