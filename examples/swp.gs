% This file contains a mCRL2/genspect version of the sliding window protocol
% as used in Fokkink, Groote, Pang, Badban and van de Pol,
% Verifying a sliding window protocol in mCRL, Report SENR0308, CWI,
% Amsterdam, 2003. 
% The `constant' n is the size of the buffers in the protocol. The purpose
% of this specification is to exercise with the syntax. At the time it
% was written type checkers and other tools were not yet ready. So, 
% typing problems may still exist.
%
% The major difference with the mCRL specification is that standard
% datatypes did not have to be formulated, and the sort Buf containing
% the buffers in the sender and the receiver has now become a function
% instead of a list. This seems to make the specification more
% compact and insightful. 

map  n: Nat;

sort D = struct d1 | d2;
     extD = struct data(getdata:D) | empty?isEmpty;
     Buf = Nat -> extD ;
     % Kan Niet: Buf = Nat -> struct data(getdata:D) | empty?isEmpty; 
     % Wel gewenst. De sort extD is verder geheel irrelevant.
map  emptyBuf : Buf;
     insert: D#Nat#Buf -> Buf;
     remove: Nat#Buf -> Buf;
     release: Nat#Nat#Buf -> Buf;
     nextempty: Nat#Buf -> Nat;
     inWindow: Nat#Nat#Nat -> Bool;
var  i,j,k: Nat; d: D; q: Buf;
eqn  emptyBuf = lambda j:Nat.empty;
     insert(d,i,q) = lambda j:Nat.if(i==j,data(d),q(j)); 
     remove(i,q) = lambda j:Nat.if(i==j,empty,q(j));
     release(i,j,q) = 
        if((i mod 2*n)==(j mod 2*n),
           q,
           release((i+1) mod 2*n,j,remove(i,q)));
     nextempty(i,q) = if(q(i)==empty,i,nextempty((i+1) mod n,q));
     inWindow(i,j,k) = (i<=j && j<k) || (k<i && i<=j) || (j<k && k<i);

act  sA,rA,sD,rD: D;
     sB,rB,cB,sC,rC,cC: D#Nat;
     sE,rE,cE,sF,rF,cF: Nat;
     j;

proc S(l,m:Nat,q:Buf)=
        sum(d:D, inWindow(l,m,(l+n) mod 2*n) -> 
                rA(d).S(l,(m+1) mod 2*n,insert(d,m,q)))+
        sum(k:Nat, (q(k)!=empty) -> sB(q(k)).S(l,m,q))+
        sum(k:Nat, rF(k).S(k,m,release(l,k,q)));

     R(l:Nat,q:Buf)=
        sum(d:D, sum(k:Nat, rC(d,k).
           (inWindow(l,k,(l+n) mod 2*n) -> R(l,k,q)+
            !inWindow(l,k,(l+n) mod 2*n) -> R(l,q))))+
% Brrr. Ik moet hier de conditie twee keer formuleren.
        (q(l)!=empty) -> sD(q(l)).R((l+1) mod 2*n,remove(l,q))+
        sE(nextempty(l,q)).R(l,q);

     K= sum(d:D,sum(k:Nat,rB(d,k).(j.sC(d,k)+j))).K;

     L = sum(k:Nat, rE(k).(j.sF(k)+j)).L;

init allow({cB,cC,cE,cF,j,rA,sD},
        comm({rB|sB->cB, rC|sC->cC, rE|sE->cE, rF|sF->cF},
           S(0,0,emptyBuf) || K || L || R(0,emptyBuf)));
