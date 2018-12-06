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
     @NatNatPair <"natnatpair">;
     @wordNatPair <"wordnatpair">;

cons @c0 <"c0"> : Nat                                                                                            internal defined_by_rewrite_rules;
     succ <"succ"> : Nat <"arg">->Pos                                                                            external defined_by_rewrite_rules;
% Is the constructor below needed?
% Constructor for natpair
     @nnPair <"nnpair"> : Nat <"arg1"> # Nat <"arg2"> -> @NatNatPair                                             internal defined_by_rewrite_rules;
     @wnPair <"wnpair"> : @word <"arg1"> # Nat <"arg2"> -> @wordNatPair                                          internal defined_by_rewrite_rules;

map  @most_significant_digitNat <"most_significant_digit_nat">: @word <"arg"> -> Nat                             internal defined_by_rewrite_rules;   
% concat_digit(p,w) represents (2^N)*p + w.
     @concat_digit <"concat_digit"> : Nat <"arg1"> # @word <"arg2"> -> Nat                                       internal defined_by_rewrite_rules;

     Pos2Nat <"pos2nat"> : Pos <"arg"> -> Nat                                                                    external defined_by_rewrite_rules;
     Nat2Pos <"nat2pos"> : Nat <"arg"> -> Pos                                                                    external defined_by_rewrite_rules;
     max <"maximum">:Pos <"left"> #Nat <"right">->Pos                                                            external defined_by_rewrite_rules;
     max <"maximum">:Nat <"left"> #Pos <"right">->Pos                                                            external defined_by_rewrite_rules;
     max <"maximum">:Nat <"left"> #Nat <"right">->Nat                                                            external defined_by_rewrite_rules;
     min <"minimum">:Nat <"left"> #Nat <"right">->Nat                                                            external defined_by_rewrite_rules;
     pred <"pred">:Pos <"arg">->Nat                                                                              external defined_by_rewrite_rules;
     @pred_whr <"pred_whr">:Nat <"arg">->Nat                                                                     internal defined_by_rewrite_rules;
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
     @is_odd <"is_odd">: Nat <"arg"> -> Bool                                                                     internal defined_by_rewrite_rules;
     @div2 <"div2">: Nat <"arg"> -> Nat                                                                          internal defined_by_rewrite_rules;
     @monus <"monus">:Nat <"left"> # Nat <"right"> -> Nat                                                        internal defined_by_rewrite_rules;
     @monus_whr <"monus_whr">:Nat <"arg1"> # @word <"arg2"> # Nat <"arg3"> # @word <"arg4"> # Nat <"arg5">-> Nat internal defined_by_rewrite_rules;
     @exp_aux3p <"exp_aux3p">: Bool <"arg1"> # Pos <"arg2"> # @word <"arg3"> -> Pos                              internal defined_by_rewrite_rules;
     @exp_aux4p <"exp_aux4p">: Bool <"arg1"> # Pos <"arg2"> # Nat <"arg3"> # @word <"arg4"> -> Pos               internal defined_by_rewrite_rules;
     @exp_aux3n <"exp_aux3n">: Bool <"arg1"> # Nat <"arg2"> # @word <"arg3"> -> Nat                              internal defined_by_rewrite_rules;
     @exp_aux4n <"exp_aux4n">: Bool <"arg1"> # Nat <"arg2"> # Nat <"arg3"> # @word <"arg4"> -> Nat               internal defined_by_rewrite_rules;
     @exp_auxtruep <"exp_auxtruep">: Pos <"arg1"> # Nat <"arg2"> # @word <"arg3"> -> Nat                         internal defined_by_rewrite_rules;
     @exp_auxtruen <"exp_auxtruen">: Nat <"arg1"> # Nat <"arg2"> # @word <"arg3"> -> Nat                         internal defined_by_rewrite_rules;
     @exp_auxfalsep <"exp_auxfalsep">: Pos <"arg1"> # Nat <"arg2"> # @word <"arg3"> -> Nat                       internal defined_by_rewrite_rules;
     @exp_auxfalsen <"exp_auxfalsen">: Nat <"arg1"> # Nat <"arg2"> # @word <"arg3"> -> Nat                       internal defined_by_rewrite_rules;
     @div_bold <"div_bold">: Nat <"arg1"> # Pos <"arg2"> -> @word                                                internal defined_by_rewrite_rules;
     @div_bold_whr <"div_bold_whr">: Nat <"arg1"> # @word <"arg2"> # Pos <"arg3"> # @word <"arg4"> # @word <"brg5"> # @word <"arg6"> -> @word   internal defined_by_rewrite_rules;
     @div_whr1 <"div_whr1">: Nat <"arg1"> # @word <"arg2"> # @word <"arg3"> # @NatNatPair <"arg4"> -> Nat        internal defined_by_rewrite_rules;
     @div_whr2 <"div_whr2">: Nat <"arg1"> # @word <"arg2"> # @word <"arg3"> # @NatNatPair <"arg4"> -> Nat        internal defined_by_rewrite_rules;
     @mod_whr1 <"mod_whr1">: @word <"arg1"> # Pos <"arg2"> # @word <"arg3"> # Nat <"arg4"> -> Nat               internal defined_by_rewrite_rules; 
     @divmod_aux <"divmod_aux">: Nat <"arg1"> # Pos <"arg2"> -> @NatNatPair                                      internal defined_by_rewrite_rules;
     @divmod_aux_whr1 <"divmod_aux_whr1">: Nat <"arg1"> # @word <"arg2"> # @word <"arg3"> # @NatNatPair <"arg4"> -> @NatNatPair  internal defined_by_rewrite_rules;
     @divmod_aux_whr2 <"divmod_aux_whr2">: Nat <"arg1"> # @word <"arg2"> # Pos <"arg3"> # @word <"arg4"> # Nat <"arg5"> -> @NatNatPair  internal defined_by_rewrite_rules;
     @divmod_aux_whr3 <"divmod_aux_whr3">: Nat <"arg1"> # @word <"arg2"> # Pos <"arg3"> # @word <"arg4"> # Nat <"arg5"> -> @NatNatPair  internal defined_by_rewrite_rules;
     @divmod_aux_whr4 <"divmod_aux_whr4">: @word <"arg1"> # Pos <"arg2"> # @word <"arg3"> # @NatNatPair <"arg4"> -> @NatNatPair  internal defined_by_rewrite_rules;
     @divmod_aux_whr5 <"divmod_aux_whr5">: Pos <"arg1"> # @word <"arg2"> # @NatNatPair <"arg3"> # Nat <"arg4"> -> @NatNatPair  internal defined_by_rewrite_rules;
     @divmod_aux_whr6 <"divmod_aux_whr6">: Pos <"arg1"> # @word <"arg2"> # @NatNatPair <"arg3"> # Nat <"arg4"> # Nat <"arg5"> -> @NatNatPair  internal defined_by_rewrite_rules;
     @msd <"msd">:  Nat <"arg"> -> @word                                                                         internal defined_by_rewrite_rules;
