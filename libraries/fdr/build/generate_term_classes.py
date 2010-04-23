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

    # expression classes (classes that have a given expression class as super class)
    make_class_declarations('../include/mcrl2/fdr/numeric_expression.h', NUMERIC_EXPRESSION_CLASSES, 'numeric_expression', 'fdr', superclass_aterm = 'Number', label = 'numeric expression')
    make_class_definitions('../source/term_functions.cpp', NUMERIC_EXPRESSION_CLASSES, 'numeric_expression', 'fdr', label = 'numeric expression')
    make_is_functions('../include/mcrl2/fdr/numeric_expression.h', NUMERIC_EXPRESSION_CLASSES, 'numeric_expression', 'fdr')

    make_class_declarations('../include/mcrl2/fdr/boolean_expression.h', BOOLEAN_EXPRESSION_CLASSES, 'boolean_expression', 'fdr', superclass_aterm = 'Bool', label = 'boolean expression')
    make_class_definitions('../source/term_functions.cpp', BOOLEAN_EXPRESSION_CLASSES, 'boolean_expression', 'fdr', label = 'boolean expression')
    make_is_functions('../include/mcrl2/fdr/boolean_expression.h', BOOLEAN_EXPRESSION_CLASSES, 'boolean_expression', 'fdr')

    make_class_declarations('../include/mcrl2/fdr/set_expression.h', SET_EXPRESSION_CLASSES, 'set_expression', 'fdr', superclass_aterm = 'Set', label = 'set expression')
    make_class_definitions('../source/term_functions.cpp', SET_EXPRESSION_CLASSES, 'set_expression', 'fdr', label = 'set expression')
    make_is_functions('../include/mcrl2/fdr/set_expression.h', SET_EXPRESSION_CLASSES, 'set_expression', 'fdr')

    make_class_declarations('../include/mcrl2/fdr/seq_expression.h', SEQ_EXPRESSION_CLASSES, 'seq_expression', 'fdr', superclass_aterm = 'Seq', label = 'seq expression')
    make_class_definitions('../source/term_functions.cpp', SEQ_EXPRESSION_CLASSES, 'seq_expression', 'fdr', label = 'seq expression')
    make_is_functions('../include/mcrl2/fdr/seq_expression.h', SEQ_EXPRESSION_CLASSES, 'seq_expression', 'fdr')

    make_class_declarations('../include/mcrl2/fdr/tuple_expression.h', TUPLE_EXPRESSION_CLASSES, 'tuple_expression', 'fdr', superclass_aterm = 'Tuple', label = 'tuple expression')
    make_class_definitions('../source/term_functions.cpp', TUPLE_EXPRESSION_CLASSES, 'tuple_expression', 'fdr', label = 'tuple expression')
    make_is_functions('../include/mcrl2/fdr/tuple_expression.h', TUPLE_EXPRESSION_CLASSES, 'tuple_expression', 'fdr')

    make_class_declarations('../include/mcrl2/fdr/dotted_expression.h', DOTTED_EXPRESSION_CLASSES, 'dotted_expression', 'fdr', superclass_aterm = 'Dotted', label = 'dotted expression')
    make_class_definitions('../source/term_functions.cpp', DOTTED_EXPRESSION_CLASSES, 'dotted_expression', 'fdr', label = 'dotted expression')
    make_is_functions('../include/mcrl2/fdr/dotted_expression.h', DOTTED_EXPRESSION_CLASSES, 'dotted_expression', 'fdr')

    make_class_declarations('../include/mcrl2/fdr/lambda_expression.h', LAMBDA_EXPRESSION_CLASSES, 'lambda_expression', 'fdr', superclass_aterm = 'Lambda', label = 'lambda expression')
    make_class_definitions('../source/term_functions.cpp', LAMBDA_EXPRESSION_CLASSES, 'lambda_expression', 'fdr', label = 'lambda expression')
    make_is_functions('../include/mcrl2/fdr/lambda_expression.h', LAMBDA_EXPRESSION_CLASSES, 'lambda_expression', 'fdr')

    make_class_declarations('../include/mcrl2/fdr/common_expression.h', COMMON_EXPRESSION_CLASSES, 'common_expression', 'fdr', superclass_aterm = 'Common', label = 'common expression')
    make_class_definitions('../source/term_functions.cpp', COMMON_EXPRESSION_CLASSES, 'common_expression', 'fdr', label = 'common expression')
    make_is_functions('../include/mcrl2/fdr/common_expression.h', COMMON_EXPRESSION_CLASSES, 'common_expression', 'fdr')

    make_class_declarations('../include/mcrl2/fdr/expression.h', EXPRESSION_CLASSES, 'expression', 'fdr', superclass_aterm = 'Expr', label = 'expression')
    make_class_definitions('../source/term_functions.cpp', EXPRESSION_CLASSES, 'expression', 'fdr', label = 'expression')
    make_is_functions('../include/mcrl2/fdr/expression.h', EXPRESSION_CLASSES, 'expression', 'fdr')

    make_class_declarations('../include/mcrl2/fdr/process.h', PROCESS_CLASSES, 'process', 'fdr', superclass_aterm = 'Proc', label = 'process expression')
    make_class_definitions('../source/term_functions.cpp', PROCESS_CLASSES, 'process', 'fdr', label = 'process expression')
    make_is_functions('../include/mcrl2/fdr/process.h', PROCESS_CLASSES, 'process', 'fdr')

    make_classes('../include/mcrl2/fdr/failuremodel.h', FAILUREMODEL_CLASSES, 'failuremodel', 'fdr')
    make_classes('../include/mcrl2/fdr/targ.h', TARG_CLASSES, 'targ', 'fdr', superclass_aterm = 'Targ')
    make_classes('../include/mcrl2/fdr/testtype.h', TESTTYPE_CLASSES, 'testtype', 'fdr')

    # other classes
    make_classes('../include/mcrl2/fdr/any.h', ANY_CLASSES, None, 'fdr')
    make_classes('../include/mcrl2/fdr/definition.h', DEFINITION_CLASSES, None, 'fdr')
    make_classes('../include/mcrl2/fdr/vartype.h', VARTYPE_CLASSES, None, 'fdr')
    make_classes('../include/mcrl2/fdr/type.h', TYPE_CLASSES, None, 'fdr')
    make_classes('../include/mcrl2/fdr/check.h', CHECK_CLASSES, None, 'fdr')
    make_classes('../include/mcrl2/fdr/refined.h', REFINED_CLASSES, None, 'fdr')
    make_classes('../include/mcrl2/fdr/model.h', MODEL_CLASSES, None, 'fdr')
    make_classes('../include/mcrl2/fdr/test.h', TEST_CLASSES, None, 'fdr')
    make_classes('../include/mcrl2/fdr/trname.h', TRNAME_CLASSES, None, 'fdr')
    make_classes('../include/mcrl2/fdr/filename.h', FILENAME_CLASSES, None, 'fdr')
    make_classes('../include/mcrl2/fdr/fdrspec.h', FDRSPEC_CLASSES, None, 'fdr')
    make_classes('../include/mcrl2/fdr/comprehension.h', COMPREHENSION_CLASSES, None, 'fdr')
    make_classes('../include/mcrl2/fdr/generator.h', GEN_CLASSES, None, 'fdr')
    make_classes('../include/mcrl2/fdr/field.h', FIELD_CLASSES, None, 'fdr')
    make_classes('../include/mcrl2/fdr/renaming.h', RENAMING_CLASSES, None, 'fdr')
    make_classes('../include/mcrl2/fdr/map.h', MAP_CLASSES, None, 'fdr')
    make_classes('../include/mcrl2/fdr/linkpar.h', LINKPAR_CLASSES, None, 'fdr')
    make_classes('../include/mcrl2/fdr/link.h', LINK_CLASSES, None, 'fdr')
