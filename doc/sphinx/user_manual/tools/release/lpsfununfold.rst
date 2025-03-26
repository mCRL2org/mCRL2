.. index:: lpsfununfold

.. highlight:: mcrl2

.. _tool-lpsfununfold:

lpsfununfold
============

The tool `lpsfununfold` takes an .lps file and transforms it by replacing process parameters of type function sorts, sets, finite sets, bags and finite
bags with finite domains by a sequence of parameters of the type equal the target sort of the function, or booleans in case of sets, and natural numbers
in case of bags. This is similar to the tool :ref:`tool-lpsparunfold`. The advantage is that the resulting linear process may be more amenable to processing 
by tools like :ref:`tool-lpsconstelm` and :ref:`tool-lpsparelm`. Furthermore, symbolic tools may be able to handle the transformed linear process more fruitfully.

In case of sets and bags, `lpsfununfold` adds a number of rewrite rules to simplify the resulting linear process. It rewrites the arguments
of process invocations, but it does not rewrite the resulting linear process completely. 

An example with function arguments
----------------------------------

If `lpsfununfold` is applied to the following linear process with process argument of sort ``Bool->Bool``::

  act  a:Bool;
  proc P(f2:Bool->Bool)=
         a(f2(true)).P(f2[true->false]);
  init P(lambda b:Bool.true);

then the result is the following linear process::

  act  a: Bool;
  proc P(f2_P1,f2_P2: Bool) =
         a((lambda x: Bool. if(x == false, f2_P2, f2_P1))(true)) .
           P(f2_P1 = false)
       + delta;
  init P(true, true);

Note that the function ``f2`` is replaced by two variables ``f2_P1`` and ``f2_P2``. The first one represents the 
value of the function ``f2(true)`` and the second represents the value of ``f2(false)``. Note that if the resulting
process is rewritten using the tool :ref:`tool-lpsrewr`, it simplifies to::

  act  a: Bool;
  proc P(f2_P1,f2_P2: Bool) =
         a(f2_P1) .
           P(f2_P1 = false)
       + delta;
  init P(true, true);

An example with finite sets
---------------------------

The tool `lpsfununfold` also unfolds parameters of type set, finite set, bag and finite bag provided that the domain of
these sets and bags are finite. This is illustrated by the following example::

  proc P(fs:FSet(Bool))=
         a(true in fs).P(fs+{true});
  
  init P({false});

After applying `lpsfununfold` and subsequently :ref:`tool-lpsrewr` we obtain::

  act  a: Bool;
       b: Nat;
  
  proc P(fs_P1,fs_P2: Bool) =
         a(fs_P1) .
           P(fs_P1 = true)
       + delta;

init P(false, true);

The rewrite rules that are added
--------------------------------

The tool `lpsfununfold` adds some rewrite rules to the specification to simplify expressions that typically occur after
applying `lpsfununfold`. 

For a function sort `s->t` a single rewrite rule is added, where ``f`` and ``g`` are of sort `s->t` and ``b`` is a boolean::

  if(b, f, g)(t) = if(b, f(t), g(t))

For ``s`` and ``t`` (finite) sets the following rules are added::

  d in s+t = d in s || d in t
  d in s-t = d in s && !(d in t)
  d in if(b,s,t) = if(b, d in s, d in t)
  if(b,true,false)=b

And for ``s`` and ``t`` (finite) bags these are the added rules::

  count(d,s+t) = count(d,s) + count(d,t)
  count(d,s-t) = monus(count(d,s), count(d,t))
  count(d,if(b,s,t)) = if(count(b, d),count(d,t))
  if(b,true,false)=b

These rewrite rules may not be enough to simplify the resulting
linear process. Using for instance the tool :ref:`tool-lpsactionrename` it is possible to add extra rewrite
rules, by adding rewrite rules in a rename file and by leaving the rename section empty.

.. mcrl2_manual:: lpsfununfold
