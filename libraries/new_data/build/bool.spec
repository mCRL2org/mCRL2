% specification for booleans

sort Bool <"bool_">;

cons true <"true_"> : Bool;
     false <"false_"> : Bool;

map ! <"not_"> : Bool <"arg"> -> Bool;
    && <"and_"> : Bool <"left"> # Bool <"right"> -> Bool;
    || <"or_"> : Bool <"left"> # Bool <"right"> -> Bool;
    => <"implies"> : Bool <"left"> # Bool <"right"> -> Bool;

var b:Bool;
eqn !(true) = false;
    !(false) = true;
    !(!(b)) = b;
    &&(b,true) = b;
    &&(b,false) = false;
    &&(true,b) = b;
    &&(false,b) = false;
    ||(b,true) = true;
    ||(b,false) = b;
    ||(true,b) = true;
    ||(false,b) = b;
    =>(b,true) = true;
    =>(b,false) = !(b);
    =>(true,b) = b;
    =>(false,b) = true;
    ==(true,b) = b;
    ==(false,b) = !(b);
    ==(b,true) = b;
    ==(b,false) = !(b);

