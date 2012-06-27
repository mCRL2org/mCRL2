# Classes for data expressions and sort expressions, used for code generation
# Basically reflects the parse tree of a specification, but allows for more
# flexibility.

import string
import copy
import re

# Remove trailing _ from a string
def remove_underscore(s):
  if s.endswith("_"):
    return s[:-1]
  else:
    return s

# Escape an initial @ sign is present. Needed for doxygen code extraction
def escape(x):
  s = "{0}".format(x)
  if s.startswith("@"):
    return "\\{0}".format(s)
  else:
    return s

def union_by_string(s,t):
  result = set()
  for e in s:
    if str(e) not in map(lambda x: str(x), result):
      result |= set([e])
  for e in t:
    if str(e) not in map(lambda x: str(x), result):
      result |= set([e])
  result = [i for i in result]
  result.sort()
  return result

def difference_by_string(s,t):
  result = set([])
  for e in s:
    if str(e) not in map(lambda x: str(x), t):
      result |= set([e])
  result = [i for i in result]
  result.sort()
  return result

def is_standard_function(s):
  standard_ids = ["equal_to", "not_equal_to", "if_", "less", "less_equal", "greater", "greater_equal"]  
  return s in standard_ids

def target_sort(sort_expr):
  if isinstance(sort_expr, sort_arrow):
    return sort_expr.codomain
  else:
    return sort_expr

class identifier():
  def __init__(self, string):
    self.string = string
    
  def __str__(self):
    return self.string
    
class label():
  def __init__(self, id):
    assert(isinstance(id, identifier))
    self.identifier = id

  def __str__(self):
    return self.identifier.__str__()

class variable_declaration():
  def __init__(self, id, sort_expr):
    assert(isinstance(id, identifier))
    assert(isinstance(sort_expr, sort_expression))
    self.id = id
    self.sort_expression = sort_expr

  def __str__(self):
    return "{0} : {1}".format(self.id, self.sort_expression)
    
  def sort_parameters(self, spec):
    return self.sort_expression.sort_parameters(spec)

  def code(self, spec):
    return "        variable v{0}(\"{0}\",{1})".format(self.id, self.sort_expression.code(spec))

class variable_declaration_list():
  def __init__(self, elements):
    assert(all(map(lambda x: isinstance(x, variable_declaration), elements)))
    self.elements = elements
    
  def __str__(self):
    return ", ".join([str(e) for e in self.elements])
    
  def push_back(self, element):
    assert(isinstance(element, variable_declaration))
    if str(element) not in map(lambda x: str(x), self.elements):
      self.elements += [element]

  def find_variable(self, variable):
    assert(isinstance(variable, data_variable))
    for e in self.elements:
      if e.id.__str__() == variable.id.__str__():
        return e
    
  def code(self, spec):
    code = ""
    for e in self.elements:
      code += "%s;\n" % (e.code(spec))
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
    
  def __cmp__(self, other):
    if not other:
      return False
  
    return self.id == other.id and self.label == other.label and self.namespace == other.namespace and self.original_namespace == other.original_namespace 

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

  def sort_parameters(self, spec):
    return self.sort_expression.sort_parameters(spec)

  def __str__(self):
    return "{0} : {1}".format(self.id, self.sort_expression)

  def generator_code(self, spec, function_declarations):
    if self.namespace == self.original_namespace and self.namespace == function_declarations.namespace and self.namespace == spec.namespace:
      sort_params = self.sort_expression.sort_parameters(spec)
      # Determine whether this function is overloaded
      functions = filter(lambda x: (x.id.__str__() == self.id.__str__()) and (x.label.__str__() == self.label.__str__()) and (x.namespace == self.namespace), function_declarations.elements)
      extra_parameters = ""
      if len(functions) > 1:
        try:
          extra_parameters = self.sort_expression.domain.code(spec)
        except:
          pass # in case sort_expression has no domain
      if len(sort_params) == 0:
        return "        result.push_back(%s(%s));\n" % (self.label, extra_parameters)
      else:
        sort_parameters = []
        for s in sort_params:
          sort_parameters += [s.code(spec)]
        if extra_parameters <> "":
          extra_parameters = ", %s" % (extra_parameters)
        return "        result.push_back(%s(%s%s));\n" % (self.label, string.join(sort_parameters, ", "), extra_parameters)
    else:
      return ""

