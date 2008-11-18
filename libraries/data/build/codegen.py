#!/bin/env python
#===============================================================================
# Copyright (c) 2007 Jason Evans <jasone@canonware.com>
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#
#===============================================================================
#
# Usage: codegen.py [-v] "<input> <output>"
#          <input> is a filename
#          <output> is a filename
#
#===============================================================================

import sys
import re
import os
import string
from optparse import OptionParser
import Parsing
import copy

# MACROS
FILE_HEADER = '''#ifndef MCRL2_DATA_%(uppercasename)s_H
#define MCRL2_DATA_%(uppercasename)s_H

#include "mcrl2/data/basic_sort.h"
#include "mcrl2/data/function_sort.h"
#include "mcrl2/data/function_symbol.h"
#include "mcrl2/data/application.h"
#include "mcrl2/data/data_equation.h"
#include "mcrl2/data/utility.h"
#include "mcrl2/data/standard.h"
%(includes)s

namespace mcrl2 {

  namespace data {

    namespace sort_%(name)s {

'''

FILE_FOOTER = '''    } // namespace %(name)s
  } // namespace data
} // namespace mcrl2

#endif // MCRL2_DATA_%(uppercasename)s_H
'''

SORT_EXPRESSION_CONSTRUCTORS = '''      // Sort expression %(fullname)s
      inline
      basic_sort %(name)s()
      {
        static basic_sort %(name)s("%(fullname)s");
        return %(name)s;
      }

      // Recogniser for sort expression %(fullname)s
      inline
      bool is_%(name)s(const sort_expression& e)
      {
        if (e.is_basic_sort())
        {
          return static_cast<const basic_sort&>(e) == %(name)s();
        }
        return false;
      }

'''

SORT_EXPRESSION_CONSTRUCTORS_PARAM = '''      // Sort expression %(container)s(%(param)s)
      inline
      container_sort %(label)s(const sort_expression& %(param)s)
      {
        //static container_sort %(label)s("%(label)s", %(param)s);
        container_sort %(label)s("%(label)s", %(param)s);
        return %(label)s;
      }

      // Recogniser for sort expression %(container)s(%(param)s)
      inline
      bool is_%(label)s(const sort_expression& e)
      {
        if (e.is_container_sort())
        {
          return static_cast<const container_sort&>(e).container_name() == "%(label)s";
        }
        return false;
      }

'''

POLYMORPHIC_FUNCTION_CONSTRUCTOR = '''      // Function symbol %(fullname)s
      inline
      function_symbol %(name)s(%(sortparams)s%(comma)s%(domainparams)s)
      {
        %(asserts)s
        %(targetsort)s
        //static function_symbol %(name)s("%(fullname)s", %(sort)s);
        function_symbol %(name)s("%(fullname)s", %(sort)s);
        return %(name)s;
      }

'''

FUNCTION_CONSTRUCTOR = '''      // Function symbol %(fullname)s
      inline
      function_symbol %(name)s(%(sortparams)s)
      {
        //static function_symbol %(name)s("%(fullname)s", %(sort)s);
        function_symbol %(name)s("%(fullname)s", %(sort)s);
        return %(name)s;
      }

'''
      
FUNCTION_RECOGNISER = '''      // Recogniser for %(fullname)s
      inline
      bool is_%(name)s_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e).name() == "%(fullname)s";
        }
        return false;
      }

'''

POLYMORPHIC_FUNCTION_APPLICATION = '''      // Application of %(fullname)s
      inline
      application %(name)s(%(formsortparams)s%(comma)s%(formparams)s)
      {
        return application(%(name)s(%(actsortparams)s),%(actparams)s);
      }
'''

FUNCTION_APPLICATION='''      // Application of %(fullname)s
      inline
      application %(name)s(%(formsortparams)s%(comma)s%(formparams)s)
      {
        %(assertions)s
        return application(%(name)s(%(actsortparams)s),%(actparams)s);
      }

'''

FUNCTION_APPLICATION_RECOGNISER='''      // Recogniser for application of %(fullname)s
      inline
      bool is_%(name)s_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_%(name)s_function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

'''

PROJECTION_CASE='''
        if (is_%(name)s_application(e))
        {
          return static_cast<const application&>(e).arguments()[%(index)s];
        }
'''

PROJECTION_FUNCTION='''      // Function for projecting out %(name)s
      inline
      data_expression %(name)s(const data_expression& e)
      {
        %(assertions)s;
        %(code)s
        // This should never be reached, otherwise something is severely wrong.
        assert(false); 
      }

'''

def add_to_comma_sep_string(string, addition):
  if string <> "":
    string += ", "
  string += addition
  return string

def add_namespace(string):
  global sorts_to_namespaces
  if string == current_sort:
    return "%s" % string
  else:
    return "sort_%s::%s" % (sorts_to_namespaces[string], string)

def get_namespace(sorts, functions, function):
  global sorts_to_functions
  global current_sort
  # Prefer current namespace
  for f in sorts_to_functions[current_sort]:
    if f[1].label == function:
      return "sort_%s" % (get_label(sorts, current_sort))
  for s in sorts_to_functions:
    for f in sorts_to_functions[s]:
      if f[1].label == function:
        return "sort_%s" % (get_label(sorts, s))
  return ""

def lookup_function_symbol(functions, function):
  for f in functions:
    if f[0].string == function[0].string:
      return f
  return function
  assert(False)

#
# get_projection_arguments
#
def get_projection_arguments(id, label, sortexpr):
    projection_arguments = {}
    ids = sortexpr.recogniserstring.split(", ")
    index = 0
    for id in ids:
        if id <> '':
            if id in projection_arguments:
                projection_arguments[id] += (label.label, index)
            else:
                projection_arguments[id] = [(label.label, index)]
            index += 1    
    return projection_arguments

#
# remove_duplicates
#
def remove_duplicates(list):
    new_list = []
    for l in list:
        if l not in new_list:
            new_list.append(l)
    return new_list

#
# merge dictionaries
#
def merge_dictionaries(dict1, dict2):
    for d in dict2:
      if d in dict1:
        dict1[d] += dict2[d]
      else:
        dict1[d] = dict2[d]
    return dict1

#--------------------------------------------------------------#
# generate_projection_functions
#--------------------------------------------------------------#
def generate_projection_functions(projection_arguments):
    code = ''

    for arg in projection_arguments:
        functions = remove_duplicates(projection_arguments[arg])
        assertions = '//assert('
        cases = ''
        for function in functions:
            name = function[0]
            index = function[1]
            if assertions <> '\\assert(':
                assertions += " || "
            assertions += "is_%s_application(e)" % (name)
            cases += PROJECTION_CASE % {
              'name'  : name, 
              'index' : index
            }

        assertions += ")"
        code += PROJECTION_FUNCTION % {
          'name'        : arg,
          'assertions'  : assertions,
          'code'        : cases
        }

    return code

def lookup_identifier(functions, data_expression, arity):
  if verbose:
    print "look up identifier %s with arity %d in %s" % (data_expression.string, arity, functions)
  if data_expression.string == "true":
    return "true_"
  elif data_expression.string == "false":
    return "false_"
  elif data_expression.string == "if":
    return "if_"
  elif data_expression.string == "==":
    return "equal_to"
  elif data_expression.string == "!=":
    return "not_equal_to"
  else:
    for f in functions:
      # The or arity == 0 is to support use of functions without argument
      if f[0].string == data_expression.string and (f[2].arity == arity or arity == 0):
        return f[1].label
    return ""

