#!/usr/bin/env python

#~ Copyright 2018 Wieger Wesselink.
#~ Distributed under the Boost Software License, Version 1.0.
#~ (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

import random

FORMULAS = '''[!a*.b]false && [!c*.b]false && [!d*.b]false && [true*.c.!a*.b]false
nu X.<a || b || c || d>X && [a]false
nu X. (([!a]X && [b]false))
nu X. (([!a]X && [c]false))
nu X. mu Y. (<a>X || <!a>Y)
nu X. mu Y. (<a>X || <!b>Y)
[true*] [a.(!a && !b)*.b.(!a)*.b]false
[true*.(a + b)]mu X.[!c]X
([true*.a]mu X.[!d]X) && ([true*.a]mu X.[!c]X) && ([true*.a]mu X.[!b]X)
[true*](([a](nu X. mu Y. ([b]X && [!b]Y))))
[true*]([a](nu X. mu Y. ([b]X && [!b]Y)))
[true*]([a](nu X. mu Y. ([c]X && [!c]Y)))
<true*>(<a>(nu X. mu Y. (<c>X || <!d && !b>Y)))
<true*.a>true
[true*] [a.(!a && !c)*.c.(!a)*.c]false
[true*]mu X.[a]X
[true*](mu Y. ([!a]Y && <true>true))
([true*] nu X. mu Y. nu Z. ([a]X && ([a]false || [!a]Y) && [!a]Z))
[true*] nu X. mu Y. nu Z. ([a]X && ([a]false || [!a]Y) && [!a]Z)'''

SRF_FORMULAS = '''[true*]<true>true
<true*.a>true
nu X. mu Y. (<a>X || <!a>Y)
nu X. mu Y. (<a>X || <!b>Y)'''

def make_modal_formula():
    return random.choice(FORMULAS.split('\n') + SRF_FORMULAS.split('\n'))

def make_srf_modal_formula():
    return random.choice(SRF_FORMULAS.split('\n'))

if __name__ == '__main__':
    print(make_modal_formula())