class function_declaration_list():
  def __init__(self, elements):
    assert(all(map(lambda x: isinstance(x, function_declaration), elements)))
    self.elements = elements
    self.namespace = ""
    self.original_namespace = ""

  def empty(self):
    return len(self.elements) == 0

  def push_back(self, element):
    assert(isinstance(element, function_declaration))
    self.elements += [element]

  def set_namespace(self, string):
    self.namespace = string
    if self.original_namespace == "":
      self.original_namespace = string
    for e in self.elements:
      e.set_namespace(string)

  def sort_parameters(self, spec):
    result = set()
    for e in self.elements:
      result = union_by_string(result, e.sort_parameters(spec))
    return result

  def find_function(self, function, argumentcount):
    if function.id.__str__() == "==":
      return function_declaration(function.id, sort_expression(), label(identifier("equal_to")))
    elif function.id.__str__() == "!=":
      return function_declaration(function.id, sort_expression(), label(identifier("not_equal_to")))
    elif function.id.__str__() == "if":
      return function_declaration(function.id, sort_expression(), label(identifier("if_")))
    elif function.id.__str__() == "<":
      return function_declaration(function.id, sort_expression(), label(identifier("less")))
    elif function.id.__str__() == ">":
      return function_declaration(function.id, sort_expression(), label(identifier("greater")))
    elif function.id.__str__() == "<=":
      return function_declaration(function.id, sort_expression(), label(identifier("less_equal")))
    elif function.id.__str__() == ">=":
      return function_declaration(function.id, sort_expression(), label(identifier("greater_equal")))
    else:
      for e in self.elements:
        if e.id.__str__() == function.id.__str__():
          if argumentcount == -1:
            return e
          elif argumentcount == 0 and not isinstance(e.sort_expression, sort_arrow):
            return e
          elif isinstance(e.sort_expression, sort_arrow) and len(e.sort_expression.domain.elements) == argumentcount:
            return e
    return None

  def merge_declarations(self, declarations, is_supertype):
    for d in declarations.elements:
      if is_supertype and any(map(lambda x: (d.id.__str__() == x.id.__str__()) and (d.label.__str__() == x.label.__str__()) and (x.namespace == self.namespace), self.elements)):
        d.set_namespace(self.namespace)
      self.elements.append(d)

  def __str__(self):
    return "".join(["{0}\n".format(e) for e in self.elements])

  def generator_code(self, spec, is_constructor_declaration = False):
    code = ""
    for e in self.elements:
      if not is_constructor_declaration or not spec.sort_specification.is_alias(target_sort(e.sort_expression)):
        code += e.generator_code(spec, self)
    return code

  def projection_code(self, spec):
    projection_arguments = {}
    for e in self.elements:
      if (e.namespace == e.original_namespace and e.namespace == spec.namespace):
        projection_arguments = e.projection_arguments(projection_arguments)
    code = ""
    for p in projection_arguments:
      # Filter duplicates
      case_arguments = projection_arguments[p]
      unique_case_arguments = []
      unique_case_arguments_string = []
      for (id, label, idx) in case_arguments:
        if not (id.__str__(), label.__str__(), idx) in unique_case_arguments_string:
          unique_case_arguments += [(id, label, idx)]
          unique_case_arguments_string += [(id.__str__(), label.__str__(), idx)]

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
        assertions.append("is_%s_application(e)" % (label.__str__()))
      case_code += "        assert(%s);\n" % (string.join(assertions, " || "))

      # First group by index
      index_table = {}
      for (id, label, idx) in unique_case_arguments:
        try:
          index_table[idx].append((id, label))
        except:
          index_table[idx] = [(id, label)]
      if len(index_table) == 1:
        case_code += "        return *boost::next(static_cast< application >(e).arguments().begin(), %s);\n" % (index_table.keys()[0])
      else:
        for i in index_table:
          clauses = []
          for c in index_table[i]:
            clauses.append("is_%s_application(e)" % (c[1]))
          case_code += "        if (%s)\n" % (string.join(clauses, " || "))
          case_code += "        {\n"
          case_code += "          return *boost::next(static_cast< application >(e).arguments().begin(), %s);\n" % (i)
          case_code += "        }\n"
        case_code += "        throw mcrl2::runtime_error(\"Unexpected expression occurred\");\n"
      case_code += "      }\n"
      code += "%s\n" % (case_code)
    return code

  def code(self, spec, is_constructor_declaration = False):
    assert(isinstance(spec, specification))
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

      def __str__(self):
        return "{0} : { {1} }".format(self.id, self.sort_expression_list)

      def function_name(self, fullname, name):
        code  = ""
        code += "      /// \\brief Generate identifier %s\n" % (escape(fullname))
        code += "      /// \\return Identifier %s\n" % (escape(fullname))
        code += "      inline\n"
        code += "      core::identifier_string const& %s_name()\n" % (name)
        code += "      {\n"
        code += "        static core::identifier_string %s_name = core::detail::initialise_static_expression(%s_name, core::identifier_string(\"%s\"));\n" % (name, name, fullname)
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
          code += "        static function_symbol %s = core::detail::initialise_static_expression(%s, function_symbol(%s_name(), %s));\n" % (name, name, name, sort)
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

      def function_recogniser(self, fullname, name, sortparams):
        code  = ""
        code += "      /// \\brief Recogniser for function %s\n" % (escape(fullname))
        code += "      /// \\param e A data expression\n"
        code += "      /// \\return true iff e is the function symbol matching %s\n" % (escape(fullname))
        code += "      inline\n"
        code += "      bool is_%s_function_symbol(const atermpp::aterm_appl& e)\n" % (name)
        code += "      {\n"
        code += "        if (is_function_symbol(e))\n"
        code += "        {\n"
        sortparams_list = string.split(sortparams, ", ")
        if sortparams_list == ['']:
          code += "          return function_symbol(e) == %s();\n" % (name)
        else:
          # TODO, make something stronger here!
          code += "          return function_symbol(e).name() == %s_name();\n" % (name)
        code += "        }\n"
        code += "        return false;\n"
        code += "      }\n"
        return code

      def polymorphic_function_recogniser(self, fullname, name, sortparams):
        code  = ""
        code += "      /// \\brief Recogniser for function %s\n" % (escape(fullname))
        code += "      /// \\param e A data expression\n"
        code += "      /// \\return true iff e is the function symbol matching %s\n" % (escape(fullname))
        code += "      inline\n"
        code += "      bool is_%s_function_symbol(const atermpp::aterm_appl& e)\n" % (name)
        code += "      {\n"
        code += "        if (is_function_symbol(e))\n"
        code += "        {\n"
        code += "          function_symbol f(e);\n"
        sortparams_list = string.split(sortparams, ", ")
        if sortparams_list == ['']:
          domain_size = len(self.sort_expression_list.elements[0].domain.elements)
          cases = []
          for s in self.sort_expression_list.elements:
            d = s.domain
            assert(len(d.elements) == domain_size)
            cases.append("f == %s(%s)" % (name, d.code(spec)))
          code += "          return f.name() == %s_name() && function_sort(f.sort()).domain().size() == %d && (%s);\n" % (name, domain_size, string.join(cases, " || "))
        else:
          # TODO, make something stronger here!
          code += "          return f.name() == %s_name();\n" % (name)
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
        code += "        return %s(%s)(%s);\n" % (name, actsortparams, actparams)
        code += "      }\n"
        return code

      def function_application_recogniser(self, fullname, name):
        code  = ""
        code += "      /// \\brief Recogniser for application of %s\n" % (escape(fullname))
        code += "      /// \\param e A data expression\n"
        code += "      /// \\return true iff e is an application of function symbol %s to a\n" % (escape(name))
        code += "      ///     number of arguments\n"
        code += "      inline\n"
        code += "      bool is_%s_application(const atermpp::aterm_appl& e)\n" % (name)
        code += "      {\n"
        code += "        if (is_application(e))\n"
        code += "        {\n"
        code += "          return is_%s_function_symbol(application(e).head());\n" % (name)
        code += "        }\n"
        code += "        return false;\n"
        code += "      }\n"
        return code

      def function_application_code(self, sort, polymorphic = False):
        code = ""
        comma = ""
        if self.sort_expression_list.formal_parameters_code(spec) <> "":
          comma = ", "
        if isinstance(sort, sort_arrow):
          act_sort_params = []
          if polymorphic:
            params = string.split(sort.domain.actual_parameters_code(spec), ", ")
            for p in params:
              act_sort_params += ["%s.sort()" % (p)]
          if self.sort_expression_list.actual_parameters_code(spec) <> "":
            act_sort_params = [self.sort_expression_list.actual_parameters_code(spec)] + act_sort_params

          code += self.function_application(self.id.__str__(), self.label.__str__(), self.sort_expression_list.formal_parameters_code(spec), comma, sort.domain.formal_parameters_code(spec), string.join(act_sort_params, ", "), sort.domain.actual_parameters_code(spec))
          code += "\n"
          code += self.function_application_recogniser(self.id.__str__(), self.label.__str__())
          code += "\n"
        return code

      def code(self, spec):
        assert(isinstance(spec, specification))
                
        if self.namespace <> spec.namespace :
          return ""

        code = ""
        if len(self.sort_expression_list.elements) == 1:
          sort = self.sort_expression_list.elements[0] # as len is 1
          code += self.function_constructor(self.id.__str__(), self.label.__str__(), self.sort_expression_list.formal_parameters_code(spec), sort.code(spec))
          code += "\n"
          code += self.function_recogniser(self.id.__str__(), self.label.__str__(), self.sort_expression_list.formal_parameters_code(spec))
          code += "\n"
          code += self.function_application_code(sort)
        else:
          # There is polymorphism in play, hence we need to compute the
          # allowed sorts first.
          # That is, we have parameters for all domain elements
          # bases on those, we compute the corresponding target sort
          new_domain_sorts = [sort_identifier(identifier("s%s" % i)) for i in range(len(self.sort_expression_list.elements[0].domain.elements))]

          first_codomain = self.sort_expression_list.elements[0].codomain
          if all(map(lambda x: str(x.codomain) == str(first_codomain), self.sort_expression_list.elements)):
            target_sort = "sort_expression target_sort(%s);\n" % (first_codomain.code(spec))
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
                target_sort += "%s == %s" % (new_domain_sorts[j].code(spec), domain_element.code(spec))
              target_sort += ")\n"
              target_sort += "        {\n"
              target_sort += "          target_sort = %s;\n" % (sort.codomain.code(spec))
              target_sort += "        }\n"
            target_sort += "        else\n"
            target_sort += "        {\n"
            target_sort += "          throw mcrl2::runtime_error(\"cannot compute target sort for %s with domain sorts \" + %s);\n" % (self.label.__str__(), string.join([("%s.to_string()" % new_domain_sorts[j].code(spec)) for j in range(len(sort.domain.elements))], " + \", \" + "))
            target_sort += "        }\n"
          target_sort_id = sort_identifier(identifier("target_sort"))
          new_sort = sort_arrow(domain(False, new_domain_sorts), target_sort_id)

          domain_param_code = map(lambda x: "const sort_expression& %s" % (x.code(spec)), new_domain_sorts)

          comma = ""
          if self.sort_expression_list.formal_parameters_code(spec) <> "":
            comma = ", "
          code += self.polymorphic_function_constructor(self.id.__str__(), self.label.__str__(), self.sort_expression_list.formal_parameters_code(spec), comma, string.join(domain_param_code, ", "), target_sort, new_sort.code(spec))
          code += "\n"
          code += self.polymorphic_function_recogniser(self.id.__str__(), self.label.__str__(), self.sort_expression_list.formal_parameters_code(spec))
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
        for e in self.elements:
          if (e.id.__str__() == element.id.__str__()) and (e.label.__str__() == element.label.__str__()):
            if all(map(lambda x: str(x) <> str(element.sort_expression), e.sort_expression_list.elements)):
              e.sort_expression_list.push_back(element.sort_expression)
            return
        
        self.elements += [multi_function_declaration(element.id, element.namespace, sort_expression_list([element.sort_expression]), element.label)]

      def __str__(self):
        return "".join(["{0}\n".format(e) for e in self.elements])

      def code(self, spec):
        assert(isinstance(spec, specification))
        code = ""
        for e in self.elements:
          code += "%s" % (e.code(spec))
        return code

    # Add all the elements to a multi_function_declaration_list,
    # and let it do the code generation
    multi_function_declarations = multi_function_declaration_list([])
    for e in self.elements:
      if (e.namespace == self.namespace):
        multi_function_declarations.push_back_function_declaration(e)

    code = multi_function_declarations.code(spec)
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

  def sort_parameters(self, spec, function_spec, variable_spec):
    result = self.lhs.sort_parameters(spec, function_spec, variable_spec)
    result = union_by_string(result, self.rhs.sort_parameters(spec, function_spec, variable_spec))
    if self.condition != None:
      result = union_by_string(result, self.condition.sort_parameters(spec, function_spec, variable_spec))
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

  def __str__(self):
    if self.condition:
      return "{0} = {1}".format(self.lhs, self.rhs)
    else:
      return "{0} -> {1} = {2}".format(self.condition, self.lhs, self.rhs)

  def determinise_variable_or_function_symbol(self, function_spec, variable_spec):
    condition = None
    if self.condition <> None:
      condition = self.condition.determinise_variable_or_function_symbol(function_spec, variable_spec)
    lhs = self.lhs.determinise_variable_or_function_symbol(function_spec, variable_spec)
    rhs = self.rhs.determinise_variable_or_function_symbol(function_spec, variable_spec)

    return equation_declaration(lhs, rhs, condition)

  def code(self, spec, function_spec, variable_spec):
    variables = self.lhs.free_variables()
    variables = union_by_string(variables, self.rhs.free_variables())
    if self.condition <> None:
      variables = union_by_string(variables, self.condition.free_variables())

    variables_code = []
    for v in variables:
      variables_code += [v.code(spec, function_spec, variable_spec)]
    if len(variables_code) == 0:
      variables_string = "variable_list()"
    else:
      variables_string = "atermpp::make_vector(%s)" % (string.join(sorted(variables_code), ", "))

    if self.condition == None:
      code = "data_equation(%s, %s, %s)" % (variables_string, self.lhs.code(spec, function_spec, variable_spec), self.rhs.code(spec, function_spec, variable_spec))
    else:
      code = "data_equation(%s, %s, %s, %s)" % (variables_string, self.condition.code(spec, function_spec, variable_spec), self.lhs.code(spec, function_spec, variable_spec), self.rhs.code(spec, function_spec, variable_spec))

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

  def sort_parameters(self, spec, function_spec, variable_spec):
    result = set()
    for e in self.elements:
      result = union_by_string(result, e.sort_parameters(spec, function_spec, variable_spec))
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

  def __str__(self):
    return "".join(["{0}\n".format(e) for e in self.elements])

  def code(self, spec, function_spec, variable_spec):
    self = self.determinise_variable_or_function_symbol(function_spec, variable_spec)
    
    sort_parameters = self.sort_parameters(spec, function_spec, variable_spec)
    formal_parameters_code = []
    for s in sort_parameters:
      formal_parameters_code += [s.formal_parameter_code()]
    code  = "      /// \\brief Give all system defined equations for %s\n" % (escape(spec.get_namespace()))
    for s in sort_parameters:
      code += "      /// \\param %s A sort expression\n" % (escape(s.code(spec)))
    code += "      /// \\return All system defined equations for sort %s\n" % (escape(spec.get_namespace()))
    code += "      inline\n"
    code += "      data_equation_vector %s_generate_equations_code(%s)\n" % (spec.get_namespace(), string.join(formal_parameters_code, ", "))
    code += "      {\n"
    code += "%s\n" % (variable_spec.code(spec))
    code += "        data_equation_vector result;\n"
    code += "%s" % (spec.sort_specification.structured_sort_equation_code())
    for e in self.elements:
      code += "        %s\n" % (e.code(spec, function_spec, variable_spec))
    code += "        return result;\n"
    code += "      }\n"

    return code