def generate_variable_code(sorts, variable_declarations, variable):
  s = lookup_sort(variable_declarations, variable)
  return "variable(\"%s\", %s)" % (variable, generate_sort_code(sorts, s.expr))

def generate_variables_code(sorts, variable_declarations, variables):
    variable_code = ''
    for var in variables:
      variable_code = add_to_comma_sep_string(variable_code, generate_variable_code(sorts, variable_declarations, var))
    if variable_code == '':
      variable_code = "variable_list()"
    else:
      variable_code = "make_vector(%s)" % variable_code
    return variable_code

def generate_data_expression_code(sorts, functions, variable_declarations, data_expression, arity = 0):
    if verbose:
      print "generate code for data expression: %s" % data_expression
    if data_expression[0] == "application":
      head_code = generate_data_expression_code(sorts, functions, variable_declarations, data_expression[1].expr, data_expression[2].count)
      f = lookup_function_symbol(functions + variable_declarations, data_expression[1].expr)
      if f[0].string == "if" or f[0].string == "==" or f[0].string == "!=":
        sort_args = ""
      else:
        sort_args = get_sort_parameters_from_sort_expression(sorts, f[len(f)-1].expr)
      args_code = generate_data_expression_code(sorts, functions, variable_declarations, data_expression[2].expr)
      sort_args_code = ""
      for a in sort_args:
        sort_args_code = add_to_comma_sep_string(sort_args_code, "%s" % (a.lower())) 
      if sort_args_code <> "":
        args_code = sort_args_code + ", " + args_code
      return "%s(%s)" % (head_code, args_code)
    elif data_expression[0] == "lambda" or data_expression[0] == "forall" or data_expression[0] == "exists":
      vardecl = data_expression[1]
      var = vardecl.vars[0]
      vardecls_code = generate_variables_code(sorts, data_expression[1].vars, [var[0].string])
      body_code = generate_data_expression_code(sorts, functions, variable_declarations + data_expression[1].vars, data_expression[2].expr)
      return "%s(%s,%s)" % (data_expression[0], vardecls_code, body_code)
    elif data_expression[0] == "dataexprs":
      code = ""
      data_expressions = data_expression[1]
      for i in data_expressions:
        code = add_to_comma_sep_string(code, generate_data_expression_code(sorts, functions, variable_declarations, i))
      return code
    else:
      # This must be a variable, or a function symbol
      id = lookup_identifier(functions, data_expression[0], arity)
      if id == "":
        return generate_variable_code(sorts, variable_declarations, data_expression[0].string)
      namespace = get_namespace(sorts, functions, id)
      if namespace <> "":
        namespace += "::"
      if arity == 0:
        f = lookup_function_symbol(functions + variable_declarations, data_expression)
        sort_args = get_sort_parameters_from_sort_expression(sorts, f[len(f)-1].expr)
        sort_args_code = ""
        for a in sort_args:
          sort_args_code = add_to_comma_sep_string(sort_args_code, "%s" % (a.lower()))
        return "%s%s(%s)" % (namespace, id, sort_args_code)
      else:
        return "%s%s" % (namespace, id)

def generate_sorts_code(sorts_ctx, sorts):
  if verbose:
    print "generate code for sorts %s" % (sorts)
  code = ""
  for i in sorts:
    code = add_to_comma_sep_string(code, generate_sort_code(sorts_ctx, i))
  return code

def generate_sort_code(sorts, sort):
  if verbose:
    print "generate code for sort %s" % (sort)
  sort_expression = sort
  if sort_expression[0] == "sortarrow":
    domain_code = generate_sort_code(sorts, sort_expression[1])
    codomain_code = generate_sort_code(sorts, sort_expression[2])
    return "function_sort(%s, %s)" % (domain_code, codomain_code)
  elif sort_expression[0] == "param":
    return generate_sort_code(sorts, sort_expression[1])
  elif sort_expression[0] == "domain":
    return generate_sorts_code(sorts, sort_expression[1])
  elif sort_expression[0] == "labelled_domain":
    domain = []
    for i in sort_expression[1]:
      domain += [i]
    return generate_sorts_code(sorts, domain)
  elif sort_expression[0] == "param_expr":
    param_code = generate_sort_code(sorts, sort_expression[2])
    l = get_label(sorts, sort_expression[1])
    return "%s(%s)" % (add_namespace(l), param_code)
  else:
    l = get_label(sorts, sort_expression[0])
    if l <> "":
      return "%s()" % (add_namespace(l))
    else:
# TODO: Fix this properly
      try:
        return sort_expression[0].string.lower()
      except:
        return sort_expression[0]

def lookup_sort(variable_declarations, var):
    try:
      s = var.string
    except:
      s = var
    if verbose:
      print "look up sort for variable %s in %s" % (s, variable_declarations)
    for v in variable_declarations:
      if v[0].string == s:
        return v[1]

def get_label(sorts, sort):
    try:
      if verbose:
        print "get label for %s in %s" % (sort.string, sorts)
      for s in sorts:
        if len(s) == 2 and s[0].string == sort.string:
          return s[1].label
        elif len(s) == 3 and s[0].string == sort.string:
          return s[2].label
      return ""
    except:
      return ""

def generate_equation_code(sorts, functions, variable_declarations, equation):
    if verbose:
      print "generate_equation_code"
    vars = []
    for v in equation[0]:
      vars += [v]
    vars.reverse()
    variable_code = generate_variables_code(sorts, variable_declarations, vars)
    condition_code = generate_data_expression_code(sorts, functions, variable_declarations, equation[1])
    lhs_code = generate_data_expression_code(sorts, functions, variable_declarations, equation[2])
    rhs_code = generate_data_expression_code(sorts, functions, variable_declarations, equation[3])
    if condition_code == "sort_bool_::true_()":
      return "data_equation(%s, %s, %s)" % (variable_code, lhs_code, rhs_code)
    else:
      return "data_equation(%s, %s, %s, %s)" % (variable_code, condition_code, lhs_code, rhs_code)

def get_sort_parameters_from_sort_expression(sorts, sortexpr):
    if verbose:
      print "get sort parameters from sort expression %s" % sortexpr
    if sortexpr[0] == "sortarrow":
      params = set()
      params |= get_sort_parameters_from_sort_expression(sorts, sortexpr[1])
      params |= get_sort_parameters_from_sort_expression(sorts, sortexpr[2])
      return params
    elif sortexpr[0] == "param":
      return set(sortexpr[1][0].string)
    elif sortexpr[0] == "domain":
      params = set()
      for i in sortexpr[1]:
        params |= get_sort_parameters_from_sort_expression(sorts, i)
      return params
    elif sortexpr[0] == "labelled_domain":
      params = set()
      for i in sortexpr[1]:
        params |= get_sort_parameters_from_sort_expression(sorts, i)
      return params
    elif sortexpr[0] == "param_expr":
        return get_sort_parameters_from_sort_expression(sorts, sortexpr[2])
    else:
      return set()

def get_sort_parameters_from_sort_expressions(sorts, sortexprs):
    params = set()
    for s in sortexprs:
      params |= get_sort_parameters_from_sort_expression(sorts, s.expr)
    return params

