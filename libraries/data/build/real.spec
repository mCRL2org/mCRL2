#include bool.spec
#include pos.spec
#include int.spec

sort Real <"real_">;

map @cReal <"creal"> : Int <"arg"> -> Real;
    Pos2Real <"pos2real"> : Pos <"arg"> -> Real;
    Nat2Real <"nat2real"> : Nat <"arg"> -> Real;
    Int2Real <"int2real"> : Int <"arg"> -> Real;
    Real2Pos <"real2pos"> : Real <"arg"> -> Pos;
    Real2Nat <"real2nat"> : Real <"arg"> -> Nat;
    Real2Int <"real2int"> : Real <"arg"> -> Int;
    <= <"less_equal"> : Real <"left"> # Real <"right"> -> Real;
    < <"less"> : Real <"left"> # Real <"right"> -> Real;
    >= <"greater_equal"> : Real <"left"> # Real <"right"> -> Real;
    > <"greater"> : Real <"left"> # Real <"right"> -> Real;
    max <"max"> : Real <"left"> # Real <"right"> -> Real;
    min <"min"> : Real <"left"> # Real <"right"> -> Real;
    abs <"abs"> : Real <"arg"> -> Real;
    - <"negate"> : Real <"arg"> -> Real;
    succ <"succ"> : Real <"arg"> -> Real;
    pred <"pred"> : Real <"arg"> -> Real;
    + <"plus"> : Real <"left"> # Real <"right"> -> Real;
    - <"minus"> : Real <"left"> # Real <"right"> -> Real;
    * <"times"> : Real <"left"> # Real <"right"> -> Real;
    exp <"exp"> : Real <"left"> # Nat <"right"> -> Real;

var n:Nat;
    p:Pos;
    x:Int;
    y:Int;
    r:Real;
    s:Real;
eqn ==(@cReal(x), @cReal(y)) = ==(x,y);
    Int2Real = @cReal;
    Nat2Real(n) = @cReal(@cInt(n));
    Pos2Real(p) = @cReal(@cInt(@cNat(p)));
    Real2Int(@cReal(x)) = x;
    Real2Nat(@cReal(x)) = Int2Nat(x);
    Real2Pos(@cReal(x)) = Int2Pos(x);
    <=(@cReal(x),@cReal(y)) = <=(x,y);
    <(@cReal(x),@cReal(y)) = <(x,y);
    >=(r,s) = <=(s,r);
    >(r,s) = <(s,r);
    max(r,s) = if(<=(r,s),s,r);
    min(r,s) = if(<=(r,s),r,s);
    abs(@cReal(x)) = @cReal(@cInt(abs(x)));
    -(@cReal(x)) = @cReal(-(x));
    succ(@cReal(x)) = @cReal(succ(x));
    pred(@cReal(x)) = @cReal(pred(x));
    +(@cReal(x),@cReal(y)) = @cReal(+(x,y));
    -(@cReal(x),@cReal(y)) = @cReal(-(x,y));
    *(@cReal(x),@cReal(y)) = @cReal(*(x,y));
    exp(@cReal(x),n) = @cReal(exp(x,n));

