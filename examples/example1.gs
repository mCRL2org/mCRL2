proc 

Sqr(i:Nat,j:Nat)=sum(n:Nat,_get(i,n)|_put(j,sqr(n))).Sqr(i,j)

P(i:Nat,j:Nat,b:Bag)=sum(n:N,put(i,n).P(i,j,put(n,b)))+sum(n:Nat,in(n,b)->get(j,n).P(take(n,b)))

DSqr(i:Nat,j:Nat,b:Bag)=hide({__put,__get},
			restrict({_get,_put,__get,__put},
				comm({get|_get->__get,put|_put->__put},
					Sqr(i,k)||P(k,l,emptyBag)||Sqr(l,j)
				)
			)
		    )

init DSqr(1,2,emptyBag)