def get_sort_parameters_from_data_expression(sorts, functions, dataexpr):
    if verbose:
      print "data expression: %s" % dataexpr
    if dataexpr[0] == "application":
      params = set()
      params |= get_sort_parameters_from_data_expression(sorts, functions, dataexpr[1].expr)
      params |= get_sort_parameters_from_data_expression(sorts, functions, dataexpr[2].expr)
      return params
    elif dataexpr[0] == "lambda" or dataexpr[0] == "forall" or dataexpr[0] == "exists":
      params = set()
      params |= get_sort_parameters_from_sort_expression(sorts, dataexpr[1].vars[0][1].expr)
      params |= get_sort_parameters_from_data_expression(sorts, functions, dataexpr[2].expr)
      return params
    elif dataexpr[0] == "dataexprs":
      params = set()
      for i in dataexpr[1]:
        params |= get_sort_parameters_from_data_expression(sorts, functions, i)
      return params
    else:
      for f in functions:
        if f[0].string == dataexpr[0].string:
          return get_sort_parameters_from_sort_expression(sorts, f[2].expr)
      return set()

def get_sort_parameters_from_variables(sorts, variable_decls, variables):
    params = set()
    for v in variables:
      params |= get_sort_parameters_from_sort_expression(sorts, lookup_sort(variable_decls, v).expr)
    return params

def get_sort_parameters_from_equations(sorts, functions, variable_decls, equations):
    if verbose:
      print "get sort parameters from equations"
    sort_parameters = set()
    for eqn in equations:
      sort_parameters |= get_sort_parameters_from_variables(sorts, variable_decls, eqn[0])
      sort_parameters |= get_sort_parameters_from_data_expression(sorts, functions, eqn[1])
      sort_parameters |= get_sort_parameters_from_data_expression(sorts, functions, eqn[2])
      sort_parameters |= get_sort_parameters_from_data_expression(sorts, functions, eqn[3])
    return sort_parameters

def generate_formal_sort_parameters_code(sorts, functions, variable_declarations, equations):
    params = get_sort_parameters_from_equations(sorts, functions, variable_declarations, equations)
    code = ""
    for p in params:
      code = add_to_comma_sep_string(code, "const sort_expression& %s" % (p.lower()))
    return code

def generate_function_code(sorts, functions, function):
    global functions_cache
    similar_functions = []
    for f in functions_cache:
      if f[0].string == function[0].string and f[1].label == function[1].label:
        for s in function[2]:
          if f[2].string <> s.string:
            similar_functions += [f]

    sortargs = ""
    code = ""
    for s in get_sort_parameters_from_sort_expressions(sorts, function[2]):
      sortargs = add_to_comma_sep_string(sortargs, s.lower());

    if len(function[2]) == 1 and similar_functions == []:
      code += "        result.push_back(%s(%s));\n" % (function[1].label, sortargs)
    else:
      for s in function[2]:
        add_sort_args = ""
        assert s.expr[0] == "sortarrow"
        domain = s.expr[1]
        if domain[0] == "labelled_domain":
          new_domain = ["domain", []]
          for i in domain[1]:
            new_domain[1] += [i]
          domain = new_domain
        for d in domain[1]:
          add_sort_args = add_to_comma_sep_string(add_sort_args, "%s()" % add_namespace(get_label(sorts, d[0])))
        code += "        result.push_back(%s(%s));\n" % (function[1].label, add_to_comma_sep_string(sortargs,add_sort_args))
    return code

def generate_functions_code(sorts, functions, function_type):
    sort = sorts[0]
    sortstring = sort[0].string
    if len(sort) == 2:
      sortlabel = sort[1].label
    else:
      sortlabel = sort[2].label
    sort_params = set()
    for f in functions:
      for s in f[2]:
        sort_params |= get_sort_parameters_from_sort_expression(sorts, s.expr)
    argument = ""
    for p in sort_params:
      argument = add_to_comma_sep_string(argument, "const sort_expression& %s" % p.lower())
    code = '''      // Give all system defined constructors for %s
      inline
      function_symbol_list %s_generate_%s_code(%s)
      {
        function_symbol_list result;
''' % (sortstring, sortlabel, function_type, argument)
    for f in functions:
      code += generate_function_code(sorts, functions, f)
    code += '''
        return result;
      }

'''
    return code

def generate_constructors_code(sorts, constructors):
    return generate_functions_code(sorts, constructors, "constructors")


def generate_mappings_code(sorts, functions):
    return generate_functions_code(sorts, functions, "functions")

def generate_equations_code(sorts, functions, variable_declarations, equations):
    if verbose:
      print "generate equations code"
    argument = generate_formal_sort_parameters_code(sorts, functions, variable_declarations, equations)
    sort = sorts[0]
    sortstring = sort[0].string
    if len(sort) == 2:
      sortlabel = sort[1].label
    else:
      sortlabel = sort[2].label
    code = '''      // Give all system defined equations for %s
      inline
      data_equation_list %s_generate_equations_code(%s)
      {
        data_equation_list result;
''' % (sortstring, sortlabel, argument)
    for equation in equations:
        code += "        result.push_back(%s);\n" % (generate_equation_code(sorts, functions, variable_declarations, equation))
    code += '''
        return result;
      }

'''
    return code


#
#
# generate_result_code
#
def generate_result_code(spec, includescode):
    if len(spec.sort) == 2:
      s = spec.sort[1].label
    else:
      s = "%s" % (spec.sort[2].label)
    file_header = FILE_HEADER % {
      'uppercasename' : s.upper(),
      'name'          : s,
      'includes'      : includescode
    }
    file_footer = FILE_FOOTER % {
      'uppercasename' : s.upper(),
      'name'          : s
    }
    return file_header + spec.code + file_footer

#
# generate_includes_code
#
def generate_includes_code(includes, sorts):
    if verbose:
      print "generate code for includes: %s" % (includes)
    code = ""
    for i in includes:
      code += "#include \"mcrl2/data/%s.h\"\n" % (i.split(".")[0])
    return code

#
# generate_sortspec_code
#
def generate_sortspec_code(sortspec):
    code = ""
    sort = sortspec.sorts[0][len(sortspec.sorts[0])-1].label
    for s in sortspec.sorts:
      sorts_to_namespaces[s[len(s)-1].label] = sort
      if len(s) == 2:
        code += SORT_EXPRESSION_CONSTRUCTORS % {
          'fullname' : s[0].string,
          'name'     : s[1].label
        }
      else:
        code += SORT_EXPRESSION_CONSTRUCTORS_PARAM % {
          'container' : s[0].string,
          'param'     : s[1].string.lower(),
          'label'     : s[2].label
        }
    return code

#
# generate_functionspec_code
#
def generate_functionspec_code(sorts, functionspec):
    functions = merge_duplicates(functionspec.functions)
    code = ""
    for f in functions:
      id = f[0]
      label = f[1]
      sortexprs = f[2]
      code += generate_function_constructors(sorts, id, label, sortexprs)
    
    code += generate_constructors_code(sorts, merge_duplicates(functionspec.constructors))
    code += generate_mappings_code(sorts, merge_duplicates(functionspec.mappings))
    return code

def merge_duplicates(list):
    new_list = []
    for l in list:
      found = False
      for m in new_list:
        if l[0].string == m[0].string and l[1].label == m[1].label:
          m[2] = m[2] + [l[2]]
          found = True
      if not(found):
        new_list.append([l[0], l[1], [l[2]]])
    return new_list