% functions for pairs.
     @first <"nnfirst"> : @NatNatPair <"arg"> -> Nat                                                             internal defined_by_rewrite_rules;
     @last <"nnlast"> : @NatNatPair <"arg"> -> Nat                                                               internal defined_by_rewrite_rules;
     @first <"wnfirst"> : @wordNatPair <"arg"> -> @word                                                          internal defined_by_rewrite_rules;
     @last <"wnlast"> : @wordNatPair <"arg"> -> Nat                                                              internal defined_by_rewrite_rules;
% The functions below are auxiliary and might be omitted. 
     @swap_zero <"swap_zero">:Nat <"left"> # Nat <"right"> -> Nat                                                internal defined_by_rewrite_rules;
     @swap_zero_add <"swap_zero_add">:Nat <"arg1"> # Nat <"arg2"> # Nat <"arg3"> # Nat <"arg4"> -> Nat           internal defined_by_rewrite_rules;
     @swap_zero_min <"swap_zero_min">:Nat <"arg1"> # Nat <"arg2"> # Nat <"arg3"> # Nat <"arg4"> -> Nat           internal defined_by_rewrite_rules;
     @swap_zero_monus <"swap_zero_monus">:Nat <"arg1"> # Nat <"arg2"> # Nat <"arg3"> # Nat <"arg4"> -> Nat       internal defined_by_rewrite_rules;
     @sqrt_nat <"sqrt_nat_aux_func">:Nat <"arg1"> # Nat <"arg2"> # Pos <"arg3"> -> Nat                           internal defined_by_rewrite_rules;
     @dub <"dub">:Bool <"left"> # Nat <"right"> -> Nat                                                           internal defined_by_rewrite_rules;
     @gtesubtb <"gte_subtract_with_borrow">: Bool <"arg1"> # Pos <"arg2"> # Pos <"arg3"> -> Nat                  internal defined_by_rewrite_rules;
