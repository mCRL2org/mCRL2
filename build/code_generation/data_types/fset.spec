% Author(s): Aad Mathijssen, Jeroen Keiren
% Copyright: see the accompanying file COPYING or copy at
% https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
%
% Distributed under the Boost Software License, Version 1.0.
% (See accompanying file LICENSE_1_0.txt or copy at
% http://www.boost.org/LICENSE_1_0.txt)
%
% Specification of the FSet data sort, denoting finite sets.

#using S
#include bool.spec
#include nat.spec

sort FSet(S) <"fset"> = struct {} <"empty"> | @fset_cons <"cons_"> : S <"left"> # FSet(S) <"right">;

map @fset_insert <"insert">: S <"left"> # FSet(S) <"right"> -> FSet(S);
    @fset_cinsert <"cinsert">: S <"arg1"> # Bool <"arg2"> # FSet(S) <"arg3"> -> FSet(S);
    in <"in">: S <"left"> # FSet(S) <"right"> -> Bool;
    @fset_union <"fset_union"> : (S -> Bool) <"arg1"> # (S -> Bool) <"arg2"> # FSet(S) <"arg3"> # FSet(S) <"arg4"> -> FSet(S);
    @fset_inter <"fset_intersection">: (S -> Bool) <"arg1"> # (S -> Bool) <"arg2"> # FSet(S) <"arg3"> # FSet(S) <"arg4"> -> FSet(S);
    - <"difference">: FSet(S) <"left"> # FSet(S) <"right"> -> FSet(S);
    + <"union_"> : FSet(S) <"left"> # FSet(S) <"right"> -> FSet(S);
    * <"intersection"> : FSet(S) <"left"> # FSet(S) <"right"> -> FSet(S);
    # <"count"> : FSet(S) <"arg"> -> Nat;

var d:S;
    e:S;
    f:S->Bool;
    g:S->Bool;
    s:FSet(S);
    t:FSet(S);
eqn @fset_insert(d, {})  =  @fset_cons(d, {});
    @fset_insert(d, @fset_cons(d, s))  =  @fset_cons(d, s);
    <(d, e)  ->  @fset_insert(d, @fset_cons(e, s))  =  @fset_cons(d, @fset_cons(e, s));
    <(e, d)  ->  @fset_insert(d, @fset_cons(e, s))  =  @fset_cons(e, @fset_insert(d, s));
    @fset_cinsert(d, false, s)  =  s;
    @fset_cinsert(d, true, s)  =  @fset_insert(d, s);
    in(d, {})  =  false;
    in(d,@fset_cons(e,s)) = ||(==(d,e),in(d,s));
% The rule below is added such that set membership can still be calculated although the set elements cannot be effectively ordered.
    in(d,@fset_insert(e,s)) = ||(==(d,e),in(d,s));
    @fset_union(f, g, {}, {})  =  {};
    @fset_union(f, g, @fset_cons(d, s), {})  =  @fset_cinsert(d, !(g(d)), @fset_union(f, g, s, {}));
    @fset_union(f, g, {}, @fset_cons(e, t))  =  @fset_cinsert(e, !(f(e)), @fset_union(f, g, {}, t));
    @fset_union(f, g, @fset_cons(d, s), @fset_cons(d, t))  =  @fset_cinsert(d, ==(f(d), g(d)), @fset_union(f, g, s, t));
    <(d, e)  ->  @fset_union(f, g, @fset_cons(d, s), @fset_cons(e, t))  =  @fset_cinsert(d, !(g(d)), @fset_union(f, g, s, @fset_cons(e, t)));
    <(e, d)  ->  @fset_union(f, g, @fset_cons(d, s), @fset_cons(e, t))  =  @fset_cinsert(e, !(f(e)), @fset_union(f, g, @fset_cons(d, s), t));
    @fset_inter(f, g, {}, {})  =  {};
    @fset_inter(f, g, @fset_cons(d, s), {})  =  @fset_cinsert(d, g(d), @fset_inter(f, g, s, {}));
    @fset_inter(f, g, {}, @fset_cons(e, t))  =  @fset_cinsert(e, f(e), @fset_inter(f, g, {}, t));
    @fset_inter(f, g, @fset_cons(d, s), @fset_cons(d, t))  =  @fset_cinsert(d, ==(f(d), g(d)), @fset_inter(f, g, s, t));
    <(d, e)  ->  @fset_inter(f, g, @fset_cons(d, s), @fset_cons(e, t))  =  @fset_cinsert(d, g(d), @fset_inter(f, g, s, @fset_cons(e, t)));
    <(e, d)  ->  @fset_inter(f, g, @fset_cons(d, s), @fset_cons(e, t))  =  @fset_cinsert(e, f(e), @fset_inter(f, g, @fset_cons(d, s), t));
    -(s,{}) = s;
    -({},t) = {};
    -(@fset_cons(d,s),@fset_cons(d,t)) = -(s,t);
    <(d,e) -> -(@fset_cons(d,s),@fset_cons(e,t)) = @fset_cons(d,-(s,@fset_cons(e,t)));
    <(e,d) -> -(@fset_cons(d,s),@fset_cons(e,t)) = -(@fset_cons(d,s),t);
    +(s,{}) = s;
    +({},t) = t;
    +(@fset_cons(d,s),@fset_cons(d,t)) = @fset_cons(d,+(s,t));
    <(d,e) -> +(@fset_cons(d,s),@fset_cons(e,t)) = @fset_cons(d,+(s,@fset_cons(e,t)));
    <(e,d) -> +(@fset_cons(d,s),@fset_cons(e,t)) = @fset_cons(e,+(@fset_cons(d,s),t));
    *(s,{}) = {};
    *({},t) = {};
    *(@fset_cons(d,s),@fset_cons(d,t)) = @fset_cons(d,*(s,t));
    <(d,e) -> *(@fset_cons(d,s),@fset_cons(e,t)) = *(s,@fset_cons(e,t));
    <(e,d) -> *(@fset_cons(d,s),@fset_cons(e,t)) = *(@fset_cons(d,s),t);
    #({}) = @c0;
    #(@fset_cons(d,s)) = @cNat(succ(#(s)));
% It is odd that the rule below has to be added separately.
    !=(s,t) = !(==(s,t));
