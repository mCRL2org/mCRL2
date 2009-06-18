# Classes for data expressions and sort expressions, used for code generation
# Basically reflects the parse tree of a specification, but allows for more
# flexibility.

import string
import copy
import re

# Remove trailing _ from a string
def remove_underscore(s):
  if len(s) > 0 and s[len(s)-1] == "_":
    return s[:len(s)-1]
  else:
    return s

# Escape an initial @ sign is present. Needed for doxygen code extraction
def escape(s):
  if len(s) > 0 and s[0] == "@":
    return "\\%s" % (s)
  else:
    return s

def union_by_string(s,t):
  result = set()
  for e in s:
    if e.to_string() not in map(lambda x: x.to_string(), result):
      result |= set([e])
  for e in t:
    if e.to_string() not in map(lambda x: x.to_string(), result):
      result |= set([e])
  return result

def difference_by_string(s,t):
  result = set([])
  for e in s:
    if e.to_string() not in map(lambda x: x.to_string(), t):
      result |= set([e])
  return result

def is_standard_function(s):
  return s == "equal_to" or s == "not_equal_to" or s == "if_" or s == "less" or s == "less_equal" or s == "greater" or s == "greater_equal"

def target_sort(sort_expr):
  if isinstance(sort_expr, sort_arrow):
    return sort_expr.codomain
  else:
    return sort_expr

class identifier():
  def __init__(self, string):
    self.string = string

  def to_string(self):
    return "%s" % (self.string)

class label():
  def __init__(self, id):
    assert(isinstance(id, identifier))
    self.identifier = id

  def to_string(self):
    return "%s" % (self.identifier.to_string())

class variable_declaration():
  def __init__(self, id, sort_expr):
    assert(isinstance(id, identifier))
    assert(isinstance(sort_expr, sort_expression))
    self.id = id
    self.sort_expression = sort_expr

  def sort_parameters(self, sort_spec):
    return self.sort_expression.sort_parameters(sort_spec)

  def to_string(self):
    return "%s : %s" % (self.id.to_string(), self.sort_expression.to_string())

  def code(self, sort_spec):
    return "        variable v%s(\"%s\",%s)" % (self.id.to_string(), self.id.to_string(), self.sort_expression.code(sort_spec))

class variable_declaration_list():
  def __init__(self, elements):
    assert(all(map(lambda x: isinstance(x, variable_declaration), elements)))
    self.elements = elements

  def push_back(self, element):
    assert(isinstance(element, variable_declaration))
    if element.to_string() not in map(lambda x: x.to_string(), self.elements):
      self.elements += [element]

  def find_variable(self, variable):
    assert(isinstance(variable, data_variable))
    for e in self.elements:
      if e.id.to_string() == variable.id.to_string():
        return e

  def to_string(self):
    s = ""
    for e in self.elements:
      if s != "":
        s += ", "
      s += e.to_string()
    return s

  def code(self, sort_spec):
    code = ""
    for e in self.elements:
      code += "%s;\n" % (e.code(sort_spec))
    return code

class function_declaration():
  def __init__(self, id, sort_expr, l):
    assert(isinstance(id, identifier))
    assert(isinstance(sort_expr, sort_expression))
    assert(isinstance(l, label))
    self.id = id
    self.sort_expression = sort_expr
    self.label = l
    self.namespace = ""
    self.original_namespace = ""

  def set_namespace(self, string):
    self.namespace = string
    if self.original_namespace == "":
      self.original_namespace = string

  def projection_arguments(self, projection_arguments):
    if isinstance(self.sort_expression, sort_arrow):
      arguments = self.sort_expression.domain.projection_arguments()
      for (a, idx) in arguments:
        value = (self.id, self.label, idx)
        if a not in projection_arguments:
          projection_arguments[a] = [value]
        else:
          projection_arguments[a] += [value]
    return projection_arguments

  def sort_parameters(self, sort_spec):
    return self.sort_expression.sort_parameters(sort_spec)

  def to_string(self):
    return "%s : %s" % (self.id.to_string(), self.sort_expression.to_string())

  def generator_code(self, sort_spec, function_declarations):
    if self.namespace == self.original_namespace and self.namespace == function_declarations.namespace and self.namespace == sort_spec.namespace:
      sort_params = self.sort_expression.sort_parameters(sort_spec)
      # Determine whether this function is overloaded
      functions = filter(lambda x: (x.id.to_string() == self.id.to_string()) and (x.label.to_string() == self.label.to_string()), function_declarations.elements)
      extra_parameters = ""
      if len(functions) > 1:
        extra_parameters = self.sort_expression.domain.code(sort_spec)
      if sort_params == set():
        return "        result.push_back(%s(%s));\n" % (self.label.to_string(), extra_parameters)
      else:
        sort_parameters = []
        for s in sort_params:
          sort_parameters += [s.code(sort_spec)]
        if extra_parameters <> "":
          extra_parameters = ", %s" % (extra_parameters)
        return "        result.push_back(%s(%s%s));\n" % (self.label.to_string(), string.join(sort_parameters, ", "), extra_parameters)
    else:
      return ""