%     @divmod <"divmod"> : Pos <"left"> # Pos <"right"> -> @NatPair                                               internal defined_by_rewrite_rules;
%     @gdivmod <"generalised_divmod"> : @NatPair <"arg1"> # Bool <"arg2"> # Pos <"arg3"> -> @NatPair              internal defined_by_rewrite_rules;
%     @ggdivmod <"doubly_generalised_divmod"> : Nat <"arg1"> # Nat <"arg2"> # Pos <"arg3"> -> @NatPair            internal defined_by_rewrite_rules;


var  b:Bool;
     p:Pos;
     n:Nat;
     n1:Nat;
     n2:Nat;
     m:Nat;
     m1:Nat;
     m2:Nat;
     predp:Nat;
     diff:Nat;
     shift_n1:Nat;
     pair_:@NatNatPair;
     lp:@NatNatPair;
     w:@word;
     w1:@word;
     w2:@word;
     w3:@word;
     w4:@word;
     shift_w:@word;

eqn  @c0 = @most_significant_digitNat(@zero_word);
     succ(@most_significant_digitNat(w)) = if(==(w,@max_word),
                                                    @concat_digit(@most_significant_digitNat(@one_word),@zero_word),
                                                    @most_significant_digitNat(@succ_word(w)));
     succ(@concat_digit(n,w)) = if(==(w,@max_word),
                                           @concat_digit(succ(n),@zero_word),
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
     pred(@most_significant_digit(w)) = @most_significant_digitNat(@pred_word(w));
     pred(@concat_digit(p,w)) = if(==(w,@zero_word),
                                   @pred_whr(pred(p)),
                                   @concat_digit(Pos2Nat(p),@pred_word(w)));
     @pred_whr(predp) = if(==(predp,@most_significant_digitNat(@zero_word)),
                                    @most_significant_digitNat(@max_word),
                                    @concat_digit(predp,@max_word));
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
     @monus_whr(n1,w1,n2,w2,diff) = if(<(w1,w2),
                                        if(==(diff,@most_significant_digitNat(@zero_word)),
                                               @most_significant_digitNat(@zero_word),
                                               if(==(diff,@most_significant_digitNat(@one_word)),
                                                        @most_significant_digitNat(@minus_word(w1,w2)),
                                                        @concat_digit(@natpred(diff),@minus_word(w1,w2)))),
                                        if(==(diff,@most_significant_digitNat(@zero_word)),
                                               @most_significant_digitNat(@minus_word(w1,w2)),
                                               @concat_digit(diff,@minus_word(w1,w2))));


%     @gtesubtb(false,p,@c1) = pred(p);
%     @gtesubtb(true,p,@c1) = pred(Nat2Pos(pred(p)));
%     @gtesubtb(b,@cDub(c,p),@cDub(c,q)) = @dub(b, @gtesubtb(b,p,q));
%     @gtesubtb(b,@cDub(false,p),@cDub(true,q)) = @dub(!(b),@gtesubtb(true,p,q));
%     @gtesubtb(b,@cDub(true,p),@cDub(false,q)) = @dub(!(b),@gtesubtb(false,p,q));

     *(@most_significant_digitNat(w1),@most_significant_digitNat(w2)) =
                           if(==(@times_overflow_word(w1,w2),@zero_word),
                                     @most_significant_digitNat(@times_word(w1,w2)),
                                     @concat_digit(@most_significant_digitNat(@times_overflow_word(w1,w2)),@times_word(w1,w2)));
     *(@concat_digit(n1,w1),@most_significant_digitNat(w2)) =
                           if(==(w2,@zero_word),
                                @most_significant_digitNat(@zero_word),
                                if(==(@times_overflow_word(w1,w2),@zero_word),
                                     @concat_digit(*(n1,@most_significant_digitNat(w2)), @times_word(w1,w2)),
                                     @concat_digit(
                                             +(*(n1,@most_significant_digitNat(w2)),@most_significant_digitNat(@times_overflow_word(w1,w2))),@times_word(w1,w2))));
     *(@most_significant_digitNat(w1),@concat_digit(n2,w2)) =
                           if(==(w1,@zero_word),
                                @most_significant_digitNat(@zero_word),
                                if(==(@times_overflow_word(w1,w2),@zero_word),
                                     @concat_digit(*(n2,@most_significant_digitNat(w1)), @times_word(w1,w2)),
                                     @concat_digit(
                                             +(*(n2,@most_significant_digitNat(w1)),@most_significant_digitNat(@times_overflow_word(w1,w2))),@times_word(w1,w2))));
     *(@concat_digit(n1,w1),@concat_digit(n2,w2)) =
                           +(@concat_digit(*(@concat_digit(n1,w1),n2),@zero_word),
                                       *(@concat_digit(n1,w1),@most_significant_digitNat(w2)));

     @is_odd(@most_significant_digitNat(w)) = @rightmost_bit(w);
     @is_odd(@concat_digit(n,w)) = @rightmost_bit(w);

     @div2(@most_significant_digitNat(w)) = @most_significant_digitNat(@shift_right(false,w));
     @div2(@concat_digit(n,w)) = if(==(n,@most_significant_digitNat(@zero_word)),
                                     @most_significant_digitNat(@shift_right(@is_odd(n),w)),
                                     @concat_digit(@div2(n),@shift_right(@is_odd(n),w)));

     @msd(@most_significant_digitNat(w)) = w;
     @msd(@concat_digit(n,w)) = @msd(n);

     exp(n,@most_significant_digitNat(w)) = @exp_aux3n(@rightmost_bit(w),n,w);
     exp(n,@concat_digit(n1,w1)) = @exp_aux4n(@rightmost_bit(w1),n,n1,w1);
 
     @exp_aux3n(true,n,w) = if(==(w,@one_word),
                                n,
                                *(n,@exp_aux3n(@rightmost_bit(@shift_right(false,w)),*(n,n),@shift_right(false,w))));
 
     @exp_aux3n(false,n,w) = if(==(w,@zero_word),
                                @most_significant_digitNat(@one_word),
                                @exp_aux3n(@rightmost_bit(@shift_right(false,w)),*(n,n),@shift_right(false,w)));
 
     @exp_aux4n(true,n,n1,w) = @exp_auxtruen(n,@div2(n1),@shift_right(@is_odd(n1), w));
     @exp_auxtruen(n,shift_n1,shift_w) =
                            if(==(shift_n1,@most_significant_digitNat(@zero_word)),
                                *(n,@exp_aux3n(@rightmost_bit(shift_w),*(n,n),shift_w)),
                                *(n,@exp_aux4n(@rightmost_bit(shift_w),*(n,n),shift_n1,shift_w)));

     @exp_aux4n(false,n,n1,w) = @exp_auxfalsen(n,shift_n1,shift_w);
     @exp_auxfalsen(n,shift_n1,shift_w) =
                            if(==(shift_n1,@most_significant_digitNat(@zero_word)),
                                @exp_aux3n(@rightmost_bit(shift_w),*(n,n),shift_w),
                                @exp_aux4n(@rightmost_bit(shift_w),*(n,n),shift_n1,shift_w));
 
     exp(p,@most_significant_digitNat(w)) = @exp_aux3p(@rightmost_bit(w),p,w);
     exp(p,@concat_digit(n1,w1)) = @exp_aux4p(@rightmost_bit(w1),p,n1,w1);
 
     @exp_aux3p(true,p,w) = if(==(w,@one_word),
                                p,
                                *(p,@exp_aux3p(@rightmost_bit(@shift_right(false,w)),*(p,p),@shift_right(false,w))));
                                
     @exp_aux3p(false,p,w) = if(==(w,@zero_word),
                                 @most_significant_digit(@zero_word),
                                 @exp_aux3p(@rightmost_bit(@shift_right(false,w)),*(p,p),@shift_right(false,w)));

     @exp_aux4p(true,p,n1,w) = @exp_auxtruep(p,@div2(n1),@shift_right(@is_odd(n1), w));
     @exp_auxtruep(p,shift_n1,shift_w) =
                           if(==(shift_n1,@most_significant_digitNat(@zero_word)),
                                *(p,@exp_aux3p(@rightmost_bit(shift_w),*(p,p),shift_w)),
                                *(p,@exp_aux4p(@rightmost_bit(shift_w),*(p,p),shift_n1,shift_w)));

     @exp_aux4p(false,p,n1,w) = @exp_auxfalsep(p,@div2(n1),@shift_right(@is_odd(n1),w));
     @exp_auxfalsep(p,shift_n1,shift_w) =
                           if(==(shift_n1,@most_significant_digitNat(@zero_word)),
                                @exp_aux3p(@rightmost_bit(shift_w),*(p,p),shift_w),
                                @exp_aux4p(@rightmost_bit(shift_w),*(p,p),shift_n1,shift_w));
 

    div(@most_significant_digitNat(w1),@most_significant_digit(w2)) = @most_significant_digitNat(@div_word(w1,w2));
    mod(@most_significant_digitNat(w1),@most_significant_digit(w2)) = @most_significant_digitNat(@mod_word(w1,w2));

    div(@most_significant_digitNat(w1),@concat_digit(p,w2)) = @most_significant_digitNat(@zero_word);
    mod(@most_significant_digitNat(w1),@concat_digit(p,w2)) = @most_significant_digitNat(w1);

    >(n,@most_significant_digitNat(@zero_word)) ->
    div(@concat_digit(n,w1),@most_significant_digit(w2)) = @div_whr1(n,w1,w2,@divmod_aux(n,@most_significant_digit(w2)));

    @div_whr1(n,w1,w2,pair_) = 
                          if(<(n,@most_significant_digitNat(w2)),
                                    @most_significant_digitNat(@div_bold(@concat_digit(n,w1),@most_significant_digit(w2))),
                                    if (==(@first(pair_),@most_significant_digitNat(@zero_word)),
                                        @most_significant_digitNat(
                                           @div_bold(
                                               if(==(@last(pair_),@most_significant_digitNat(@zero_word)), 
                                                   @most_significant_digitNat(w1),
                                                   @concat_digit(@last(pair_),w1)),
                                               @most_significant_digit(w2))),
                                        @concat_digit(@first(pair_),
                                           @div_bold(
                                               if(==(@last(pair_),@most_significant_digitNat(@zero_word)), 
                                                   @most_significant_digitNat(w1),
                                                   @concat_digit(@last(pair_),w1)),
                                               @most_significant_digit(w2)))));

    >(n,@most_significant_digitNat(@zero_word)) ->
    mod(@concat_digit(n,w1),@most_significant_digit(w2)) = @most_significant_digitNat(@mod_doubleword(@msd(mod(n,@most_significant_digit(w2))),w1,w2));

    >(n,@most_significant_digitNat(@zero_word)) ->
    div(@concat_digit(n,w1),@concat_digit(p,w2)) =
                          if(<(n,Pos2Nat(@concat_digit(p,w2))),
                                  @most_significant_digitNat(@div_bold(@concat_digit(n,w1),@concat_digit(p,w2))),
                                  @div_whr2(n,w1,w2,@divmod_aux(n,@concat_digit(p,w2))));
    @div_whr2(n,w1,w2,pair_) = 
                          +(if(==(@first(pair_),@most_significant_digitNat(@zero_word)), 
                                    @most_significant_digitNat(@zero_word),
                                    @concat_digit(@first(pair_),@zero_word)),
                               @most_significant_digitNat(@div_bold(if(==(@last(pair_),@most_significant_digitNat(@zero_word)), 
                                                                         @most_significant_digitNat(w1),
                                                                         @concat_digit(@last(pair_),w1)),
                                                          @concat_digit(p,w2))));

    >(n,@most_significant_digitNat(@zero_word)) ->
    mod(@concat_digit(n,w1),@concat_digit(p,w2)) = @mod_whr1(w1,p,w2,mod(n,@concat_digit(p,w2)));

    @mod_whr1(w1,p,w2,m1) = 
                          @monus(if(>(m1,@most_significant_digitNat(@zero_word)),@concat_digit(m1,w1),@most_significant_digitNat(w1)),
                                 *(@concat_digit(Pos2Nat(p),w2),
                                   @most_significant_digitNat(
                                        @div_bold(
                                             if(>(m1,@most_significant_digitNat(@zero_word)),@concat_digit(m1,w1),@most_significant_digitNat(w1)),
                                             @concat_digit(p,w2)))));

    @divmod_aux(@most_significant_digitNat(w1),@most_significant_digit(w2)) = 
                                      @nnPair(@most_significant_digitNat(@div_word(w1,w2)),
                                              @most_significant_digitNat(@mod_word(w1,w2)));

    @divmod_aux(@most_significant_digitNat(w1),@concat_digit(p,w2)) = @nnPair(@most_significant_digitNat(@zero_word),@most_significant_digitNat(w1));

    >(n,@most_significant_digitNat(@zero_word)) ->
    @divmod_aux(@concat_digit(n,w1),@most_significant_digit(w2)) = @divmod_aux_whr1(n,w1,w2,@divmod_aux(n,@most_significant_digit(w2)));
    
    @divmod_aux_whr1(n,w1,w2,pair_) =
                          @nnPair(if(<(n,@most_significant_digitNat(w2)),
                                    @most_significant_digitNat(@div_bold(@concat_digit(n,w1),@most_significant_digit(w2))),
                                    if (==(@first(pair_),@most_significant_digitNat(@zero_word)),
                                        @most_significant_digitNat(@div_bold(
                                            if(==(@last(pair_),@most_significant_digitNat(@zero_word)), 
                                                 @most_significant_digitNat(w1),
                                                 @concat_digit(@last(pair_),w1)),@most_significant_digit(w2))),
                                        @concat_digit(@first(pair_),@div_bold(
                                            if(==(@last(pair_),@most_significant_digitNat(@zero_word)), 
                                                 @most_significant_digitNat(w1),
                                                 @concat_digit(@last(pair_),w1)),@most_significant_digit(w2))))),
                                  @most_significant_digitNat(@mod_doubleword(@msd(@last(pair_)),w1,w2)));

    >(n,@most_significant_digitNat(@zero_word)) ->
    @divmod_aux(@concat_digit(n,w1),@concat_digit(p,w2)) =
                          if(<(n,Pos2Nat(@concat_digit(p,w2))),
                             @divmod_aux_whr2(n,w1,p,w2,mod(n,@concat_digit(p,w2))),
                             @divmod_aux_whr4(w1,p,w2,@divmod_aux(n,@concat_digit(p,w2))));

    @divmod_aux_whr2(n,w1,p,w2,lp) = @divmod_aux_whr3(n,w1,p,w2,if(==(lp,@most_significant_digitNat(@zero_word)),@most_significant_digitNat(w1),@concat_digit(lp,w1)));
    @divmod_aux_whr3(n,w1,p,w2,m) = @nnPair(@most_significant_digitNat(@div_bold(@concat_digit(n,w1),@concat_digit(p,w2))),
                                            @monus(m,*(@concat_digit(Pos2Nat(p),w2),@most_significant_digitNat(@div_bold(m,@concat_digit(p,w2))))));
    @divmod_aux_whr4(w1,p,w2,pair_) = 
                         @divmod_aux_whr5(p,w2,pair_,
                                          if(==(@last(pair_),@most_significant_digitNat(@zero_word)),@most_significant_digitNat(w1),@concat_digit(@last(pair_),w1)));
    @divmod_aux_whr5(p,w2,pair_,m) = @divmod_aux_whr6(p,w2,pair_,m,@most_significant_digitNat(@div_bold(m,@concat_digit(p,w2))));

    @divmod_aux_whr6(p,w2,pair_,m,m1) = 
                         @nnPair(+(@concat_digit(@first(pair_),@zero_word),m1),
                                 @monus(m,*(@concat_digit(Pos2Nat(p),w2),m1)));

    @div_bold(@most_significant_digitNat(w1),@most_significant_digit(w2)) = @div_word(w1,w2);

    @div_bold(@most_significant_digitNat(w1),@concat_digit(p,w2)) = @zero_word;

    @div_bold(@concat_digit(@most_significant_digitNat(w1),w2),@most_significant_digit(w3)) = @div_doubleword(w1,w2,w3);

    @div_bold(@concat_digit(@most_significant_digitNat(w),w1),@concat_digit(@most_significant_digit(w2),w3)) =
                                               @div_double_doubleword(w,w1,w2,w3);

    @div_bold(@concat_digit(@concat_digit(@most_significant_digitNat(w),w1),w2),@concat_digit(@most_significant_digit(w3),w4)) =
                                               @div_triple_doubleword(w,w1,w2,w3,w4);

    >(n,@most_significant_digitNat(@zero_word)) ->
    @div_bold(@concat_digit(n,w1),@concat_digit(@concat_digit(p,w2),w3)) =
                          if(>=(n,@concat_digit(Pos2Nat(@concat_digit(p,w2)),@zero_word)),
                             @max_word,
                             @div_bold_whr(n,w1,p,w2,w3,@div_bold(n,@concat_digit(p,w2))));

    @div_bold_whr(n,w1,p,w2,w3,m) = 
                             if(>(*(@most_significant_digitNat(m),Pos2Nat(@concat_digit(@concat_digit(p,w2),w3))),@concat_digit(n,w1)),
                                @pred_word(m),
                                m);


% equations for pairs
     ==(@nnPair(n1,n2), @nnPair(m1,m2)) = &&(==(n1,m1),==(n2,m2));
     <(@nnPair(n1,n2), @nnPair(m1,m2)) = ||(<(n1,m1), &&(==(n1,m1), <(n2,m2)));
     <=(@nnPair(n1,n2), @nnPair(m1,m2)) = ||(<(n1,m1), &&(==(n1,m1), <=(n2,m2)));
     ==(@wnPair(w1,n1), @wnPair(w2,n2)) = &&(==(w1,w2),==(n1,n2));
     <(@wnPair(w1,n1), @wnPair(w2,n2)) = ||(<(w1,w2), &&(==(w1,w2), <(n1,n2)));
     <=(@wnPair(w1,n1), @wnPair(w2,n2)) = ||(<(w1,w2), &&(==(w1,w2), <=(n1,n2)));
     @first(@nnPair(m,n)) = m;
     @last(@nnPair(m,n)) = n;
     @first(@wnPair(w,n)) = w;
     @last(@wnPair(w,n)) = n;

%Residues. 

%     @swap_zero(m,@c0) = m;
%     @swap_zero(@c0,n) = n;
%     @swap_zero(@cNat(p),@cNat(p)) = @c0;
%     !=(p,q) -> @swap_zero(@cNat(p),@cNat(q)) = @cNat(q);
     @swap_zero(m,n) = if(==(n,@most_significant_digitNat(@zero_word)), m,
                       if(==(m,@most_significant_digitNat(@zero_word)), n,
                       if(==(n,m), @most_significant_digitNat(@zero_word), n)));

%     @swap_zero_add(@c0, @c0, m, n) = +(m,n);
%     @swap_zero_add(@c0, @cNat(p), @c0, n) = n;
%     @swap_zero_add(@c0, @cNat(p), @cNat(q), n) = @swap_zero(@cNat(p), +(@cNat(q), @swap_zero(@cNat(p), n)));
%     @swap_zero_add(@cNat(p), @c0, m, @c0) = m;
%     @swap_zero_add(@cNat(p), @c0, m, @cNat(q)) = @swap_zero(@cNat(p), +(@swap_zero(@cNat(p),m), @cNat(q)));
%     @swap_zero_add(@cNat(p), @cNat(q), m, n) = @swap_zero(+(@cNat(p), @cNat(q)), +(@swap_zero(@cNat(p),m),@swap_zero(@cNat(q),n)));
     @swap_zero_add(n1,n2,m1,m2) = 
                       if(==(n1,@most_significant_digitNat(@zero_word)),
                            if(==(n2,@most_significant_digitNat(@zero_word)), 
                                 +(m1,m2),
                                 if(==(m1,@most_significant_digitNat(@zero_word)),
                                    m2,
                                    @swap_zero(n2, +(m1, @swap_zero(n2, m2))))),
                            if(==(n2,@most_significant_digitNat(@zero_word)), 
                                 if(==(m2,@most_significant_digitNat(@zero_word)),
                                      n1,
                                      @swap_zero(n1, +(@swap_zero(n1,m1), m2))),
                                 @swap_zero(+(n1, n2), +(@swap_zero(n1,m1),@swap_zero(n2,m2)))));

%     @swap_zero_min(@c0, @c0, m, n) = min(m,n);
%     @swap_zero_min(@c0, @cNat(p), @c0, n) = @c0;
%     @swap_zero_min(@c0, @cNat(p), @cNat(q), n) = min(@cNat(q), @swap_zero(@cNat(p),n));
%     @swap_zero_min(@cNat(p), @c0, m, @c0) = @c0;
%     @swap_zero_min(@cNat(p), @c0, m, @cNat(q)) = min(@swap_zero(@cNat(p),m), @cNat(q));
%     @swap_zero_min(@cNat(p), @cNat(q), m, n) = @swap_zero(min(@cNat(p), @cNat(q)), min(@swap_zero(@cNat(p), m), @swap_zero(@cNat(q), n)));
     @swap_zero_min(n1, n2, m1, m2) =
                       if(==(n1,@most_significant_digitNat(@zero_word)),
                            if(==(n2,@most_significant_digitNat(@zero_word)), 
                                 min(m1,m2),
                                 if(==(m1,@most_significant_digitNat(@zero_word)),
                                    @most_significant_digitNat(@zero_word),
                                    min(m1, @swap_zero(n2,m2)))),
                            if(==(n2,@most_significant_digitNat(@zero_word)), 
                                 if(==(m2,@most_significant_digitNat(@zero_word)),
                                      @most_significant_digitNat(@zero_word),
                                      min(@swap_zero(n1,m1), m2)),
                                 @swap_zero(min(n1, n2), min(@swap_zero(n1, m1), @swap_zero(n2, m2)))));

%     @swap_zero_monus(@c0, @c0, m, n) = @monus(m,n);
%     @swap_zero_monus(@c0, @cNat(p), @c0, n) = @c0;
%     @swap_zero_monus(@c0, @cNat(p), @cNat(q), n) = @monus(@cNat(q), @swap_zero(@cNat(p), n));
%     @swap_zero_monus(@cNat(p), @c0, m, @c0) = m;
%     @swap_zero_monus(@cNat(p), @c0, m, @cNat(q)) = @swap_zero(@cNat(p), @monus(@swap_zero(@cNat(p), m), @cNat(q)));
%     @swap_zero_monus(@cNat(p), @cNat(q), m, n) = @swap_zero(@monus(@cNat(p),@cNat(q)),@monus(@swap_zero(@cNat(p),m), @swap_zero(@cNat(q),n)));
     @swap_zero_monus(n1, n2, m1, m2) =
                       if(==(n1,@most_significant_digitNat(@zero_word)),
                            if(==(n2,@most_significant_digitNat(@zero_word)), 
                                 @monus(m1,m2),
                                 if(==(m1,@most_significant_digitNat(@zero_word)),
                                    @most_significant_digitNat(@zero_word),
                                    @monus(m1, @swap_zero(n2, m2)))),
                            if(==(n2,@most_significant_digitNat(@zero_word)), 
                                 if(==(m2,@most_significant_digitNat(@zero_word)),
                                      m1,
                                      @swap_zero(n1, @monus(@swap_zero(n1, m1), m2))),
                                 @swap_zero(@monus(n1,n2),@monus(@swap_zero(n1,m1), @swap_zero(n2,m2)))));

% TODO:
%     sqrt(@c0) = @c0;
%     sqrt(@cNat(p)) = @sqrt_nat(@cNat(p),@c0,@powerlog2(p));
%     @sqrt_nat(n,m,@c1) = if(<=(n,m),@c0,@cNat(@c1));
%     @sqrt_nat(n,m,@cDub(b,p)) =
%               if(>(*(+(@cNat(@cDub(b,p)),m),@cNat(@cDub(b,p))),n),
%                    @sqrt_nat(n,m,p),
%                    +(@cNat(@cDub(b,p)),@sqrt_nat(@monus(n,*(+(@cNat(@cDub(b,p)),m),@cNat(@cDub(b,p)))),+(m,@cNat(@cDub(false,@cDub(b,p)))),p)));

%     @divmod(@c1,@c1) = @nnPair(@cNat(@c1),@c0);
%     @divmod(@c1,@cDub(b,p)) = @nnPair(@c0, @cNat(@c1));
%     @divmod(@cDub(b,p),q) = @gdivmod(@divmod(p,q),b,q);
%     @gdivmod(@nnPair(m,n),b,p) = @ggdivmod(@dub(b,n),m,p);
%     @ggdivmod(@c0,n,p) = @nnPair(@dub(false,n),@c0);
%     <(p,q) -> @ggdivmod(@cNat(p),n,q) = @nnPair(@dub(false,n),@cNat(p));
%     <=(q,p) -> @ggdivmod(@cNat(p),n,q) = @nnPair(@dub(true,n),@gtesubtb(false,p,q));
