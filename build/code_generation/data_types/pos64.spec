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

sort Pos64 <"pos64">;

cons @one64 <"one64">:Pos64;
     succ64 <"succ64">:Pos64 <"arg"> -> Pos64;

map  @most_significant_digit <"most_significant_digit">: @word <"arg"> -> Pos64;
     @concat_digit <"concat_digit">: Pos64 <"arg1"> # @word <"arg2"> -> Pos64;        
     max <"maximum">: Pos64 <"left"> # Pos64 <"right"> -> Pos64;
     min <"minimum">: Pos64 <"left"> # Pos64 <"right"> -> Pos64;
     @pospred <"pos_predecessor">: Pos64 <"arg"> -> Pos64;
     + <"plus">: Pos64 <"left"> # Pos64 <"right"> -> Pos64;
     * <"times">: Pos64 <"left"> # Pos64 <"right"> -> Pos64;
     powerlog2 <"powerlog2_pos">: Pos64 <"arg"> -> Pos64;
% auxiliary functions that should not be made availabe to users of mCRL2.
% @Pos2Pos64 : Pos -> Pos64;
% @Pos642Pos : Pos64 -> Pos;

var  p: Pos64;
     p1: Pos64;
     p2: Pos64;
     w: @word;
     w1: @word;
     w2: @word;
%      n:Pos;
eqn  @one64 = @most_significant_digit(@one_word);
 
     succ64(@most_significant_digit(w1)) = if(==(w1,max_word),
                                             @concat_digit(@most_significant_digit(one_word),zero_word),
                                             @most_significant_digit(succ_word(w1)));
     succ64(@concat_digit(p,w1)) = if(==(w1,max_word),
                                            @concat_digit(succ64(p),zero_word),
                                            @concat_digit(p,succ_word(w1)));

     ==(@most_significant_digit(w1),@most_significant_digit(w2)) = ==(w1,w2);
     ==(@concat_digit(p,w1),@most_significant_digit(w2)) = false;
     ==(@most_significant_digit(w1),@concat_digit(p,w2)) = false;
     ==(@concat_digit(p1,w1),@concat_digit(p2,w2)) = &&(==(p1,p2), ==(w1,w2));

     <(@most_significant_digit(w1),@most_significant_digit(w2)) = <(w1,w2);
     <(@concat_digit(p,w1),@most_significant_digit(w2)) = false;
     <(@most_significant_digit(w1),@concat_digit(p,w2)) = true;
     <(@concat_digit(p1,w1),@concat_digit(p2,w2)) = if(<(w1,w2),<=(p1,p2),<(p1,p2));

     <=(@most_significant_digit(w1),@most_significant_digit(w2)) = <=(w1,w2);
     <=(@concat_digit(p,w1),@most_significant_digit(w2)) = false;
     <=(@most_significant_digit(w1),@concat_digit(p,w2)) = true;
     <=(@concat_digit(p1,w1),@concat_digit(p2,w2)) = if(<=(w1,w2),<=(p1,p2),<(p1,p2));

     max(p1,p2) = if(<=(p1,p2),p2,p1);
     min(p1,p2) = if(<=(p1,p2),p1,p2);

     @pospred(@most_significant_digit(w1)) = if(==(w1,one_word),
                                              @most_significant_digit(one_word),
                                              @most_significant_digit(pred_word(w1)));
     @pospred(@concat_digit(p,w1)) = if(==(w1,zero_word),
                                            if(==(p,@most_significant_digit(one_word)),
                                                         @most_significant_digit(max_word),
                                                         @concat_digit(@pospred(p),max_word)),
                                            @concat_digit(p,pred_word(w1)));

     +(@most_significant_digit(w1),@most_significant_digit(w2)) =
                                     if(==(add_overflow_word(w1,w2),zero_word),
                                                    @most_significant_digit(add_word(w1,w2)),
                                                    @concat_digit(@most_significant_digit(one_word),(add_word(w1,w2))));
     +(@concat_digit(p1,w1),@most_significant_digit(w2)) = @concat_digit(+(@most_significant_digit(add_overflow_word(w1,w2)),p1),
                                                                         add_word(w1,w2));
     +(@most_significant_digit(w1),@concat_digit(p2,w2)) = @concat_digit(+(@most_significant_digit(add_overflow_word(w1,w2)), p2),
                                                                         add_word(w1,w2));
     +(@concat_digit(p1,w1),@concat_digit(p2,w2)) = @concat_digit(+(@most_significant_digit(add_overflow_word(w1,w2)), +(p1,p2)),
                                                                       add_word(w1,w2));

%      *(@most_significant_digit(w1),@most_significant_digit(w2)) =
%                                                 if(==(overflow,zero_word),
%                                                       @most_significant_digit(mult),
%                                                       @concat_digit(@most_significant_digit(overflow),mult))
%                                                 whr mult=times_word(w1,w2),
%                                                     overflow=times_overflow_word(w1,w2) end;
 
     *(@concat_digit(p1,w1),@most_significant_digit(w2)) = @concat_digit(
                                                                  +(*(p1,@most_significant_digit(w2)),@most_significant_digit(times_overflow_word(w1,w2))),
                                                                  times_word(w1,w2));

     *(@most_significant_digit(w1),@concat_digit(p2,w2)) = @concat_digit(
                                                                  +(*(p2,@most_significant_digit(w1)),@most_significant_digit(times_overflow_word(w1,w2))),
                                                                  times_word(w1,w2));

     *(@concat_digit(p1,w1),@concat_digit(p2,w2)) = +(@concat_digit(*(@concat_digit(p1,w1),p2),zero_word),
                                                           *(@concat_digit(p1,w1),@most_significant_digit(w2)));