class function_declaration_list():
  def __init__(self, elements):
    assert(all(map(lambda x: isinstance(x, function_declaration), elements)))
    self.elements = elements
    self.namespace = ""
    self.original_namespace = ""

  def push_back(self, element):
    assert(isinstance(element, function_declaration))
    self.elements += [element]

  def set_namespace(self, string):
    self.namespace = string
    if self.original_namespace == "":
      self.original_namespace = string
    for e in self.elements:
      e.set_namespace(string)

  def sort_parameters(self, sort_spec):
    result = set()
    for e in self.elements:
      result = union_by_string(result, e.sort_parameters(sort_spec))
    return result

  def find_function(self, function, argumentcount):
    if function.id.to_string() == "==":
      return function_declaration(function.id, sort_expression(), label(identifier("equal_to")))
    elif function.id.to_string() == "!=":
      return function_declaration(function.id, sort_expression(), label(identifier("not_equal_to")))
    elif function.id.to_string() == "if":
      return function_declaration(function.id, sort_expression(), label(identifier("if_")))
    elif function.id.to_string() == "<":
      return function_declaration(function.id, sort_expression(), label(identifier("less")))
    elif function.id.to_string() == ">":
      return function_declaration(function.id, sort_expression(), label(identifier("greater")))
    elif function.id.to_string() == "<=":
      return function_declaration(function.id, sort_expression(), label(identifier("less_equal")))
    elif function.id.to_string() == ">=":
      return function_declaration(function.id, sort_expression(), label(identifier("greater_equal")))
    else:
      for e in self.elements:
        if e.id.to_string() == function.id.to_string():
          if argumentcount == -1:
            return e
          elif argumentcount == 0 and not isinstance(e.sort_expression, sort_arrow):
            return e
          elif isinstance(e.sort_expression, sort_arrow) and len(e.sort_expression.domain.elements) == argumentcount:
            return e
    return None

  def merge_declarations(self, declarations):
    for d in declarations.elements:
      if any(map(lambda x: (d.id.to_string() == x.id.to_string()) and (d.label.to_string() == x.label.to_string()) and (x.namespace == self.namespace), self.elements)):
        d.set_namespace(self.namespace)
      if not any(map(lambda x: (d.id.to_string() == x.id.to_string()) and (d.label.to_string() == x.label.to_string()) and (d.sort_expression.to_string() == x.sort_expression.to_string()), self.elements)):
        self.elements += [d]

  def to_string(self):
    s = ""
    for e in self.elements:
      s += "%s\n" % (e.to_string())
    return s

  def generator_code(self, sort_spec, is_constructor_declaration = False):
    code = ""
    for e in self.elements:
      if not is_constructor_declaration or not sort_spec.is_alias(target_sort(e.sort_expression)):
        code += e.generator_code(sort_spec, self)
    return code

  def projection_code(self, sort_spec):
    projection_arguments = {}
    for e in self.elements:
      if (e.namespace == e.original_namespace and e.namespace == sort_spec.namespace):
        projection_arguments = e.projection_arguments(projection_arguments)
    code = ""
    for p in projection_arguments:
      # Filter duplicates
      case_arguments = projection_arguments[p]
      unique_case_arguments = []
      unique_case_arguments_string = []
      for (id, label, idx) in case_arguments:
        if not (id.to_string(), label.to_string(), idx) in unique_case_arguments_string:
          unique_case_arguments += [(id, label, idx)]
          unique_case_arguments_string += [(id.to_string(), label.to_string(), idx)]

      # Generate code for projection function
      case_code  = "      ///\\brief Function for projecting out argument\n"
      case_code += "      ///        %s from an application\n" % (escape(p))
      case_code += "      /// \\param e A data expression\n"
      case_code += "      /// \\pre %s is defined for e\n" % (escape(p))
      case_code += "      /// \\return The argument of e that corresponds to %s\n" % (escape(p))
      case_code += "      inline\n"
      case_code += "      data_expression %s(const data_expression& e)\n" % (p)
      case_code += "      {\n"
      assertions = []
      for (id, label, idx) in unique_case_arguments:
        assertions.append("is_%s_application(e)" % (label.to_string()))
      case_code += "        assert(%s);\n" % (string.join(assertions, " || "))

      # First group by index
      index_table = {}
      for (id, label, idx) in unique_case_arguments:
        try:
          index_table[idx].append((id, label))
        except:
          index_table[idx] = [(id, label)]
      if len(index_table) == 1:
        case_code += "        return static_cast< application >(e).arguments()[%s];\n" % (index_table.keys()[0])
      else:
        for i in index_table:
          clauses = []
          for c in index_table[i]:
            clauses.append("is_%s_application(e)" % (c[1].to_string()))
          case_code += "        if (%s)\n" % (string.join(clauses, " || "))
          case_code += "        {\n"
          case_code += "          return static_cast< application >(e).arguments()[%s];\n" % (i)
          case_code += "        }\n"
        case_code += "        throw mcrl2::runtime_error(\"Unexpected expression \" + pp(e) + \" occurred\");\n"
      case_code += "      }\n"
      code += "%s\n" % (case_code)
    return code

  def code(self, sort_spec, is_constructor_declaration = False):
    assert(isinstance(sort_spec, sort_specification))
    # First we merge function declarations with the same function symbol/label,
    # hence we get function symbols with multiple sorts. Then for each of these
    # we generate the code
    class multi_function_declaration():
      def __init__(self, id, namespace, sort_expressions, l):
        assert(isinstance(id, identifier))
        assert(isinstance(sort_expressions, sort_expression_list))
        assert(isinstance(l, label))
        self.id = id
        self.namespace = namespace
        self.sort_expression_list = sort_expressions
        self.label = l

      def to_string(self):
        return "%s : { %s }" % (self.id.to_string(), self.sort_expression_list.to_string())

      def function_name(self, fullname, name):
        code  = ""
        code += "      /// \\brief Generate identifier %s\n" % (escape(fullname))
        code += "      /// \\return Identifier %s\n" % (escape(fullname))
        code += "      inline\n"
        code += "      core::identifier_string const& %s_name()\n" % (name)
        code += "      {\n"
        code += "        static core::identifier_string %s_name = data::detail::initialise_static_expression(%s_name, core::identifier_string(\"%s\"));\n" % (name, name, fullname)
        code += "        return %s_name;\n" % (name)
        code += "      }\n\n"
        return code

      def function_constructor(self, fullname, name, sortparams, sort):
        code  = ""
        code += self.function_name(fullname, name)
        code += "      /// \\brief Constructor for function symbol %s\n" % (escape(fullname))
        sortparams_list = string.split(sortparams, ", ")
        if sortparams_list <> ['']:
          for s in sortparams_list:
            code += "      /// \\param %s A sort expression\n" % (escape(s[len("const sort_expression& "):]))
        code += "      /// \\return Function symbol %s\n" % (escape(name))
        code += "      inline\n"
        if sortparams_list <> ['']:
          code += "      function_symbol %s(%s)\n" % (name, sortparams)
          code += "      {\n"
          code += "        function_symbol %s(%s_name(), %s);\n" % (name, name, sort)
        else:
          code += "      function_symbol const& %s(%s)\n" % (name, sortparams)
          code += "      {\n"
          code += "        static function_symbol %s = data::detail::initialise_static_expression(%s, function_symbol(%s_name(), %s));\n" % (name, name, name, sort)
        code += "        return %s;\n" % (name)
        code += "      }\n\n"
        return code

      def polymorphic_function_constructor(self, fullname, name, sortparams, comma, domainparams, targetsort, sort):
        code  = ""
        code += self.function_name(fullname, name)
        code += "      ///\\brief Constructor for function symbol %s\n" % (escape(fullname))
        sortparams_list = string.split(sortparams, ", ")
        if sortparams_list <> ['']:
          for s in sortparams_list:
            code += "       /// \\param %s A sort expression\n" % (escape(s[len("const sort_expression& "):]))
        domainparams_list = string.split(domainparams, ", ")
        if domainparams_list <> ['']:
          for s in domainparams_list:
            code += "      /// \\param %s A sort expression\n" % (escape(s[len("const sort_expression& "):]))
        code += "      ///\\return Function symbol %s\n" % (escape(name))
        code += "      inline\n"
        code += "      function_symbol %s(%s%s%s)\n" % (name, sortparams, comma, domainparams)
        code += "      {\n"
        code += "        %s\n" % (targetsort)
        code += "        function_symbol %s(%s_name(), %s);\n" % (name, name, sort)
        code += "        return %s;\n" % (name)
        code += "      }\n"
        return code 

      def function_recogniser(self, fullname, name):
        code  = ""
        code += "      /// \\brief Recogniser for function %s\n" % (escape(fullname))
        code += "      /// \\param e A data expression\n"
        code += "      /// \\return true iff e is the function symbol matching %s\n" % (escape(fullname))
        code += "      inline\n"
        code += "      bool is_%s_function_symbol(const data_expression& e)\n" % (name)
        code += "      {\n"
        code += "        if (e.is_function_symbol())\n"
        code += "        {\n"
        code += "          return static_cast< function_symbol >(e).name() == %s_name();\n" % (name)
        code += "        }\n"
        code += "        return false;\n"
        code += "      }\n"
        return code

      def function_application(self, fullname, name, formsortparams, comma, formparams, actsortparams, actparams):
        code  = ""
        code += "      /// \\brief Application of function symbol %s\n" % (escape(fullname))
        formsortparams_list = string.split(formsortparams, ", ")
        if formsortparams_list <> ['']:
          for s in formsortparams_list:
            code += "      /// \\param %s A sort expression\n" % (escape(s[len("const sort_expression& "):]))
        formparams_list = string.split(formparams, ", ")
        if formparams_list <> ['']:
          for d in formparams_list:
            code += "      /// \\param %s A data expression\n" % (escape(d[len("const data_expression& "):]))
        code += "      /// \\return Application of %s to a number of arguments\n" % (escape(fullname))
        code += "      inline\n"
        code += "      application %s(%s%s%s)\n" % (name, formsortparams, comma, formparams)
        code += "      {\n"
        code += "        return application(%s(%s),%s);\n" % (name, actsortparams, actparams)
        code += "      }\n"
        return code

      def function_application_recogniser(self, fullname, name):
        code  = ""
        code += "      /// \\brief Recogniser for application of %s\n" % (escape(fullname))
        code += "      /// \\param e A data expression\n"
        code += "      /// \\return true iff e is an application of function symbol %s to a\n" % (escape(name))
        code += "      ///     number of arguments\n"
        code += "      inline\n"
        code += "      bool is_%s_application(const data_expression& e)\n" % (name)
        code += "      {\n"
        code += "        if (e.is_application())\n"
        code += "        {\n"
        code += "          return is_%s_function_symbol(static_cast< application >(e).head());\n" % (name)
        code += "        }\n"
        code += "        return false;\n"
        code += "      }\n"
        return code

      def function_application_code(self, sort, polymorphic = False):
        code = ""
        comma = ""
        if self.sort_expression_list.formal_parameters_code(sort_spec) <> "":
          comma = ", "
        if isinstance(sort, sort_arrow):
          act_sort_params = []
          if polymorphic:
            params = string.split(sort.domain.actual_parameters_code(sort_spec), ", ")
            for p in params:
              act_sort_params += ["%s.sort()" % (p)]
          if self.sort_expression_list.actual_parameters_code(sort_spec) <> "":
            act_sort_params = [self.sort_expression_list.actual_parameters_code(sort_spec)] + act_sort_params

          code += self.function_application(self.id.to_string(), self.label.to_string(), self.sort_expression_list.formal_parameters_code(sort_spec), comma, sort.domain.formal_parameters_code(sort_spec), string.join(act_sort_params, ", "), sort.domain.actual_parameters_code(sort_spec))
          code += "\n"
          code += self.function_application_recogniser(self.id.to_string(), self.label.to_string())
          code += "\n"
        return code

      def code(self, sort_spec):
        assert(isinstance(sort_spec, sort_specification))
        if self.namespace <> sort_spec.namespace :
          return ""

        assert(self.namespace == sort_spec.namespace)
        code = ""
        if len(self.sort_expression_list.elements) == 1:
          sort = self.sort_expression_list.elements[0] # as len is 1
          code += self.function_constructor(self.id.to_string(), self.label.to_string(), self.sort_expression_list.formal_parameters_code(sort_spec), sort.code(sort_spec))
          code += "\n"
          code += self.function_recogniser(self.id.to_string(), self.label.to_string())
          code += "\n"
          code += self.function_application_code(sort)
        else:
          # There is polymorphism in play, hence we need to compute the
          # allowed sorts first.
          # That is, we have parameters for all domain elements
          # bases on those, we compute the corresponding target sort
          new_domain_sorts = [sort_identifier(identifier("s%s" % i)) for i in range(len(self.sort_expression_list.elements[0].domain.elements))]

          first_codomain = self.sort_expression_list.elements[0].codomain
          if all(map(lambda x: x.codomain.to_string() == first_codomain.to_string(), self.sort_expression_list.elements)):
            target_sort = "sort_expression target_sort(%s);\n" % (first_codomain.code(sort_spec))
          else:
            target_sort = "sort_expression target_sort;\n"
            for (i,sort) in enumerate(self.sort_expression_list.elements):
              if i == 0:
                target_sort += "        if ("
              else:
                target_sort += "        else if ("
              for (j,domsort) in enumerate(sort.domain.elements):
                if j <> 0:
                  target_sort += " && "
                if sort.domain.labelled:
                  domain_element = domsort[0]
                else:
                  domain_element = domsort
                target_sort += "%s == %s" % (new_domain_sorts[j].code(sort_spec), domain_element.code(sort_spec))
              target_sort += ")\n"
              target_sort += "        {\n"
              target_sort += "          target_sort = %s;\n" % (sort.codomain.code(sort_spec))
              target_sort += "        }\n"
            target_sort += "        else\n"
            target_sort += "        {\n"
            target_sort += "          assert(false);\n"
            target_sort += "        }\n"
          target_sort_id = sort_identifier(identifier("target_sort"))
          new_sort = sort_arrow(domain(False, new_domain_sorts), target_sort_id)

          domain_param_code = map(lambda x: "const sort_expression& %s" % (x.code(sort_spec)), new_domain_sorts)

          comma = ""
          if self.sort_expression_list.formal_parameters_code(sort_spec) <> "":
            comma = ", "
          code += self.polymorphic_function_constructor(self.id.to_string(), self.label.to_string(), self.sort_expression_list.formal_parameters_code(sort_spec), comma, string.join(domain_param_code, ", "), target_sort, new_sort.code(sort_spec))
          code += "\n"
          code += self.function_recogniser(self.id.to_string(), self.label.to_string())
          code += "\n"
          code += self.function_application_code(self.sort_expression_list.elements[0], True)

        return "%s" % (code)

    class multi_function_declaration_list():
      def __init__(self, elements):
        assert(all(map(lambda x: isinstance(x, multi_function_declaration), elements)))
        self.elements = elements

      def push_back(self, element):
        assert(isinstance(element, multi_function_declaration))
        self.elements += [element]

      def push_back_function_declaration(self, element):
        assert(isinstance(element, function_declaration))
        found = False
        for e in self.elements:
          if (e.id.to_string() == element.id.to_string()) and (e.label.to_string() == element.label.to_string()):
            e.sort_expression_list.push_back(element.sort_expression)
            found = True
        if not found:
          self.elements += [multi_function_declaration(element.id, element.namespace, sort_expression_list([element.sort_expression]), element.label)]

      def to_string(self):
        s = ""
        for e in self.elements:
          s += "%s\n" % (e.to_string())
        return s

      def code(self, sort_spec):
        assert(isinstance(sort_spec, sort_specification))
        code = ""
        for e in self.elements:
          code += "%s" % (e.code(sort_spec))
        return code

    # Add all the elements to a multi_function_declaration_list,
    # and let it do the code generation
    multi_function_declarations = multi_function_declaration_list([])
    for e in self.elements:
      if (e.namespace == self.namespace):
        multi_function_declarations.push_back_function_declaration(e)

    code = multi_function_declarations.code(sort_spec)
    return code

