% This example can only be parsed unambiguously by an LR(k) parser generator
% for the current grammar, where k > 1. Namely, process expression 'a + tau'
% cannot be parsed unambiguously. After parsing the identifier 'a', it has to
% be determined if 'a' is an action or process reference, or if 'a' is a data
% expression, viz. part of the left hand side of a conditional process
% expression. With a lookahead of 1, we may only use the '+' as extra
% information, which is not enough, because this symbol is also ambiguous.

act
  a;

init
  a + tau;
