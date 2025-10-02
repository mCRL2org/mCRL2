

Parsing changes in the 202507.0 Release
========================================

In the 202507.0 release of mCRL2 there are some changes in the parsing of mCRL2
specifications and modal formulas. These changes are primarily related to the
operator precendence defined in the grammar, and removing some incomprehensible
disambiguation functions.

From our example directory we have seen that these changes in particular affect
the parsing of modal formulas, and we have seen no examples where mCRL2
specification are changed. The most observable change is that operators of a
higher precendence now *always* bind stronger than operators of a lower
precendence, whereas, before this was done inconsistently. Consider for example
the following modal formula:

``exists j: Int. val(j == 0) && val(j == 0)``

The exists binds weaker than the conjunction operator `&&`, so this formula
parses in the following way:

``exists j: Int. (val(j == 0) && val(j == 0))``

Now, if we extend this formula with a diamond operator, which binds stronger than
both the exists and conjunctive operator, as follows:

``<true> exists j: Int. val(j == 0) && val(j == 0)``

Then in both releases, this will be parsed as below:

``(<true> exists j: Int. val(j == 0)) && val(j == 0)``

This might not be what the user intended, but it is the way that our parser
generator deals with the precedence of operators, and it is one way to do so.
However, in the previous releases, it was the case that this binding was not
applied consistently. For example adding more modal operators could cause the
subformula to suddently be parsed differently, in some cases combining the
conjunctive operands. That issue has been amended in this release, in such a
way that the precendence rules are applied consistently. However, it does mean
that such cases that relied on the previous (inconsistent) parsing will now parse
differently.

A separate tool `Parse Checker <https://github.com/mlaveaux/parse-checker>`_ can
be used to verify whether a given mCRL2 specification or modal formula parses
differently in the 202507.0 release compared to previous releases.