proc 

P(i:Nat,j:Nat,b:Bag)=sum(n:N,put(i,n).P(i,j,put(n,b)))
			+sum(n:Nat,get(j,n).P(take(n,b))<|in(n,b)|>delta)

P2=hide({__pass},
		restrict({get,put,__pass},
			comm({get|put->__pass},
				P(i,k,emptyBag)||P(k,j,emptyBag)
			)
		)
   )

init P2
