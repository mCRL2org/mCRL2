#~ Copyright 2010 Wieger Wesselink.
#~ Distributed under the Boost Software License, Version 1.0.
#~ (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

from generate_term_functions import *
from generate_classes import *
from fdr_classes import *

if __name__ == "__main__":
    rules = parse_ebnf('../doc/CSP-term.txt')
    generate_soundness_check_functions(rules, '../include/mcrl2/fdr/detail/syntax_checks.h')
    generate_libstruct_functions(rules, '../include/mcrl2/fdr/detail/term_functions.h')
    generate_constructor_functions(rules, '../include/mcrl2/fdr/detail/constructors.h')
    make_expression_classes('../include/mcrl2/fdr/numeric_expression.h', NUMERIC_EXPRESSION_CLASSES, 'numeric_expression', 'fdr')
    make_is_functions('../include/mcrl2/fdr/numeric_expression.h', NUMERIC_EXPRESSION_CLASSES, 'numeric_expression', 'fdr')