class data_expression():
  def __init__(self):
    pass

  def __str__(self):
    raise Exception("String operation for data_expression is not implemented")

class data_application(data_expression):
  def __init__(self, head, arguments):
    assert(isinstance(head, data_expression))
    assert(isinstance(arguments, data_expression_list))
    self.head = head
    self.arguments = arguments

  def sort_parameters(self, spec, function_spec, variable_spec):
    result = self.head.sort_parameters(spec, function_spec, variable_spec)
    result = union_by_string(result, self.arguments.sort_parameters(spec, function_spec, variable_spec))
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

  def __str__(self):
    return "{0}({1})".format(self.head, self.arguments)

  def code(self, spec, function_spec, variable_spec):
    if isinstance(self.head, function_symbol):
      # FIXME: Extremely ugly workaround for set complement
      if self.head.id.__str__() == "!" and len(self.arguments.elements) == 1 and isinstance(self.arguments.elements[0], data_application) and isinstance(self.arguments.elements[0].head, data_variable):
        head_code = "sort_bool_::not_()"
      # FIXME: Extremely ugly workaround for bag join
      elif self.head.id.__str__() == "+" and len(self.arguments.elements) == 2 and isinstance(self.arguments.elements[0], data_application) and isinstance(self.arguments.elements[0].head, data_variable):
        head_code = "sort_nat::plus()"
      else:
        head_code = self.head.code(spec, function_spec, variable_spec, len(self.arguments.elements))
    else:
      head_code = self.head.code(spec, function_spec, variable_spec)
    # make generated code a bit more readable 
    if isinstance(self.head, function_symbol):
      head_code = head_code[:head_code.find("(")]
    sort_parameters_code = []
      # FIXME: Extremely ugly workaround for set complement / bag joint
    if isinstance(self.head, function_symbol) and (head_code <> "sort_bool_::not_" and head_code <> "sort_nat::plus"):
      sort_parameters = self.sort_parameters(spec, function_spec, variable_spec)
      for s in sort_parameters:
        sort_parameters_code += [s.code(spec)]
    sort_parameters_code += [self.arguments.code(spec, function_spec, variable_spec)]

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

  def __str__(self):
    return str(self.id)

  def determinise_variable_or_function_symbol(self, function_spec, variable_spec):
    result = function_spec.find_function(self, -1)
    if result:
      return function_symbol(self.id)
    else:
      return data_variable(self.id)