class equation_declaration():
  def __init__(self, lhs, rhs, condition = None):
    assert(isinstance(lhs, data_expression))
    assert(isinstance(rhs, data_expression))
    self.namespace = ""
    self.original_namespace = ""
    if condition != None: assert(isinstance(condition, data_expression))
    self.lhs = lhs
    self.rhs = rhs
    self.condition = condition

  def set_namespace(self, string):
    self.namespace = string
    if self.original_namespace == "":
      self.original_namespace = string

  def sort_parameters(self, sort_spec, function_spec, variable_spec):
    result = self.lhs.sort_parameters(sort_spec, function_spec, variable_spec)
    result = union_by_string(result, self.rhs.sort_parameters(sort_spec, function_spec, variable_spec))
    if self.condition != None:
      result = union_by_string(result, self.condition.sort_parameters(sort_spec, function_spec, variable_spec))
    return result

  def has_lambda(self):
    res = False
    if self.condition <> None:
      res = res or self.condition.has_lambda()
    res = res or self.lhs.has_lambda() or self.rhs.has_lambda()
    return res

  def has_forall(self):
    res = False
    if self.condition <> None:
      res = res or self.condition.has_forall()
    res = res or self.lhs.has_forall() or self.rhs.has_forall()
    return res

  def has_exists(self):
    res = False
    if self.condition <> None:
      res = res or self.condition.has_exists()
    res = res or self.lhs.has_exists() or self.rhs.has_exists()
    return res


  def to_string(self):
    if self.condition == None:
      return "%s = %s" % (self.lhs.to_string(), self.rhs.to_string())
    else:
      return "%s -> %s = %s" % (self.condition.to_string(), self.lhs.to_string(), self.rhs.to_string())

  def determinise_variable_or_function_symbol(self, function_spec, variable_spec):
    condition = None
    if self.condition <> None:
      condition = self.condition.determinise_variable_or_function_symbol(function_spec, variable_spec)
    lhs = self.lhs.determinise_variable_or_function_symbol(function_spec, variable_spec)
    rhs = self.rhs.determinise_variable_or_function_symbol(function_spec, variable_spec)

    return equation_declaration(lhs, rhs, condition)

  def code(self, sort_spec, function_spec, variable_spec):
    variables = self.lhs.free_variables()
    variables = union_by_string(variables, self.rhs.free_variables())
    if self.condition <> None:
      variables = union_by_string(variables, self.condition.free_variables())

    variables_code = []
    for v in variables:
      variables_code += [v.code(sort_spec, function_spec, variable_spec)]
    if len(variables_code) == 0:
      variables_string = "variable_list()"
    else:
      variables_string = "make_vector(%s)" % (string.join(sorted(variables_code), ", "))

    if self.condition == None:
      code = "data_equation(%s, %s, %s)" % (variables_string, self.lhs.code(sort_spec, function_spec, variable_spec), self.rhs.code(sort_spec, function_spec, variable_spec))
    else:
      code = "data_equation(%s, %s, %s, %s)" % (variables_string, self.condition.code(sort_spec, function_spec, variable_spec), self.lhs.code(sort_spec, function_spec, variable_spec), self.rhs.code(sort_spec, function_spec, variable_spec))

    return "result.push_back(%s);" % (code)

class equation_declaration_list():
  def __init__(self, elements):
    assert(all(map(lambda x: isinstance(x, equation_declaration), elements)))
    self.elements = elements
    self.namespace = ""
    self.original_namespace = ""

  def push_back(self, element):
    assert(isinstance(element, equation_declaration))
    self.elements += [element]

  def set_namespace(self, string):
    self.namespace = string
    if self.original_namespace == "":
      self.original_namespace = string
    for e in self.elements:
      e.set_namespace(string)

  def sort_parameters(self, sort_spec, function_spec, variable_spec):
    result = set()
    for e in self.elements:
      result = union_by_string(result, e.sort_parameters(sort_spec, function_spec, variable_spec))
    return result

  def determinise_variable_or_function_symbol(self, function_spec, variable_spec):
    result = equation_declaration_list([])
    result.namespace = self.namespace
    result.original_namespace = self.original_namespace
    for e in self.elements:
      result.push_back(e.determinise_variable_or_function_symbol(function_spec, variable_spec))
    return result

  def has_lambda(self):
    return any(map(lambda x: x.has_lambda(), self.elements))

  def has_forall(self):
    return any(map(lambda x: x.has_forall(), self.elements))

  def has_exists(self):
    return any(map(lambda x: x.has_exists(), self.elements))

  def to_string(self):
    s = ""
    for e in self.elements:
      s += "%s\n" % (e.to_string())
    return s

  def code(self, sort_spec, function_spec, variable_spec):
    self = self.determinise_variable_or_function_symbol(function_spec, variable_spec)
    sort_parameters = self.sort_parameters(sort_spec, function_spec, variable_spec)
    formal_parameters_code = []
    for s in sort_parameters:
      formal_parameters_code += [s.formal_parameter_code()]
    code  = "      /// \\brief Give all system defined equations for %s\n" % (escape(self.namespace))
    for s in sort_parameters:
      code += "      /// \\param %s A sort expression\n" % (escape(s.code(sort_spec)))
    code += "      /// \\return All system defined equations for sort %s\n" % (escape(self.namespace))
    code += "      inline\n"
    code += "      data_equation_vector %s_generate_equations_code(%s)\n" % (self.namespace, string.join(formal_parameters_code, ", "))
    code += "      {\n"
    code += "%s\n" % (variable_spec.code(sort_spec))
    code += "        data_equation_vector result;\n"
    code += "%s" % (sort_spec.structured_sort_equation_code())
    for e in self.elements:
      code += "        %s\n" % (e.code(sort_spec, function_spec, variable_spec))
    code += "        return result;\n"
    code += "      }\n"

    return code


class data_expression():
  def __init__(self):
    pass

  def to_string(self):
    pass

