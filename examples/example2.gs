% This is the GenSpect example from Section 10.2 of
% the Master's thesis by Muck van Weerdenburg
sort BagNat=Bag(Nat)

proc 

P(i:Nat,j:Nat,b:BagNat)=sum(n:N,put(i,n).P(i,j,{n}+b)))
			+sum(n:Nat,get(j,n).P(b\\{n})<|n in b|>delta)

P2=hide({__pass},
		restrict({get,put,__pass},
			comm({get|put->__pass},
				P(i,k,emptyBag)||P(k,j,emptyBag)
			)
		)
   )

init P2