# -------------------------------------------------------#
# generate_function_constructors
# -------------------------------------------------------#
def generate_function_constructors(sorts, id, label, sortexprs):
    if verbose:
      print "generate constructors for function %s, with label %s" % (id.string, label.label)

    # See if the function contains a parameterised sort
    formalsortparameters = ""
    actualsortparameters = ""
    sort_params = []
    for s in sortexprs:
      sort_params += get_sort_parameters_from_sort_expression(sorts, s.expr)

    for s in sort_params:
        formalsortparameters = add_to_comma_sep_string(formalsortparameters, "const sort_expression& %s" % s.lower())
        actualsortparameters = add_to_comma_sep_string(actualsortparameters, "%s" % s.lower())

    comma = ""
    if formalsortparameters <> "":
      comma = ", "

    if len(sortexprs) == 1:
      # No polymorphism, generate simple functions
      # Generate code for constructors for functions
      code = FUNCTION_CONSTRUCTOR % {
        'sortparams' : formalsortparameters,
        'fullname'  : id.string,
        'name'      : label.label,
        'sort'      : generate_sort_code(sorts, sortexprs[0].expr)
      }
      code += FUNCTION_RECOGNISER % {
      'fullname'  : id.string,
      'name'      : label.label
      }

      # Compute number and sorts of arguments for applications
      index = 0
      formalparameters = ""
      actualparameters = ""
      assertions = ""
      sort = sortexprs[0].expr
      if sort[0] == "sortarrow":
        domain = sort[1][1]
        for a in domain:
          if index <> 0:
            formalparameters += ", "
            actualparameters += ", "
          formalparameters += "const data_expression& arg%s" % index
          actualparameters += "arg%s" % index
          if a[0] == "param_expr":
            l = get_label(sorts, a[1])
            assertion = "//assert(sort_%s::is_%s(arg%s.sort()));\n        " % (sorts_to_namespaces[l], l, index)
          elif a[0].string in sort_params:
            assertion = "//assert(arg%s.sort() == %s);\n        " % (index, a[0].string.lower())
          else:
            l = get_label(sorts, a[0])
            assertion = "//assert(sort_%s::is_%s(arg%s.sort()));\n        " % (sorts_to_namespaces[l], l, index)
          assertions += assertion
          index +=1

        # Generate code for applications
        code += FUNCTION_APPLICATION % {
          'formsortparams': formalsortparameters,
          'comma'     : comma,
          'actsortparams': actualsortparameters,
          'fullname'  : id.string,
          'name'      : label.label,
          'formparams': formalparameters,
          'actparams' : actualparameters,
          'assertions': assertions
        }
        code += FUNCTION_APPLICATION_RECOGNISER % {
          'fullname'  : id.string,
          'name'      : label.label
        }

    else:
      # We have polymorphism, so we need to determine correct types
      domains = []
      codomains = []
      for s in sortexprs:
        assert s.expr[0] == "sortarrow"
        domain = s.expr[1]
        codomain = s.expr[2]
        if domain[0] == "labelled_domain":
          domain[0] = "domain"
          index = 0
          for i in domain[1]:
            domain[1][index] = [i[0]]
            index += 1
        assert domain[0] == "domain"
        assert len(codomain) == 1
        domains = domains + domain[1:]
        codomains = codomains + codomain[0:]

      domain_strings = {}
      for (index, i) in enumerate(domains[0]):
        domain_strings[index] = [i[0].string]
      for d in domains[1:]:
        for (index, i) in enumerate(d):
          domain_strings[index].append(i[0].string)

      codomain_strings = []
      for i in codomains:
        codomain_strings.append(i.string)

      domainparams = ""
      newdomain = ["domain", []]
      for d in domain_strings:
        #if len(remove_duplicates(domain_strings[d])) <> 1:
          domainparams = add_to_comma_sep_string(domainparams, "const sort_expression& s%s" % (d))
          newdomain[1] += [["s%s" % (d)]]

      # Only allow sorts that were specified
      assertions = "//assert("
      for i in range(len(domains)):
        if i <> 0:
          assertions += "||\n               "
        if len(domains) <> 1:
          assertions += "//("
        for (index, j) in enumerate(domains[i]):
          if index <> 0:
            assertions += " && "
          assertions += "s%s == %s" % (index, generate_sort_code(sorts, j))
        if len(domains) <> 1:
          assertions += ")"
      assertions += ");\n"

      # Bases on the parameters determine the target sort
      if len(remove_duplicates(codomain_strings)) <> 1:
        target_sort = "sort_expression target_sort;\n"
        for i in range(len(domains)):
          if i == 0:
            target_sort += "        if("
          else:
            target_sort += "        else if("
          for (index, j) in enumerate(domains[i]):
            if index <> 0:
              target_sort += " && "
            target_sort += "s%s == %s" % (index, generate_sort_code(sorts, j))
          target_sort += ")\n        {\n"
          target_sort += "          target_sort = %s;" % (generate_sort_code(sorts, [codomains[i]]))
          target_sort += "\n        }\n"
        target_sort += "        else\n        {\n          assert(false);\n        }\n"
      else:
        target_sort = "sort_expression target_sort(%s);" % (generate_sort_code(sorts, [codomains[0]]))
      newsort = ["sortarrow", newdomain, ["target_sort"]]
      sortcode = generate_sort_code(sorts, newsort)

      code = POLYMORPHIC_FUNCTION_CONSTRUCTOR % {
        'fullname'   : id.string,
        'name'       : label.label,
        'sortparams' : formalsortparameters,
        'comma'      : comma,
        'domainparams' : domainparams,
        'asserts'    : assertions,
        'targetsort' : target_sort,
        'sort'       : sortcode
      }
      
      code += FUNCTION_RECOGNISER % {
      'fullname'  : id.string,
      'name'      : label.label
      }

      # Compute number and sorts of arguments for applications
      index = 0
      formalparameters = ""
      actualparameters = ""
      assertions = ""
      sort = sortexprs[0].expr
      if sort[0] == "sortarrow":
        domain = sort[1][1]
        for a in domain:
          formalparameters =  add_to_comma_sep_string(formalparameters, "const data_expression& arg%s" % index)
          actualparameters = add_to_comma_sep_string(actualparameters, "arg%s" % index)
          actualsortparameters = add_to_comma_sep_string(actualsortparameters, "arg%s.sort()" % index)
          index +=1


        # Generate code for applications
        code += FUNCTION_APPLICATION % {
          'formsortparams': formalsortparameters,
          'comma'     : comma,
          'actsortparams': actualsortparameters,
          'fullname'  : id.string,
          'name'      : label.label,
          'formparams': formalparameters,
          'actparams' : actualparameters,
          'assertions': assertions
        }
        code += FUNCTION_APPLICATION_RECOGNISER % {
          'fullname'  : id.string,
          'name'      : label.label
        }
    
    return code


#===============================================================================
# Tokens/precedences. 

# Precedences
class PArrow(Parsing.Precedence):
    "%right pArrow"
class PHash(Parsing.Precedence):
    "%left pHash"

# Tokens
class TokenArrow(Parsing.Token):
    "%token arrow [pArrow]"
class TokenHash(Parsing.Token):
    "%token hash [pHash]"
class TokenInclude(Parsing.Token):
    "%token include"
class TokenColon(Parsing.Token):
    "%token colon"
class TokenSemiColon(Parsing.Token):
    "%token semicolon"
class TokenComma(Parsing.Token):
    "%token comma"
class TokenLBrack(Parsing.Token):
    "%token lbrack"
class TokenRBrack(Parsing.Token):
    "%token rbrack"
class TokenLAng(Parsing.Token):
    "%token lang"
class TokenRAng(Parsing.Token):
    "%token rang"
class TokenEquals(Parsing.Token):
    "%token equals"

class TokenSort(Parsing.Token):
    "%token sort"
class TokenCons(Parsing.Token):
    "%token cons"
class TokenMap(Parsing.Token):
    "%token map"
