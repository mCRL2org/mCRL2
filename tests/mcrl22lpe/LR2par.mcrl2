% This example can only be parsed unambiguously by an LR(k) parser generator
% for the current grammar, where k > 1. Namely, process expression '(a)'
% cannot be parsed unambiguously. After parsing the left parenthesis '(', it
% has to be determined if it is part of a process or data expression, viz.
% part of the left hand side of a conditional process expression. With a
% lookahead of 1, we may only use the identifier 'a' as extra information,
% which is not enough, because this symbol is also ambiguous.

act
  a;

init
  (a);
