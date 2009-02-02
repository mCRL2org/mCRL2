#include bool.spec
#include pos.spec
#include nat.spec

sort List(S) <"list">;

cons [] <"nil"> : List(S);
     |> <"cons_"> : S <"head"> # List(S) <"tail"> -> List(S);

map in <"in"> : S <"arg1"> # List(S) <"arg2"> -> Bool;
    # <"count"> : List(S) <"list"> -> Nat;
    <| <"snoc"> : List(S) <"rtail"> # S <"rhead"> -> List(S);
    ++ <"concat"> : List(S) <"left"> # List(S) <"right"> -> List(S);
    . <"element_at"> : List(S) <"list"> # Nat <"position"> -> S;
    head <"head"> : List(S) <"list"> -> S;
    tail <"tail"> : List(S) <"list"> -> List(S);
    rhead <"rhead"> : List(S) <"list"> -> S;
    rtail <"rtail"> : List(S) <"list"> -> List(S);

var d:S;
    e:S;
    s:List(S);
    t:List(S);
    p:Pos;
eqn ==([], |>(d,s)) = false;
    ==(|>(d,s), []) = false;
    ==(|>(d,s), |>(e,t)) = &&(==(d,e), ==(s,t));
    in(d,[]) = false;
    in(d,|>(e,s)) = ||(==(d,e), in(d,s));
    #([]) = @c0;
    #(|>(d,s)) = @cNat(succ(#(s)));
    <|([],d) = |>(d,[]);
    <|(|>(d,s), e) = |>(d, <|(s,e));
    ++([],s) = s;
    ++(|>(d,s), t) = |>(d, ++(s,t));
    ++(s,[]) = s;
    .(|>(d,s),@c0) = d;
    .(|>(d,s),@cNat(p)) = .(s, pred(p));
    head(|>(d,s)) = d;
    tail(|>(d,s)) = s;
    rhead(|>(d,[])) = d;
    rhead(|>(d,|>(e,s))) = rhead(|>(e,s));
    rtail(|>(d,[])) = [];
    rtail(|>(d,|>(e,s))) = |>(d,rtail(|>(e,s)));