class TokenVar(Parsing.Token):
    "%token var"
class TokenEqn(Parsing.Token):
    "%token eqn"
class TokenLambda(Parsing.Token):
    "%token lambda"
class TokenForall(Parsing.Token):
    "%token forall"
class TokenExists(Parsing.Token):
    "%token exists"

class TokenID(Parsing.Token):
    "%token id"
    def __init__(self, parser, s):
        Parsing.Token.__init__(self, parser)
        self.string = s

#===============================================================================
# Nonterminals, with associated productions.  In traditional BNF, the following
# productions would look something like:
# Result ::= Spec
#          | Includes Spec
# IncludesSpec ::= Spec
#          | Includes Spec
# Includes ::= Include
#          | Includes Include
# Include ::= "#include" ID
# Spec ::= SortSpec FunctionSpec VarSpec EqnSpec
# SortSpec ::= "sort" SortDecls        
# FunctionSpec ::= MapSpec
#                | ConsSpec MapSpec
# ConsSpec ::= "cons" OpDecls
# MapSpec ::= "map" OpDecls
# VarSpec ::= "var" VarDecls
# EqnSpec ::= "eqn" EqnDecls
# SortDecls ::= SortDecl
#            | SortDecls SortDecl
# SortDecl ::= SortExpr Label
#            | ID "(" SortParam ")" Label
# VarDecls ::= VarDecl
#            | VarDecls VarDecl
# VarDecl ::= ID ":" SortExpr
# OpDecls ::= OpDecl
#           | OpDecls OpDecl
# OpDecl ::= ID Label ":" SortExpr
# EqnDecls ::= EqnDecl
#            | EqnDecls EqnDecl
# EqnDecl ::= DataExpr "=" DataExpr
# DataExpr ::= DataExprPrimary
#            | DataExpr "(" DataExprs ")"
#            | lambda(VarDecl, DataExpr)
#            | forall(VarDecls, DataExpr)
#            | exists(VarDecls, DataExpr)
# DataExprPrimary ::= ID
#                   | "(" DataExpr ")"
# DataExprs ::= DataExpr
#            | DataExprs "," DataExpr
# SortExpr ::= SortExprPrimary
#            | LabelledDomain -> SortExpr
#            | Domain -> SortExpr
#            | ID "(" SortParam ")"
# SortParam ::= ID
# LabelledDomain ::= SortExprPrimary Label
#                  | LabelledDomain # SortExprPrimary Label
# Domain ::= SortExprPrimary Label
#          | Domain # SortExprPrimary Label
# SortExprPrimary ::= ID
#                  | LPAR SortExpr RPAR
# Label ::= LANG ID RANG

class Result(Parsing.Nonterm):
    "%start"
    def reduceIncludesSpec(self, result):
        "%reduce IncludesSpec"
        global outputcode
        global current_sort
        global sorts_cache
        global functions_cache
        global sorts_to_functions
        self.includes = result.includes
        self.spec = result.spec

        has_container = False
        for s in self.spec.sorts:
          if len(s) == 3:
            has_container = True
        if has_container:
          self.includes += ["container_sort"]
        if has_lambda:
          self.includes += ["lambda"]
        if has_forall:
          self.includes += ["forall"]
        if has_exists:
          self.includes += ["exists"]

        # This is to make sure all possible sorts get correctly coded.
        # This should eventually be solved by a recursive call in the generated
        # code.
        old_functions = copy.deepcopy(self.spec.functions)

        new_functions = self.spec.functions
        self.spec.functionspec.functions = copy.deepcopy(self.spec.functionspec.functions) # To prevent undesired sharing with mappings and constructors
        for f in functions_cache:
          for g in self.spec.functionspec.functions:
            if f[0].string == g[0].string and f[1].label == g[1].label:
              self.spec.functionspec.functions += [f]
              break

        functions_cache = functions_cache + old_functions

        current_sort = self.spec.sorts[0][0]
        sorts_cache += self.spec.sorts
        sorts_to_functions[current_sort] = self.spec.functions


        self.spec.code = generate_sortspec_code(self.spec.sortspec) + \
                    generate_functionspec_code(self.spec.sorts + sorts_cache, self.spec.functionspec) + \
                    generate_projection_functions(self.spec.functionspec.projection_arguments) + \
                    generate_equations_code(self.spec.sorts + sorts_cache, self.spec.functionspec.functions + functions_cache, self.spec.varspec.vars, self.spec.eqnspec.equations)

        outputcode = generate_result_code(self.spec, generate_includes_code(self.includes, self.spec.sorts))

        # Debugging
        self.string = result.string
        if verbose:
          print "Parsed %s" % (self.string)

class IncludesSpec(Parsing.Nonterm):
    "%nonterm"
    def reduceSpec(self, spec):
        "%reduce Spec"
        self.includes = []
        self.spec = spec
        
        self.string = spec.string

    def reduceIncludesSpec(self, includes, spec):
        "%reduce Includes Spec"
        self.includes = includes.includes
        self.spec = spec

        # Debugging
        self.string = includes.string + "\n\n" + spec.string
        if verbose:
          print "Parsed specification with include(s):\n%s" % (self.string)

class Includes(Parsing.Nonterm):
    "%nonterm"
    def reduceInclude(self, include):
        "%reduce Include"
        self.includes = include.includes

        # Debugging
        self.string = include.string
        if verbose:
          print "Parsed include: %s" % (self.string)

    def reduceIncludes(self, includes, include):
        "%reduce Includes Include"
        self.includes = includes.includes + include.includes

        # Debugging
        self.string = includes.string + include.string
        if verbose:
          print "Parsed includes: %s" % (self.string)

class Include(Parsing.Nonterm):
    "%nonterm"
    def reduceInclude(self, include, id):
        "%reduce include id"
        self.includes = [id.string]

        # Debugging
        self.string = "#include %s" % (id.string)
        if verbose:
          print "Parsed include: %s" % (self.string)

class Spec(Parsing.Nonterm):
    "%nonterm"
    def reduce(self, sortspec, functionspec, varspec, eqnspec):
        "%reduce SortSpec FunctionSpec VarSpec EqnSpec"
        global sorts_cache
        global functions_cache
        self.sortspec = sortspec
        self.functionspec = functionspec
        self.varspec = varspec
        self.eqnspec = eqnspec
        self.sorts = sortspec.sorts
        self.functions = functionspec.functions
        self.sort = self.sortspec.sorts[0]

        # Debugging
        self.string = sortspec.string + '\n' + functionspec.string + '\n' + varspec.string + '\n' + eqnspec.string
        if verbose:
          print "Parsed specification:\n%s\n" % self.string

class SortSpec(Parsing.Nonterm):
    "%nonterm"
    def reduceSortSpec(self, sort, sortdecls):
        "%reduce sort SortDecls"
        self.sorts = sortdecls.sorts
        
        # Debugging
        self.string = "sort %s;" % (sortdecls.string)
        if verbose:
          print "Parsed sort specification: %s" % (self.string)

class FunctionSpec(Parsing.Nonterm):
    "%nonterm"
    def reduceMapSpec(self, mapspec):
        "%reduce MapSpec"
        self.functions = mapspec.functions
        self.constructors = []
        self.mappings = mapspec.functions

        self.projection_arguments = mapspec.projection_arguments

        # Debugging
        self.string = mapspec.string
        if verbose:
          print "Parsed function specification : %s" % (self.string)

    def reduceConsMapSpec(self, consspec, mapspec):
        "%reduce ConsSpec MapSpec"
        self.functions = consspec.functions + mapspec.functions
        self.constructors = consspec.functions
        self.mappings = mapspec.functions
        self.projection_arguments = merge_dictionaries(consspec.projection_arguments, mapspec.projection_arguments)

        # Debugging
        self.string = consspec.string + '\n' + mapspec.string
        if verbose:
          print "Parsed function specification : %s" % (self.string)