class data_application(data_expression):
  def __init__(self, head, arguments):
    assert(isinstance(head, data_expression))
    assert(isinstance(arguments, data_expression_list))
    self.head = head
    self.arguments = arguments

  def sort_parameters(self, sort_spec, function_spec, variable_spec):
    result = self.head.sort_parameters(sort_spec, function_spec, variable_spec)
    result = union_by_string(result, self.arguments.sort_parameters(sort_spec, function_spec, variable_spec))
    return result

  def determinise_variable_or_function_symbol(self, function_spec, variable_spec):
    return data_application(self.head.determinise_variable_or_function_symbol(function_spec, variable_spec), self.arguments.determinise_variable_or_function_symbol(function_spec, variable_spec))

  def free_variables(self):
    result = self.head.free_variables()
    result = union_by_string(result, self.arguments.free_variables())
    return result

  def has_lambda(self):
    return self.head.has_lambda() or self.arguments.has_lambda()

  def has_forall(self):
    return self.head.has_forall() or self.arguments.has_forall()

  def has_exists(self):
    return self.head.has_exists() or self.arguments.has_exists()

  def to_string(self):
    return "%s(%s)" % (self.head.to_string(), self.arguments.to_string())

  def code(self, sort_spec, function_spec, variable_spec):
    if isinstance(self.head, function_symbol):
      # FIXME: Extremely ugly workaround for set complement
      if self.head.id.to_string() == "!" and len(self.arguments.elements) == 1 and isinstance(self.arguments.elements[0], data_application) and isinstance(self.arguments.elements[0].head, data_variable):
        head_code = "sort_bool_::not_()"
      # FIXME: Extremely ugly workaround for bag join
      elif self.head.id.to_string() == "+" and len(self.arguments.elements) == 2 and isinstance(self.arguments.elements[0], data_application) and isinstance(self.arguments.elements[0].head, data_variable):
        head_code = "sort_nat::plus()"
      else:
        head_code = self.head.code(sort_spec, function_spec, variable_spec, len(self.arguments.elements))
    else:
      head_code = self.head.code(sort_spec, function_spec, variable_spec)
    # make generated code a bit more readable 
    if isinstance(self.head, function_symbol):
      head_code = head_code[:head_code.find("(")]
    sort_parameters_code = []
      # FIXME: Extremely ugly workaround for set complement / bag joint
    if isinstance(self.head, function_symbol) and (head_code <> "sort_bool_::not_" and head_code <> "sort_nat::plus"):
      sort_parameters = self.sort_parameters(sort_spec, function_spec, variable_spec)
      for s in sort_parameters:
        sort_parameters_code += [s.code(sort_spec)]
    sort_parameters_code += [self.arguments.code(sort_spec, function_spec, variable_spec)]

    return "%s(%s)" % (head_code, string.join(sort_parameters_code, ", "))

class data_variable_or_function_symbol(data_expression):
  def __init__(self, identifier):
    self.id = identifier

  def has_lambda(self):
    return False

  def has_forall(self):
    return False

  def has_exists(self):
    return False

  def to_string(self):
    return self.id.to_string()

  def determinise_variable_or_function_symbol(self, function_spec, variable_spec):
    if function_spec.find_function(self, -1) <> None:
      return function_symbol(self.id)
    else:
      return data_variable(self.id)

class data_variable(data_expression):
  def __init__(self, id):
    assert(isinstance(id, identifier))
    self.id = id

  def sort_parameters(self, sort_spec, function_spec, variable_spec):
    return variable_spec.find_variable(self).sort_expression.sort_parameters(sort_spec)

  def determinise_variable_or_function_symbol(self, function_spec, variable_spec):
    return self

  def free_variables(self):
    result = set([self])
    return result

  def has_lambda(self):
    return False

  def has_forall(self):
    return False

  def has_exists(self):
    return False

  def to_string(self):
    return "%s" % (self.id.to_string())

  def code(self, sort_spec, function_spec, variable_spec):
    return "v%s" % (self.id.to_string())

class function_symbol(data_expression):
  def __init__(self, id):
    assert(isinstance(id, identifier))
    self.id = id

  def sort_parameters(self, sort_spec, function_spec, variable_spec):
    return function_spec.find_function(self, -1).sort_expression.sort_parameters(sort_spec)

  def determinise_variable_or_function_symbol(self, function_spec, variable_spec):
    return self

  def free_variables(self):
    result = set()
    return result

  def has_lambda(self):
    return False

  def has_forall(self):
    return False

  def has_exists(self):
    return False

  def to_string(self):
    return "%s" % (self.id.to_string())

  def code(self, sort_spec, function_spec, variable_spec, argumentcount = -1):
    f = function_spec.find_function(self, argumentcount)
    sort_parameters_code = []
    for s in f.sort_parameters(sort_spec):
      sort_parameters_code += [s.code(sort_spec)]
    if f.namespace == function_spec.namespace or is_standard_function(f.label.to_string()):
      return "%s(%s)" % (f.label.to_string(), string.join(sort_parameters_code, ", "))
    else:
      return "sort_%s::%s(%s)" % (f.namespace, f.label.to_string(), string.join(sort_parameters_code, ", "))

class lambda_abstraction(data_expression):
  def __init__(self, var_declaration, expression):
    assert(isinstance(var_declaration, variable_declaration))
    assert(isinstance(expression, data_expression))
    self.variable_declaration = var_declaration
    self.expression = expression

  def sort_parameters(self, sort_spec, function_spec, variable_spec):
    result = self.variable_declaration.sort_parameters(sort_parameters)
    result = union_by_string(result, self.expression.sort_parameters(sort_spec, function_spec, variable_spec))
    return result

  def determinise_variable_or_function_symbol(self, function_spec, variable_spec):
    variable_spec.declarations.push_back(self.variable_declaration)
    return lambda_abstraction(self.variable_declaration, self.expression.determinise_variable_or_function_symbol(function_spec, variable_spec))

  def free_variables(self):
    result = self.expression.free_variables()
    result = difference_by_string(result, set([data_variable(self.variable_declaration.id)]))
    return result

  def has_lambda(self):
    return True

  def has_forall(self):
    return self.expression.has_forall()

  def has_exists(self):
    return self.expression.has_exists()

  def to_string(self):
    return "lambda(%s, %s)" % (self.variable_declaration.to_string(), self.expression.to_string())

  def code(self, sort_spec, function_spec, variable_spec):
    return "lambda(make_vector(%s), %s)" % (data_variable(self.variable_declaration.id).code(sort_spec, function_spec, variable_spec), self.expression.code(sort_spec, function_spec, variable_spec))

class forall(data_expression):
  def __init__(self, var_declaration, expression):
    assert(isinstance(var_declaration, variable_declaration))
    assert(isinstance(expression, data_expression))
    self.variable_declaration = var_declaration
    self.expression = expression
    
  def sort_parameters(self, sort_spec, function_spec, variable_spec):
    result = self.variable_declaration.sort_parameters(sort_spec)
    result = union_by_string(result, self.expression.sort_parameters(sort_spec, function_spec, variable_spec))
    return result

  def determinise_variable_or_function_symbol(self, function_spec, variable_spec):
    variable_spec.declarations.push_back(self.variable_declaration)
    return forall(self.variable_declaration, self.expression.determinise_variable_or_function_symbol(function_spec, variable_spec))

  def free_variables(self):
    result = self.expression.free_variables()
    result = difference_by_string(result, set([data_variable(self.variable_declaration.id)]))
    return result

  def has_lambda(self):
    return self.expression.has_lambda()

  def has_forall(self):
    return True

  def has_exists(self):
    return self.expression.has_exists()

  def to_string(self):
    return "forall(%s, %s)" % (self.variable_declaration.to_string(), self.expression.to_string())

  def code(self, sort_spec, function_spec, variable_spec):
    return "forall(make_vector(%s), %s)" % (data_variable(self.variable_declaration.id).code(sort_spec, function_spec, variable_spec), self.expression.code(sort_spec, function_spec, variable_spec))

class exists(data_expression):
  def __init__(self, var_declaration, expression):
    assert(isinstance(var_declaration, variable_declaration))
    assert(isinstance(expression, data_expression))
    self.variable_declaration = var_declaration
    self.expression = expression

  def sort_parameters(self, sort_spec, function_spec, variable_spec):
    result = self.variable_declaration.sort_parameters(sort_spec)
    result = union_by_string(result, self.expression.sort_parameters(sort_spec, function_spec, variable_spec))
    return result

  def determinise_variable_or_function_symbol(self, function_spec, variable_spec):
    variable_spec.declarations.push_back(self.variable_declaration)
    return exists(self.variable_declaration, self.expression.determinise_variable_or_function_symbol(function_spec, variable_spec))

  def free_variables(self):
    result = self.expression.free_variables()
    result = difference_by_string(result, set([data_variable(self.variable_declaration.id)]))
    return result

  def has_lambda(self):
    return self.expression.has_lambda()

  def has_forall(self):
    return self.expression.has_forall()

  def has_exists(self):
    return True

  def to_string(self):
    return "exists(%s, %s)" % (self.variable_declaration.to_string(), self.expression.to_string())

  def code(self, sort_spec, function_spec, variable_spec):
    return "exists(make_vector(%s), %s)" % (data_variable(self.variable_declaration.id).code(sort_spec, function_spec, variable_spec), self.expression.code(sort_spec, function_spec, variable_spec))

