% This is the GenSpect variant of the 'small1' mCRL example.
% This specification is much shorter and more readable than its mCRL counter-
% part, because of the availability of predefined data types.
act
  a,b: Nat;
proc
  X(n: Nat) = sum(m: Nat, a(n).b(m).X(n+1));
init
  X(0);
