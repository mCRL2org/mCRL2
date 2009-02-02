% This is an example of a simple process, that describes 
% two parallel processes that can respectively do an a and
% a b, which must communicate to c.

act
  a, b, c;

proc
  X = a.X;
  Y = b.Y;

init
  allow({c},comm({a|b->c},X||Y));