class ConsSpec(Parsing.Nonterm):
    "%nonterm"
    def reduceConsSpec(self, cons, opdecls):
        "%reduce cons OpDecls"
        self.functions = opdecls.functions
        self.projection_arguments = opdecls.projection_arguments
        
        # Debugging
        self.string = "cons %s" % (opdecls.string)
        if verbose:
          print "Parsed constructor specification: %s" % (self.string)

class MapSpec(Parsing.Nonterm):
    "%nonterm"
    def reduceMapSpec(self, map, opdecls):
        "%reduce map OpDecls"
        self.functions = opdecls.functions
        self.projection_arguments = opdecls.projection_arguments

        # Debugging
        self.string = "map %s" % (opdecls.string)
        if verbose:
          print "Parsed mapping specification %s" % (self.string)

class VarSpec(Parsing.Nonterm):
    "%nonterm"
    def reduceVarSpec(self, var, vardecls):
        "%reduce var VarDecls"
        self.vars = vardecls.vars

        # Debugging        
        self.string = "var %s" % (vardecls.string)
        if verbose:
          print "Parsed variable specification %s" % (self.string)

class EqnSpec(Parsing.Nonterm):
    "%nonterm"
    def reduceEqnSpec(self, eqn, eqndecls):
        "%reduce eqn EqnDecls"
        self.equations = eqndecls.equations

        # Debugging
        self.string = "eqn %s" % (eqndecls.string)
        if verbose:
          print "Parsed equation specification %s" % (self.string)

class SortDecls(Parsing.Nonterm):
    "%nonterm"
    def reduceSortDecl(self, sortdecl, semicolon):
        "%reduce SortDecl semicolon"
        self.sorts = sortdecl.sorts
        
        self.string = sortdecl.string + ";"
        if verbose:
          print "Parsed sort declarations: %s" % (self.string)

    def reduceSortDecls(self, sortdecls, sortdecl, semicolon):
        "%reduce SortDecls SortDecl semicolon"
        self.sorts = sortdecls.sorts + sortdecl.sorts

        self.string = "%s\n%s;" % (sortdecls.string, sortdecl.string)
        if verbose:
          print "Parsed sort declarations: %s" % (self.string)

class SortDecl(Parsing.Nonterm):
    "%nonterm"
    def reduceSortDecl(self, sortexpr, label):
        "%reduce id Label"
        self.sorts = [[sortexpr, label]]

        self.string = "%s %s" % (sortexpr.string, label.label)
        if verbose:
          print "Parsed single sort declaration %s" % (self.string)

    def reduceSortDeclParam(self, container, lbrack, param, rbrack, label):
        "%reduce id lbrack SortParam rbrack Label"
        self.sorts = [[container, param, label]]

        self.string = "%s(%s) %s" % (container.string, param.string, label.string)
        if verbose:
          print "Parsed parameterised sort declaration %s" % (self.string)

class VarDecls(Parsing.Nonterm):
    "%nonterm"
    def reduceVarDecl(self, vardecl, semicolon):
        "%reduce VarDecl semicolon"
        self.vars = vardecl.vars

        # Debugging
        self.string = vardecl.string
        if verbose:
          print "Parsed single variable declaration: %s" % (self.string)

    def reduceVarDecls(self, vardecls, vardecl, semicolon):
        "%reduce VarDecls VarDecl semicolon"
        self.vars = vardecls.vars + vardecl.vars

        # Debugging
        self.string = vardecls.string + ';\n' + vardecl.string + ";"
        if verbose:
          print "Parsed variable declarations: %s" % (self.string)

class VarDecl(Parsing.Nonterm):
    "%nonterm"
    def reduceVarDecl(self, id, colon, sortexpr):
        "%reduce id colon SortExpr"
        self.vars = [[id, sortexpr]]
        self.name = id.string

        # Debugging
        self.string = id.string + ":" + sortexpr.string
        if verbose:
          print "Parsed single variable declaration: %s" % (self.string)

class OpDecls(Parsing.Nonterm):
    "%nonterm"
    def reduceOpDecl(self, opdecl, semicolon):
        "%reduce OpDecl semicolon"
        self.functions = opdecl.functions
        self.projection_arguments = opdecl.projection_arguments

        # Debugging
        self.string = opdecl.string
        if verbose:
          print "Parsed single function declaration: %s" % (self.string)

    def reduceOpDecls(self, opdecls, opdecl, semicolon):
        "%reduce OpDecls OpDecl semicolon"
        self.functions = opdecls.functions + opdecl.functions
        self.projection_arguments = merge_dictionaries(opdecls.projection_arguments, opdecl.projection_arguments)

        # Debugging
        self.string = opdecls.string + ';\n' + opdecl.string + ";"
        if verbose:
          print "Parsed function declarations: %s" % (self.string)    

class OpDecl(Parsing.Nonterm):
    "%nonterm"
    def reduceOpDecl(self, id, label, colon, sortexpr):
        "%reduce id Label colon SortExpr"
        self.functions = [[id, label, sortexpr]]
        functions_table[id.string] = label.label
        self.projection_arguments = get_projection_arguments(id, label, sortexpr)
        
        # Debugging
        self.string = id.string + label.string + ":" + sortexpr.string
        if verbose:
          print "Parsed single function declaration: %s" % (self.string)

class EqnDecls(Parsing.Nonterm):
    "%nonterm"
    def reduceDataEqn(self, eqndecl, semicolon):
        "%reduce EqnDecl semicolon"
        self.equations = eqndecl.equations

        self.string = eqndecl.string
        if verbose:
          print "Parsed single equation declaration: %s" % (self.string)

    def reduceDataEqns(self, eqndecls, eqndecl, semicolon):
        "%reduce EqnDecls EqnDecl semicolon"
        self.equations = eqndecls.equations + eqndecl.equations

        self.string = eqndecls.string + "\n" + eqndecl.string
        if verbose:
          print "Parsed equation declarations: %s" % (self.string)

class EqnDecl(Parsing.Nonterm):
    "%nonterm"
    def reduceDataEqn(self, lhs, equals, rhs):
        "%reduce DataExpr equals DataExpr"
        self.variables = lhs.variables | rhs.variables
        self.condition = TokenID(parser, "true")
        self.condition.expr = [self.condition]
        self.equations = [[self.variables, self.condition.expr, lhs.expr, rhs.expr]]

        # Debugging
        self.string = lhs.string + " = " + rhs.string
        if verbose:
          print "Parsed single data equation: %s" % (self.string)

    def reduceDataEqnCondition(self, condition, arrow, lhs, equals, rhs):
        "%reduce DataExpr arrow DataExpr equals DataExpr"
        self.variables = lhs.variables | rhs.variables
        self.equations = [[self.variables, condition.expr, lhs.expr, rhs.expr]]

        # Debugging
        self.string = condition.string + " -> " + lhs.string + " = " + rhs.string
        if verbose:
          print "Parsed single data equation: %s" % (self.string)

