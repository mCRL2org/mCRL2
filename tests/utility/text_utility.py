#~ Copyright 2014 Wieger Wesselink.
#~ Distributed under the Boost Software License, Version 1.0.
#~ (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

def read_text(filename):
    with open(filename, 'r', encoding="UTF-8") as f:
        return f.read()

def write_text(filename, text):
    with open(filename, 'w', encoding="UTF-8") as f:
        f.write(text)