class data_variable(data_expression):
  def __init__(self, id):
    assert(isinstance(id, identifier))
    self.id = id

  def sort_parameters(self, spec, function_spec, variable_spec):
    variable = variable_spec.find_variable(self)
    if variable == None:
      print "%s is not a variable" % self.id.__str__()
      return []

    return variable.sort_expression.sort_parameters(spec)

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

  def __str__(self):
    return str(self.id)

  def code(self, spec, function_spec, variable_spec):
    return "v{0}".format(self.id)

class function_symbol(data_expression):
  def __init__(self, id):
    assert(isinstance(id, identifier))
    self.id = id

  def sort_parameters(self, spec, function_spec, variable_spec):
    return function_spec.find_function(self, -1).sort_expression.sort_parameters(spec)

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

  def __str__(self):
    return str(self.id)

  def code(self, spec, function_spec, variable_spec, argumentcount = -1):
    f = function_spec.find_function(self, argumentcount)
    sort_parameters_code = []
    for s in f.sort_parameters(spec):
      sort_parameters_code += [s.code(spec)]
    if f.namespace == function_spec.namespace or is_standard_function(f.label.__str__()) or f.namespace == "undefined":
      return "%s(%s)" % (f.label.__str__(), string.join(sort_parameters_code, ", "))
    else:
      return "sort_%s::%s(%s)" % (f.namespace, f.label.__str__(), string.join(sort_parameters_code, ", "))

class lambda_abstraction(data_expression):
  def __init__(self, var_declaration, expression):
    assert(isinstance(var_declaration, variable_declaration))
    assert(isinstance(expression, data_expression))
    self.variable_declaration = var_declaration
    self.expression = expression

  def sort_parameters(self, spec, function_spec, variable_spec):
    result = self.variable_declaration.sort_parameters(spec)
    result = union_by_string(result, self.expression.sort_parameters(spec, function_spec, variable_spec))
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

  def __str__(self):
    return "lambda({0}, {1})".format(self.variable_declaration, self.expression)

  def code(self, spec, function_spec, variable_spec):
    return "lambda(atermpp::make_vector(%s), %s)" % (data_variable(self.variable_declaration.id).code(spec, function_spec, variable_spec), self.expression.code(spec, function_spec, variable_spec))

class forall(data_expression):
  def __init__(self, var_declaration, expression):
    assert(isinstance(var_declaration, variable_declaration))
    assert(isinstance(expression, data_expression))
    self.variable_declaration = var_declaration
    self.expression = expression
    
  def sort_parameters(self, spec, function_spec, variable_spec):
    result = self.variable_declaration.sort_parameters(spec)
    result = union_by_string(result, self.expression.sort_parameters(spec, function_spec, variable_spec))
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

  def __str__(self):
    return "forall({0}, {1})".format(self.variable_declaration, self.expression)

  def code(self, spec, function_spec, variable_spec):
    return "forall(atermpp::make_vector(%s), %s)" % (data_variable(self.variable_declaration.id).code(spec, function_spec, variable_spec), self.expression.code(spec, function_spec, variable_spec))

