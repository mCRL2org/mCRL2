% This is the GenSpect variant of the 'small2' mCRL example.
% This simple process describes an inifite loop of 'a' actions.
% The smallest state space corresponding to this specification
% consists of one state and one transition. 

act
  a;

proc
  X = a.X;

init
  X;
