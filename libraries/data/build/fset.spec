% Author(s): Aad Mathijssen, Jeroen Keiren
% Copyright: see the accompanying file COPYING or copy at
% https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
%
% Distributed under the Boost Software License, Version 1.0.
% (See accompanying file LICENSE_1_0.txt or copy at
% http://www.boost.org/LICENSE_1_0.txt)
%
% Specification of the FSet data sort, denoting finite sets.

#include bool.spec

sort FSet(S) <"fset"> = struct @fset_empty <"fset_empty"> | @fset_cons <"fset_cons"> : S <"head"> # FSet(S) <"tail">;

map @fset_insert <"fsetinsert">: S <"left"> # FSet(S) <"right"> -> FSet(S);
    @fset_cinsert <"fsetcinsert">: S <"arg1"> # Bool <"arg2"> # FSet(S) <"arg3"> -> FSet(S);
    @fset_in <"fsetin">: S <"left"> # FSet(S) <"right"> -> Bool;
    @fset_lte <"fsetlte">: (S -> Bool) <"arg1"> # FSet(S) <"arg2"> # FSet(S) <"arg3"> -> Bool;
    @fset_union <"fsetunion"> : (S -> Bool) <"arg1"> # (S -> Bool) <"arg2"> # FSet(S) <"arg3"> # FSet(S) <"arg4"> -> FSet(S);
    @fset_inter <"fsetintersection">: (S -> Bool) <"arg1"> # (S -> Bool) <"arg2"> # FSet(S) <"arg3"> # FSet(S) <"arg4"> -> FSet(S);

var d:S;
    e:S;
    f:S->Bool;
    g:S->Bool;
    s:FSet(S);
    t:FSet(S);
eqn @fset_insert(d, @fset_empty)  =  @fset_cons(d, @fset_empty);
    @fset_insert(d, @fset_cons(d, s))  =  @fset_cons(d, s);
    <(d, e)  ->  @fset_insert(d, @fset_cons(e, s))  =  @fset_cons(d, @fset_cons(e, s));
    <(e, d)  ->  @fset_insert(d, @fset_cons(e, s))  =  @fset_cons(e, @fset_insert(d, s));
    @fset_cinsert(d, false, s)  =  s;
    @fset_cinsert(d, true, s)  =  @fset_insert(d, s);
    @fset_in(d, @fset_empty)  =  false;
    @fset_in(d, @fset_cons(d, s))  =  true;
    <(d, e)  ->  @fset_in(d, @fset_cons(e, s))  =  false;
    <(e, d)  ->  @fset_in(d, @fset_cons(e, s))  =  @fset_in(d, s);
    @fset_lte(f, @fset_empty, @fset_empty)  =  true;
    @fset_lte(f, @fset_cons(d, s), @fset_empty)  =  &&(f(d), @fset_lte(f, s, @fset_empty));
    @fset_lte(f, @fset_empty, @fset_cons(e, t))  =  &&(!(f(e)), @fset_lte(f, @fset_empty, t));
    @fset_lte(f, @fset_cons(d, s), @fset_cons(d, t))  =  @fset_lte(f, s, t);
    <(d, e)  ->  @fset_lte(f, @fset_cons(d, s), @fset_cons(e, t))  =  &&(f(d), @fset_lte(f, s, @fset_cons(e, t)));
    <(e, d)  ->  @fset_lte(f, @fset_cons(d, s), @fset_cons(e, t))  =  &&(!(f(e)), @fset_lte(f, @fset_cons(d, s), t));
    @fset_union(f, g, @fset_empty, @fset_empty)  =  @fset_empty;
    @fset_union(f, g, @fset_cons(d, s), @fset_empty)  =  @fset_cinsert(d, !(g(d)), @fset_union(f, g, s, @fset_empty));
    @fset_union(f, g, @fset_empty, @fset_cons(e, t))  =  @fset_cinsert(e, !(f(e)), @fset_union(f, g, @fset_empty, t));
    @fset_union(f, g, @fset_cons(d, s), @fset_cons(d, t))  =  @fset_cinsert(d, ==(f(d), g(d)), @fset_union(f, g, s, t));
    <(d, e)  ->  @fset_union(f, g, @fset_cons(d, s), @fset_cons(e, t))  =  @fset_cinsert(d, !(g(d)), @fset_union(f, g, s, @fset_cons(e, t)));
    <(e, d)  ->  @fset_union(f, g, @fset_cons(d, s), @fset_cons(e, t))  =  @fset_cinsert(e, !(f(e)), @fset_union(f, g, @fset_cons(d, s), t));
    @fset_inter(f, g, @fset_empty, @fset_empty)  =  @fset_empty;
    @fset_inter(f, g, @fset_cons(d, s), @fset_empty)  =  @fset_cinsert(d, g(d), @fset_inter(f, g, s, @fset_empty));
    @fset_inter(f, g, @fset_empty, @fset_cons(e, t))  =  @fset_cinsert(e, f(e), @fset_inter(f, g, @fset_empty, t));
    @fset_inter(f, g, @fset_cons(d, s), @fset_cons(d, t))  =  @fset_cinsert(d, ==(f(d), g(d)), @fset_inter(f, g, s, t));
    <(d, e)  ->  @fset_inter(f, g, @fset_cons(d, s), @fset_cons(e, t))  =  @fset_cinsert(d, g(d), @fset_inter(f, g, s, @fset_cons(e, t)));
    <(e, d)  ->  @fset_inter(f, g, @fset_cons(d, s), @fset_cons(e, t))  =  @fset_cinsert(e, f(e), @fset_inter(f, g, @fset_cons(d, s), t));
    ==(@fset_empty, @fset_empty)  =  true;
    ==(@fset_empty, @fset_cons(e, s))  =  false;
    ==(@fset_cons(e, s), @fset_empty)  =  false;
    ==(@fset_cons(e, t), @fset_cons(d, s))  =  &&(==(e, d), ==(t, s));
    <(@fset_empty, @fset_empty)  =  false;
    <(@fset_empty, @fset_cons(e, s))  =  true;
    <(@fset_cons(e, s), @fset_empty)  =  false;
    <(@fset_cons(e, t), @fset_cons(d, s))  =  ||(<(e, d), &&(==(e, d), <(t, s)));
    <=(@fset_empty, @fset_empty)  =  true;
    <=(@fset_empty, @fset_cons(e, s))  =  true;
    <=(@fset_cons(e, s), @fset_empty)  =  false;
    <=(@fset_cons(e, t), @fset_cons(d, s))  =  ||(<(e, d), &&(==(e, d), <=(t, s)));