class DataExpr(Parsing.Nonterm):
    "%nonterm"
    def reduceDataExprPrimary(self, dataexprprimary):
        "%reduce DataExprPrimary"
        self.expr = dataexprprimary.expr

        self.variables = dataexprprimary.variables
        self.abstraction = False

        # Debugging
        self.string = dataexprprimary.string
        if verbose:
          print "Parsed simple data expression: %s" % (self.string)

    def reduceApplication(self, head, lbrack, arguments, rbrack):
        "%reduce DataExpr lbrack DataExprs rbrack"
        self.expr = ["application", head, arguments]

        self.variables = head.variables | arguments.variables

        # Debugging
        self.string = head.string +"(" + arguments.string + ")"
        if verbose:
          print "Parsed function application: %s" % (self.string)

    def reduceLambda(self, lmb, lbrack, vardecl, comma, expr, rbrack):
        "%reduce lambda lbrack VarDecl comma DataExpr rbrack"
        global has_lambda
        has_lambda = True
        self.expr = ["lambda", vardecl, expr]

        self.variables = expr.variables
        self.variables = self.variables.difference(set(vardecl.name))

        # Debugging
        self.string = "lambda(%s, %s)" % (vardecl.string, expr.string)
        if verbose:
          print "Parsed lambda expression: %s" % (self.string)

    def reduceForall(self, forall, lbrack, vardecl, comma, expr, rbrack):
        "%reduce forall lbrack VarDecl comma DataExpr rbrack"
        global has_forall
        has_forall = True
        self.expr = ["forall", vardecl, expr]

        self.variables = expr.variables
        self.variables = self.variables.difference(set(vardecl.name))

        # Debugging
        self.string = "forall(%s, %s)" % (vardecl.string, expr.string)
        if verbose:
          print "Parsed forall expression: %s" % (self.string)

    def reduceExists(self, exists, lbrack, vardecl, comma, expr, rbrack):
        "%reduce exists lbrack VarDecl comma DataExpr rbrack"
        global has_exists
        has_exists = True
        self.expr = ["exists", vardecl, expr]

        self.variables = expr.variables
        self.variables = self.variables.difference(set(vardecl.name))

        # Debugging
        self.string = "exists(%s, %s)" % (vardecl.string, expr.string)
        if verbose:
          print "Parsed exists expression: %s" % (self.string)

class DataExprPrimary(Parsing.Nonterm):
    "%nonterm"
    def reduceID(self, id):
        "%reduce id"
        self.expr = [id]

        if id.string in functions_table:
            self.variables = set()
        else:
            self.variables = set(id.string)

        # Debugging
        self.string = id.string
        if verbose:
          print "Parsed simple data expression: %s" % (self.string)

    def reduceBracketedDataExpr(self, lbrack, dataexpr, rbrack):
        "%reduce lbrack DataExpr rbrack"
        self.expr = dataexpr.expr

        self.variables = dataexpr.variables

        # Debugging
        self.string = "(" + dataexpr.string + ")"
        if verbose:
          print "Parsed bracketed data expression: %s" % (self.string)

class DataExprs(Parsing.Nonterm):
    "%nonterm"
    def reduceDataExpr(self, dataexpr):
        "%reduce DataExpr"
        self.expr = ["dataexprs", [dataexpr.expr]]

        self.variables = dataexpr.variables
        self.count = 1

        # Debugging
        self.string = dataexpr.string
        if verbose:
          print "Parsed data expression: %s" % (self.string)

    def reduceDataExprs(self, dataexprs, comma, dataexpr):
        "%reduce DataExprs comma DataExpr"
        exprs = dataexprs.expr[1]
        exprs = exprs + [dataexpr.expr]
        self.expr = ["dataexprs", exprs]

        self.variables = dataexprs.variables | dataexpr.variables
        self.count = dataexprs.count + 1

        # Debugging
        self.string = dataexprs.string + ", " + dataexpr.string
        if verbose:
          print "Parsed data expressions: %s" % (self.string)

class SortExpr(Parsing.Nonterm):
    "%nonterm"
    def reduceSortExprPrimary(self, sortexpr):
        "%reduce SortExprPrimary"
        self.expr = sortexpr.expr

        self.recogniserstring = sortexpr.recogniserstring
        self.arity = 0
        
        # Debugging
        self.string = sortexpr.string
        if verbose:
          print "Parsed SortExprPrimary: %s" % (sortexpr.string)

    def reduceSortExprArrowLabelled(self, domain, arrow, sortexpr):
        "%reduce LabelledDomain arrow SortExpr"
        self.expr = ["sortarrow", domain.expr, sortexpr.expr]

        self.recogniserstring = domain.recogniserstring
        self.arity = domain.arity

        # Debugging
        self.string = "%s -> %s" % (domain.string, sortexpr.string)
        if verbose:
          print "Parsed SortExprArrowLabelled: %s" % (self.string)

    def reduceSortExprArrow(self, domain, arrow, sortexpr):
        "%reduce Domain arrow SortExpr"
        self.expr = ["sortarrow", domain.expr, sortexpr.expr]

        self.recogniserstring = domain.recogniserstring
        self.arity = domain.arity

        # Debugging
        self.string = "%s -> %s" % (domain.string, sortexpr.string)
        if verbose:
          print "Parsed SortExprArrow: %s" % (self.string)

class SortParam(Parsing.Nonterm):
    "%nonterm"
    def reduceId(self, id):
        "%reduce id"
        self.expr = ["param", [id]]

        self.string = id.string
        if verbose:
          print "Parsed SortParam: %s" % (self.string)

class Domain(Parsing.Nonterm):
    "%nonterm"
    def reduceSortExprPrimary(self, expr):
        "%reduce SortExprPrimary"
        self.expr = ["domain", [expr.expr]]

        self.recogniserstring =""
        self.arity = 1

        # Debugging
        self.string = expr.string
        if verbose:
          print "Parsed SortExprPrimary: %s" % (self.string)

    def reduceHashedDomain(self, Domain, hash, SortExprPrimary):
        "%reduce Domain hash SortExprPrimary"
        exprs = Domain.expr[1]
        exprs = exprs + [SortExprPrimary.expr]
        self.expr = ["domain", exprs]

        self.recogniserstring = "%s" % (Domain.recogniserstring)
        self.arity = Domain.arity + 1

        # Debugging
        self.string = Domain.string + " # " + SortExprPrimary.string
        if verbose:
          print "Parsed SortExprHashedDomain: %s" % (self.string)

class LabelledDomain(Parsing.Nonterm):
    "%nonterm"
    def reduceSortExprPrimary(self, expr, label):
        "%reduce SortExprPrimary Label"
        self.expr = ["labelled_domain", [expr.expr]]

        self.recogniserstring = "%s" % (label.label)
        self.arity = 1

        # Debugging
        self.string = expr.string + label.string
        if verbose:
          print "Parsed SortExprPrimary: %s" % (self.string)

    def reduceHashedLabelledDomain(self, Domain, hash, SortExprPrimary, label):
        "%reduce LabelledDomain hash SortExprPrimary Label"
        exprs = Domain.expr[1]
        exprs = exprs + [SortExprPrimary.expr]
        self.expr = ["labelled_domain", exprs]

        self.recogniserstring = "%s, %s" % (Domain.recogniserstring, label.label)
        self.arity = Domain.arity + 1

        # Debugging
        self.string = Domain.string + " # " + SortExprPrimary.string + label.label
        if verbose:
          print "Parsed SortExprHashedDomain: %s" % (self.string)

