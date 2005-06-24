%                                   Function composition
%                                   --------------------
%
% This is a GenSpect specification of the composition f ° g for function f of
% type B -> C and function g of type A -> B.

sort
  A,B,C;

map
  comp: (B -> C) # (A -> B) -> (A -> C);
  
var
  f: B -> C;
  g: A -> B;
eqn
  comp(f,g) = lambda x:A.f(g(x));

init
  delta;