class data_expression_list():
  def __init__(self, elements):
    assert(all(map(lambda x: isinstance(x, data_expression), elements)))
    self.elements = elements

  def push_back(self, element):
    assert(isinstance(element, data_expression))
    self.elements += [element]

  def determinise_variable_or_function_symbol(self, function_spec, variable_spec):
    result = data_expression_list([])
    for e in self.elements:
      result.push_back(e.determinise_variable_or_function_symbol(function_spec, variable_spec))
    return result

  def sort_parameters(self, sort_spec, function_spec, variable_spec):
    result = set()
    for e in self.elements:
      union_by_string(result, e.sort_parameters(sort_spec, function_spec, variable_spec))
    return result

  def free_variables(self):
    result = set()
    for e in self.elements:
      result = union_by_string(result, e.free_variables())
    return result

  def has_lambda(self):
    return any(map(lambda x: x.has_lambda(), self.elements))

  def has_forall(self):
    return any(map(lambda x: x.has_forall(), self.elements))

  def has_exists(self):
    return any(map(lambda x: x.has_exists(), self.elements))

  def to_string(self):
    s = ""
    for e in self.elements:
      if s != "":
        s += ", "
      s += e.to_string()
    return s

  def code(self, sort_spec, function_spec, variable_spec):
    code = []
    for e in self.elements:
      code += [e.code(sort_spec, function_spec, variable_spec)]
    return string.join(code, ", ")

class sort_expression_list():
  def __init__(self, elements):
    assert(all(map(lambda x: isinstance(x, sort_expression), elements)))
    self.elements = elements

  def push_back(self, element):
    assert(isinstance(element, sort_expression))
    self.elements += [element]

  def sort_parameters(self, sort_spec):
    result = set()
    for e in self.elements:
      result = union_by_string(result, e.sort_parameters(sort_spec))
    return result

  def formal_parameters_code(self, sort_spec):
    code = ""
    for p in self.sort_parameters(sort_spec):
      if code != "":
        code += ", "
      code += "%s" % (p.formal_parameter_code())
    return code

  def actual_parameters_code(self, sort_spec):
    code = ""
    for p in self.sort_parameters(sort_spec):
      if code != "":
        code += ", "
      code += "%s" % (p.actual_parameter_code())
    return code

  def to_string(self):
    s = ""
    for e in self.elements:
      if s != "":
        s += ", "
      s += e.to_string()
    return s

  def code(self):
    s = "make_vector("
    args = []
    for e in self.elements:
      args += [e.code()]
    s += string.join(args, ", ")
    s += ")"
    return s

class sort_expression():
  def __init__(self):
    pass

  def to_string(self):
    return ""

  def sort_parameters(self, sort_spec):
    return set()
#
#  def formal_parameter_code(self):
#    return ""
#
#  def actual_parameter_code(self):
#    return ""
#
#  def to_string(self):
#    pass

class sort_identifier(sort_expression):
  def __init__(self, id):
    assert(isinstance(id, identifier))
    self.name = id

  def sort_parameters(self, sort_spec):
    if any(map(lambda x: x.to_string() == self.to_string(), sort_spec.sort_parameters())):
      return set([self])
    return set()

  def to_string(self):
    return "%s" % (self.name.to_string())

  def formal_parameter_code(self):
    return "const sort_expression& %s" % (self.name.to_string().lower())

  def actual_parameter_code(self):
    return "%s" % (self.name.to_string().lower())

  def code(self, sort_spec):
    assert(isinstance(sort_spec, sort_specification))
    if (sort_spec.has_sort(self)):
      return sort_spec.get_sort(self).inline_code(sort_spec)
    else:
      return "%s" % (self.name.to_string().lower())

class domain(sort_expression):
  def __init__(self, labelled, elements):
    assert(isinstance(labelled, bool))
    #if labelled:
    #  assert(all(map(lambda x: isinstance(x, (sort_expression, label)), elements)))
    #else:
    #  assert(all(map(lambda x: isinstance(x, sort_expression), elements)))
    self.labelled = labelled
    self.elements = elements

  def push_back(self, element):
#    if self.labelled:
#      assert(isinstance(element, (sort_expression, label)))
#    else:
#      assert(isinstance(x, sort_expression))
    self.elements += [element]

  def projection_arguments(self):
    if not self.labelled:
      return []
    else:
      arguments = []
      for (i,e) in enumerate(self.elements):
        arguments += [(e[1].to_string(), i)]
      return arguments

  def sort_parameters(self, sort_spec):
    result = set()
    for e in self.elements:
      if self.labelled:
        result = union_by_string(result, e[0].sort_parameters(sort_spec))
      else:
        result = union_by_string(result, e.sort_parameters(sort_spec))
    return result

  def formal_parameters_code(self, sort_spec):
    index = 0
    parameters = []
    for e in self.elements:
      parameters += ["const data_expression& arg%s" % (index)]
      index += 1
    return string.join(parameters, ", ")

  def actual_parameters_code(self, sort_spec):
    index = 0
    parameters = []
    for e in self.elements:
      parameters += ["arg%d" % (index)]
      index += 1
    return string.join(parameters, ", ")

  def assertions_code(self):
    # TODO, find a nice and suitable set of assertions here
    return ""

  def to_string(self):
    if self.labelled:
      s = ""
      for e in self.elements:
        if s != "":
          s += " # "
        s += "%s <\"%s\">" % (e[0].to_string(), e[1].to_string())
      return s
    else:
      s = ""
      for e in self.elements:
        if s != "":
          s += " # "
        s += e.to_string()
      return s

  def code(self, sort_spec):
    assert(isinstance(sort_spec, sort_specification))
    code = ""
    for e in self.elements:
      if code != "":
        code += ", "
      if self.labelled:
        code += e[0].code(sort_spec)
      else:
        code += e.code(sort_spec)
    return code

class sort_arrow(sort_expression):
  def __init__(self, dom, codom):
    assert(isinstance(dom, domain))
    assert(isinstance(codom, sort_expression))
    self.domain = dom
    self.codomain = codom

  def sort_parameters(self, sort_spec):
    result = self.codomain.sort_parameters(sort_spec)
    result = union_by_string(result, self.domain.sort_parameters(sort_spec))
    return result

  def to_string(self):
    return "%s -> %s" % (self.domain.to_string(), self.codomain.to_string())

  def code(self, sort_spec):
    assert(isinstance(sort_spec, sort_specification))
    return "function_sort(%s, %s)" % (self.domain.code(sort_spec), self.codomain.code(sort_spec))

class sort_container(sort_expression):
  def __init__(self, container, element_sort):
    assert(isinstance(container, identifier))
    assert(isinstance(element_sort, sort_expression))
    self.container = container
    self.element_sort = element_sort

  def sort_parameters(self, sort_spec):
    return set([self.element_sort])

  def to_string(self):
    return "%s(%s)" % (self.container.to_string(), self.element_sort.to_string())

  def code(self, sort_spec):
    assert(isinstance(sort_spec, sort_specification))
    if (sort_spec.has_sort(self)):
      sort = sort_spec.get_sort(self)
      result = "%s(%s)" % (sort.label.to_string(), self.element_sort.code(sort_spec))
      if sort.original_namespace <> sort_spec.namespace:
        result = "sort_%s::%s" % (sort.original_namespace, result)
      return result
    else:
      return "%s(%s)" % (self.container.to_string().lower(), self.element_sort.code(sort_spec))

class structured_sort_declaration():
  def __init__(self, id, l, arguments = None):
    assert(isinstance(id, identifier))
    assert(isinstance(l, label))
    assert(arguments == None or isinstance(arguments, domain))
    self.id = id
    self.label = l
    self.arguments = arguments
    self.namespace = ""
    self.original_namespace = ""

  def merge_structured_sort(self, function_spec, sort_expr):
    if self.arguments == None:
      s = sort_expr
    else:
      s = sort_arrow(self.arguments, sort_expr)
    f = function_declaration(self.id, s, self.label)
    # f.set_namespace should not be used here, as this function may be
    # introduced in an included specification, and hence we need to preserve
    # original_namespace
    f.original_namespace = self.original_namespace
    f.namespace = self.namespace
    constructor_spec = function_spec.constructor_specification
    constructor_spec.declarations.push_back(f)
    function_spec.constructor_specification = constructor_spec
    return function_spec

  def set_namespace(self, string):
    self.namespace = string
    if self.original_namespace == "":
      self.original_namespace = string

  def to_string(self):
    if self.arguments == None:
      return "%s" % (self.id.to_string())
    else:
      return "%s(%s)" % (self.id.to_string(), self.arguments.to_string())

  def struct_constructor_arguments(self, sort_spec):
    assert self.arguments <> None
    code = []
    for a in self.arguments.elements:
      code += ["structured_sort_constructor_argument(%s, \"%s\")" % (a[0].code(sort_spec), a[1].to_string())]
    return "make_vector(%s)" % (string.join(code, ", "))

  def code(self, sort_spec):
    if self.arguments == None:
      return "structured_sort_constructor(\"%s\", \"%s\")" % (self.id.to_string(), self.label.to_string())
    else:
      return "structured_sort_constructor(\"%s\", %s, \"%s\")" % (self.id.to_string(), self.struct_constructor_arguments(sort_spec), self.label.to_string())

