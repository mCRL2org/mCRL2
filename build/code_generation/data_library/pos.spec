% Author(s): Aad Mathijssen, Jeroen Keiren
% Copyright: see the accompanying file COPYING or copy at
% https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
%
% Distributed under the Boost Software License, Version 1.0.
% (See accompanying file LICENSE_1_0.txt or copy at
% http://www.boost.org/LICENSE_1_0.txt)
%
% Specification of the Pos data sort.

#include bool.spec

sort Pos <"pos">;

cons @c1 <"c1">:Pos;
     @cDub <"cdub">:Bool <"left"> #Pos <"right"> ->Pos;

map max <"maximum">:Pos <"left"> #Pos <"right">->Pos;
    min <"minimum">:Pos <"left"> #Pos <"right">->Pos;
    succ <"succ">:Pos <"arg">->Pos;
    @pospred <"pos_predecessor">:Pos <"arg">->Pos;
    + <"plus">:Pos <"left"> #Pos <"right">->Pos;
    @addc <"add_with_carry">:Bool <"arg1"> #Pos <"arg2"> #Pos <"arg3">->Pos;
    * <"times">:Pos <"left"> #Pos <"right">->Pos;
    @monusc <"monus_with_carry">:Bool <"arg1"> #Pos <"arg2"> #Pos <"arg3">->Pos;

var b:Bool;
    c:Bool;
    p:Pos;
    q:Pos;
eqn ==(@c1, @cDub(b,p)) = false;
    ==(@cDub(b,p), @c1) = false;
    ==(@cDub(b,p), @cDub(c, q)) = &&(==(b,c), ==(p,q));
    ==(succ(p),@c1) = false;
    ==(@c1,succ(q)) = false;
    ==(succ(p),@cDub(c,q)) = ==(p,@pospred(@cDub(c,q)));
    ==(@cDub(b,p),succ(q)) = ==(@pospred(@cDub(b,p)),q);
    <(p, @c1) = false;
    <(@c1, @cDub(b,p)) = true;
% The equation below is required for the enumeration of lists
    <(@cDub(b,p), @cDub(c,q)) = if(=>(c,b), <(p,q), <=(p,q));
    <(succ(p),@cDub(c,q)) = <(p,@pospred(@cDub(c,q)));
    <(@cDub(b,p),succ(q)) = <=(@cDub(b,p),q);
    <(@c1,succ(q)) = true;
    <=(@c1, p) = true;
    <=(@cDub(b,p), @c1) = false;
% The equation below is required for the enumeration of lists
    <=(@cDub(b,p), @cDub(c,q)) = if(=>(b,c), <=(p,q), <(p,q));
    <=(succ(p),@cDub(c,q)) = <(p,@cDub(c,q));
    <=(@cDub(b,p),succ(q)) = <=(@pospred(@cDub(b,p)),q);
    <=(succ(p),@c1) = false;
    max(p,q) = if(<=(p,q),q,p);
    min(p,q) = if(<=(p,q),p,q);
    succ(@c1) = @cDub(false, @c1);
    succ(@cDub(false,p)) = @cDub(true,p);
    succ(@cDub(true,p)) = @cDub(false,succ(p));
    @pospred(@c1) = @c1;
    @pospred(@cDub(false,@c1)) = @c1;
    @pospred(@cDub(false,@cDub(b,p))) = @cDub(true,@pospred(@cDub(b,p)));
    @pospred(@cDub(true,p)) = @cDub(false,p);
    +(p,q) = @addc(false,p,q);
    @addc(false,@c1,p) = succ(p);
    @addc(true,@c1,p) = succ(succ(p));
    @addc(false,p,@c1) = succ(p);
    @addc(true,p,@c1) = succ(succ(p));
    @addc(b,@cDub(c,p),@cDub(c,q)) = @cDub(b,@addc(c,p,q));
    @addc(b,@cDub(false,p),@cDub(true,q)) = @cDub(!(b), @addc(b,p,q));
    @addc(b,@cDub(true,p),@cDub(false,q)) = @cDub(!(b), @addc(b,p,q));
    *(@c1,p) = p;
    *(p,@c1) = p;
    *(@cDub(false,p),q) = @cDub(false,*(p,q));
    *(p,@cDub(false,q)) = @cDub(false,*(p,q));
    *(@cDub(true,p),@cDub(true,q)) = @cDub(true,@addc(false,p,@addc(false,q,@cDub(false,*(p,q)))));
    @monusc(b,@c1,p) = @c1;
    @monusc(false,@cDub(false,p),@c1) = @cDub(true,@monusc(false,p,@c1));
    @monusc(false,@cDub(true,p),@c1) = @cDub(false,p);
    @monusc(true,@cDub(b,p),@c1) = @cDub(b,@monusc(false,p,@c1));
    @monusc(b,@cDub(c,p),@cDub(c,q)) = @cDub(b,@monusc(b,p,q));
    @monusc(b,@cDub(false,p),@cDub(true,q)) = @cDub(!(b), @monusc(true,p,q));
    @monusc(b,@cDub(true,p),@cDub(false,q)) = @cDub(!(b), @monusc(false,p,q));
