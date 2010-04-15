% Author(s): Aad Mathijssen, Jeroen Keiren
% Copyright: see the accompanying file COPYING or copy at
% https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
%
% Distributed under the Boost Software License, Version 1.0.
% (See accompanying file LICENSE_1_0.txt or copy at
% http://www.boost.org/LICENSE_1_0.txt)
%
% Specification of the Bool data sort.

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
    <(false,b) = b;
    <(true,b) = false;
    <(b,false) = false;
    <(b,true) = !(b);
    <=(false,b) = true;
    <=(true,b) = b;
    <=(b,false) = !(b);
    <=(b,true) = true;


