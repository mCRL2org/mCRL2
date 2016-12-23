.. index:: lpsactionrename

.. _tool-lpsactionrename:

.. highlight:: mcrl2

lpsactionrename
===============

The ``lpsactionrename`` tool renames actions in an LPS, based on their names and
on the data parameters they carry.

Structure of rename files
-------------------------

.. dparser:: ActionRenameRuleRHS ActionRenameRule ActionRenameRuleSpec ActionRenameSpec

The format of the RENAMEFILE can contain ``sort``, ``cons``, ``map``, ``eqn``
and ``act`` sections as in a mcrl2 file. This is followed by a ``rename`` 
section to define the rename rules. The sections ``sort``, ``cons``, ``map``, 
``eqn`` and ``act`` are meant for new declarations that will be added to the LPS 
and can be used in the rename rules. The new declarations are not allowed to 
contain any conflicts with the declarations of the LPS. The ``rename`` section 
can be preceded by a ``var`` section, where variables can be declared for the 
rename rules.

The rename rules have the format: ``rename c -> a1 => a2;`` where ``c`` is a
boolean expression that has to hold to rename an occurrence of ``a1`` into
``a2``. The condition can be left out, in which case it is interpreted as
``true`` (*i.e.*, all occurrences of ``a1`` will be renamed). The action ``a1``
can contain arguments that can either be uniquely occurring variables or closed
terms. The arguments of ``a2`` can be arbitrary terms, but the variables
occurring in it must also occur in ``a1``. The condition is an expression of
sort ``Bool`` and can also only use variables that also occur in ``a1``.

It is possible use ``tau`` for ``a2``; note that this means that a 
multi-action of the form ``a1|b`` will be replaced by ``b``. Instead of an 
action, ``a2`` may also be ``delta``. In this case, the action and the following
process call are replaced by ``delta``.

The renaming rules are applied from top to bottom to an linear process equation.
If no value for the variables in a rename rule can be found to match an action,
the next rule is applied. If no rule applies the action is left untouched.
Variables in different rename rules with the same variable names are independent
when being matched.

After the LPS has been renamed, sum elimination and rewriting will be applied to
simplify the result. This can be skipped using appropriate switches.

Rename rule conditions
----------------------

Upon loading the rename file, ``lpsactionrename`` will check if the following 
conditions hold:

- Variables used in the condition or in the right side of a rename rule must 
  also occur in the left side of that rename rule.
- All arguments of the action at the left hand side must either be closed terms 
  or variables. Each variable can only occur once in the left hand side.
- All used actions and data types must be declared in the LPS file or locally.
- All conditions are data expressions of sort ``Bool``.
- All elements are well typed with respect to the declarations in the LPS or the 
  rename file.

Example
-------

Consider an LPS with the process specification::

  P(x:Bool) = sum y:Nat. (y < 6) -> a(x,y). P(!x);

and a rename file with the following rename rules::

  act b: Bool;
  var v: Nat; w:Bool;
  rename 
    w -> a(w,v) => b(v==5);
    (v==v*2)==w -> a(w,v) => tau;
    a(w,5) => delta;

The arguments of an action do not have to consist of a single variable, as is
done in the second rename rule. In the second rename rule, ``a(w,2*v)``, ``w`` 
and ``2*v`` will be respectively equal to ``x`` and ``y`` from the LPS action 
``a(x,y)``.

The result of applying the rename rules to the LPS without sum elimination will 
give::

  proc P(x_P0: Bool) =
         true ->
           delta
       + sum w: Bool,v,y_P0: Nat.
           ((y_P0 < 6 && w==x_P0 && v==y_P0) && w) -> b(v==5).P(!x_P0);
       + sum w00: Bool,v00: Nat,w: Bool,v,y_P0: Nat.
           ((((y_P0 < 6 && w==x_P0 && v==y_P0) && !w) && w00==x_P0 && v00==y_P0) && 
                     (v00==v00*2)==w00) -> tau.P(!x_P0)
       + sum w01,w00: Bool,v00: Nat,w: Bool,v,y_P0: Nat.
           ((((((y_P0 < 6 && w==x_P0 && v==y_P0) && !w) && w00==x_P0 && v00==y_P0) && 
                     !((v00==v00*2)==w00)) && w01==x_P0) && 5==y_P0) -> delta
       + sum w01,w00: Bool,v00: Nat,w: Bool,v,y_P0: Nat.
           ((((((y_P0 < 6 && w==x_P0 && v==y_P0) && !w) && w00==x_P0 && v00==y_P0) && 
                     !((v00==v00*2)==w00)) && w01==x_P0) && !(5==y_P0)) -> a(x_P0, y_P0).P(!x_P0)

Most of the introduced sum variables have a single point domain, namely: ``u``, 
``w``, ``w_S00``, ``w_s01``, ``v_S00`` and in the last two summands, ``y``. 
These variables can be eliminated by applying sum elimination. For example: in 
the first summand ``w`` is equal to ``x``. Therefore ``w`` can be substituted by
``x``, and ``w`` can then be removed from the sum since it is no longer used.

Applying sum elimination will give the following result::

  proc P(x_P0: Bool) =
       true -> delta
       + sum y_P0: Nat.(y_P0 < 6 && x_P0) ->b(y_P0 == 5) .P(!x_P0);
       + sum y_P0: Nat.(y_P0 < 6 && !(y_P0 == y_P0 * 2)) ->tau.P(!(y_P0 == y_P0 * 2))
       + (!x_P0 && x_P0) ->delta
       + sum y_P0: Nat.(((y_P0 < 6 && !x_P0) && !((y_P0 == y_P0 * 2) == x_P0)) &&
                     !(5 == y_P0)) -> a(x_P0, y_P0) .P(!x_P0)

