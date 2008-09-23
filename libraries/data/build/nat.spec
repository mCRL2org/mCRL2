#include bool.spec
#include pos.spec

sort Nat <"nat">;
% Auxiliary sort natpair, pair of natural numbers
     @NatPair <"natpair">;

cons @c0 <"c0"> : Nat;
     @cNat <"cnat"> : Pos <"arg"> -> Nat;
% Constructor for natpair
     @cPair <"cpair"> : Nat <"proj1"> # Nat <"proj2"> -> @NatPair;

map Pos2Nat <"pos2nat"> : Pos <"arg"> -> Nat;
    Nat2Pos <"nat2pos"> : Nat <"arg"> -> Pos;
    <= <"less_equal">:Nat <"left"> #Nat <"right">->Bool;
    < <"less">:Nat <"left"> #Nat <"right">->Bool;
    >= <"geq">:Nat <"left"> #Nat <"right">->Bool;
    > <"greater">:Nat <"left"> #Nat <"right">->Bool;
    max <"max">:Pos <"left"> #Nat <"right">->Pos;
    max <"max">:Nat <"left"> #Pos <"right">->Pos;
    max <"max">:Nat <"left"> #Nat <"right">->Nat;
    min <"min">:Nat <"left"> #Nat <"right">->Nat;
    abs <"abs">:Nat <"number">->Nat;
    succ <"succ">:Nat <"number">->Pos;
    pred <"pred">:Pos <"number">->Nat;
    @dub <"dub">:Bool <"bit"> # Nat <"arg"> -> Nat;
    + <"plus">:Pos <"left"> #Nat <"right">->Pos;
    + <"plus">:Nat <"left"> #Pos <"right">->Pos;
    + <"plus">:Nat <"left"> #Nat <"right">->Nat;
    @gtesubt <"gtesubt">:Pos <"arg1"> # Pos <"arg2"> -> Nat;
    @gtesubt <"gtesubt">:Nat <"arg1"> # Nat <"arg2"> -> Nat;
    @gtesubtb <"gtesubt">: Bool <"bit"> # Pos <"arg1"> # Pos <"arg2"> -> Nat;
    * <"times">:Nat <"left"> #Nat <"right">->Nat;
    div <"div">: Pos <"arg1"> # Pos <"arg2"> -> Nat;
    div <"div">: Nat <"arg1"> # Pos <"arg2"> -> Nat;
    | <"mod">:Pos <"arg1"> # Pos <"arg2"> -> Nat;
    | <"mod">:Nat <"arg1"> # Pos <"arg2"> -> Nat;
    exp <"exp">:Pos <"arg1"> # Nat <"arg2"> -> Pos;
    exp <"exp">:Nat <"arg1"> # Nat <"arg2"> -> Nat;
    @even <"even">:Nat <"arg"> -> Bool;

% functions for natpair
    @first <"first"> : @NatPair <"pair"> -> Nat;
    @last <"last"> : @NatPair <"pair"> -> Nat;
    @divmod <"divmod"> : Pos <"arg1"> # Pos <"arg2"> -> @NatPair;
    @gdivmod <"divmod"> : @NatPair <"arg1"> # Bool <"bit"> # Pos <"arg3"> -> @NatPair;
    @ggdivmod <"divmod"> : Nat <"arg1"> # Nat <"arg2"> # Pos <"arg3"> -> @NatPair;


var b:Bool;
    p:Pos;
    q:Pos;
    n:Nat;
    m:Nat;
    u:Nat;
    v:Nat;