class structured_sort_declaration_list():
  def __init__(self, elements):
    assert(all(map(lambda x: isinstance(x, structured_sort_declaration), elements)))
    self.elements = elements
    self.namespace = ""
    self.original_namespace = ""

  def merge_structured_sort(self, function_spec, sort_expr):
    for e in self.elements:
      function_spec = e.merge_structured_sort(function_spec, sort_expr)
    return function_spec

  def push_back(self, element):
    assert(isinstance(element, structured_sort_declaration))
    self.elements += [element]

  def set_namespace(self, string):
    self.namespace = string
    if self.original_namespace == "":
      self.original_namespace = string
    for e in self.elements:
      e.set_namespace(string)

  def to_string(self):
    s = ""
    for e in self.elements:
      if s != "":
        s += " | "
      s += e.to_string()
    return s

  def code(self, sort_spec):
    code = ""
    for e in self.elements:
      code += "          constructors.push_back(%s);\n" % (e.code(sort_spec))
    return code

class structured_sort_specification():
  def __init__(self, elements):
    assert(isinstance(elements, structured_sort_declaration_list))
    self.elements = elements
    self.namespace = ""
    self.original_namespace = ""

  def merge_structured_sort(self, function_spec, sort_expr):
    return self.elements.merge_structured_sort(function_spec, sort_expr)

  def set_namespace(self, string):
    self.namespace = string
    if self.original_namespace == "":
      self.original_namespace = string
    self.elements.set_namespace(string)

  def to_string(self):
    return "struct %s" % (self.elements.to_string())

  def code(self, sort_spec):
    return self.elements.code(sort_spec)

class sort_declaration():
  def __init__(self, sort_expr, l, alias = None):
    assert(isinstance(sort_expr, sort_expression))
    assert(alias == None or isinstance(alias, structured_sort_specification))
    assert(isinstance(l, label))
    self.sort_expression = sort_expr
    self.label = l
    self.alias = alias
    self.namespace = ""
    self.original_namespace = ""

  def sort_parameters(self):
    if isinstance(self.sort_expression, sort_container):
      return set([self.sort_expression.element_sort])
    else:
      return set()

  def merge_structured_sort(self, function_spec):
    if self.alias <> None:
      function_spec = self.alias.merge_structured_sort(function_spec, self.sort_expression)
    return function_spec

  def set_namespace(self, string):
    self.namespace = string
    if self.original_namespace == "":
      self.original_namespace = string
    if self.alias <> None:
      self.alias.set_namespace(string)

  def name(self):
    return self.sort_expression

  def defines_container(self):
    return isinstance(self.sort_expression, sort_container)

  def defines_struct(self):
    return self.alias <> None

  def to_string(self):
    if self.alias == None:
      return "%s" % (self.sort_expression.to_string())
    else:
      return "%s = %s" % (self.sort_expression.to_string(), self.alias.to_string())

  def inline_code(self, sort_spec):
    if (self.namespace == sort_spec.namespace) or (self.namespace == ""):
      return "%s()" % (self.label.to_string())
    else:
      return "sort_%s::%s()" % (self.namespace, self.label.to_string())

  def structured_sort_constructor_code(self):
    if self.alias == None:
      return ""
    else:
      param = ""
      if isinstance(self.sort_expression, sort_container):
        param = self.sort_expression.element_sort.to_string().lower()
      sort = "%s(%s)" % (self.label.to_string(), param)
      result = "        function_symbol_vector %s_constructors = detail::%s_struct(%s).constructor_functions(%s);\n" % (self.label.to_string(), self.label.to_string(), param, sort)
      return result + "        result.insert(result.end(), %s_constructors.begin(), %s_constructors.end());\n" % (self.label.to_string(), self.label.to_string())

  def structured_sort_equation_code(self):
    if self.alias == None:
      return ""
    else:
      param = ""
      if isinstance(self.sort_expression, sort_container):
        param = self.sort_expression.element_sort.to_string().lower()
      sort = "%s(%s)" % (self.label.to_string(), param)
      result = "        data_equation_vector %s_equations = detail::%s_struct(%s).constructor_equations(%s);\n" % (self.label.to_string(), self.label.to_string(), param, sort)
      return result + "        result.insert(result.end(), %s_equations.begin(), %s_equations.end());\n" % (self.label.to_string(), self.label.to_string())

  def sort_name(self, id, label):
    code = ""
    code += "      inline\n"
    code += "      core::identifier_string const& %s_name()\n" % (label.to_string())
    code += "      {\n"
    code += "        static core::identifier_string %s_name = data::detail::initialise_static_expression(%s_name, core::identifier_string(\"%s\"));\n" % (label.to_string(), label.to_string(), id.to_string())
    code += "        return %s_name;\n" % (label.to_string())
    code += "      }\n\n"
    return code

  def sort_expression_constructors(self, id, label):
    code = ""
    code += self.sort_name(id, label)
    code += "      /// \\brief Constructor for sort expression %s\n" % (escape(id.to_string()))
    code += "      /// \\return Sort expression %s\n" % (escape(id.to_string()))
    code += "      inline\n"
    code += "      basic_sort const& %s()\n" % (label.to_string())
    code += "      {\n"
    code += "        static basic_sort %s = data::detail::initialise_static_expression(%s, basic_sort(%s_name()));\n" % (label.to_string(), label.to_string(), label.to_string())
    code += "        return %s;\n" % (label.to_string())
    code += "      }\n\n"

    code += "      /// \\brief Recogniser for sort expression %s\n" % (escape(id.to_string()))
    code += "      /// \\param e A sort expression\n"
    code += "      /// \\return true iff e == %s()\n" % (escape(label.to_string()))
    code += "      inline\n"
    code += "      bool is_%s(const sort_expression& e)\n" % (label.to_string())
    code += "      {\n"
    code += "        if (e.is_basic_sort())\n"
    code += "        {\n"
    code += "          return static_cast< basic_sort >(e) == %s();\n" % (label.to_string())
    code += "        }\n"
    code += "        return false;\n"
    code += "      }\n"
    return code

  def sort_expression_constructors_container_sort(self, id, label, parameter):
    code = ""
    code += self.sort_name(id, label)
    code += "      /// \\brief Constructor for sort expression %s(%s)\n" % (escape(id.to_string()), escape(parameter.to_string()))
    code += "      /// \\param %s A sort expression\n" % (escape(parameter.to_string().lower()))
    code += "      /// \\return Sort expression %s(%s)\n" % (escape(label.to_string()), escape(parameter.to_string().lower()))
    code += "      inline\n"
    code += "      container_sort %s(const sort_expression& %s)\n" % (label.to_string(), parameter.to_string().lower())
    code += "      {\n"
    code += "        container_sort %s(%s_name(), %s);\n" % (label.to_string(), label.to_string(), parameter.to_string().lower())
    code += "        return %s;\n" % (label.to_string())
    code += "      }\n\n"

    code += "      /// \\brief Recogniser for sort expression %s(%s)\n" % (escape(id.to_string()), escape(parameter.to_string().lower()))
    code += "      /// \\param e A sort expression\n"
    code += "      /// \\return true iff e is a container sort of which the name matches\n"
    code += "      ///      %s\n" % (escape(label.to_string()))
    code += "      inline\n"
    code += "      bool is_%s(const sort_expression& e)\n" % (label.to_string())
    code += "      {\n"
    code += "        if (e.is_container_sort())\n"
    code += "        {\n"
    code += "          return static_cast< container_sort >(e).container_name() == %s_name();\n" % (label.to_string())
    code += "        }\n"
    code += "        return false;\n"
    code += "      }\n"
    return code

  def code(self, sort_spec):
    if isinstance(self.sort_expression, sort_identifier):
      code = self.sort_expression_constructors(self.sort_expression, self.label)
    else:
      assert(isinstance(self.sort_expression, sort_container))
      code = self.sort_expression_constructors_container_sort(self.sort_expression.container, self.label, self.sort_expression.element_sort)

    if self.alias <> None:
      code += "\n"
      code += "      namespace detail {\n\n"
      code += "        /// \\brief Declaration for sort %s as structured sort\n" % (escape(self.label.to_string()))
      if isinstance(self.sort_expression, sort_container):
        param = self.sort_expression.element_sort.to_string().lower()
        code += "        /// \\param %s A sort expression\n" % (escape(param))
      else:
        param = ""
      code += "        /// \\ret The structured sort representing %s\n" % (escape(self.label.to_string()))
      code += "        inline\n"
      code += "        structured_sort %s_struct(const sort_expression& %s)\n" % (self.label.to_string(), param)
      code += "        {\n"
      code += "          structured_sort_constructor_vector constructors;\n"
      code += "%s" % (self.alias.code(sort_spec))
      code += "          return structured_sort(constructors);\n"
      code += "        }\n\n"
      code += "      } // namespace detail\n"
    return code