class exists(data_expression):
  def __init__(self, var_declaration, expression):
    assert(isinstance(var_declaration, variable_declaration))
    assert(isinstance(expression, data_expression))
    self.variable_declaration = var_declaration
    self.expression = expression

  def sort_parameters(self, spec, function_spec, variable_spec):
    result = self.variable_declaration.sort_parameters(spec)
    result = union_by_string(result, self.expression.sort_parameters(spec, function_spec, variable_spec))
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

  def __str__(self):
    return "exists({0}, {1})".format(self.variable_declaration, self.expression)

  def code(self, spec, function_spec, variable_spec):
    return "exists(atermpp::make_vector(%s), %s)" % (data_variable(self.variable_declaration.id).code(spec, function_spec, variable_spec), self.expression.code(spec, function_spec, variable_spec))

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

  def sort_parameters(self, spec, function_spec, variable_spec):
    result = set()
    for e in self.elements:
      union_by_string(result, e.sort_parameters(spec, function_spec, variable_spec))
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

  def __str__(self):
    return ", ".join(self.elements)

  def code(self, spec, function_spec, variable_spec):
    code = []
    for e in self.elements:
      code += [e.code(spec, function_spec, variable_spec)]
    return string.join(code, ", ")

class sort_expression_list():
  def __init__(self, elements):
    assert(all(map(lambda x: isinstance(x, sort_expression), elements)))
    self.elements = elements

  def push_back(self, element):
    assert(isinstance(element, sort_expression))
    self.elements += [element]

  def sort_parameters(self, spec):
    result = set()
    for e in self.elements:
      result = union_by_string(result, e.sort_parameters(spec))
    return result

  def formal_parameters_code(self, spec):
    code = ""
    for p in self.sort_parameters(spec):
      if code != "":
        code += ", "
      code += "%s" % (p.formal_parameter_code())
    return code

  def actual_parameters_code(self, spec):
    code = ""
    for p in self.sort_parameters(spec):
      if code != "":
        code += ", "
      code += "%s" % (p.actual_parameter_code())
    return code

  def __str__(self):
    return ", ".join(self.elements)

  def code(self):
    s = "atermpp::make_vector("
    args = []
    for e in self.elements:
      args += [e.code()]
    s += string.join(args, ", ")
    s += ")"
    return s

class sort_expression():
  def __init__(self):
    pass

  def sort_parameters(self, spec):
    return set()

class sort_identifier(sort_expression):
  def __init__(self, id):
    assert(isinstance(id, identifier))
    self.name = id
    
  def sort_parameters(self, spec):
    if any(map(lambda x: str(x) == str(self), spec.sort_parameters())):
      return set([self])
    return set()

  def __str__(self):
    return str(self.name)

  def formal_parameter_code(self):
    return "const sort_expression& {0}".format(self.name).lower()

  def actual_parameter_code(self):
    return self.name.__str__().lower()

  def code(self, spec):
    assert(isinstance(spec, specification))
    if (spec.sort_specification.has_sort(self)):
      return spec.sort_specification.get_sort(self).inline_code(spec)
    else:
      return "{0}".format(self.name).lower()

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
        arguments += [(e[1].__str__(), i)]
      return arguments

  def sort_parameters(self, spec):
    result = set()
    for e in self.elements:
      if self.labelled:
        result = union_by_string(result, e[0].sort_parameters(spec))
      else:
        result = union_by_string(result, e.sort_parameters(spec))
    return result

  def formal_parameters_code(self, spec):
    index = 0
    parameters = []
    for e in self.elements:
      parameters += ["const data_expression& arg%s" % (index)]
      index += 1
    return string.join(parameters, ", ")

  def actual_parameters_code(self, spec):
    index = 0
    parameters = []
    for e in self.elements:
      parameters += ["arg%d" % (index)]
      index += 1
    return string.join(parameters, ", ")

  def assertions_code(self):
    # TODO, find a nice and suitable set of assertions here
    return ""

  def __str__(self):
    if self.labelled:
      return " # ".join(["{0} <\"{1}\">".format(e[0], e[1]) for e in self.elements])
    else:
      return " # ".join(map(str, self.elements))
  
  def code(self, spec):
    assert(isinstance(spec, specification))
    code = ""
    for e in self.elements:
      if code != "":
        code += ", "
      if self.labelled:
        code += e[0].code(spec)
      else:
        code += e.code(spec)
    return code

class sort_arrow(sort_expression):
  def __init__(self, dom, codom):
    assert(isinstance(dom, domain))
    assert(isinstance(codom, sort_expression))
    self.domain = dom
    self.codomain = codom
    
  def sort_parameters(self, spec):
    result = self.codomain.sort_parameters(spec)
    result = union_by_string(result, self.domain.sort_parameters(spec))
    return result

  def __str__(self):
    return "{0} -> {1}".format(self.domain, self.codomain)

  def code(self, spec):
    assert(isinstance(spec, specification))
    return "make_function_sort(%s, %s)" % (self.domain.code(spec), self.codomain.code(spec))