eqn ==(@c0, @cNat(p)) = false;
    ==(@cNat(p), @c0) = false;
    ==(@cNat(p), @cNat(q)) = ==(p,q);
    Pos2Nat = @cNat;
    Nat2Pos(p) = p;
    <=(@c0, n) = true;
    <=(@cNat(p), @c0) = false;
    <=(@cNat(p), @cNat(q)) = <=(p,q);
    <(n, @c0) = false;
    <(@c0, @cNat(p)) = true;
    <(@cNat(p), @cNat(q)) = <(p,q);
    >=(m,n) = <=(n,m);
    >(m,n) = <(n,m);
    max(p, @c0) = p;
    max(p, @cNat(q)) = max(p,q);
    max(@c0,p) = p;
    max(@cNat(p),q) = max(p,q);
    max(m,n) = if(<=(m,n),n,m);
    min(m,n) = if(<=(m,n),m,n);
    abs(n) = n;
    succ(@c0) = @cNat(@c1);
    succ(@cNat(p)) = succ(p);
    pred(@c1) = @c0;
    pred(@cDub(true,p)) = @cDub(false,p);
    pred(@cDub(false,p)) = @dub(true,pred(p));
    @dub(false,@c0) = @c0;
    @dub(true,@c0) = @cNat(@c1);
    @dub(b,@cNat(p)) = @cDub(b,p);
    +(p,@c0) = p;
    +(p,@cNat(q)) = +(p,q);
    +(@c0,p) = p;
    +(@cNat(p),q) = +(p,q);
    +(@c0,n) = n;
    +(n,@c0) = n;
    +(@cNat(p),@cNat(q)) = @addc(false,p,q);
    @gtesubt(p,q) = @gtesubtb(false,p,q);
    @gtesubt(n,@c0) = n;
    @gtesubt(@cNat(p),@cNat(q)) = @gtesubt(p,q);
    @gtesubtb(false,p,@c1) = pred(p);
    @gtesubtb(true,p,@c1) = pred(Nat2Pos(pred(p)));
    @gtesubtb(b,@cDub(false,p),@cDub(true,q)) = @dub(!(b),@gtesubtb(true,p,q));
    @gtesubtb(b,@cDub(true,p),@cDub(false,q)) = @dub(!(b),@gtesubtb(false,p,q));
    *(@c0,n) = @c0;
    *(n,@c0) = @c0;
    *(@cNat(p),@cNat(q)) = @cNat(*(p,q));
    exp(p,@c0) = @c1;
    exp(p,@cNat(@c1)) = p;
    exp(p,@cNat(@cDub(false,q))) = exp(@multir(false,@c1,p,p),q);
    exp(p,@cNat(@cDub(true,q))) = @multir(false,@c1,p,exp(@multir(false,@c1,p,p),q));
    exp(n,@c0) = @cNat(@c1);
    exp(@cNat(p),n) = @cNat(exp(p,n));
    @even(@c0) = true;
    @even(@cNat(@c1)) = false;
    @even(@cNat(@cDub(p,p))) = !(b);
    div(p,@c1) = @cNat(p);
    div(@c1, @cDub(b,p)) = @c0;
    div(@cDub(b,p),@cDub(false,q)) = div(p,q);
    <=(p,q) -> div(@cDub(false,p), @cDub(true,q)) = @c0;
    >(p,q) -> div(@cDub(false,p), @cDub(true,q)) = @first(@divmod(@cDub(false,p),@cDub(true,q)));
    <=(p,q) -> div(@cDub(true,p), @cDub(true,q)) = if(==(p,q),@cNat(@c1),@c0);
    >(p,q) -> div(@cDub(true,p), @cDub(true,q)) = @first(@divmod(@cDub(true,p),@cDub(true,q)));
    div(@c0,p) = @c0;
    div(@cNat(p),q) = div(p,q);
    |(p,@c1) = @c0;
    |(@c1,@cDub(b,p)) = @cNat(@c1);
    |(@cDub(b,p),@cDub(false,q)) = @dub(b,|(p,q));
    <=(p,q) -> |(@cDub(false,p),@cDub(true,q)) = @cNat(@cDub(false,p));
    >(p,q) -> |(@cDub(false,p),@cDub(true,q)) = @last(@divmod(@cDub(false,p),@cDub(true,q)));
    <=(p,q) -> |(@cDub(true,p),@cDub(true,q)) = if(==(p,q), @c0, @cNat(@cDub(true,p)));
    >(p,q) -> |(@cDub(true,p),@cDub(true,q)) = @last(@divmod(@cDub(true,p),@cDub(true,q)));
    |(@c0,p) = @c0;
    |(@cNat(p),q) = |(p,q);

% equations for natpair
    ==(@cPair(m,n), @cPair(v,u)) = &&(==(m,v),==(n,u));
    @first(@cPair(m,n)) = m;
    @last(@cPair(m,n)) = n;
    @divmod(@c1,@c1) = @cPair(@cNat(@c1),@c0);
    @divmod(@c1,@cDub(b,p)) = @cPair(@c0, @cNat(@c1));
    @divmod(@cDub(b,p),q) = @gdivmod(@divmod(p,q),b,q);
    @gdivmod(@cPair(m,n),b,p) = @ggdivmod(@dub(b,n),m,p);
    @ggdivmod(@c0,n,p) = @cPair(@dub(false,n),@cNat(p));
    <(p,q) -> @ggdivmod(p,n,q) = @cPair(@dub(false,n),@cNat(p));
    >=(p,q) -> @ggdivmod(@cNat(p),n,q) = @cPair(@dub(true,n),@gtesubtb(false,p,q));
    



    




