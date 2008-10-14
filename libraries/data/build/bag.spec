#include bool.spec
#include nat.spec
#include set.spec

sort Bag(S) <"bag">;

map @bag <"bag_comprehension"> : S <"arg"> -> Bag(S);
    {} <"emptybag"> : Bag(S);
    count <"count"> : S <"left"> # Bag(S) <"right"> -> Nat;
    in <"in"> : S <"left"> # Bag(S) <"right"> -> Bool;
    <= <"subbag_or_equal"> : Bag(S) <"left"> # Bag(S) <"right"> -> Bool;
    < <"subbag"> : Bag(S) <"left"> # Bag(S) <"right"> -> Bool;
    + <"union_"> : Bag(S) <"left"> # Bag(S) <"right"> -> Bag(S);
    - <"difference"> : Bag(S) <"left"> # Bag(S) <"right"> -> Bag(S);
    * <"intersection"> : Bag(S) <"left"> # Bag(S) <"right"> -> Bag(S);
    Bag2Set <"bag2set"> : Bag(S) <"arg"> -> Set(S);
    Set2Bag <"set2bag"> : Set(S) <"arg"> -> Bag(S);

var d:S;
    f:S->Nat;
    g:S->Nat;
    s:Bag(S);
    t:Bag(S);
    u:Set(S);
eqn ==(@bag(f), @bag(g)) = ==(f, g);
    {} = @bag(lambda(x:S, @c0));
    count(d, @bag(f)) = f(d);
    in(d,s) = >(count(d,s), @c0);
    <=(@bag(f), @bag(g)) = forall(x:S, <=(f(x), g(x)));
    <(s,t) = &&(<=(s,t), !=(s,t));
    +(@bag(f),@bag(g)) = @bag(lambda(x:S, +(f(x),g(x))));
    -(@bag(f),@bag(g)) = @bag(lambda(x:S, if(>(f(x),g(x)), @gtesubt(f(x),g(x)),@c0)));
    *(@bag(f),@bag(g)) = @bag(lambda(x:S, min(f(x),g(x))));
    Bag2Set(s) = @set(lambda(x:S, in(x,s)));
    Set2Bag(u) = @bag(lambda(x:S, if(in(x,u), @cNat(@c1), @c0)));