class sort_container(sort_expression):
  def __init__(self, container, element_sort):
    assert(isinstance(container, identifier))
    assert(isinstance(element_sort, sort_expression))
    self.container = container
    self.element_sort = element_sort
    
  def sort_parameters(self, spec):
    return self.element_sort.sort_parameters(spec)

  def __str__(self):
    return "{0}({1})".format(self.container, self.element_sort)

  def code(self, spec):
    assert(isinstance(spec, specification))
    if (spec.sort_specification.has_sort(self)):
      sort = spec.sort_specification.get_sort(self)
      result = "%s(%s)" % (sort.label.__str__(), self.element_sort.code(spec))
      if sort.original_namespace <> spec.namespace and sort.original_namespace <> "undefined":
        result = "sort_%s::%s" % (sort.original_namespace, result)
      return result
    else:
      return "%s(%s)" % (str(self.container).lower(), self.element_sort.code(spec))

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

  def __str__(self):
    if self.arguments:
      return "{0}({1})".format(self.id, self.arguments)
    else:
      return str(self.id)

  def struct_constructor_arguments(self, spec):
    assert self.arguments <> None
    code = []
    for a in self.arguments.elements:
      code += ["structured_sort_constructor_argument(\"%s\", %s)" % (a[1], a[0].code(spec))]
    return "atermpp::make_vector(%s)" % (string.join(code, ", "))

  def code(self, spec):
    if self.arguments == None:
      return "structured_sort_constructor(\"%s\", \"%s\")" % (self.id.__str__(), self.label.__str__())
    else:
      return "structured_sort_constructor(\"%s\", %s, \"%s\")" % (self.id.__str__(), self.struct_constructor_arguments(spec), self.label.__str__())

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

  def __str__(self):
    return " | ".join(map(str, self.elements))

  def code(self, spec):
    code = ""
    for e in self.elements:
      code += "          constructors.push_back(%s);\n" % (e.code(spec))
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

  def __str__(self):
    return "struct {0}".format(self.elements)

  def code(self, spec):
    return self.elements.code(spec)

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

  def __str__(self):
    if self.alias:
      return "{0} = {1}".format(self.sort_expression, self.alias)
    else:
      return str(self.sort_expression)

  def inline_code(self, spec):
    if (self.namespace == spec.namespace) or (self.namespace == "") or self.namespace == "undefined":
      return "%s()" % (self.label)
    else:
      return "sort_%s::%s()" % (self.namespace, self.label)

  def structured_sort_constructor_code(self):
    if self.alias == None:
      return ""
    else:
      param = ""
      if isinstance(self.sort_expression, sort_container):
        param = str(self.sort_expression.element_sort).lower()
      sort = "%s(%s)" % (self.label.__str__(), param)
      result = "        function_symbol_vector %s_constructors = detail::%s_struct(%s).constructor_functions(%s);\n" % (self.label.__str__(), self.label.__str__(), param, sort)
      return result + "        result.insert(result.end(), %s_constructors.begin(), %s_constructors.end());\n" % (self.label.__str__(), self.label.__str__())

  def structured_sort_equation_code(self):
    if self.alias == None:
      return ""
    else:
      param = ""
      if isinstance(self.sort_expression, sort_container):
        param = str(self.sort_expression.element_sort).lower()
      sort = "%s(%s)" % (self.label, param)
      result = "        data_equation_vector %s_equations = detail::%s_struct(%s).constructor_equations(%s);\n" % (self.label, self.label, param, sort)
      return result + "        result.insert(result.end(), %s_equations.begin(), %s_equations.end());\n" % (self.label, self.label)

  def sort_name(self, id, label):
    code = ""
    code += "      inline\n"
    code += "      core::identifier_string const& %s_name()\n" % (label)
    code += "      {\n"
    code += "        static core::identifier_string %s_name = core::detail::initialise_static_expression(%s_name, core::identifier_string(\"%s\"));\n" % (label, label, id)
    code += "        return %s_name;\n" % (label)
    code += "      }\n\n"
    return code

  def container_name(self, id, label):
    code = ""
    code += "      inline\n"
    code += "      core::identifier_string const& %s_name()\n" % (label)
    code += "      {\n"
    code += "        static core::identifier_string %s_name = core::detail::initialise_static_expression(%s_name, core::identifier_string(\"%s\"));\n" % (label, label, label)
    code += "        return %s_name;\n" % (label.__str__())
    code += "      }\n\n"
    return code

  def sort_expression_constructors(self, id, label):
    code = ""
    code += self.sort_name(id, label)
    code += "      /// \\brief Constructor for sort expression %s\n" % (escape(id))
    code += "      /// \\return Sort expression %s\n" % (escape(id))
    code += "      inline\n"
    code += "      basic_sort const& %s()\n" % (label)
    code += "      {\n"
    code += "        static basic_sort %s = core::detail::initialise_static_expression(%s, basic_sort(%s_name()));\n" % (label, label, label)
    code += "        return %s;\n" % (label)
    code += "      }\n\n"

    code += "      /// \\brief Recogniser for sort expression %s\n" % (escape(str(id)))
    code += "      /// \\param e A sort expression\n"
    code += "      /// \\return true iff e == %s()\n" % (escape(label))
    code += "      inline\n"
    code += "      bool is_%s(const sort_expression& e)\n" % (label)
    code += "      {\n"
    code += "        if (is_basic_sort(e))\n"
    code += "        {\n"
    code += "          return basic_sort(e) == %s();\n" % (label)
    code += "        }\n"
    code += "        return false;\n"
    code += "      }\n"
    return code

  def sort_expression_constructors_container_sort(self, id, label, parameter):
    code = ""
#    code += self.container_name(id, label)
    code += "      /// \\brief Constructor for sort expression %s(%s)\n" % (escape(id), escape(parameter))
    code += "      /// \\param %s A sort expression\n" % (escape(str(parameter).lower()))
    code += "      /// \\return Sort expression %s(%s)\n" % (escape(label), escape(str(parameter).lower()))
    code += "      inline\n"
    code += "      container_sort %s(const sort_expression& %s)\n" % (label, str(parameter).lower())
    code += "      {\n"
    code += "        container_sort %s(%s_container(), %s);\n" % (label, label, str(parameter).lower())
    code += "        return %s;\n" % (label)
    code += "      }\n\n"

    code += "      /// \\brief Recogniser for sort expression %s(%s)\n" % (escape(id), escape(str(parameter).lower()))
    code += "      /// \\param e A sort expression\n"
    code += "      /// \\return true iff e is a container sort of which the name matches\n"
    code += "      ///      %s\n" % (escape(label))
    code += "      inline\n"
    code += "      bool is_%s(const sort_expression& e)\n" % (label)
    code += "      {\n"
    code += "        if (is_container_sort(e))\n"
    code += "        {\n"
    code += "          return container_sort(e).container_name() == %s_container();\n" % (label)
    code += "        }\n"
    code += "        return false;\n"
    code += "      }\n"
    return code

  def code(self, spec):
    if isinstance(self.sort_expression, sort_identifier):
      code = self.sort_expression_constructors(self.sort_expression, self.label)
    else:
      assert(isinstance(self.sort_expression, sort_container))
      code = self.sort_expression_constructors_container_sort(self.sort_expression.container, self.label, self.sort_expression.element_sort)

    if self.alias <> None:
      code += "\n"
      code += "      namespace detail {\n\n"
      code += "        /// \\brief Declaration for sort %s as structured sort\n" % (escape(self.label.__str__()))
      if isinstance(self.sort_expression, sort_container):
        param = str(self.sort_expression.element_sort).lower()
        code += "        /// \\param %s A sort expression\n" % (escape(param))
      else:
        param = ""
      code += "        /// \\ret The structured sort representing %s\n" % (escape(self.label.__str__()))
      code += "        inline\n"
      code += "        structured_sort %s_struct(const sort_expression& %s)\n" % (self.label.__str__(), param)
      code += "        {\n"
      code += "          structured_sort_constructor_vector constructors;\n"
      code += "%s" % (self.alias.code(spec))
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

  def merge_declarations(self, declarations, is_supertype):
    self.elements += declarations.elements

  def defines_container(self):
    return any(map(lambda x: x.defines_container(), self.elements))

  def defines_struct(self):
    return any(map(lambda x: x.defines_struct(), self.elements))

  def __str__(self):
    return "".join(["{0}\n".format(e) for e in self.elements])

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

  def code(self, spec):
    code = ""
    for e in self.elements:
      if e.namespace == self.namespace:
        code += "%s\n" % (e.code(spec))
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

  def __str__(self):
    return "eqn {0}".format(self.declarations)

  def code(self, spec, function_spec, variable_spec):
    return self.declarations.code(spec, function_spec, variable_spec)

