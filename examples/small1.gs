% This is the GenSpect variant of the 'small1' mCRL example.
% The process exhibits a simple sequence of a and b actions,
% where the parameter of 'a' is a increasing number and the parameter
% of 'b' can be freely chosen. The state space of this example is infinite
% and it is also infinitely branching.
%
% Note that this specification is much shorter and more readable 
% than its mCRL counterpart, because of the availability of predefined 
% data types.

act
  a,b: Nat;

proc
  X(n: Nat) = sum m:Nat. a(n).b(m).X(n+1);

init
  X(0);
