#!/usr/bin/python
import os
import re
import sys

def find_matching_bracket(text, startpos, left, right):
  c, i = 1, startpos
  while c > 0 and i < len(text):
    if text[i:].startswith(left):
      c += 1
      i += len(left)
    elif text[i:].startswith(right):
      c -= 1
      i += len(right)
    else:
      i += 1
  if c > 0:
    raise Exception('Brackets don\'t match: could not find {0} for {1}'.format(right, left))
  return i - len(right)

def skip(text):
  m = re.match(r'\s+', text)
  if not m:
    m = re.match(r'%.*?$', text, re.MULTILINE)
  if not m:
    m = re.match(r'.*?;', text, re.DOTALL)
  if m:
    return text[m.end():], text[:m.end()]
  raise Exception('Could not skip to next parsable statement.')

def get_item(text):
  item = ''
  c = 0
  while text:
    if text[0] == ',' and c == 0:
      return item
    else:
      if text[0] == '(':
        c += 1
      elif text[0] == ')':
        c -= 1
      item += text[0]
      text = text[1:]
  if c > 0:
    raise Exception('Unbalanced brackets: {0}'.format(text))
  return item
    
class ParamList(object):
  def __init__(self, text=''):
    self.params = []
    while True:
      text = text.lstrip()
      m = re.match(r'(\S+(,\s*\S+)*)\s*:\s*(\S+?)(,|$)', text)
      if m:
        self.params.append(([var.lstrip() for var in m.group(1).split(',')], m.group(3)))
        text = text[m.end():]
      else:
        break

  def __str__(self):
    return ', '.join(['{0}: {1}'.format(', '.join(vnames), vtype) for vnames, vtype in self.params])
 
  def __add__(self, other):
    sumlist = ParamList()
    sumlist.params = self.params + other.params
    return sumlist
    
  def names(self, replace=[]):
    items = sum([vnames for vnames, vtype in self.params], [])
    i = 1
    for name, value in reversed(replace):
      if name is None:
        items[-i] = value
        i += 1
      else:
        items = [value if i == name else i for i in items]
    return ', '.join(items)

class ParamAppl(object):
  def __init__(self, params, text=''):
    self.params = params
    self.assignform = re.match(r'(\S+)\s*=', text) is not None
    self.items = []
    pname = None
    while text:
      if self.assignform:
        m = re.match(r'(\S+)\s*=', text)
        if not m:
          raise Exception('Assignment form for parameter application not consistently used: {0}'.format(text))
        pname = m.group(1)
        text = text[m.end():]
      text = text.lstrip()
      item = get_item(text)
      self.items.append((pname, item))
      text = text[len(item):].lstrip()
      if text.startswith(','):
        text = text[1:].lstrip()
      elif text:
        raise Exception('Parameter list does not end as expected: {0}'.format(text))   

  def __str__(self):
    return self.params.names(self.items)


class ProcObj(object):
  def __init__(self, parent=None):
    self._name = '__procobj'
    self._params = ParamList()
    self.parent = parent
    
  @property
  def shortname(self):
    return self._name

  @property
  def name(self):
    return '{0}\'{1}'.format(self.parent.name, self._name) if self.parent else self._name
  
  @property
  def params(self):
    return self.parent.params + self._params if self.parent else self._params

class State(ProcObj):
  def __init__(self, parent):
    ProcObj.__init__(self, parent)
    self.procexp = 'delta;'
    self.initial = False
    
  def replace(self, states):
    for s in states:
      self.procexp, text = '', self.procexp
      while text:
        m = re.search(r"(?<![A-Za-z_']){0}(?![A-Za-z_'])(\s*\()?".format(s.shortname), text, re.DOTALL)
        if m:
          self.procexp += text[:m.start()] + s.name
          if m.group(1):
            i = find_matching_bracket(text, m.end(), '(', ')')
            self.procexp += '({0})'.format(ParamAppl(s.params, text[m.end():i].strip()))
            text = text[i+1:]
          else:
            text = text[m.end():]
        else:
          self.procexp += text
          text = ''
  
  def parse(self, text):
    m = re.match(r'(state|initial\s+state)\s+([A-Za-z_]+)\s*(.*?)=\s*(.*?;)', text, re.DOTALL)
    if m:
      self.initial = m.group(1).startswith('initial')
      self._name = m.group(2)
      if m.group(3):
        self._params = ParamList(m.group(3).strip()[1:-1])
      self.procexp = m.group(4)
      return m.span()     
    else:
      return (0, 0)
  
  def __str__(self):
    return 'proc {0}({1}) = {2}'.format(self.name, self.params, self.procexp)
    

class Nested(ProcObj):
  def __init__(self, parent=None):
    ProcObj.__init__(self, parent)
    self.states = []
    self.initial = False
    
  def replace(self, states):
    for s in self.states:
      s.replace(states)
    
  def parse(self, text):
    m = re.match(r'(proc|state|initial\s*state)\s+(\S+)\s*=\s*nested\s*\(', text)
    if m:
      self.initial = m.group(1).startswith('initial')
      self._name = m.group(2)
      i = find_matching_bracket(text, m.end(), '(', ')')
      self._params = ParamList(text[m.end():i])
      i = text.find('{', i) + 1
      end = find_matching_bracket(text, i, '{', '};')
      inner = text[i:end].lstrip()
      
      nested = Nested(self)
      state = State(self)      
      while inner:        
        s, e = nested.parse(inner)
        if e:
          self.states.append(nested)
          nested = Nested(self)
          inner = inner[e:].lstrip()
        else:
          s, e = state.parse(inner)
          if e:
            self.states.append(state)
            state = State(self)
            inner = inner[e:].lstrip()
          else:
            inner = skip(inner)[0]
      
      for s in self.states:
        s.replace(self.states)
      
      return (m.start(), end + 2)
    else:
      return (0, 0)
  
  def __str__(self):
    initial = [s for s in self.states if s.initial][0]
    maindec = 'proc {0}({1}) = {2}({3});'.format(self.name, self.params, initial.name, self.params.names())
    return '\n'.join([maindec] + [str(s) for s in self.states])

def preprocess(text):
  lastpos = 0
  nested = Nested()
  result = ''
  while text:
    start, end = nested.parse(text)
    if end:
      result += str(nested)
      text = text[end:] 
      nested = Nested()
    else:
      text, skipped = skip(text)
      result += skipped
  return result

def main():
  if len(sys.argv) != 2 or not os.path.isfile(sys.argv[1]):
    print('Usage: preprocessor.py FILE\n' +
          '\n' +
          'Expands the pseudo-mCRL2 specification in FILE and writes the result to the standard output.')
  else:
    f = open(sys.argv[1], 'r')
    text = f.read()
    f.close()
    if text.startswith('#!'):
      text = text[text.find('\n')+1:]
    print(preprocess(text))

if __name__ == '__main__':
  main()
