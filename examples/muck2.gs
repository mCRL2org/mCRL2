sort Dict = Nat -> struct data(p: Pos) | empty;

map emptyDict: Dict;
    setDict: Dict # Nat # Pos -> Dict;
    getdata: struct data(p: Pos) | empty -> Pos;
var d: Dict;
    n: Nat;
    p: Pos;
eqn emptyDict = lambda n: Nat . empty;
    setDict(d,n,p) = lambda m: Nat . if(m==n,data(p),d(m));
    getdata(data(p))=p;

act a: Dict # Dict;
    b;
    c: Pos;

proc P = b.Q(emptyDict)+delta;
     Q(d: Dict) = ((d(0) == empty) || true) -> P + ((d(1) == empty) =>
false) -> R;
     R = c(getdata(setDict(setDict(setDict(emptyDict,0,5),1,10),2,15)(1)));

init a(emptyDict,setDict(emptyDict,5 + 3 mod 2,9*54)).P;
