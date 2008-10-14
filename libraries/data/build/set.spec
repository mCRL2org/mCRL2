#include bool.spec

sort Set(S) <"set">;

map @set <"set_comprehension"> : S <"arg"> -> Set(S);
    {} <"emptyset"> : Set(S);
    in <"in"> : S <"left"> # Set(S) <"right"> -> Bool;
    <= <"subset_or_equal"> : Set(S) <"left"> # Set(S) <"right"> -> Bool;
    < <"subset"> : Set(S) <"left"> # Set(S) <"right"> -> Bool;
    + <"union_"> : Set(S) <"left"> # Set(S) <"right"> -> Set(S);
    - <"difference"> : Set(S) <"left"> # Set(S) <"right"> -> Set(S);
    * <"intersection"> : Set(S) <"left"> # Set(S) <"right"> -> Set(S);
    - <"complement"> : Set(S) <"arg"> -> Set(S);

var d : S;
    s : Set(S);
    t : Set(S);
    f : S->Bool;
    g : S->Bool;
eqn ==(@set(f),@set(g)) = ==(f,g); 
    {} = @set(lambda(x:S, false));
    in(d,@set(f)) = f(d);
    <=(@set(f),@set(g)) = forall(x:S, <=(f(x),g(x)));
    <(s,t) = &&(<=(s,t), !=(s,t));
    +(@set(f),@set(g)) = @set(lambda(x:S, ||(f(x), g(x))));
    -(s,t) = *(s, -(t));
    *(@set(f),@set(g)) = @set(lambda(x:S, &&(f(x), g(x))));
    -(@set(f)) = @set(lambda(x:S, !(f(x))));

