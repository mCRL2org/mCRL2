#~ Copyright 2010 Jonathan Nelisse.
#~ Distributed under the Boost Software License, Version 1.0.
#~ (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

# This file contains tables that are used to generate classes and traversal functions
# for these classes. A prerequisite is that each class has a corresponding ATerm
# representation (the first column of each table). The second column contains the
# constructor of the classes. If the name of the class has a postfix between brackets
# like variable[_base] then the generated class will be called variable_base, but the
# traversal will use variable. This is done to enable the user of the class to add
# additional behavior to the base class.

import re
import string

NUMERIC_EXPRESSION_CLASSES = r'''
Common          | common(const common_expression& operand)                                | A common
Number          | number(const number_expression& number)                                 | A number
Card            | card(const set& set)                                                    | A card
Length          | length(const seq& seq)                                                  | A length
Plus            | plus(const numeric_expression& left, const numeric_expression& right)   | An addition
Minus           | minus(const numeric_expression& left, const numeric_expression& right)  | A subtraction
Times           | times(const numeric_expression& left, const numeric_expression& right)  | A multiplication
Div             | div(const numeric_expression& left, const numeric_expression& right)    | A division
Mod             | mod(const numeric_expression& left, const numeric_expression& right)    | A modulo operation
Min             | min(const numeric_expression& operand)                                  | A minimum?
'''                                                                                                                                 