class variable_specification():
  def __init__(self, declarations):
    assert(isinstance(declarations, variable_declaration_list))
    self.declarations = declarations

  def find_variable(self, variable):
    return self.declarations.find_variable(variable)

  def __str__(self):
    return "var {0}".format(self.declarations)

  def code(self, spec):
    return self.declarations.code(spec)

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

  def __str__(self):
    return "map {0}".format(self.declarations)

  def merge_specification(self, spec, is_supertype):
    self.declarations.merge_declarations(spec.declarations, is_supertype)

  def code(self, spec):
    if not self.declarations.empty():
      assert(isinstance(spec, specification))
      sort_parameters = string.join(map(lambda x: "const sort_expression& %s" % (str(x).lower()), self.declarations.sort_parameters(spec)), ", ")
      assert(self.namespace == spec.namespace)
      namespace_string = self.namespace
      if namespace_string == "undefined":
        namespace_string = spec.get_namespace()
      code = ""
      code += self.declarations.code(spec)
      code += "      /// \\brief Give all system defined mappings for %s\n" % (escape(namespace_string))
      for s in self.declarations.sort_parameters(spec):
        code += "      /// \\param %s A sort expression\n" % (escape(str(s).lower()))
      code += "      /// \\return All system defined mappings for %s\n" % (escape(namespace_string))
      code += "      inline\n"
      code += "      function_symbol_vector %s_generate_functions_code(%s)\n" % (namespace_string, sort_parameters)
      code += "      {\n"
      code += "        function_symbol_vector result;\n"
      code += self.declarations.generator_code(spec)
      code += "        return result;\n"
      code += "      }\n"
      return code
    else:
      return ""

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

  def __str__(self):
    return "cons {0}".format(self.declarations)

  def merge_specification(self, spec, is_supertype):
    self.declarations.merge_declarations(spec.declarations, is_supertype)

  def code(self, spec):
    if not self.declarations.empty():
      assert(isinstance(spec, specification))
      sort_parameters = string.join(map(lambda x: "const sort_expression& %s" % (str(x).lower()), self.declarations.sort_parameters(spec)), ", ")
      assert(self.namespace == spec.namespace)
      namespace_string = self.namespace
      if namespace_string == "undefined":
        namespace_string = spec.get_namespace()
      code  = self.declarations.code(spec)
      code += "      /// \\brief Give all system defined constructors for %s\n" % (escape(namespace_string))
      for s in self.declarations.sort_parameters(spec):
        code += "      /// \\param %s A sort expression\n" % (escape(str(s).lower()))
      code += "      /// \\return All system defined constructors for %s\n" % (escape(namespace_string))
      code += "      inline\n"
      code += "      function_symbol_vector %s_generate_constructors_code(%s)\n" % (namespace_string,sort_parameters)
      code += "      {\n"
      code += "        function_symbol_vector result;\n"
      add_constructors_code = self.declarations.generator_code(spec) + (spec.sort_specification.structured_sort_constructor_code())
      if add_constructors_code == "":
        for s in self.declarations.sort_parameters(spec):
          code += "        static_cast< void >(%s); // suppress unused variable warnings\n" % (str(s).lower())
      else:
        code += "%s" % (spec.sort_specification.structured_sort_constructor_code())
        code += "%s\n" % (self.declarations.generator_code(spec, True))
      code += "        return result;\n"
      code += "      }\n"
      return code
    else:
      return ""

class function_specification():
  def __init__(self, mapping_spec, constructor_spec = constructor_specification(function_declaration_list([]))):
    assert(isinstance(mapping_spec, mapping_specification))
    assert(isinstance(constructor_spec, constructor_specification))
    self.constructor_specification = constructor_spec
    self.mapping_specification = mapping_spec
    self.namespace = ""
    self.original_namespace = ""

  def merge_specification(self, spec, is_supertype):
    self.constructor_specification.merge_specification(spec.constructor_specification, is_supertype)
    self.mapping_specification.merge_specification(spec.mapping_specification, is_supertype)

  def set_namespace(self, string):
    self.namespace = string
    if self.original_namespace == "":
      self.original_namespace = string
    if self.constructor_specification <> None:
      self.constructor_specification.set_namespace(string)
    self.mapping_specification.set_namespace(string)

  def find_function(self, function, argumentcount):
    result = self.constructor_specification.find_function(function, argumentcount)
    if not result:
      result = self.mapping_specification.find_function(function, argumentcount)
    return result

  def __str__(self):
    return "{0}\n{1}" % (self.constructor_specification, self.mapping_specification)

  def code(self, spec):
    assert(isinstance(spec, specification))
    # This assumes that no functions occur with the same name, but one is
    # constructor and one is mapping.
    code = ""
    code += self.constructor_specification.code(spec)
    code += self.mapping_specification.code(spec)
# We need deepcopy here to prevent undesired duplication
    merged_declarations = copy.deepcopy(self.constructor_specification.declarations)
    for e in self.mapping_specification.declarations.elements:
      merged_declarations.push_back(copy.deepcopy(e))
    code += merged_declarations.projection_code(spec)
    return code

