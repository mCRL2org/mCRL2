% Author(s): Aad Mathijssen, Jeroen Keiren
% Copyright: see the accompanying file COPYING or copy at
% https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
%
% Distributed under the Boost Software License, Version 1.0.
% (See accompanying file LICENSE_1_0.txt or copy at
% http://www.boost.org/LICENSE_1_0.txt)
%
% Specification of the Real data sort.

#include bool.spec
#include pos.spec
#include nat.spec
#include int.spec

sort Real <"real_">;

map @cReal <"creal">: Int <"numerator"> # Pos <"denominator"> -> Real;
    Pos2Real <"pos2real"> : Pos <"arg"> -> Real;
    Nat2Real <"nat2real"> : Nat <"arg"> -> Real;
    Int2Real <"int2real"> : Int <"arg"> -> Real;
    Real2Pos <"real2pos"> : Real <"arg"> -> Pos;
    Real2Nat <"real2nat"> : Real <"arg"> -> Nat;
    Real2Int <"real2int"> : Real <"arg"> -> Int;
    min <"minimum"> : Real <"left"> # Real <"right"> -> Real;
    max <"maximum"> : Real <"left"> # Real <"right"> -> Real;
    abs <"abs"> : Real <"arg"> -> Real;
    - <"negate"> : Real <"arg"> -> Real;
    succ <"succ"> : Real <"arg"> -> Real;
    pred <"pred"> : Real <"arg"> -> Real;
    + <"plus"> : Real <"left"> # Real <"right"> -> Real;
    - <"minus"> : Real <"left"> # Real <"right"> -> Real;
    * <"times"> : Real <"left"> # Real <"right"> -> Real;
    exp <"exp"> : Real <"left"> # Int <"right"> -> Real;
    / <"divides"> : Pos <"left"> # Pos <"right"> -> Real;
    / <"divides"> : Nat <"left"> # Nat <"right"> -> Real;
    / <"divides"> : Int <"left"> # Int <"right"> -> Real;
    / <"divides"> : Real <"left"> # Real <"right"> -> Real;
    floor <"floor"> : Real <"arg"> -> Int;
    ceil <"ceil"> : Real <"arg"> -> Int;
    round <"round"> : Real <"arg"> -> Int;
    @redfrac <"redfrac"> : Int # Int -> Real;
    @redfracwhr <"redfracwhr"> : Pos <"arg1"> # Int <"arg2"> # Nat <"arg3"> -> Real;
    @redfrachlp <"redfrachlp"> : Real <"left"> # Int <"right"> -> Real;

var m:Nat;
    n:Nat;
    p:Pos;
    q:Pos;
    x:Int;
    y:Int;
    r:Real;
    s:Real;

eqn  ==(@cReal(x, p), @cReal(y, q))  =  ==(*(x, @cInt(@cNat(q))), *(y, @cInt(@cNat(p))));
     <(r,r) = false;
     <(@cReal(x, p), @cReal(y, q))  =  <(*(x, @cInt(@cNat(q))), *(y, @cInt(@cNat(p))));
     <=(r,r) = true;
     <=(@cReal(x, p), @cReal(y, q))  =  <=(*(x, @cInt(@cNat(q))), *(y, @cInt(@cNat(p))));
     Int2Real(x)  =  @cReal(x, @c1);
     Nat2Real(n)  =  @cReal(@cInt(n), @c1);
     Pos2Real(p)  =  @cReal(@cInt(@cNat(p)), @c1);
     Real2Int(@cReal(x, @c1))  =  x;
     Real2Nat(@cReal(x, @c1))  =  Int2Nat(x);
     Real2Pos(@cReal(x, @c1))  =  Int2Pos(x);
     min(r, s)  =  if(<(r, s), r, s);
     max(r, s)  =  if(<(r, s), s, r);
     abs(r)  =  if(<(r, @cReal(@cInt(@c0), @c1)), -(r), r);
     -(@cReal(x, p))  =  @cReal(-(x), p);
     succ(@cReal(x, p))  =  @cReal(+(x, @cInt(@cNat(p))), p);
     pred(@cReal(x, p))  =  @cReal(-(x, @cInt(@cNat(p))), p);
     +(@cReal(x, p), @cReal(y, q))  =  @redfrac(+(*(x, @cInt(@cNat(q))), *(y, @cInt(@cNat(p)))), @cInt(@cNat(*(p, q))));
     -(@cReal(x, p), @cReal(y, q))  =  @redfrac(-(*(x, @cInt(@cNat(q))), *(y, @cInt(@cNat(p)))), @cInt(@cNat(*(p, q))));
     *(@cReal(x, p), @cReal(y, q))  =  @redfrac(*(x, y), @cInt(@cNat(*(p, q))));
     !=(y, @cInt(@c0))  ->  /(@cReal(x, p), @cReal(y, q))  =  @redfrac(*(x, @cInt(@cNat(q))), *(y, @cInt(@cNat(p))));
     /(p, q)  =  @redfrac(@cInt(@cNat(p)), @cInt(@cNat(q)));
     !=(m, @c0)  ->  /(m, n)  =  @redfrac(@cInt(m), @cInt(n));
     !=(y, @cInt(@c0))  ->  /(x, y)  =  @redfrac(x, y);
     exp(@cReal(x, p), @cInt(n))  =  @redfrac(exp(x, n), @cInt(@cNat(exp(p, n))));
     !=(x, @cInt(@c0))  ->  exp(@cReal(x, p), @cNeg(q))  =  @redfrac(@cInt(@cNat(exp(p, @cNat(q)))), exp(x, @cNat(q)));
     floor(@cReal(x, p))  =  div(x, p);
     ceil(r)  =  -(floor(-(r)));
     round(r)  =  floor(+(r, @cReal(@cInt(@cNat(@c1)), @cDub(false, @c1))));
     @redfrac(x, @cNeg(p))  =  @redfrac(-(x), @cInt(@cNat(p)));
     @redfrac(x, @cInt(@cNat(p)))  =  @redfracwhr(p, div(x, p), mod(x, p));
     @redfracwhr(p, x, @c0)  =  @cReal(x, @c1);
     @redfracwhr(p, x, @cNat(q))  =  @redfrachlp(@redfrac(@cInt(@cNat(p)), @cInt(@cNat(q))), x);
     @redfrachlp(@cReal(x, p), y)  =  @cReal(+(@cInt(@cNat(p)), *(y, x)), Int2Pos(x));