class sort_declaration_list():
  def __init__(self, elements):
    assert(all(map(lambda x: isinstance(x, sort_declaration), elements)))
    self.elements = elements
    self.namespace = ""
    self.original_namespace = ""

  def sort_parameters(self):
    result = set()
    for e in self.elements:
      result = union_by_string(result, e.sort_parameters())
    return result

  def merge_structured_sorts(self, function_spec):
    for e in self.elements:
      function_spec = e.merge_structured_sort(function_spec)
    return function_spec

  def push_back(self, element):
    assert(isinstance(element, sort_declaration))
    self.elements += [element]

  def is_alias(self, sort_expr):
    assert(isinstance(sort_expr, sort_expression))
    return any(map(lambda x: x.sort_expression == sort_expr and x.alias <> None, self.elements))

  def set_namespace(self, string):
    self.namespace = string
    if self.original_namespace == "":
      self.original_namespace = string
    for e in self.elements:
      e.set_namespace(string)

  def merge_declarations(self, declarations):
    self.elements += declarations.elements

  def defines_container(self):
    return any(map(lambda x: x.defines_container(), self.elements))

  def defines_struct(self):
    return any(map(lambda x: x.defines_struct(), self.elements))

  def to_string(self):
    s = ""
    for e in self.elements:
      s += "%s\n" % (e.to_string())
    return s

  def structured_sort_constructor_code(self):
    code = ""
    for e in self.elements:
      if e.namespace == self.namespace:
        code += "%s" % (e.structured_sort_constructor_code())
    return code

  def structured_sort_equation_code(self):
    code = ""
    for e in self.elements:
      if e.namespace == self.namespace:
        code += "%s" % (e.structured_sort_equation_code())
    return code

  def code(self, sort_spec):
    code = ""
    for e in self.elements:
      if e.namespace == self.namespace:
        code += "%s\n" % (e.code(sort_spec))
    return code

class equation_specification():
  def __init__(self, declarations):
    assert(isinstance(declarations, equation_declaration_list))
    self.declarations = declarations
    self.namespace = ""
    self.original_namespace = ""

  def set_namespace(self, string):
    self.namespace = string
    if self.original_namespace == "":
      self.original_namespace = string
    self.declarations.set_namespace(string)

  def has_lambda(self):
    return self.declarations.has_lambda()

  def has_forall(self):
    return self.declarations.has_forall()

  def has_exists(self):
    return self.declarations.has_exists()

  def to_string(self):
    return "eqn %s" % (self.declarations.to_string())

  def code(self, sort_spec, function_spec, variable_spec):
    return self.declarations.code(sort_spec, function_spec, variable_spec)

class variable_specification():
  def __init__(self, declarations):
    assert(isinstance(declarations, variable_declaration_list))
    self.declarations = declarations

  def find_variable(self, variable):
    return self.declarations.find_variable(variable)

  def to_string(self):
    return "var %s" % (self.declarations.to_string())

  def code(self, sort_spec):
    return self.declarations.code(sort_spec)

class mapping_specification():
  def __init__(self, declarations):
    assert(isinstance(declarations, function_declaration_list))
    self.declarations = declarations
    self.namespace = ""
    self.original_namespace = ""

  def set_namespace(self, string):
    self.namespace = string
    if self.original_namespace == "":
      self.original_namespace = string
    self.declarations.set_namespace(string)

  def find_function(self, function, argumentcount):
    return self.declarations.find_function(function, argumentcount)

  def to_string(self):
    return "map %s" % (self.declarations.to_string())

  def merge_specification(self, spec):
    self.declarations.merge_declarations(spec.declarations)

  def code(self, sort_spec):
    assert(isinstance(sort_spec, sort_specification))
    sort_parameters = string.join(map(lambda x: "const sort_expression& %s" % (x.to_string().lower()), self.declarations.sort_parameters(sort_spec)), ", ")
    assert(self.namespace == sort_spec.namespace)
    code = ""
    code += self.declarations.code(sort_spec)
    code += "      /// \\brief Give all system defined mappings for %s\n" % (escape(self.namespace))
    for s in self.declarations.sort_parameters(sort_spec):
      code += "      /// \\param %s A sort expression\n" % (escape(s.to_string().lower()))
    code += "      /// \\return All system defined mappings for %s\n" % (escape(self.namespace))
    code += "      inline\n"
    code += "      function_symbol_vector %s_generate_functions_code(%s)\n" % (self.namespace, sort_parameters)
    code += "      {\n"
    code += "        function_symbol_vector result;\n"
    code += self.declarations.generator_code(sort_spec)
    code += "        return result;\n"
    code += "      }\n"
    return code

class constructor_specification():
  def __init__(self, declarations):
    assert(isinstance(declarations, function_declaration_list))
    self.declarations = declarations
    self.namespace = ""
    self.original_namespace = ""

  def set_namespace(self, string):
    self.namespace = string
    if self.original_namespace == "":
      self.original_namespace = string
    self.declarations.set_namespace(string)

  def find_function(self, function, argumentcount):
    return self.declarations.find_function(function, argumentcount)

  def to_string(self):
    return "cons %s" % (self.declarations.to_string())

  def merge_specification(self, spec):
    self.declarations.merge_declarations(spec.declarations)

  def code(self, sort_spec):
    assert(isinstance(sort_spec, sort_specification))
    sort_parameters = string.join(map(lambda x: "const sort_expression& %s" % (x.to_string().lower()), self.declarations.sort_parameters(sort_spec)), ", ")
    assert(self.namespace == sort_spec.namespace)
    code  = self.declarations.code(sort_spec)
    code += "      /// \\brief Give all system defined constructors for %s\n" % (escape(self.namespace))
    for s in self.declarations.sort_parameters(sort_spec):
      code += "      /// \\param %s A sort expression\n" % (escape(s.to_string().lower()))
    code += "      /// \\return All system defined constructors for %s\n" % (escape(self.namespace))
    code += "      inline\n"
    code += "      function_symbol_vector %s_generate_constructors_code(%s)\n" % (self.namespace,sort_parameters)
    code += "      {\n"
    code += "        function_symbol_vector result;\n"
    code += "%s" % (sort_spec.structured_sort_constructor_code())
    code += "%s\n" % (self.declarations.generator_code(sort_spec, True))
    code += "        return result;\n"
    code += "      }\n"
    return code

class function_specification():
  def __init__(self, mapping_spec, constructor_spec = constructor_specification(function_declaration_list([]))):
    assert(isinstance(mapping_spec, mapping_specification))
    assert(isinstance(constructor_spec, constructor_specification))
    self.constructor_specification = constructor_spec
    self.mapping_specification = mapping_spec
    self.namespace = ""
    self.original_namespace = ""

  def merge_specification(self, spec):
    self.constructor_specification.merge_specification(spec.constructor_specification)
    self.mapping_specification.merge_specification(spec.mapping_specification)

  def set_namespace(self, string):
    self.namespace = string
    if self.original_namespace == "":
      self.original_namespace = string
    if self.constructor_specification <> None:
      self.constructor_specification.set_namespace(string)
    self.mapping_specification.set_namespace(string)

  def find_function(self, function, argumentcount):
    result = self.constructor_specification.find_function(function, argumentcount)
    if result == None:
      result = self.mapping_specification.find_function(function, argumentcount)
    return result

  def to_string(self):
    return "%s\n%s" % (self.constructor_specification.to_string(), self.mapping_specification.to_string())

  def code(self, sort_spec):
    assert(isinstance(sort_spec, sort_specification))
    # This assumes that no functions occur with the same name, but one is
    # constructor and one is mapping.
    code = ""
    code += self.constructor_specification.code(sort_spec)
    code += self.mapping_specification.code(sort_spec)
# We need deepcopy here to prevent undesired duplication
    merged_declarations = copy.deepcopy(self.constructor_specification.declarations)
    for e in self.mapping_specification.declarations.elements:
      merged_declarations.push_back(copy.deepcopy(e))
    code += merged_declarations.projection_code(sort_spec)
    return code

class sort_specification():
  def __init__(self, declarations):
    assert(isinstance(declarations, sort_declaration_list))
    self.declarations = declarations
    self.namespace = ""
    self.original_namespace = ""

  def sort_parameters(self):
    return self.declarations.sort_parameters()

  def is_alias(self, sort_expr):
    assert(isinstance(sort_expr, sort_expression))
    return self.declarations.is_alias(sort_expr)

  def merge_structured_sorts(self, function_spec):
    return self.declarations.merge_structured_sorts(function_spec)

  def set_namespace(self, string):
    self.namespace = string
    if self.original_namespace == "":
      self.original_namespace = string
    self.declarations.set_namespace(string)

  def has_sort(self, sort):
    assert(isinstance(sort, sort_expression))
    for d in self.declarations.elements:
      if d.sort_expression.to_string() == sort.to_string():
        return True
    return False

  def get_sort(self, sort):
    assert(self.has_sort(sort))
    for d in self.declarations.elements:
      if d.sort_expression.to_string() == sort.to_string():
        return d

  def defines_container(self):
    return self.declarations.defines_container()

  def defines_struct(self):
    return self.declarations.defines_struct()

  def merge_specification(self, spec):
    self.declarations.merge_declarations(spec.declarations)

  def to_string(self):
    return "sort %s" % (self.declarations.to_string())

  def structured_sort_constructor_code(self):
    return self.declarations.structured_sort_constructor_code()

  def structured_sort_equation_code(self):
    return self.declarations.structured_sort_equation_code()

  def code(self):
    return self.declarations.code(self)

