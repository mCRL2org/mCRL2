% Author(s): Aad Mathijssen, Jeroen Keiren
% Copyright: see the accompanying file COPYING or copy at
% https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
%
% Distributed under the Boost Software License, Version 1.0.
% (See accompanying file LICENSE_1_0.txt or copy at
% http://www.boost.org/LICENSE_1_0.txt)
%
% Specification of the Set data sort.

#using S
#include bool.spec
#include fset.spec
#supertypeof FSet

sort Set(S) <"set_">;

cons @set <"constructor"> : (S -> Bool) <"left"> # FSet(S) <"right"> -> Set(S);
% map {} <"empty"> : Set(S); Move this to FSet(S);
% I think that @setfset and @setcomp should not be part of the rewrite system, but
% become part of the internal generation of set representations. JFG
map @setfset <"set_fset"> : FSet(S) <"arg"> -> Set(S);
    @setcomp <"set_comprehension"> : (S -> Bool) <"arg"> -> Set(S);
    in <"in"> : S <"left"> # Set(S) <"right"> -> Bool;
    ! <"complement"> : Set(S) <"arg"> -> Set(S);
    + <"union_"> : Set(S) <"left"> # Set(S) <"right"> -> Set(S);
    * <"intersection"> : Set(S) <"left"> # Set(S) <"right"> -> Set(S);
    - <"difference"> : Set(S) <"left"> # Set(S) <"right"> -> Set(S);
    @false_ <"false_function"> : S <"arg"> -> Bool;
    @true_ <"true_function"> : S <"arg"> -> Bool;
    @not_ <"not_function"> : (S -> Bool) <"arg"> -> S -> Bool;
    @and_ <"and_function"> : (S -> Bool) <"left"> # (S -> Bool) <"right"> -> S -> Bool;
    @or_ <"or_function"> : (S -> Bool) <"left"> # (S -> Bool) <"right"> -> S -> Bool;

var e : S;
    s : FSet(S);
    t : FSet(S);
    f : S->Bool;
    g : S->Bool;
    x : Set(S);
    y : Set(S);
% eqn {}  =  @set(@false_, {});
eqn @setfset(s)  =  @set(@false_, s);
    @setcomp(f)  =  @set(f, {});
    in(e, @set(f, s))  =  !=(f(e), in(e, s));
    ==(@set(f, s), @set(g, t))  =  forall(c:S, !=(==(f(c),g(c)),in(c,-(s,t))));
    <(x, y)  =  &&(<=(x, y), !=(x, y));
    <=(x,y) = ==(*(x,y),x);
    !(@set(f, s))  =  @set(@not_(f), s);
    +(x, x) = x;
    +(x, +(x, y)) = +(x, y);
    +(x, +(y, x)) = +(y, x);
    +(+(x, y), x) = +(x, y);
    +(+(y, x), x) = +(y, x);
    +(@set(f, s), @set(g, t))  =  @set(@or_(f, g), @fset_union(f, g, s, t));
    *(x, x) = x;
    *(x, *(x, y)) = *(x, y);
    *(x, *(y, x)) = *(y, x);
    *(*(x, y), x) = *(x, y);
    *(*(y, x), x) = *(y, x);
    *(@set(f, s), @set(g, t))  =  @set(@and_(f, g), @fset_inter(f, g, s, t));
    -(x, y)  =  *(x, !(y));
    @false_(e)  =  false;
    @true_(e)  =  true;
    ==(@false_, @true_)  =  false;
    ==(@true_, @false_)  =  false;
    @not_(f)(e)  =  !(f(e));
    @not_(@false_)  =  @true_;
    @not_(@true_)  =  @false_;
    @and_(f, g)(e)  =  &&(f(e), g(e));
    @and_(f, f)  =  f;
    @and_(f, @false_)  =  @false_;
    @and_(@false_, f)  =  @false_;
    @and_(f, @true_)  =  f;
    @and_(@true_, f)  =  f;
    @or_(f, f)  =  f;
    @or_(f, @false_)  =  f;
    @or_(@false_, f)  =  f;
    @or_(f, @true_)  =  @true_;
    @or_(@true_, f)  =  @true_;
    @or_(f, g)(e)  =  ||(f(e), g(e));

