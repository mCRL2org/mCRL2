% This file contains the alternating bit protocol, as described in J.C.M.
% Baeten and W.P. Weijland, Process Algebra, Cambridge Tracts in Theoretical
% Computer Science 18, Cambridge University Press, 1990.
%
% The only exception is that the domain D consists of two data elements to
% facilitate simulation.

sort
  D     = d1 | d2;
  Error = e;
  Bit   = b0 | b1;

map
  invert: Bit -> Bit;

eqn
  invert(b1)= b0;
  invert(b0)= b1;

act
  r1,s4: D;
  s2,r2: D # Bit;
  s3,r3: D # Bit;
  s3,r3: Error;
  s5,r5: Bit;
  s6,r6: Bit;
  s6,r6: Error;
  tau_s3db,tau_s3e,tau_s6b,tau_s6e;

proc
  S            = S(b0).S(b1).S;
  S(b:Bit)     = sum(d:D, r1(d).S(d,b));
  S(d:D,b:Bit) = s2(d,b).((r6(invert(b))+r6(e)).S(d,b)+r6(b));

  R            = R(b1).R(b0).R;
  R(b:Bit)     = (sum(d:D, r3(d,b))+r3(e)).s5(b).R(b) +
                 sum(d:D, r3(d,invert(b)).s4(d).s5(invert(b)));

  K            = sum(d:D, sum(b:Bit, 
                   r2(d,b).(tau_s3db.s3(d,b)+tau_s3e.s3(e))
                 )).K;

  L            = sum(b:Bit,r5(b).(tau_s6b.s6(b)+tau_s6e.s6(e))).L;

init
  hide({tau_s3db, tau_s3e, tau_s6b, tau_s6e},
    restrict({r2, r3, r5, r6, s2, s3, s5, s6},
      comm({r2|s2, r3|s3, r5|s5, r6|s6},
        S || K || L || R
      )
    )
  );

% This expression is equivalent to the following:
%  allow({r1, s4, r1|s4},
%    hide({tau_s3db, tau_s3e, tau_s6b, tau_s6e},
%      comm({r2|s2, r3|s3, r5|s5, r6|s6},
%        S || K || L || R
%      }    
%    )
%  );