class SortExprPrimary(Parsing.Nonterm):
    "%nonterm"
    def reduceId(self, expr):
        "%reduce id"
        self.expr = [expr]

        self.recogniserstring = ""
        self.arity = 0

        # Debugging
        self.string = expr.string
        if verbose:
          print "Parsed sort identifier: %s" % (self.string)

    def reduceParen(self, lbrack, SortExpr, rbrack):
        "%reduce lbrack SortExpr rbrack"
        self.expr = SortExpr.expr

        self.recogniserstring = SortExpr.recogniserstring
        self.arity = self.arity = 0

        # Debugging
        self.string = "(" + SortExpr.string + ")"
        if verbose:
          print "Parsed bracketed expression: %s" % (self.string)

    def reduceSortExprParam(self, container, lbrack, param, rbrack):
        "%reduce id lbrack SortParam rbrack"
        self.expr = ["param_expr", container, param.expr]

        self.recogniserstring = ""
        self.arity = 0

        # Debugging
        self.string = "%s(%s)" % (container.string, param.string)
        if verbose:
          print "Parsed SortExprParam: %s" % (self.string)

class Label(Parsing.Nonterm):
    "%nonterm"
    def reduce(self, lang, id, rang):
        "%reduce lang id rang"
        self.label = id.string

        # Debugging
        self.string = "< %s >" % (self.label)
        if verbose:
          print "Parsed label: %s" % self.string

# -------------------------------------------------------#
# parser
# -------------------------------------------------------#
# Parser subclasses the Lr parser driver.  Since the grammar is unambiguous, we
# have no need of the Glr driver's extra functionality, though there is nothing
# preventing us from using it.
#
# If you are curious how much more work the GLR driver has to do, simply change
# the superclass from Parsing.Lr to Parsing.Glr, then, run this program with
# verbosity enabled.
class Parser(Parsing.Lr):
    def __init__(self, spec):
	Parsing.Lr.__init__(self, spec)

    # Brain-dead scanner.  The scanner does not have to be a method of this
    # class, so for more complex parsers it is no problem to separate the
    # scanner into a separate module.
    def scan(self, input):
        syms = {"->"      : TokenArrow,
                "#"       : TokenHash,
                "#include": TokenInclude,
                ":"       : TokenColon,
                ";"       : TokenSemiColon,
                ","       : TokenComma,
                "("       : TokenLBrack,
                ")"       : TokenRBrack,
                "<\""     : TokenLAng,
                "\">"     : TokenRAng,
                "="       : TokenEquals,
                "sort"    : TokenSort,
                "cons"    : TokenCons,
                "map"     : TokenMap,
                "var"     : TokenVar,
                "eqn"     : TokenEqn,
                "lambda"  : TokenLambda,
                "forall"  : TokenForall,
                "exists"  : TokenExists
               }

        # First make sure the needed separators are surrounded by spaces
        # Some parts always need to get extra whitespace
        input = re.sub('(->|[():;,])', r" \1 ", input)
        # # needs to get whitespace if it is not followed by "include"
        input = re.sub('(#)(?!include)', r" \1 ", input)
        # < needs to get whitespace if it starts a label
        input = re.sub('(<\")(?=\w)', r"\1 ", input)
        # > needs to get whitespace if it ends a label
        input = re.sub('(?<=\w)(\">)', r" \1", input)

        # Split the input at whitespace, producing the tokens
        p=re.compile(r'\s+')
        if verbose:
          print "split input: %s" % (p.split(input))

        for word in p.split(input):
            if word != '':
    	        if word in syms:
		    token = syms[word](self)
	        else:
		    token = TokenID(parser, word)
                    # Feed token to parser.
	        self.token(token)
	# Tell the parser that the end of input has been reached.
	self.eoi()

#--------------------------------------------------------#
#                  read_text
#--------------------------------------------------------#
# returns the contents of the file 'filename' as a string
def read_text(filename):
    try:
        f = open(filename, 'r')
    except IOError, e:
        print 'Unable to open file ' + filename + ' ', e
        sys.exit(0)

    text = f.read()
    f.close()
    return text

#--------------------------------------------------------#
#                  read_paragraphs
#--------------------------------------------------------#
# returns the contents of the file 'filename' as a list of paragraphs
def read_paragraphs(file):
    text       = read_text(file)
    paragraphs = re.split('\n\s*\n', text)
    return paragraphs

#
# filter_comments
#
def filter_comments(filename):
    paragraphs = read_paragraphs(filename)
    clines = [] # comment lines
    glines = [] # grammar lines

    for paragraph in paragraphs:
        lines = string.split(paragraph, '\n')
        for line in lines:
            if re.match('%.*', line):
                clines.append(line)
            else:
                glines.append(line)
    comment = string.join(clines, '\n')
    spec = string.join(glines, '\n')
    return spec

#
# get_includes
#
def get_includes(input):
    lines = string.split(input, '\n')
    includes = []
    for line in lines:
        if re.match('#include.*', line):
            includes.append(line.replace('#include ',''))
        else:
            break
    return includes

#-------------------------------------------------------#
# parse_spec
#-------------------------------------------------------#
# This parses the input file and removes comment lines from it
def parse_spec(infilename):
    global outputcode
    global recognisers
    global parser
    global includes_table
    input = filter_comments(infilename)
    includes = get_includes(input)

    parser.reset()
    outputcode = ""
    recognisers = {}

    # Now first process the includes:
    for include in includes:
        if not includes_table.has_key(include):
            includeinput = parse_spec(include)
            includes_table[include] = current_sort

    outputcode = ""
    recognisers = {}
    parser.reset()
    if input not in includes_table:
        parser.scan(input)


# -------------------------------------------------------#
# main
# -------------------------------------------------------#
def main():
    global outputcode
    global verbose
    global debug
    usage = "usage: %prog [options] infile outfile"
    option_parser = OptionParser(usage)
    option_parser.add_option("-v", "--verbose", action="store_true", help="give verbose output")
    option_parser.add_option("-d", "--debug", action="store_true", help="give debug output")
    (options, args) = option_parser.parse_args()

    if options.verbose:
        verbose = True

    if options.debug:
        verbose = True
        debug = True
        parser.verbose = True

    if len(args) > 0:
        infilename = args[0]
        outfilename = args[1]
        try:
            infile = open(infilename)
            outfile = open(outfilename, "w")
        except IOError, e:
            print "Unable to open file ", filename, " ", e
            return

        parse_spec(infilename)
        
        outfile.write(outputcode)

    else:
        option_parser.print_help()



# -------------------------------------------------------#
# global parser stuff, needs to be here
# -------------------------------------------------------#

# Introspect this module to generate a parser.  Enable all the bells and
# whistles.
spec = Parsing.Spec(sys.modules[__name__],
                    pickleFile="codegen.pickle",
                    skinny=False,
                    logFile="codegen.log",
                    graphFile="codegen.dot",
                    verbose=True)

# Create a parser that uses the parser tables encapsulated by spec.  In this
# program, we are only creating one parser instance, but it is possible for
# multiple parsers to use the same Spec simultaneously.
parser = Parser(spec)


#
# Global variables to collect needed information during parsing.
#
verbose = False       # Switch on debug output?
debug = False         # Switch on verbose output? 
has_lambda = False
has_forall = False
has_exists = False
sorts_cache = []
sorts_to_functions = {}
sorts_to_namespaces = {}
functions_cache = []
functions_table = {'==':'equal_to', '!=':'not_equal_to', 'if':'if_'}  # Maps ids to function names
includes_table = {}   # Maps sorts to include files
current_sort = ""     # current sort, to map include file to sort
outputcode = ""       # String to collect the generated code

if __name__ == "__main__":
    main()

