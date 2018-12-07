% Author(s): Aad Mathijssen, Jeroen Keiren
% Copyright: see the accompanying file COPYING or copy at
% https://github.com/mCRL2org/mCRL2/blob/master/COPYING
%
% Distributed under the Boost Software License, Version 1.0.
% (See accompanying file LICENSE_1_0.txt or copy at
% http://www.boost.org/LICENSE_1_0.txt)
%
% Specification of the FBag data sort, denoting finite bags.
% Note that the specification relies on the underlying data type S to have a total ordering.
%
% The definition of an FBag originally had the shape 
%
% sort FBag(S) <"fbag"> = struct {:} <"empty"> | @fbag_cons <"cons_"> : S <"arg1"> # Pos <"arg2"> # FBag(S) <"arg3">;
%
% However, this does not work as the automatically generated comparison operators <=, <, > and >= do not act
% as subset operators. Therefore, the constructors have been made explicit, as have the comparison operators.
% (April, 2017, Jan Friso Groote).
%
% Also changed @fbag_insert to become the constructor and @fbag_cons to become a map. All bags should have
% their elements in a list with @fbag_cons as head symbol be ordered. This is not the case in lists with @fbag_insert.
% If the @fbag_cons would be a constructor illegal lists would be constructed when evaluationg quantifications and 
% sum operators. Now it is the case that too many bags will be generated when evaluating for instance a sum operator, 
% but they are at least not incorrect. 

#using S
#include bool.spec
#include pos.spec
#include nat.spec
#include fset.spec

sort FBag(S) <"fbag">;

cons {:} <"empty"> : FBag(S)                                                                                                         external defined_by_rewrite_rules;
     @fbag_insert <"insert"> : S <"arg1"> # Pos <"arg2"> # FBag(S) <"arg3"> -> FBag(S)                                               internal defined_by_rewrite_rules;

map @fbag_cons <"cons_"> : S <"arg1"> # Pos <"arg2"> # FBag(S) <"arg3"> -> FBag(S)                                                   internal defined_by_rewrite_rules;
    @fbag_cinsert <"cinsert"> : S <"arg1"> # Nat <"arg2"> # FBag(S) <"arg3"> -> FBag(S)                                              internal defined_by_rewrite_rules;
    count <"count"> : S <"left"> # FBag(S) <"right"> -> Nat                                                                          external defined_by_rewrite_rules;
    in <"in"> : S <"left"> # FBag(S) <"right"> -> Bool                                                                               external defined_by_rewrite_rules;
    @fbag_join <"join"> : (S -> Nat) <"arg1"> # (S -> Nat) <"arg2"> # FBag(S) <"arg3"> # FBag(S) <"arg4"> -> FBag(S)                 internal defined_by_rewrite_rules;
    @fbag_inter <"fbag_intersect">: (S -> Nat) <"arg1"> # (S -> Nat) <"arg2"> # FBag(S) <"arg3"> # FBag(S) <"arg4"> -> FBag(S)       internal defined_by_rewrite_rules;
    @fbag_diff <"fbag_difference">: (S -> Nat) <"arg1"> # (S -> Nat) <"arg2"> # FBag(S) <"arg3"> # FBag(S) <"arg4"> -> FBag(S)       internal defined_by_rewrite_rules;
    @fbag2fset <"fbag2fset">: (S -> Nat) <"left"> # FBag(S) <"right"> -> FSet(S)                                                     internal defined_by_rewrite_rules;
    @fset2fbag <"fset2fbag">: FSet(S) <"arg"> -> FBag(S)                                                                             internal defined_by_rewrite_rules;
    + <"union_"> : FBag(S) <"left"> # FBag(S) <"right"> -> FBag(S)                                                                   external defined_by_rewrite_rules;
    * <"intersection"> : FBag(S) <"left"> # FBag(S) <"right"> -> FBag(S)                                                             external defined_by_rewrite_rules;
    - <"difference"> : FBag(S) <"left"> # FBag(S) <"right"> -> FBag(S)                                                               external defined_by_rewrite_rules;
    # <"count_all"> : FBag(S) <"arg"> -> Nat                                                                                         external defined_by_rewrite_rules;


var d: S;
    e: S;
    p: Pos;
    q: Pos;
    b: FBag(S);
    c: FBag(S);
    s: FSet(S);
    f: S -> Nat;
    g: S -> Nat;
eqn ==(@fbag_cons(d, p, b), {:})  =  false;
    ==({:}, @fbag_cons(d, p, b))  =  false;
    ==(@fbag_cons(d, p, b), @fbag_cons(e, q, c))  =  &&(==(p, q), &&(==(d,e),==(b, c)));
    <=(@fbag_cons(d, p, b), {:})  =  false;
    <=({:}, @fbag_cons(d, p, b))  =  true;
    <=(@fbag_cons(d, p, b), @fbag_cons(e, q, c))  =  if(<(d, e), false, if(==(d, e), &&(<=(p, q), <=(b, c)), <=(@fbag_cons(d, p, b), c)));
    <(@fbag_cons(d, p, b), {:})  =  false;
    <({:}, @fbag_cons(d, p, b))  =  true;
    <(@fbag_cons(d, p, b), @fbag_cons(e, q, c))  =  if(<(d, e), false, if(==(d, e), ||(&&(==(p, q), <(b,c)),  &&(<(p, q), <=(b, c))), <=(@fbag_cons(d, p, b), c)));
    @fbag_insert(d, p, {:})  =  @fbag_cons(d, p, {:});
     @fbag_insert(d, p, @fbag_cons(d, q, b))  =  @fbag_cons(d, @addc(false,p,q), b);
     <(d, e)  ->  @fbag_insert(d, p, @fbag_cons(e, q, b))  =  @fbag_cons(d, p, @fbag_cons(e, q, b));
     <(e, d)  ->  @fbag_insert(d, p, @fbag_cons(e, q, b))  =  @fbag_cons(e, q, @fbag_insert(d, p, b));
     @fbag_cinsert(d, @c0, b)  =  b;
     @fbag_cinsert(d, Pos2Nat(p), b)  =  @fbag_insert(d, p, b);
     count(d, {:})  =  @c0;
     count(d, @fbag_cons(d, p, b))  =  Pos2Nat(p);
     <(d, e)  ->  count(d, @fbag_cons(e, p, b))  =  @c0;
     <(e, d)  ->  count(d, @fbag_cons(e, p, b))  =  count(d, b);
     in(d, b)  =  >(count(d, b), @c0);
     @fbag_join(f, g, {:}, {:})  =  {:};
     @fbag_join(f, g, @fbag_cons(d, p, b), {:})  =  @fbag_cinsert(d, @swap_zero_add(f(d), g(d), Pos2Nat(p), @c0), @fbag_join(f, g, b, {:}));
     @fbag_join(f, g, {:}, @fbag_cons(e, q, c))  =  @fbag_cinsert(e, @swap_zero_add(f(e), g(e), @c0, Pos2Nat(q)), @fbag_join(f, g, {:}, c));
     @fbag_join(f, g, @fbag_cons(d, p, b), @fbag_cons(d, q, c))  =  @fbag_cinsert(d, @swap_zero_add(f(d), g(d), Pos2Nat(p), Pos2Nat(q)), @fbag_join(f, g, b, c));
     <(d, e)  ->  @fbag_join(f, g, @fbag_cons(d, p, b), @fbag_cons(e, q, c))  =  @fbag_cinsert(d, @swap_zero_add(f(d), g(d), Pos2Nat(p), @c0), @fbag_join(f, g, b, @fbag_cons(e, q, c)));
     <(e, d)  ->  @fbag_join(f, g, @fbag_cons(d, p, b), @fbag_cons(e, q, c))  =  @fbag_cinsert(e, @swap_zero_add(f(e), g(e), @c0, Pos2Nat(q)), @fbag_join(f, g, @fbag_cons(d, p, b), c));
     @fbag_inter(f, g, {:}, {:})  =  {:};
     @fbag_inter(f, g, @fbag_cons(d, p, b), {:})  =  @fbag_cinsert(d, @swap_zero_min(f(d), g(d), Pos2Nat(p), @c0), @fbag_inter(f, g, b, {:}));
     @fbag_inter(f, g, {:}, @fbag_cons(e, q, c))  =  @fbag_cinsert(e, @swap_zero_min(f(e), g(e), @c0, Pos2Nat(q)), @fbag_inter(f, g, {:}, c));
     @fbag_inter(f, g, @fbag_cons(d, p, b), @fbag_cons(d, q, c))  =  @fbag_cinsert(d, @swap_zero_min(f(d), g(d), Pos2Nat(p), Pos2Nat(q)), @fbag_inter(f, g, b, c));
     <(d, e)  ->  @fbag_inter(f, g, @fbag_cons(d, p, b), @fbag_cons(e, q, c))  =  @fbag_cinsert(d, @swap_zero_min(f(d), g(d), Pos2Nat(p), @c0), @fbag_inter(f, g, b, @fbag_cons(e, q, c)));
     <(e, d)  ->  @fbag_inter(f, g, @fbag_cons(d, p, b), @fbag_cons(e, q, c))  =  @fbag_cinsert(e, @swap_zero_min(f(e), g(e), @c0, Pos2Nat(q)), @fbag_inter(f, g, @fbag_cons(d, p, b), c));
     @fbag_diff(f, g, {:}, {:})  =  {:};
     @fbag_diff(f, g, @fbag_cons(d, p, b), {:})  =  @fbag_cinsert(d, @swap_zero_monus(f(d), g(d), Pos2Nat(p), @c0), @fbag_diff(f, g, b, {:}));
     @fbag_diff(f, g, {:}, @fbag_cons(e, q, c))  =  @fbag_cinsert(e, @swap_zero_monus(f(e), g(e), @c0, Pos2Nat(q)), @fbag_diff(f, g, {:}, c));
     @fbag_diff(f, g, @fbag_cons(d, p, b), @fbag_cons(d, q, c))  =  @fbag_cinsert(d, @swap_zero_monus(f(d), g(d), Pos2Nat(p), Pos2Nat(q)), @fbag_diff(f, g, b, c));
     <(d, e)  ->  @fbag_diff(f, g, @fbag_cons(d, p, b), @fbag_cons(e, q, c))  =  @fbag_cinsert(d, @swap_zero_monus(f(d), g(d), Pos2Nat(p), @c0), @fbag_diff(f, g, b, @fbag_cons(e, q, c)));
     <(e, d)  ->  @fbag_diff(f, g, @fbag_cons(d, p, b), @fbag_cons(e, q, c))  =  @fbag_cinsert(e, @swap_zero_monus(f(e), g(e), @c0, Pos2Nat(q)), @fbag_diff(f, g, @fbag_cons(d, p, b), c));
     @fbag2fset(f, {:})  =  {};
     @fbag2fset(f, @fbag_cons(d, p, b))  =  @fset_cinsert(d, ==(==(f(d), Pos2Nat(p)), >(f(d), @c0)), @fbag2fset(f, b));
     @fset2fbag({})  =  {:};
     @fset2fbag(@fset_cons(d, s))  =  @fbag_cinsert(d, Pos2Nat(@c1), @fset2fbag(s));
    -(b,{:}) = b;
    -({:},c) = {:};
    -(@fbag_cons(d,p,b),@fbag_cons(d,p,c)) = -(b,c);
    <(p,q) -> -(@fbag_cons(d,p,b),@fbag_cons(d,q,c)) = -(b,c);
    <(q,p) -> -(@fbag_cons(d,p,b),@fbag_cons(d,q,c)) = @fbag_cons(d,Nat2Pos(@monus(p,q)),-(b,c));
    <(d,e) -> -(@fbag_cons(d,p,b),@fbag_cons(e,q,c)) = @fbag_cons(d,p,-(b,@fbag_cons(e,q,c)));
    <(e,d) -> -(@fbag_cons(d,p,b),@fbag_cons(e,q,c)) = @fbag_cons(e,q,-(@fbag_cons(d,p,b),c));
    +(b,{:}) = b;
    +({:},c) = c;
    +(@fbag_cons(d,p,b),@fbag_cons(d,q,c)) = @fbag_cons(d,@addc(false,p,q),+(b,c));
    <(d,e) -> +(@fbag_cons(d,p,b),@fbag_cons(e,q,c)) = @fbag_cons(d,p,+(b,@fbag_cons(e,q,c)));
    <(e,d) -> +(@fbag_cons(d,p,b),@fbag_cons(e,q,c)) = @fbag_cons(e,q,+(@fbag_cons(d,p,b),c));
    *(b,{:}) = {:};
    *({:},c) = {:};
    *(@fbag_cons(d,p,b),@fbag_cons(d,q,c)) = @fbag_cons(d,min(p,q),*(b,c));
    <(d,e) -> *(@fbag_cons(d,p,b),@fbag_cons(e,q,c)) = *(b,@fbag_cons(e,q,c));
    <(e,d) -> *(@fbag_cons(d,p,b),@fbag_cons(e,q,c)) = *(@fbag_cons(d,p,b),c);
    #({:}) = @c0;
    #(@fbag_cons(d,p,{:})) = Pos2Nat(p);
    #(@fbag_cons(d,p,@fbag_cons(e,q,b))) = Pos2Nat(@addc(false,p,Nat2Pos(#(@fbag_cons(e,q,b)))));
