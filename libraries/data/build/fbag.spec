% Author(s): Aad Mathijssen, Jeroen Keiren
% Copyright: see the accompanying file COPYING or copy at
% https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
%
% Distributed under the Boost Software License, Version 1.0.
% (See accompanying file LICENSE_1_0.txt or copy at
% http://www.boost.org/LICENSE_1_0.txt)
%
% Specification of the FBag data sort, denoting finite bags.

#using S
#include bool.spec
#include pos.spec
#include nat.spec
#include fset.spec

sort FBag(S) <"fbag"> = struct @fbag_empty <"empty"> | @fbag_cons <"cons_"> : S <"arg1"> # Pos <"arg2"> # FBag(S) <"arg3">;

map @fbag_insert <"insert"> : S <"arg1"> # Pos <"arg2"> # FBag(S) <"arg3"> -> FBag(S);
    @fbag_cinsert <"cinsert"> : S <"arg1"> # Nat <"arg2"> # FBag(S) <"arg3"> -> FBag(S);
    @fbag_count <"count"> : S <"left"> # FBag(S) <"right"> -> Nat;
    @fbag_in <"in"> : S <"left"> # FBag(S) <"right"> -> Bool;
    @fbag_join <"join"> : (S -> Nat) <"arg1"> # (S -> Nat) <"arg2"> # FBag(S) <"arg3"> # FBag(S) <"arg4"> -> FBag(S);
    @fbag_inter <"intersect">: (S -> Nat) <"arg1"> # (S -> Nat) <"arg2"> # FBag(S) <"arg3"> # FBag(S) <"arg4"> -> FBag(S);
    @fbag_diff <"difference">: (S -> Nat) <"arg1"> # (S -> Nat) <"arg2"> # FBag(S) <"arg3"> # FBag(S) <"arg4"> -> FBag(S);
    @fbag2fset <"fbag2fset">: (S -> Nat) <"left"> # FBag(S) <"right"> -> FSet(S);
    @fset2fbag <"fset2fbag">: FSet(S) <"arg"> -> FBag(S);

var d: S;
    e: S;
    p: Pos;
    q: Pos;
    b: FBag(S);
    c: FBag(S);
    s: FSet(S);
    f: S -> Nat;
    g: S -> Nat;
eqn @fbag_insert(d, p, @fbag_empty)  =  @fbag_cons(d, p, @fbag_empty);
     @fbag_insert(d, p, @fbag_cons(d, q, b))  =  @fbag_cons(d, +(p, q), b);
     <(d, e)  ->  @fbag_insert(d, p, @fbag_cons(e, q, b))  =  @fbag_cons(d, p, @fbag_cons(e, q, b));
     <(e, d)  ->  @fbag_insert(d, p, @fbag_cons(e, q, b))  =  @fbag_cons(e, q, @fbag_insert(d, p, b));
     @fbag_cinsert(d, @c0, b)  =  b;
     @fbag_cinsert(d, @cNat(p), b)  =  @fbag_insert(d, p, b);
     @fbag_count(d, @fbag_empty)  =  @c0;
     @fbag_count(d, @fbag_cons(d, p, b))  =  @cNat(p);
     <(d, e)  ->  @fbag_count(d, @fbag_cons(e, p, b))  =  @c0;
     <(e, d)  ->  @fbag_count(d, @fbag_cons(e, p, b))  =  @fbag_count(d, b);
     @fbag_in(d, b)  =  >(@fbag_count(d, b), @c0);
%     @fbag_lte(f, @fbag_empty, @fbag_empty)  =  true;
%     @fbag_lte(f, @fbag_cons(d, p, b), @fbag_empty)  =  &&(@swap_zero_lte(f(d), @cNat(p), @c0), @fbag_lte(f, b, @fbag_empty));
%     @fbag_lte(f, @fbag_empty, @fbag_cons(e, q, c))  =  &&(@swap_zero_lte(f(e), @c0, @cNat(q)), @fbag_lte(f, @fbag_empty, c));
%     @fbag_lte(f, @fbag_cons(d, p, b), @fbag_cons(d, q, c))  =  &&(@swap_zero_lte(f(d), @cNat(p), @cNat(q)), @fbag_lte(f, b, c));
%     <(d, e)  ->  @fbag_lte(f, @fbag_cons(d, p, b), @fbag_cons(e, q, c))  =  &&(@swap_zero_lte(f(d), @cNat(p), @c0), @fbag_lte(f, b, @fbag_cons(e, q, c)));
%     <(e, d)  ->  @fbag_lte(f, @fbag_cons(d, p, b), @fbag_cons(e, q, c))  =  &&(@swap_zero_lte(f(e), @c0, @cNat(q)), @fbag_lte(f, @fbag_cons(d, p, b), c));
     @fbag_join(f, g, @fbag_empty, @fbag_empty)  =  @fbag_empty;
     @fbag_join(f, g, @fbag_cons(d, p, b), @fbag_empty)  =  @fbag_cinsert(d, @swap_zero_add(f(d), g(d), @cNat(p), @c0), @fbag_join(f, g, b, @fbag_empty));
     @fbag_join(f, g, @fbag_empty, @fbag_cons(e, q, c))  =  @fbag_cinsert(e, @swap_zero_add(f(e), g(e), @c0, @cNat(q)), @fbag_join(f, g, @fbag_empty, c));
     @fbag_join(f, g, @fbag_cons(d, p, b), @fbag_cons(d, q, c))  =  @fbag_cinsert(d, @swap_zero_add(f(d), g(d), @cNat(p), @cNat(q)), @fbag_join(f, g, b, c));
     <(d, e)  ->  @fbag_join(f, g, @fbag_cons(d, p, b), @fbag_cons(e, q, c))  =  @fbag_cinsert(d, @swap_zero_add(f(d), g(d), @cNat(p), @c0), @fbag_join(f, g, b, @fbag_cons(e, q, c)));
     <(e, d)  ->  @fbag_join(f, g, @fbag_cons(d, p, b), @fbag_cons(e, q, c))  =  @fbag_cinsert(e, @swap_zero_add(f(e), g(e), @c0, @cNat(q)), @fbag_join(f, g, @fbag_cons(d, p, b), c));
     @fbag_inter(f, g, @fbag_empty, @fbag_empty)  =  @fbag_empty;
     @fbag_inter(f, g, @fbag_cons(d, p, b), @fbag_empty)  =  @fbag_cinsert(d, @swap_zero_min(f(d), g(d), @cNat(p), @c0), @fbag_inter(f, g, b, @fbag_empty));
     @fbag_inter(f, g, @fbag_empty, @fbag_cons(e, q, c))  =  @fbag_cinsert(e, @swap_zero_min(f(e), g(e), @c0, @cNat(q)), @fbag_inter(f, g, @fbag_empty, c));
     @fbag_inter(f, g, @fbag_cons(d, p, b), @fbag_cons(d, q, c))  =  @fbag_cinsert(d, @swap_zero_min(f(d), g(d), @cNat(p), @cNat(q)), @fbag_inter(f, g, b, c));
     <(d, e)  ->  @fbag_inter(f, g, @fbag_cons(d, p, b), @fbag_cons(e, q, c))  =  @fbag_cinsert(d, @swap_zero_min(f(d), g(d), @cNat(p), @c0), @fbag_inter(f, g, b, @fbag_cons(e, q, c)));
     <(e, d)  ->  @fbag_inter(f, g, @fbag_cons(d, p, b), @fbag_cons(e, q, c))  =  @fbag_cinsert(e, @swap_zero_min(f(e), g(e), @c0, @cNat(q)), @fbag_inter(f, g, @fbag_cons(d, p, b), c));
     @fbag_diff(f, g, @fbag_empty, @fbag_empty)  =  @fbag_empty;
     @fbag_diff(f, g, @fbag_cons(d, p, b), @fbag_empty)  =  @fbag_cinsert(d, @swap_zero_monus(f(d), g(d), @cNat(p), @c0), @fbag_diff(f, g, b, @fbag_empty));
     @fbag_diff(f, g, @fbag_empty, @fbag_cons(e, q, c))  =  @fbag_cinsert(e, @swap_zero_monus(f(e), g(e), @c0, @cNat(q)), @fbag_diff(f, g, @fbag_empty, c));
     @fbag_diff(f, g, @fbag_cons(d, p, b), @fbag_cons(d, q, c))  =  @fbag_cinsert(d, @swap_zero_monus(f(d), g(d), @cNat(p), @cNat(q)), @fbag_diff(f, g, b, c));
     <(d, e)  ->  @fbag_diff(f, g, @fbag_cons(d, p, b), @fbag_cons(e, q, c))  =  @fbag_cinsert(d, @swap_zero_monus(f(d), g(d), @cNat(p), @c0), @fbag_diff(f, g, b, @fbag_cons(e, q, c)));
     <(e, d)  ->  @fbag_diff(f, g, @fbag_cons(d, p, b), @fbag_cons(e, q, c))  =  @fbag_cinsert(e, @swap_zero_monus(f(e), g(e), @c0, @cNat(q)), @fbag_diff(f, g, @fbag_cons(d, p, b), c));
     @fbag2fset(f, @fbag_empty)  =  @fset_empty;
     @fbag2fset(f, @fbag_cons(d, p, b))  =  @fset_cinsert(d, ==(==(f(d), @cNat(p)), >(f(d), @c0)), @fbag2fset(f, b));
     @fset2fbag(@fset_empty)  =  @fbag_empty;
     @fset2fbag(@fset_cons(d, s))  =  @fbag_cinsert(d, @cNat(@c1), @fset2fbag(s));