class sort_specification():
  def __init__(self, declarations):
    assert(isinstance(declarations, sort_declaration_list))
    self.declarations = declarations
    self.namespace = ""
    self.original_namespace = ""

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
      if str(d.sort_expression) == str(sort):
        return True
    return False

  def get_sort(self, sort):
    assert(self.has_sort(sort))
    for d in self.declarations.elements:
      if str(d.sort_expression) == str(sort):
        return d

  def defines_container(self):
    return self.declarations.defines_container()

  def defines_struct(self):
    return self.declarations.defines_struct()

  def merge_specification(self, spec, is_supertype):
    self.declarations.merge_declarations(spec.declarations, is_supertype)

  def __str__(self):
    return "sort {0}".format(self.declarations)

  def structured_sort_constructor_code(self):
    return self.declarations.structured_sort_constructor_code()

  def structured_sort_equation_code(self):
    return self.declarations.structured_sort_equation_code()

  def code(self,spec):
    return self.declarations.code(spec)

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
    self.uses = None
    self.subtypes = None
    self.namespace = ""
    self.original_namespace = ""
    self.originfile = ""

  def set_includes(self, includes):
    assert(isinstance(includes, include_list))
    self.includes = includes

  def set_using(self, uses):
    assert(isinstance(uses, using_list))
    self.uses = uses
    
  def set_subtypes(self, subtypes):
    self.subtypes = subtypes

  def sort_parameters(self):
    if self.uses != None:
      return self.uses.sort_parameters()
    else:
      return []

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

  def set_origin_file(self, f):
    self.origin_file = f

  def get_namespace(self):
    if self.namespace <> "undefined" and self.namespace <> "":
      return self.namespace
    else:
      return self.origin_file[:len(self.origin_file)-5]

  def set_namespace(self):
    if (len(self.sort_specification.declarations.elements) == 0):
      s = "undefined"
    else:
      s = "{0}".format(self.sort_specification.declarations.elements[0].label)
    self.namespace = s
    if self.original_namespace == "":
      self.original_namespace = s
    self.sort_specification.set_namespace(s)
    self.function_specification.set_namespace(s)
    self.equation_specification.set_namespace(s)

  def merge_specification(self, spec):
    assert(isinstance(spec, specification))
    self.sort_specification.merge_specification(spec.sort_specification, self.subtypes <> None)
    self.function_specification.merge_specification(spec.function_specification, self.subtypes <> None)

  def __str__(self):
    res = []
    if self.includes:
      res.append(str(self.includes))
    res.append(str(self.sort_specification))
    res.append(str(self.function_specification))
    res.append(str(self.variable_specification))
    res.append(str(self.equation_specification))
    return "\n".join(res)

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
    code += "/// \\file mcrl2/data/%s.h\n" % (remove_underscore(self.get_namespace()))
    code += "/// \\brief The standard sort %s.\n" % (self.get_namespace())
    code += "///\n"
    code += "/// This file was generated from the data sort specification\n"
    code += "/// mcrl2/data/build/%s.spec.\n" % (remove_underscore(self.get_namespace()))
    code += "\n"
    code += "#ifndef MCRL2_DATA_%s_H\n" % (self.get_namespace().upper())
    code += "#define MCRL2_DATA_%s_H\n\n" % (self.get_namespace().upper())
    code += "#include \"boost/utility.hpp\"\n\n"
    code += "#include \"workarounds.h\"\n"
    code += "#include \"mcrl2/exception.h\"\n"
    code += "#include \"mcrl2/data/basic_sort.h\"\n"
    code += "#include \"mcrl2/data/function_sort.h\"\n"
    code += "#include \"mcrl2/data/function_symbol.h\"\n"
    code += "#include \"mcrl2/data/application.h\"\n"
    code += "#include \"mcrl2/data/data_equation.h\"\n"
    code += "#include \"mcrl2/atermpp/container_utility.h\"\n"
    code += "#include \"mcrl2/data/standard.h\"\n"
#    code += "#include \"mcrl2/data/data_specification.h\"\n"
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
    if self.namespace <> "undefined":
      code += "    /// \\brief Namespace for system defined sort %s\n" % (escape(self.get_namespace()))
      code += "    namespace sort_%s {\n\n" % (self.get_namespace())
    code += self.sort_specification.code(self)
    code += self.function_specification.code(self)
    code += self.equation_specification.code(self, self.function_specification, self.variable_specification)
    code += "\n"
    dependent_sorts = set([])
    auxiliary_sorts = set([])
    for e in self.sort_specification.declarations.elements:
      if str(e).startswith("@") and e.original_namespace == self.namespace:
        auxiliary_sorts.add("         specification.add_system_defined_sort(%s);\n" % (e.inline_code(self)))
    if self.namespace <> "undefined":
      code += "    } // namespace sort_%s\n\n" % (self.get_namespace())
    code += "  } // namespace data\n\n"
    code += "} // namespace mcrl2\n\n"
    code += "#endif // MCRL2_DATA_%s_H\n" % (self.get_namespace().upper())
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

  def __str__(self):
    return "".join(["{0}\n".format(e) for e in self.elements])

  def code(self):
    s = ""
    for e in self.elements:
      s += "%s\n" % (e.code())
    return s

class include():
  def __init__(self, id):
    assert(isinstance(id, identifier))
    self.identifier = id

  def __str__(self):
    return "#include {0}".format(self.identifier)

  def code(self):
    return "#include \"mcrl2/data/{0}.h\"".format(self.identifier.__str__()[:-5])

class subtype_list():
  def __init__(self, elements):
    assert(all(map(lambda x: isinstance(x, subtype), elements)))
    self.elements = elements

  def push_back(self, element):
    assert(isinstance(element, subtype))
    self.elements += [element]

  def __str__(self):
    return "".join(["{0}\n".format(e) for e in self.elements])

  def code(self):
    s = ""
    for e in self.elements:
      s += "%s\n" % (e.code())
    return s

class subtype():
  def __init__(self, id):
    assert(isinstance(id, identifier))
    self.identifier = id

  def __str__(self):
    return "#supertypeof {0}".format(self.identifier)

class using_list():
  def __init__(self, elements):
    assert(all(map(lambda x: isinstance(x, using), elements)))
    self.elements = elements

  def push_back(self, element):
    assert(isinstance(element, using))
    self.elements += [element]

  def sort_parameters(self):
    return map(lambda x: x.sort_parameters(), self.elements)

  def __str__(self):
    return ", ".join(map(str, self.elements))

  def code(self):
    code = ""
    for e in self.elements:
      code += "%s\n" % (e.code())
    return code

class using():
  def __init__(self, identifier):
    self.identifier = identifier

  def __str__(self):
    return str(self.identifier)

  def sort_parameters(self):
    return sort_identifier(self.identifier)

  def code(self):
    return str(self)

