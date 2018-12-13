% Author(s): Jan Friso Groote
% Copyright: see the accompanying file COPYING or copy at
% https://github.com/mCRL2org/mCRL2/blob/master/COPYING
%
% Distributed under the Boost Software License, Version 1.0.
% (See accompanying file LICENSE_1_0.txt or copy at
% http://www.boost.org/LICENSE_1_0.txt)
%
% Specification of the Pos data sort that uses machine numbers for efficiency.

#include bool.spec
#include machine_word.spec

sort Pos <"pos">;

cons @one <"one">:Pos                                                                                                                   internal defined_by_rewrite_rules;
% The successor constructor should be merged with the successor below, by removing the latter.
% Currently, this does not work, as the translator to C code does not see that there are multiple
% successor functions of different types, as this one is a constructor. 
    @succ_pos <"succpos">:Pos <"arg"> -> Pos                                                                                            internal defined_by_rewrite_rules;

map  @most_significant_digit <"most_significant_digit">: @word <"arg"> -> Pos                                                           internal defined_by_rewrite_rules;
     @concat_digit <"concat_digit">: Pos <"arg1"> # @word <"arg2"> -> Pos                                                               internal defined_by_rewrite_rules;
     max <"maximum">: Pos <"left"> # Pos <"right"> -> Pos                                                                               external defined_by_rewrite_rules;
     min <"minimum">: Pos <"left"> # Pos <"right"> -> Pos                                                                               external defined_by_rewrite_rules;
% There is a special mapping succ, as overloading a constructor is not possible. Therefore the constructor @succ_pos has a unique name. 
     succ <"succ">: Pos <"arg"> -> Pos                                                                                                  external defined_by_rewrite_rules;
     @pospred <"pos_predecessor">: Pos <"arg"> -> Pos                                                                                   internal defined_by_rewrite_rules;
     + <"plus">: Pos <"left"> # Pos <"right"> -> Pos                                                                                    external defined_by_rewrite_rules;
% The following function is used when the symbol + is overloaded, such as in fbags. 
     @plus_pos <"auxiliary_plus_pos">: Pos <"left"> # Pos <"right"> -> Pos                                                              external defined_by_rewrite_rules;
     * <"times">: Pos <"left"> # Pos <"right"> -> Pos                                                                                   external defined_by_rewrite_rules;
     @powerlog2 <"powerlog2_pos">: Pos <"arg"> -> Pos                                                                                   internal defined_by_rewrite_rules;
% Auxiliary function to implement multiplication that uses where clauses.
     @times_whr_mult_overflow <"times_whr_mult_overflow">: @word <"arg1"> # @word <"arg2"> # Pos <"arg3"> # @word <"arg4"> -> Pos       internal defined_by_rewrite_rules;
% The functions below are used in the old datatypes (as described in the 2014 book). If not used anymore, they can be removed. 
%     @addc <"add_with_carry">: Bool <"arg1"> #Pos <"arg2"> #Pos <"arg3">->Pos                                                           internal defined_by_rewrite_rules;
     @c1 <"c1">: Pos                                                                                                                    internal defined_by_rewrite_rules;
%     @cDub <"cdub">:Bool <"left"> # Pos <"right"> -> Pos                                                                                internal defined_by_rewrite_rules;

% auxiliary functions that should not be made availabe to users of mCRL2.
% @Pos2Pos64 : Pos_old -> Pos;
% @Pos642Pos : Pos -> Pos_old;

var  b: Bool;
     p: Pos;
     p1: Pos;
     p2: Pos;
     w: @word;
     w1: @word;
     w2: @word;
eqn  @one = @most_significant_digit(@one_word);
     @succ_pos(p) = succ(p);
     succ(@most_significant_digit(w1)) = if(==(w1,@max_word),
                                              @concat_digit(@most_significant_digit(@one_word),@zero_word),
                                              @most_significant_digit(@succ_word(w1)));
     succ(@concat_digit(p,w1)) = if(==(w1,@max_word),
                                             @concat_digit(succ(p),@zero_word),
                                             @concat_digit(p,@succ_word(w1)));
 
% The rules for comparison operators must be explicitly be defined on succ (= @succ_pos) to allow enumeration of positive numbers.
     ==(@most_significant_digit(w1),@most_significant_digit(w2)) = ==(w1,w2);
     ==(@concat_digit(p,w1),@most_significant_digit(w2)) = false;
     ==(@most_significant_digit(w1),@concat_digit(p,w2)) = false;
     ==(@concat_digit(p1,w1),@concat_digit(p2,w2)) = &&(==(p1,p2), ==(w1,w2));
     ==(succ(p1),p2) = &&(!(==(p2,@most_significant_digit(@one_word))),==(p1,@pospred(p2)));
     ==(p1, succ(p2)) = &&(!(==(p1,@most_significant_digit(@one_word))),==(@pospred(p1),p2));
 
     <(@most_significant_digit(w1),@most_significant_digit(w2)) = <(w1,w2);
     <(@concat_digit(p,w1),@most_significant_digit(w2)) = false;
     <(@most_significant_digit(w1),@concat_digit(p,w2)) = true;
     <(@concat_digit(p1,w1),@concat_digit(p2,w2)) = if(<(w1,w2),<=(p1,p2),<(p1,p2));
     <(succ(p1),p2) = &&(<(@most_significant_digit(@two_word),p2),<(p1,@pospred(p2)));
     <(p1, succ(p2)) = <=(p1,p2);
     ==(w1,@one_word) ->  <(p,@most_significant_digit(w1)) = false;
 
     <=(@most_significant_digit(w1),@most_significant_digit(w2)) = <=(w1,w2);
     <=(@concat_digit(p,w1),@most_significant_digit(w2)) = false;
     <=(@most_significant_digit(w1),@concat_digit(p,w2)) = true;
     <=(@concat_digit(p1,w1),@concat_digit(p2,w2)) = if(<=(w1,w2),<=(p1,p2),<(p1,p2));
     <=(succ(p1),p2) = <(p1,p2);
     <=(p1, succ(p2)) = <=(@pospred(p1),p2);
     ==(w1,@one_word) ->  <=(@most_significant_digit(w1), p) = true;
 
     max(p1,p2) = if(<=(p1,p2),p2,p1);
     min(p1,p2) = if(<=(p1,p2),p1,p2);

     @pospred(@most_significant_digit(w1)) = if(==(w1,@one_word),
                                                         @most_significant_digit(@one_word),
                                                         @most_significant_digit(@pred_word(w1)));
     @pospred(@concat_digit(p,w1)) = if(==(w1,@zero_word),
                                            if(==(p,@most_significant_digit(@one_word)),
                                                         @most_significant_digit(@max_word),
                                                         @concat_digit(@pospred(p),@max_word)),
                                            @concat_digit(p,@pred_word(w1)));

     +(@most_significant_digit(w1),@most_significant_digit(w2)) =
                                     if(==(@add_overflow_word(w1,w2),@zero_word),
                                                    @most_significant_digit(@add_word(w1,w2)),
                                                    @concat_digit(@most_significant_digit(@one_word),(@add_word(w1,w2))));
     +(@concat_digit(p1,w1),@most_significant_digit(w2)) = @concat_digit(+(@most_significant_digit(@add_overflow_word(w1,w2)),p1),
                                                                         @add_word(w1,w2));
     +(@most_significant_digit(w1),@concat_digit(p2,w2)) = @concat_digit(+(@most_significant_digit(@add_overflow_word(w1,w2)), p2),
                                                                         @add_word(w1,w2));
     +(@concat_digit(p1,w1),@concat_digit(p2,w2)) = @concat_digit(+(@most_significant_digit(@add_overflow_word(w1,w2)), +(p1,p2)),
                                                                       @add_word(w1,w2));
% The rules below are useful in solving expressions with plus and quantifiers.
     +(succ(p1),p2) = succ(+(p1,p2));
     +(p1,succ(p2)) = succ(+(p1,p2));

     @plus_pos(p1,p2) = +(p1,p2);

% The definition below uses where clauses. The where clauses are translated away by the introduction of the 
% function @times_whr_mult_overflow. 
%      *(@most_significant_digit(w1),@most_significant_digit(w2)) =
%                                                 if(==(overflow,@zero_word),
%                                                       @most_significant_digit(mult),
%                                                       @concat_digit(@most_significant_digit(overflow),mult))
%                                                 whr mult=@times_word(w1,w2),
%                                                     overflow=@times_overflow_word(w1,w2) end;
 
     *(@most_significant_digit(w1),@most_significant_digit(w2)) =
                @times_whr_mult_overflow(w1,w2,@times_word(w1,w2),@times_overflow_word(w1,w2));

     @times_whr_mult_overflow(w1,w2,p,w) = if(==(w,@zero_word),
                                              @most_significant_digit(p),
                                              @concat_digit(@most_significant_digit(w),p));
 
     *(@concat_digit(p1,w1),@most_significant_digit(w2)) = @concat_digit(
                                                                  +(*(p1,@most_significant_digit(w2)),@most_significant_digit(@times_overflow_word(w1,w2))),
                                                                  @times_word(w1,w2));

     *(@most_significant_digit(w1),@concat_digit(p2,w2)) = @concat_digit(
                                                                  +(*(p2,@most_significant_digit(w1)),@most_significant_digit(@times_overflow_word(w1,w2))),
                                                                  @times_word(w1,w2));

     *(@concat_digit(p1,w1),@concat_digit(p2,w2)) = +(@concat_digit(*(@concat_digit(p1,w1),p2),@zero_word),
                                                           *(@concat_digit(p1,w1),@most_significant_digit(w2)));
%     @addc(true,p1,p2) = +(+(p1,p2),@one);
%     @addc(false,p1,p2) = +(p1,p2);
     @c1 = @most_significant_digit(@one_word);
%     @cDub(true,p) = +(+(p,p),@one);
%     @cDub(false,p) = +(p,p);

