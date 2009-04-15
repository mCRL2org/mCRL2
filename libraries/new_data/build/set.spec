% Author(s): Aad Mathijssen, Jeroen Keiren
% Copyright: see the accompanying file COPYING or copy at
% https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
%
% Distributed under the Boost Software License, Version 1.0.
% (See accompanying file LICENSE_1_0.txt or copy at
% http://www.boost.org/LICENSE_1_0.txt)
%
% Specification of the Set data sort.

#include bool.spec
#include fset.spec

sort Set(S) <"set_">;

map @set <"setconstructor"> : (S -> Bool) <"left"> # FSet(S) <"right"> -> Set(S);
    {} <"emptyset"> : Set(S);
    @setfset <"setfset"> : FSet(S) <"arg"> -> Set(S);
    @setcomp <"setcomprehension"> : (S -> Bool) <"arg"> -> Set(S);
    in <"setin"> : S <"left"> # Set(S) <"right"> -> Bool;
    ! <"setcomplement"> : Set(S) <"arg"> -> Set(S);
    + <"setunion_"> : Set(S) <"left"> # Set(S) <"right"> -> Set(S);
    * <"setintersection"> : Set(S) <"left"> # Set(S) <"right"> -> Set(S);
    - <"setdifference"> : Set(S) <"left"> # Set(S) <"right"> -> Set(S);
    @false_ <"false_function"> : S <"arg"> -> Bool;
    @true_ <"true_function"> : S <"arg"> -> Bool;
    @not_ <"not_function"> : (S -> Bool) <"arg"> -> S -> Bool;
    @and_ <"and_function"> : (S -> Bool) <"left"> # (S -> Bool) <"right"> -> S -> Bool;
    @or_ <"or_function"> : (S -> Bool) <"left"> # (S -> Bool) <"right"> -> S -> Bool;

var e : S;
    d : S;
    s : FSet(S);
    t : FSet(S);
    f : S->Bool;
    g : S->Bool;
    x : Set(S);
    y : Set(S);
eqn {}  =  @set(@false_, @fset_empty);
    @setfset(s)  =  @set(@false_, s);
    @setcomp(f)  =  @set(f, @fset_empty);
    in(e, @set(f, s))  =  !=(f(e), @fset_in(e, s));
    ==(f, g)  ->  ==(@set(f, s), @set(g, t))  =  ==(s, t);
    !=(f, g)  ->  ==(@set(f, s), @set(g, t))  =  forall(c:S, ==(in(c, @set(f, s)), in(c, @set(g, t))));
    <(x, y)  =  &&(<=(x, y), !=(x, y));
    ==(f, g)  ->  <=(@set(f, s), @set(g, t))  =  @fset_lte(f, s, t);
    !=(f, g)  ->  <=(@set(f, s), @set(g, t))  =  forall(c:S, =>(in(c, @set(f, s)), in(c, @set(g, t))));
    !(@set(f, s))  =  @set(@not_(f), s);
    +(@set(f, s), @set(g, t))  =  @set(@or_(f, g), @fset_union(f, g, s, t));
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
    @or_(f, g)(e)  =  ||(f(e), g(e));
    @or_(f, f)  =  f;
    @or_(f, @false_)  =  f;
    @or_(@false_, f)  =  f;
    @or_(f, @true_)  =  @true_;
    @or_(@true_, f)  =  @true_;


