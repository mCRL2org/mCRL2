% Author(s): Jan Friso Groote 
% Copyright: see the accompanying file COPYING or copy at
% https://github.com/mCRL2org/mCRL2/blob/master/COPYING
%
% Distributed under the Boost Software License, Version 1.0.
% (See accompanying file LICENSE_1_0.txt or copy at
% http://www.boost.org/LICENSE_1_0.txt)
%
% Specification of the Nat data sort.

#include machine_word.spec
#include bool.spec
#include pos.spec
#supertypeof Pos

sort Nat <"nat">;
% Auxiliary sort natpair, pair of natural numbers
     @NatPair <"natnatpair">;
     @WordNatPair <"wordnatpair">;

cons @c0 <"c0"> : Nat                                                                                            internal defined_by_rewrite_rules;
     succ <"succ"> : Nat <"arg">->Pos                                                                            external defined_by_rewrite_rules;
% Is the constructor below needed?
% Constructor for natpair
     @cPair <"cpair"> : Nat <"arg1"> # Nat <"arg2"> -> @NatPair                                                  internal defined_by_rewrite_rules;

map  @most_significant_digitNat <"most_significant_digit_nat">: Word <"arg"> -> Nat64                            internal defined_by_rewrite_rules;   
% concat_digit(p,w) represents (2^N)*p + w.
     @concat_digit <"concat_digit"> : Nat64 <"arg1"> # Word <"arg2"> -> Nat64                                    internal defined_by_rewrite_rules;

     Pos2Nat <"pos2nat"> : Pos <"arg"> -> Nat                                                                    external defined_by_rewrite_rules;
     Nat2Pos <"nat2pos"> : Nat <"arg"> -> Pos                                                                    external defined_by_rewrite_rules;
     max <"maximum">:Pos <"left"> #Nat <"right">->Pos                                                            external defined_by_rewrite_rules;
     max <"maximum">:Nat <"left"> #Pos <"right">->Pos                                                            external defined_by_rewrite_rules;
     max <"maximum">:Nat <"left"> #Nat <"right">->Nat                                                            external defined_by_rewrite_rules;
     min <"minimum">:Nat <"left"> #Nat <"right">->Nat                                                            external defined_by_rewrite_rules;
     pred <"pred">:Pos <"arg">->Nat                                                                              external defined_by_rewrite_rules;
     + <"plus">:Pos <"left"> #Nat <"right">->Pos                                                                 external defined_by_rewrite_rules;
     + <"plus">:Nat <"left"> #Pos <"right">->Pos                                                                 external defined_by_rewrite_rules;
     + <"plus">:Nat <"left"> #Nat <"right">->Nat                                                                 external defined_by_rewrite_rules;
     * <"times">:Nat <"left"> #Nat <"right">->Nat                                                                external defined_by_rewrite_rules;
     div <"div">: Nat <"left"> # Pos <"right"> -> Nat                                                            external defined_by_rewrite_rules;
     mod <"mod">:Nat <"left"> # Pos <"right"> -> Nat                                                             external defined_by_rewrite_rules;
     exp <"exp">:Pos <"left"> # Nat <"right"> -> Pos                                                             external defined_by_rewrite_rules;
     exp <"exp">:Nat <"left"> # Nat <"right"> -> Nat                                                             external defined_by_rewrite_rules;
     sqrt <"sqrt">:Nat <"arg"> -> Nat                                                                            external defined_by_rewrite_rules;
     @natpred <"natpred">: Nat <"arg"> -> Nat                                                                    internal defined_by_rewrite_rules;
     @monus <"monus">:Nat <"left"> # Nat <"right"> -> Nat                                                        internal defined_by_rewrite_rules;
     @monus_whr <"monus_whr">:Nat <"arg1"> # @word <"arg2"> # Nat <"arg3"> # @word <"arg4"> # Nat <"arg5">-> Nat internal defined_by_rewrite_rules;
% The functions below are auxiliary and might be omitted. 
     @even <"even">:Nat <"arg"> -> Bool                                                                          external defined_by_rewrite_rules;
     @swap_zero <"swap_zero">:Nat <"left"> # Nat <"right"> -> Nat                                                internal defined_by_rewrite_rules;
     @swap_zero_add <"swap_zero_add">:Nat <"arg1"> # Nat <"arg2"> # Nat <"arg3"> # Nat <"arg4"> -> Nat           internal defined_by_rewrite_rules;
     @swap_zero_min <"swap_zero_min">:Nat <"arg1"> # Nat <"arg2"> # Nat <"arg3"> # Nat <"arg4"> -> Nat           internal defined_by_rewrite_rules;
     @swap_zero_monus <"swap_zero_monus">:Nat <"arg1"> # Nat <"arg2"> # Nat <"arg3"> # Nat <"arg4"> -> Nat       internal defined_by_rewrite_rules;
     @sqrt_nat <"sqrt_nat_aux_func">:Nat <"arg1"> # Nat <"arg2"> # Pos <"arg3"> -> Nat                           internal defined_by_rewrite_rules;
     @dub <"dub">:Bool <"left"> # Nat <"right"> -> Nat                                                           internal defined_by_rewrite_rules;
     @gtesubtb <"gte_subtract_with_borrow">: Bool <"arg1"> # Pos <"arg2"> # Pos <"arg3"> -> Nat                  internal defined_by_rewrite_rules;
% functions for natpair
     @first <"first"> : @NatPair <"arg"> -> Nat                                                                  internal defined_by_rewrite_rules;
     @last <"last"> : @NatPair <"arg"> -> Nat                                                                    internal defined_by_rewrite_rules;
     @divmod <"divmod"> : Pos <"left"> # Pos <"right"> -> @NatPair                                               internal defined_by_rewrite_rules;
     @gdivmod <"generalised_divmod"> : @NatPair <"arg1"> # Bool <"arg2"> # Pos <"arg3"> -> @NatPair              internal defined_by_rewrite_rules;
     @ggdivmod <"doubly_generalised_divmod"> : Nat <"arg1"> # Nat <"arg2"> # Pos <"arg3"> -> @NatPair            internal defined_by_rewrite_rules;


var  p:Pos;
     n:Nat;
     n1:Nat;
     n2:Nat;
     w:@word;
     w1:@word;
     w2:@word;

eqn  @c0 = @most_significant_digitNat(@zero_word);
     succ(@most_significant_digitNat(w)) = if(==(w,@max_word),
                                                    @concat_digit(@most_significant_digitNat(@one_word),@zero_word),
                                                    @most_significant_digitNat(@succ_word(w)));
     succ(@concat_digit(n,w)) = if(==(w,@max_word),
                                           @concat_digit(@succ_nat64(n),@zero_word),
                                           @concat_digit(n,@succ_word(w)));

     ==(@most_significant_digitNat(w1), @most_significant_digitNat(w2)) = ==(w1,w2);
     ==(@concat_digit(n,w1), @most_significant_digitNat(w2)) = false;
     ==(@most_significant_digitNat(w1),@concat_digit(n,w2)) = false;
     ==(@concat_digit(n1,w1), @concat_digit(n2,w2)) = &&(==(w1,w2), ==(n1,n2));
     <(@most_significant_digitNat(w1), @most_significant_digitNat(w2)) = <(w1,w2);
     <(@concat_digit(n,w1), @most_significant_digitNat(w2)) = false;
     <(@most_significant_digitNat(w1),@concat_digit(n,w2)) = true;
     <(@concat_digit(n1,w1), @concat_digit(n2,w2)) = if(<(w1,w2),<=(n1,n2),<(n1,n2));
     <=(@most_significant_digitNat(w1), @most_significant_digitNat(w2)) = <=(w1,w2);
     <=(@concat_digit(n,w1), @most_significant_digitNat(w2)) = false;
     <=(@most_significant_digitNat(w1),@concat_digit(n,w2)) = true;
     <=(@concat_digit(n1,w1), @concat_digit(n2,w2)) = if(<=(w1,w2),<=(n1,n2),<(n1,n2));
     Pos2Nat(@most_significant_digit(w)) = @most_significant_digitNat(w);
     Pos2Nat(@concat_digit(p,w)) = @concat_digit(Pos2Nat(p),w);
     Nat2Pos(@most_significant_digitNat(w)) = @most_significant_digit(w);
     Nat2Pos(@concat_digit(p,w)) = @concat_digit(Nat2Pos(p),w);
% If important the 2 max functions below could be made more efficient by introducing a <=:Pos#Nat and <=:Nat#Pos.
     max(p, n) = if(<=(n,Pos2Nat(p)), p, Nat2Pos(n));
     max(n, p) = if(<=(n,Pos2Nat(p)), p, Nat2Pos(n));
     max(m,n) = if(<=(m,n),n,m);
     min(m,n) = if(<=(m,n),m,n);
     pred(@most_significant_digit(w)) = @most_significant_digitNat(pred_word(w));
     pred(@concat_digit(p,w)) = if(==(w,zero_word),
                                   @pred_whr(pred(p)),
                                   @concat_digit(Pos2Nat(p),pred_word(w)));
     @pred_whr(predp) = if(==(predp,@most_significant_digitNat(zero_word)),
                                    @most_significant_digitNat(max_word),
                                    @concat_digit(predp,max_word));
% needed?
     @dub(b,n) = if(b, +(n,n), succ(+(n,n)));

     +(p,n) = +(n,p);
     +(@most_significant_digitNat(w1),@most_significant_digit(w2)) =
                                     if(==(@add_overflow_word(w1,w2),@zero_word),
                                                 @most_significant_digit(@add_word(w1,w2)),
                                                 @concat_digit(@most_significant_digit(@one_word),@add_word(w1,w2)));
     +(@concat_digit(n1,w1),@most_significant_digit(w2)) = @concat_digit(
                                                                   +(n1,@most_significant_digitNat(@add_overflow_word(w1,w2))),
                                                                   @add_word(w1,w2));
     +(@most_significant_digitNat(w1),@concat_digit(p,w2)) = @concat_digit(
                                                                   +(@most_significant_digitNat(@add_overflow_word(w1,w2)), p),
                                                                   @add_word(w1,w2));
     +(@concat_digit(n1,w1),@concat_digit(p,w2)) =
                                        @concat_digit(+(@most_significant_digitNat(@add_overflow_word(w1,w2)),+(n1,p)),
                                                      @add_word(w1,w2));


     +(@most_significant_digitNat(w1),@most_significant_digitNat(w2)) =
                                     if(==(@add_overflow_word(w1,w2),@zero_word),
                                                 @most_significant_digitNat(@add_word(w1,w2)),
                                                 @concat_digit(@most_significant_digitNat(@one_word),@add_word(w1,w2)));
     +(@concat_digit(n1,w1),@most_significant_digitNat(w2)) = @concat_digit(
                                                                   +(@most_significant_digitNat(@add_overflow_word(w1,w2)),n1),
                                                                   @add_word(w1,w2));
     +(@most_significant_digitNat(w1),@concat_digit(n2,w2)) = @concat_digit(
                                                                   +(@most_significant_digitNat(@add_overflow_word(w1,w2)), n2),
                                                                   @add_word(w1,w2));
     +(@concat_digit(n1,w1),@concat_digit(n2,w2)) =
                                        @concat_digit(+(@most_significant_digitNat(@add_overflow_word(w1,w2)),+(n1,n2)),
                                                      @add_word(w1,w2));

     @natpred(@most_significant_digitNat(w)) = if(==(w,@zero_word), @most_significant_digitNat(@zero_word), @most_significant_digitNat(@pred_word(w)));
     @natpred(@concat_digit(n,w)) = if(==(w,@zero_word),
                                       if(==(n,@most_significant_digitNat(@one_word)),
                                               @most_significant_digitNat(@max_word),
                                               @concat_digit(@natpred(n),@max_word)),
                                       @concat_digit(n,@pred_word(w)));



     @monus(@most_significant_digitNat(w1),@most_significant_digitNat(w2)) =
                                              if(<(w1,w2),
                                                    @most_significant_digitNat(@zero_word),
                                                    @most_significant_digitNat(@minus_word(w1,w2)));
     @monus(@concat_digit(n1,w1),@most_significant_digitNat(w2)) = if(<(w1,w2),
                                                                      if(==(n1,@most_significant_digitNat(@one_word)),
                                                                               @most_significant_digitNat(@minus_word(w1,w2)),
                                                                               @concat_digit(@natpred(n1),@minus_word(w1,w2))),
                                                                      @concat_digit(n1,@minus_word(w1,w2)));
     @monus(@most_significant_digitNat(w1),@concat_digit(n2,w2)) = @most_significant_digitNat(@zero_word);
     @monus(@concat_digit(n1,w1),@concat_digit(n2,w2)) = @monus_whr(n1,w1,n2,w2,@monus(n1,n2));
     @monus_whr(n1,w1,n2,w2,diff) = if(w1<w2,
                                        if(==(diff,@most_significant_digitNat(@zero_word)),
                                               @most_significant_digitNat(@zero_word),
                                               if(==(diff,@most_significant_digitNat(@one_word)),
                                                        @most_significant_digitNat(@minus_word(w1,w2)),
                                                        @concat_digit(@natpred(diff),@minus_word(w1,w2)))),
                                        if(diff==most_significant_digitNat(zero_word),
                                               most_significant_digitNat(minus_word(w1,w2)),
                                               concat_digit(diff,minus_word(w1,w2))));


     @monus(@c0,n) = @c0;
     @monus(n,@c0) = n;
     @monus(@cNat(p),@cNat(q)) = @gtesubtb(false,p,q);

%     @gtesubtb(false,p,@c1) = pred(p);
%     @gtesubtb(true,p,@c1) = pred(Nat2Pos(pred(p)));
%     @gtesubtb(b,@cDub(c,p),@cDub(c,q)) = @dub(b, @gtesubtb(b,p,q));
%     @gtesubtb(b,@cDub(false,p),@cDub(true,q)) = @dub(!(b),@gtesubtb(true,p,q));
%     @gtesubtb(b,@cDub(true,p),@cDub(false,q)) = @dub(!(b),@gtesubtb(false,p,q));
     *(@c0,n) = @c0;
     *(n,@c0) = @c0;
     *(@cNat(p),@cNat(q)) = @cNat(*(p,q));

     *(@most_significant_digitNat(w1),@most_significant_digitNat(w2)) =
                           if(==(@times_overflow_word(w1,w2),@zero_word),
                                     @most_significant_digitNat(@times_word(w1,w2)),
                                     @concat_digit(@most_significant_digitNat(@times_overflow_word(w1,w2)),@times_word(w1,w2)));
     *(@concat_digit(n1,w1),@most_significant_digitNat(w2)) =
                           if(==(w2,zero_word),
                                @most_significant_digitNat(@zero_word),
                                if(==(@times_overflow_word(w1,w2),@zero_word),
                                     @concat_digit(*(n1,@most_significant_digitNat(w2)), @times_word(w1,w2)),
                                     @concat_digit(
                                             +(*(n1,@most_significant_digitNat(w2)),@most_significant_digitNat(@times_overflow_word(w1,w2))),@times_word(w1,w2))));
     *(@most_significant_digitNat(w1),@concat_digit(n2,w2)) =
                           if(==(w1,zero_word),
                                @most_significant_digitNat(@zero_word),
                                if(==(@times_overflow_word(w1,w2),@zero_word),
                                     @concat_digit(*(n2,@most_significant_digitNat(w1)), @times_word(w1,w2)),
                                     @concat_digit(
                                             +(*(n2,@most_significant_digitNat(w1)),@most_significant_digitNat(@times_overflow_word(w1,w2))),@times_word(w1,w2))));
     *(@concat_digit(n1,w1),@concat_digit(n2,w2)) =
                           +(@concat_digit(*(@concat_digit(n1,w1),n2),@zero_word),
                                       *(@concat_digit(n1,w1),@most_significant_digitNat(w2)));


     exp(p,@c0) = @c1;
     exp(p,@cNat(@c1)) = p;
     exp(p,@cNat(@cDub(false,q))) = exp(*(p,p),@cNat(q));
     exp(p,@cNat(@cDub(true,q))) = *(p,exp(*(p,p),@cNat(q)));
     exp(n,@c0) = @cNat(@c1);
     exp(@c0, @cNat(p)) = @c0;
     exp(@cNat(p),n) = @cNat(exp(p,n));
     @even(@c0) = true;
     @even(@cNat(@c1)) = false;
     @even(@cNat(@cDub(b,p))) = !(b);
     div(@c0,p) = @c0;
     div(@cNat(p),q) = @first(@divmod(p,q));
     mod(@c0,p) = @c0;
     mod(@cNat(p),q) = @last(@divmod(p,q));
     @swap_zero(m,@c0) = m;
     @swap_zero(@c0,n) = n;
     @swap_zero(@cNat(p),@cNat(p)) = @c0;
     !=(p,q) -> @swap_zero(@cNat(p),@cNat(q)) = @cNat(q);
     @swap_zero_add(@c0, @c0, m, n) = +(m,n);
     @swap_zero_add(@cNat(p), @c0, m, @c0) = m;
     @swap_zero_add(@cNat(p), @c0, m, @cNat(q)) = @swap_zero(@cNat(p), +(@swap_zero(@cNat(p),m), @cNat(q)));
     @swap_zero_add(@c0, @cNat(p), @c0, n) = n;
     @swap_zero_add(@c0, @cNat(p), @cNat(q), n) = @swap_zero(@cNat(p), +(@cNat(q), @swap_zero(@cNat(p), n)));
     @swap_zero_add(@cNat(p), @cNat(q), m, n) = @swap_zero(+(@cNat(p), @cNat(q)), +(@swap_zero(@cNat(p),m),@swap_zero(@cNat(q),n)));
     @swap_zero_min(@c0, @c0, m, n) = min(m,n);
     @swap_zero_min(@cNat(p), @c0, m, @c0) = @c0;
     @swap_zero_min(@cNat(p), @c0, m, @cNat(q)) = min(@swap_zero(@cNat(p),m), @cNat(q));
     @swap_zero_min(@c0, @cNat(p), @c0, n) = @c0;
     @swap_zero_min(@c0, @cNat(p), @cNat(q), n) = min(@cNat(q), @swap_zero(@cNat(p),n));
     @swap_zero_min(@cNat(p), @cNat(q), m, n) = @swap_zero(min(@cNat(p), @cNat(q)), min(@swap_zero(@cNat(p), m), @swap_zero(@cNat(q), n)));
     @swap_zero_monus(@c0, @c0, m, n) = @monus(m,n);
     @swap_zero_monus(@cNat(p), @c0, m, @c0) = m;
     @swap_zero_monus(@cNat(p), @c0, m, @cNat(q)) = @swap_zero(@cNat(p), @monus(@swap_zero(@cNat(p), m), @cNat(q)));
     @swap_zero_monus(@c0, @cNat(p), @c0, n) = @c0;
     @swap_zero_monus(@c0, @cNat(p), @cNat(q), n) = @monus(@cNat(q), @swap_zero(@cNat(p), n));
     @swap_zero_monus(@cNat(p), @cNat(q), m, n) = @swap_zero(@monus(@cNat(p),@cNat(q)),@monus(@swap_zero(@cNat(p),m), @swap_zero(@cNat(q),n)));
     sqrt(@c0) = @c0;
     sqrt(@cNat(p)) = @sqrt_nat(@cNat(p),@c0,@powerlog2(p));
     @sqrt_nat(n,m,@c1) = if(<=(n,m),@c0,@cNat(@c1));
     @sqrt_nat(n,m,@cDub(b,p)) =
               if(>(*(+(@cNat(@cDub(b,p)),m),@cNat(@cDub(b,p))),n),
                    @sqrt_nat(n,m,p),
                    +(@cNat(@cDub(b,p)),@sqrt_nat(@monus(n,*(+(@cNat(@cDub(b,p)),m),@cNat(@cDub(b,p)))),+(m,@cNat(@cDub(false,@cDub(b,p)))),p)));

% eq uations for natpair
     ==(@cPair(m,n), @cPair(u,v)) = &&(==(m,u),==(n,v));
     <(@cPair(m,n), @cPair(u,v)) = ||(<(m,u), &&(==(m,u), <(n,v)));
     <=(@cPair(m,n), @cPair(u,v)) = ||(<(m,u), &&(==(m,u), <=(n,v)));
     @first(@cPair(m,n)) = m;
     @last(@cPair(m,n)) = n;
     @divmod(@c1,@c1) = @cPair(@cNat(@c1),@c0);
     @divmod(@c1,@cDub(b,p)) = @cPair(@c0, @cNat(@c1));
     @divmod(@cDub(b,p),q) = @gdivmod(@divmod(p,q),b,q);
     @gdivmod(@cPair(m,n),b,p) = @ggdivmod(@dub(b,n),m,p);
     @ggdivmod(@c0,n,p) = @cPair(@dub(false,n),@c0);
     <(p,q) -> @ggdivmod(@cNat(p),n,q) = @cPair(@dub(false,n),@cNat(p));
     <=(q,p) -> @ggdivmod(@cNat(p),n,q) = @cPair(@dub(true,n),@gtesubtb(false,p,q));