class specification():
  def __init__(self, sort_spec, function_spec, variable_spec, equation_spec):
    assert(isinstance(sort_spec, sort_specification))
    assert(isinstance(function_spec, function_specification))
    assert(isinstance(variable_spec, variable_specification))
    assert(isinstance(equation_spec, equation_specification))
    self.sort_specification = sort_spec
    self.function_specification = function_spec
    self.variable_specification = variable_spec
    self.equation_specification = equation_spec
    self.includes = None
    self.namespace = ""
    self.original_namespace = ""

  def set_includes(self, includes):
    assert(isinstance(includes, include_list))
    self.includes = includes

  def has_lambda(self):
    return self.equation_specification.has_lambda()

  def has_forall(self):
    return self.equation_specification.has_forall()

  def has_exists(self):
    return self.equation_specification.has_exists()

  def defines_container(self):
    return self.sort_specification.defines_container()

  def defines_struct(self):
    return self.sort_specification.defines_struct()

  def set_namespace(self):
    s = self.sort_specification.declarations.elements[0].label.to_string()
    s = "%s" % (s)
    self.namespace = s
    if self.original_namespace == "":
      self.original_namespace = s
    self.sort_specification.set_namespace(s)
    self.function_specification.set_namespace(s)
    self.equation_specification.set_namespace(s)

  def merge_specification(self, spec):
    assert(isinstance(spec, specification))
    self.sort_specification.merge_specification(spec.sort_specification)
    self.function_specification.merge_specification(spec.function_specification)

  def to_string(self):
    s = ""
    if self.includes != None:
      s += "%s\n" % (self.includes.to_string())
    s += "%s\n" % (self.sort_specification.to_string())
    s += "%s\n" % (self.function_specification.to_string())
    s += "%s\n" % (self.variable_specification.to_string())
    s += "%s\n" % (self.equation_specification.to_string())
    return s

  def code(self):
    # Add structured sorts to constructor declarations
    self.function_specification = self.sort_specification.merge_structured_sorts(self.function_specification)
    code  = ""
    code += "// Author(s): Jeroen Keiren\n"
    code += "// Copyright: see the accompanying file COPYING or copy at\n"
    code += "// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING\n"
    code += "//\n"
    code += "// Distributed under the Boost Software License, Version 1.0.\n"
    code += "// (See accompanying file LICENSE_1_0.txt or copy at\n"
    code += "// http://www.boost.org/LICENSE_1_0.txt)\n"
    code += "//\n"
    code += "/// \\file mcrl2/data/%s.h\n" % (remove_underscore(self.namespace))
    code += "/// \\brief The standard sort %s.\n" % (self.namespace)
    code += "///\n"
    code += "/// This file was generated from the data sort specification\n"
    code += "/// mcrl2/data/build/%s.spec.\n" % (remove_underscore(self.namespace))
    code += "\n"
    code += "#ifndef MCRL2_DATA_%s_H\n" % (self.namespace.upper())
    code += "#define MCRL2_DATA_%s_H\n\n" % (self.namespace.upper())
    code += "#include \"mcrl2/data/basic_sort.h\"\n"
    code += "#include \"mcrl2/data/function_sort.h\"\n"
    code += "#include \"mcrl2/data/function_symbol.h\"\n"
    code += "#include \"mcrl2/data/application.h\"\n"
    code += "#include \"mcrl2/data/data_equation.h\"\n"
    code += "#include \"mcrl2/data/detail/container_utility.h\"\n"
    code += "#include \"mcrl2/data/standard.h\"\n"
    code += "#include \"mcrl2/data/data_specification.h\"\n"
    if self.has_lambda():
      code += "#include \"mcrl2/data/lambda.h\"\n"
    if self.has_forall():
      code += "#include \"mcrl2/data/forall.h\"\n"
    if self.has_exists():
      code += "#include \"mcrl2/data/exists.h\"\n"
    if self.defines_container():
      code += "#include \"mcrl2/data/container_sort.h\"\n"
    if self.defines_struct():
      code += "#include \"mcrl2/data/structured_sort.h\"\n"
    if self.includes != None:
      code += self.includes.code()
    code += "\n"
    code += "namespace mcrl2 {\n\n"
    code += "  namespace data {\n\n"
    code += "    /// \\brief Namespace for system defined sort %s\n" % (escape(self.namespace))
    code += "    namespace sort_%s {\n\n" % (self.namespace)
    code += self.sort_specification.code()
    code += self.function_specification.code(self.sort_specification)
    code += self.equation_specification.code(self.sort_specification, self.function_specification, self.variable_specification)
    code += "\n"
    dependent_sorts = set([])
    auxiliary_sorts = set([])
    for e in self.sort_specification.declarations.elements:
      if e.original_namespace <> self.namespace:
        if e.defines_container():
          result  = "         if (specification.constructors(sort_%s::%s(element)).empty())\n" % (e.original_namespace, e.original_namespace)
          result += "         {\n"
          result += "           sort_%s::add_%s_to_specification(specification, element);\n" % (e.original_namespace, e.original_namespace)
          result += "         }\n"
          dependent_sorts.add(result)
        else:
          result  = "         if (specification.constructors(sort_%s::%s()).empty())\n" % (e.original_namespace, e.original_namespace)
          result += "         {\n"
          result += "           sort_%s::add_%s_to_specification(specification);\n" % (e.original_namespace, e.original_namespace)
          result += "         }\n"
          dependent_sorts.add(result)
      if e.to_string()[0] == '@' and e.original_namespace == self.namespace:
        auxiliary_sorts.add("         specification.add_system_defined_sort(%s);\n" % (e.inline_code(self.sort_specification)))
    if self.defines_container():
      code += "      /// \\brief Add sort, constructors, mappings and equations for %s\n" % (escape(self.namespace))
      code += "      /// \\param specification a specification\n"
      code += "      /// \\param element the sort of elements stored by the container\n"
      code += "      inline\n"
      code += "      void add_%s_to_specification(data_specification& specification, sort_expression const& element)\n" % (self.namespace)
      code += "      {\n"
      code += string.join(dependent_sorts, "")
      code += string.join(auxiliary_sorts, "")
      code += "         specification.add_system_defined_sort(%s(element));\n" % (escape(self.namespace))
      code += "         specification.add_system_defined_constructors(%s_generate_constructors_code(element));\n" % (self.namespace)
      code += "         specification.add_system_defined_mappings(%s_generate_functions_code(element));\n" % (self.namespace)
      code += "         specification.add_system_defined_equations(%s_generate_equations_code(element));\n" % (self.namespace)
      code += "      }\n"
    else:
      code += "      /// \\brief Add sort, constructors, mappings and equations for %s\n" % (escape(self.namespace))
      code += "      /// \\param specification a specification\n"
      code += "      inline\n"
      code += "      void add_%s_to_specification(data_specification& specification)\n" % (self.namespace)
      code += "      {\n"
      code += string.join(dependent_sorts, "")
      code += string.join(auxiliary_sorts, "")
      code += "         specification.add_system_defined_sort(%s());\n" % (escape(self.namespace))
      code += "         specification.add_system_defined_constructors(%s_generate_constructors_code());\n" % (self.namespace)
      code += "         specification.add_system_defined_mappings(%s_generate_functions_code());\n" % (self.namespace)
      code += "         specification.add_system_defined_equations(%s_generate_equations_code());\n" % (self.namespace)
      code += "      }\n"
    code += "    } // namespace sort_%s\n\n" % (self.namespace)
    code += "  } // namespace data\n\n"
    code += "} // namespace mcrl2\n\n"
    code += "#endif // MCRL2_DATA_%s_H\n" % (self.namespace.upper())
    code = string.replace(code, "__", "_")
    p = re.compile('sort_([A-Za-z0-9]*)_([ ]|:)')
    code = p.sub(r'sort_\1\2', code)
    p = re.compile('is_([A-Za-z0-9]*)_\(')
    code = p.sub(r'is_\1(',code)
    
    return code

class include_list():
  def __init__(self, elements):
    assert(all(map(lambda x: isinstance(x, include), elements)))
    self.elements = elements

  def push_back(self, element):
    assert(isinstance(element, include))
    self.elements += [element]

  def to_string(self):
    s = ""
    for e in elements:
      s += "%s\n" % (e.to_string())
    return s

  def code(self):
    code = ""
    for e in elements:
      s += "%s\n" % (e.code())
    return s

class include():
  def __init__(self, id):
    assert(isinstance(id, identifier))
    self.identifier = id

  def to_string(self):
    return "#include %s" % (self.identifier.to_string())

  def code(self):
    s = self.identifier.to_string()
    s = s[:len(s)-5]
    return "#include \"mcrl2/data/%s.h\"" % (s)

class include_list():
  def __init__(self, elements):
    assert(all(map(lambda x: isinstance(x, include), elements)))
    self.elements = elements

  def push_back(self, element):
    assert(isinstance(element, include))
    self.elements += [element]

  def to_string(self):
    s = ""
    for e in self.elements:
      s += "%s\n" % (e.to_string())
    return s

  def code(self):
    code = ""
    for e in self.elements:
      code += "%s\n" % (e.code())
    return code

