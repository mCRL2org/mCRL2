.. index:: lpsparunfold

.. _tool-lpsparunfold:

lpsparunfold
============

The algorithm applies a transformation on data expressions of a linear process
specification (LPS), by which other tools (such as lpsparelm, lpsconstelm) can
apply their transformations more effectively. Concretely, this tool unfolds a
sort with associated constructor functions to a set of process parameters in the
LPS, in such a way that all behaviour is preserved.

Let's consider an example LPS, saved in the file ``Turnonoff.mcrl2``::

  sort
    Sys = struct sys(status: Type, n:Nat);
    Type = struct p_on | p_off;
  map
    set_status: Sys # Bool # Type -> Type;
    set_n: Sys # Nat -> Nat;
  var
    s_Sys : Sys;
    p1,p2: Type;
    b:Bool;
    n2:Nat;
  eqn
    set_status(sys(p1,n2), b, p2) = if(!b, p1, p2);
    set_n(s_Sys, n2) = n2;
  act
    on, off;
  proc
    P(s_Sys : Sys) =
      (status(s_Sys) == p_on && set_status(s_Sys, status(s_Sys) == p_on, p_off) == p_off) 
        -> on . P(sys(set_status(s_Sys, status(s_Sys) == p_on, p_off), set_n(s_Sys, 0)))
      + sum n1: Nat . (status(s_Sys) == p_off && set_status(s_Sys, status(s_Sys) == p_off, p_on) == p_on)
        -> off . P(sys(set_status(s_Sys, status(s_Sys) == p_off, p_on), set_n(s_Sys, n1)));

  init P(sys(p_off, 0));

If we execute ``txt2lps Turnonoff.mcrl2 | lpsparunfold --sort=Sys | lpspp``, then we obtain an LPS that has the parameters ``P(s_Sys_pp: Sys1, s_Sys_pp1: Type, s_Sys_pp2: Nat)``. Here, the single parameter ``s_Sys`` has been replaced by three parameters as follows:

* ``s_Sys_pp`` indicates which constructor was stored in ``s_Sys``. In this case, the sort ``Sys`` has just one constructor, which is now represented by the constructor ``c_sys`` of the new sort ``Sys1``.
* ``s_Sys_pp1`` and ``s_Sys_pp2`` are the values of the ``status`` and ``n`` parameters that were originally stored inside the ``sys`` constructor.
* Additional mappings and rewrite rules are introduced to reconstruct the original value of ``s_Sys`` from the three new parameters (mapping ``C_Sys1``) and to project out the constructor type (mapping ``Det_Sys1``) and arguments (mappings ``pi_Sys`` and ``pi_Sys1``). The expressions that occur in the LPS are manipulated to use these mappings. In the general case, the new rewrite rules are such that ``s == C_Sort(Det_Sort(s), c_1(pi_1(s),..,pi_n(s)), .., c_k(pi_1(s),..,pi_n(s)))``, where ``c_1,..,c_k`` are the constructors of the unfolded sort ``Sort``.

Thus, the original parameter ``s_Sys`` is now effectively unfolded intro three parameters and its internal data is now stored in separate process parameters. This means they can be processed by tools such as :ref:`tool-lpsparelm`, :ref:`tool-lpsconstelm` and :ref:`tool-lpsstategraph`.

Options
-------

Selecting which parameter to unfold can be done with the `--sort` or `--index` options (exactly one should be given). Unfolding may be repeated with the `--num` option. For example, to unfold the first 4 elements of a list into process parameters, one may use ``lpsparunfold --sort=List(Nat) --num=4``. The LPS is rewritten using the rewrite rules after every unfolding step (similar to using the tool :ref:`tool-lpsrewr`).

There are also a few advanced options. First, there is `--alt-case`, which nests the newly introduced mapping ``C_Sort`` (where ``Sort`` is the sort to unfold) at a higher level, potentially allowing more rewriting. For example, when unfolding ``l : List(Nat)``, the expression ``l != []`` becomes ``C_ListNat(stack_pp, false, true)`` instead of ``!(C_ListNat(stack_pp, [], stack_pp1 |> stack_pp2) == [])``. In some corner cases, this may create exponentially large expressions with the number of unfoldings.

Second, the option `--no-pattern` disables using rewrite rules defined with pattern matching to manipulate state update expressions. Using this option makes the state update expressions smaller, but reduces opportunity for rewriting and simplifying the LPS. Finally, there is the option `--possibly-inconsistent`, which adds an additional rewrite rule ``C_Sort(x, d_1, ..., d_n) = (d_1 && x == c_1) || (c_2 && x == c_2) || .... (d_n && x == c_n)`` (where ``d_1,..,d_n`` are of sort ``Bool``) when unfolding ``Sort`` and rewrite rules for equality on the sort that is newly introduced. This can enable some simplifications, but may also make the data specification *inconsistent*, which means that it is possible to derive ``true == false``.

Background
----------

All theoretic background can be found in the paper

  A\. Stramaglia, J.J.A. Keiren, T. Neele. *Simplifying Process Parameters by Unfolding Algebraic Data Types*. ICTAC 2023. LNCS vol. 14446, pp. 399-416. `(DOI) <https://doi.org/10.1007/978-3-031-47963-2_24>`__

.. mcrl2_manual:: lpsparunfold
