% This is the GenSpect example from Section 10.1 of 
% the Master's thesis by Muck van Weerdenburg

proc 

Sqr(i:Nat,j:Nat)=
   sum(n:Nat,_get(i,n)|_put(j,n*n)).Sqr(i,j);

P(i:Nat,j:Nat,b:Bag)=
   sum(n:N,put(i,n).P(i,j,put(n,b)))+
   sum(n:Nat,in(n,b)->get(j,n).P(take(n,b)));

DSqr(i:Nat,j:Nat,b:Bag)=
   hide({__put,__get},
     restrict({_get,_put,__get,__put},
       comm({get|_get->__get,put|_put->__put},
          Sqr(i,k)||P(k,l,{})||Sqr(l,j)
				)
			)
		    );

init DSqr(1,2,{});
