% Author(s): Aad Mathijssen, Jeroen Keiren
% Copyright: see the accompanying file COPYING or copy at
% https://github.com/mCRL2org/mCRL2/blob/master/COPYING
%
% Distributed under the Boost Software License, Version 1.0.
% (See accompanying file LICENSE_1_0.txt or copy at
% http://www.boost.org/LICENSE_1_0.txt)
%
% Specification of the Int data sort.

#include bool.spec
#include pos.spec
#include nat.spec
#supertypeof Pos
#supertypeof Nat

sort Int <"int_">;

cons @cInt <"cint"> : Nat <"arg"> -> Int;
     @cNeg <"cneg"> : Pos <"arg"> -> Int;

map Nat2Int <"nat2int"> : Nat <"arg"> -> Int;
    Int2Nat <"int2nat"> : Int <"arg"> -> Nat;
    Pos2Int <"pos2int"> : Pos <"arg"> -> Int;
    Int2Pos <"int2pos"> : Int <"arg"> -> Pos;
    max <"maximum">:Pos <"left"> #Int <"right">->Pos;
    max <"maximum">:Int <"left"> #Pos <"right">->Pos;
    max <"maximum">:Nat <"left"> #Int <"right">->Nat;
    max <"maximum">:Int <"left"> #Nat <"right">->Nat;
    max <"maximum">:Int <"left"> #Int <"right">->Int;
    min <"minimum">:Int <"left"> #Int <"right">->Int;
    abs <"abs">:Int <"arg">->Nat;
    - <"negate">:Pos <"arg">->Int;
    - <"negate">:Nat <"arg">->Int;
    - <"negate">:Int <"arg">->Int;
    succ <"succ">:Int <"arg">->Int;
    pred <"pred">:Nat <"arg">->Int;
    pred <"pred">:Int <"arg">->Int;
    + <"plus">:Int <"left"> #Int <"right">->Int;
    - <"minus">:Pos <"left"> # Pos <"right">->Int;
    - <"minus">:Nat <"left"> # Nat <"right">->Int;
    - <"minus">:Int <"left"> # Int <"right">->Int;
    * <"times">:Int <"left"> # Int <"right">->Int;
    div <"div">: Int <"left"> # Pos <"right"> -> Int;
    mod <"mod">:Int <"left"> # Pos <"right"> -> Nat;
    exp <"exp">:Int <"left"> # Nat <"right"> -> Int;

var b:Bool;
    n:Nat;
    m:Nat;
    p:Pos;
    q:Pos;
    x:Int;
    y:Int;

eqn ==(@cInt(m),@cInt(n)) = ==(m,n);
    ==(@cInt(n), @cNeg(p)) = false;
    ==(@cNeg(p), @cInt(n)) = false;
    ==(@cNeg(p),@cNeg(q)) = ==(p,q);
    <(@cInt(m),@cInt(n)) = <(m,n);
    <(@cInt(n),@cNeg(p)) = false;
    <(@cNeg(p),@cInt(n)) = true;
    <(@cNeg(p),@cNeg(q)) = <(q,p);
    <=(@cInt(m),@cInt(n)) = <=(m,n);
    <=(@cInt(n),@cNeg(p)) = false;
    <=(@cNeg(p),@cInt(n)) = true;
    <=(@cNeg(p),@cNeg(q)) = <=(q,p);
    Nat2Int(n) = @cInt(n);
    Int2Nat(@cInt(n)) = n;
    Pos2Int(p) = @cInt(@cNat(p));
    Int2Pos(@cInt(n)) = Nat2Pos(n);
    max(p,@cInt(n)) = max(p,n);
    max(p,@cNeg(q)) = p;
    max(@cInt(n),p) = max(n,p);
    max(@cNeg(q),p) = p;
    max(m,@cInt(n)) = if(<=(m,n),n,m);
    max(n,@cNeg(p)) = n;
    max(@cInt(m),n) = if(<=(m,n),n,m);
    max(@cNeg(p),n) = n;
    max(x,y) = if(<=(x,y),y,x);
    min(x,y) = if(<=(x,y),x,y);
    abs(@cInt(n)) = n;
    abs(@cNeg(p)) = @cNat(p);
    -(p) = @cNeg(p);
    -(@c0) = @cInt(@c0);
    -(@cNat(p)) = @cNeg(p);
    -(@cInt(n)) = -(n);
    -(@cNeg(p)) = @cInt(@cNat(p));
    succ(@cInt(n)) = @cInt(@cNat(succ(n)));
    succ(@cNeg(p)) = -(pred(p));
    pred(@c0) = @cNeg(@c1);
    pred(@cNat(p)) = @cInt(pred(p));
    pred(@cInt(n)) = pred(n);
    pred(@cNeg(p)) = @cNeg(succ(p));
    +(@cInt(m),@cInt(n)) = @cInt(+(m,n));
    +(@cInt(n),@cNeg(p)) = -(n,@cNat(p));
    +(@cNeg(p),@cInt(n)) = -(n,@cNat(p));
    +(@cNeg(p),@cNeg(q)) = @cNeg(@addc(false,p,q));
    <=(q,p) -> -(p,q) = @cInt(@gtesubtb(false,p,q));
    <(p,q) -> -(p,q) = -(@gtesubtb(false,q,p));
    <=(n,m) -> -(m,n) = @cInt(@monus(m,n));
    <(m,n) -> -(m,n) = -(@monus(n,m));
    -(x,y) = +(x, -(y));
    *(@cInt(m),@cInt(n)) = @cInt(*(m,n));
    *(@cInt(n),@cNeg(p)) = -(*(@cNat(p),n));
    *(@cNeg(p),@cInt(n)) = -(*(@cNat(p),n));
    *(@cNeg(p),@cNeg(q)) = @cInt(@cNat(*(p,q)));
    div(@cInt(n),p) = @cInt(div(n,p));
    div(@cNeg(p),q) = @cNeg(succ(div(pred(p),q)));
    mod(@cInt(n),p) = mod(n,p);
    mod(@cNeg(p),q) = Int2Nat(-(q,succ(mod(pred(p),q))));
    exp(@cInt(m),n) = @cInt(exp(m,n));
    @even(n) -> exp(@cNeg(p),n) = @cInt(@cNat(exp(p,n)));
    !(@even(n)) -> exp(@cNeg(p),n) = @cNeg(exp(p,n));
