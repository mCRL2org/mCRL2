#!/usr/bin/env python

#~ Copyright 2013, 2014 Mark Geelen.
#~ Distributed under the Boost Software License, Version 1.0.
#~ (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

import random
import grammar
import copy

class dotdict(dict):
    def __getattr__(self, attr):
        return self.get(attr, None)
    __setattr__= dict.__setitem__
    __delattr__= dict.__delitem__

class RandGen:
    def __init__(self, bnfgrammar, constraints):
        self.grammar = grammar.CFG(bnfgrammar)
        self.ft = dict()
        self.ft_ = dict()
        self.attributes = []
        self.attrs = dict()
        self.names = dict()
        self.constraints = constraints
        self.secondpass = []
        self.meta = dict()
        for i in range(1, self.grammar.r() + 1):
            for j in range(1, self.grammar.s(i) + 1):
                self.meta[(i, j)] = 0

        self.glbs = {'choice': random.choice, 'odd': self.odd, 'deepcopy': copy.deepcopy} # predefined functions for use in attr rules

    def prep(self, n):
        for m in range(1, n + 1):
            for i in range(1, self.grammar.r()):
                if (i, m) not in self.ft:
                    self.ft[(i, m)] = self.f(i, m)
                                    

    def f(self, i, n):
        # Returns [sum(f'(i, j, k, n)) for 1 <= j <= s_i]
      #  print 'f(%s, %s)' % (i, n)
        if (i, n) in self.ft:
            return self.ft[(i, n)]
        res = []
        for j in range(1, self.grammar.s(i) + 1):

            if (i, j, 1, n) not in self.ft_:
                self.ft_[(i, j, 1, n)] = self.f_(i, j, 1, n)  
            res.append(sum(self.ft_[(i, j, 1, n)]))
        return res

    def f_(self, i, j, k, n):
        # Pre: 0 < n && 1 <= i <= r && 1 <= j <= s_i && 1 <= k <= t_ij
       # print 'f(%s, %s, %s, %s)' % (i, j, k, n)
        if (i, j, k, n) in self.ft_:
            return self.ft_[(i, j, k, n)]
        tij = self.grammar.t(i, j)
        xijk = self.grammar.x(i, j, k)
        if isinstance(xijk, grammar.AttributeRule):
            if k == tij:
                if n == 0:
                    return [1]
                else:
                    return [0]
            else:
                if (i, j, k + 1, n) not in self.ft_:
                       self.ft_[(i, j, k + 1, n)] = self.f_(i, j, k + 1, n)
                return [sum(self.ft_[(i, j, k + 1, n)])]
        if n == 0:
            return []
        if xijk in self.grammar.T or isinstance(xijk, grammar.AttributeValue):
            if k == tij:
                if n == 1: # hier klopt dus iets niet in de pseudocode
                    return [1]
                else:
                    return [0]
            else:
                if (i, j, k + 1, n - 1) not in self.ft_:
                       self.ft_[(i, j, k + 1, n - 1)] = self.f_(i, j, k + 1, n - 1)
                return [sum(self.ft_[(i, j, k + 1, n - 1)])]
        if k == tij:
            if (xijk, n) not in self.ft:
                self.ft[(xijk, n)] = self.f(xijk, n)
            return [sum(self.ft[(xijk, n)])]
        else:
            res = []
            for l in range(1, n - self.grammar.t(i,j) + k + 1):
                if (xijk, l) not in self.ft:
                    self.ft[(xijk, l)] = self.f(xijk, l)
                if (i, j, k + 1, n - l) not in self.ft_:
                    self.ft_[(i, j, k + 1, n - l)] = self.f_(i, j, k + 1, n - l)
                res.append(sum(self.ft[(xijk, l)]) * sum(self.ft_[(i, j, k + 1, n - l)]))
            return res
        
    def getConstraint(self, i, j, key):
        # try/catch better?
        if (i, j) not in self.grammar.labels:
            return []
        if self.grammar.labels[(i, j)] not in self.constraints:
            return []
        if key not in self.constraints[self.grammar.labels[(i, j)]]:
            return []
        return self.constraints[self.grammar.labels[(i, j)]][key]

    def getAttrRulesForSymbol(self, i, j, k):
        xijk = self.grammar.x(i, j, k)
        rules = self.getConstraint(i, j, 'rules')
        if (i, j, k) not in self.names:
            return []
        res = []
        name = self.names[(i, j, k)]
        for r in rules:
            if '.' in r and r[:r.index('.')] == name:
                res.append(r)
            if ' ' in r and r[:r.index(' ')] == name:
                res.append(r)
        return res

    def getPreRules(self, i, j):
        return self.getConstraint(i, j, 'pre')

    def getPostRules(self, i, j):
        return self.getConstraint(i, j, 'post')

    def getSubVars(self, i):
        return self.getConstraint(i, 1, 'attrs')

    def secondPass(self, term, vars):
        res = []
        for x in term:
            if isinstance(x, str):
                res.append(x)
            else:
                res = res + self.g_(x[0], x[1], 1, x[2], dict(x[3].items() + vars.items()))
        return res
     
    def Guard(self, c, i, j, subvars):
        g = self.getConstraint(i, j, 'guard')
        z = dict(self.glbs.items() + subvars.items())
        if g:
            try:
                res = eval(g, z, subvars)
                return res
            except Exception as e:
                print 'Unable to evaluate guard "%s" in rule %s' % (g, self.grammar.labels[(i, j)])
                raise e
        return True

    def g(self, i, n, subvars):
        c = []
        j = 0
        alts = []
        for x in self.f(i, n):
            j = j + 1
            w = 1
            if self.getConstraint(i, j, 'weight'):
                w = self.getConstraint(i, j, 'weight')
            if x == 0:
                c.append(0)
                if self.Guard(c, i, j, subvars):
                    alts.append(j)
            else:
                if self.Guard(c, i, j, subvars):
                    c.append(w)
                else:
                    c.append(0)

        # find new m heuristics
        if not sum(c):
            r = random.choice(alts)
            k = 1
            step = 1
            m = n
            while True:
                m = n + k           
                if sum(self.f_(i, r, 1, m)) > 0:
                    break
                m = max(0, n - k)
                if sum(self.f_(i, r, 1, m)) > 0:
                    break
                k = k + step
            n = m

        else:
            r = self.choose(c) + 1
        #r = self.choose(self.f(i, n))

        self.meta[(i,r)] = self.meta[(i,r)] + 1
 
        occurrences = dict()
        vars = subvars.copy()
        tij = self.grammar.t(i, r)
        for k in range(1, tij + 1):
            xijk = self.grammar.x(i, r, k)            
            if not isinstance(xijk, str):
                if isinstance(xijk, int):
                    name = self.grammar.lefthandsides[xijk-1]
                elif isinstance(xijk, grammar.AttributeValue):
                    name = xijk.name
                if name in occurrences:
                    occurrences[name] = occurrences[name] + 1
                    name = name + str(occurrences[name])                 
                else:
                    occurrences[name] = 1
                vars[name] = dotdict()
                if (i, r, k) not in self.names:
                    self.names[(i, r, k)] = name 

        # calculate pre-rules
        for prerule in self.getPreRules(i, r):
            try:
                exec prerule in self.glbs, vars
            except Exception as e:
                print 'Unable to evaluate attribute rule "%s" for production rule %s' % (prerule, self.grammar.labels[(i, r)])
                raise e

        sp = self.getConstraint(i, r, 'secondpass')
        if sp:
            if sp not in self.secondpass:
                self.secondpass.append(sp)
            return [(i, r, n, vars.copy())]

        res = self.g_(i, r, 1, n, vars)

        # calculate post rules
        for postrule in self.getPostRules(i, r):
            try:
                exec postrule in self.glbs, vars
            except Exception as e:
                print 'Unable to evaluate attribute rule "%s" for production rule %s' % (postrule, self.grammar.labels[(i, r)])
                raise e

        # update inherited attributes
        for x in subvars:
            subvars[x] = vars[x]    

        # if certain secondpass constraint is completed, secondpass
        if (i, r) in self.grammar.labels and self.grammar.labels[(i, r)] in self.secondpass:
            res = self.secondPass(res, vars)

        return res

    def g_(self, i, j, k, n, vars):
        #print '(%s, %s, %s, %s, %s)' % (i, j, k, n, str(vars.values()))
        tij = self.grammar.t(i, j)
        xijk = self.grammar.x(i, j, k)
        
        if xijk in self.grammar.T:
            if k == tij:
                return [xijk]
            else:
                return [xijk] + self.g_(i, j, k + 1, n - 1, vars)

        toApply = self.getAttrRulesForSymbol(i, j, k)
        for rule in toApply:
            try:
                exec rule in self.glbs, vars
            except Exception as e:
                print 'Unable to evaluate attribute rule "%s" in production rule %s, symbol number %s' % (rule, self.grammar.labels[(i, j)], k)
                raise e

        if isinstance(xijk, grammar.AttributeValue): # Token / attribute used as value
            w = ''
            try:
                w = [vars[xijk.name]]    
            except Exception as e:
                print 'Unknown value for token %s in production rule %s, symbol number %s' % (xijk.name, self.grammar.labels[(i, j)], k)
                raise e
            if k == tij:
                return w
            else:
                return w + self.g_(i, j, k + 1, n - 1, vars)
               
        # xijk is a non-terminal
        subvars = dotdict()

        for x in self.getSubVars(xijk):
            try:
                subvars[x] = vars[self.names[(i, j, k)]][x]    
            except Exception as e:
                print 'Unknown value for inherited attribute %s in production rule %s, symbol number %s' % (x, self.grammar.labels[(i, j)], k)
                raise e

        if k == tij:
            res = self.g(xijk, n, subvars)
            for x in self.getSubVars(xijk):
                vars[self.names[(i, j, k)]][x] = subvars[x]
            return res
        else:
            c = []
            for x in self.f_(i, j, k, n):
                if x == 0:
                    c.append(0)
                else:
                    c.append(1)
            l = self.choose(c) + 1 
            #l = self.choose(self.f_(i, j, k, n)) + 1   
            res1 = self.g(xijk, l, subvars) 

            # update var with attributes set in subvars
            for x in self.getSubVars(xijk):
                vars[self.names[(i, j, k)]][x] = subvars[x]
        
            return res1 + self.g_(i, j, k + 1, n - l, vars)

    def choose(self, l): 
        totals = []
        running_total = 0
        for w in l:
            running_total += w
            totals.append(running_total)
        rnd = random.random() * running_total
        for i, total in enumerate(totals):
            if rnd < total:
                return i

    def validTermLength(self, sort_name, termlength):
        index = self.grammar.index(sort_name)
        return sum(self.f(index, termlength)) > 0 

    def generate(self, sort_name, termlength):
        if not self.validTermLength(sort_name, termlength):
            print 'Invalid string length'
            raise ValueError()
        index = self.grammar.index(sort_name)
        attrs = {'choice': random.choice, 'odd': self.odd}
        result = self.g(index, termlength, dict())
        self.length = len(result)
        return ' '.join(result)

    def newAttrs(self, i, j, k, attrs):
        xijk = self.grammar.x(i, j, k)
        baseAttrs = ['__builtins__', 'choice', 'odd']
        res = {x: attrs[x] for x in attrs if x in baseAttrs}
        for x in range(len(self.grammar.args[(i, j, k)])):
            res[self.grammar.lhs_args[xijk - 1][x]] = attrs[self.grammar.args[(i, j, k)][x]]
        return res

    def odd(self, a, b, c):
        return (a and b and c) or (a ^ b ^ c)
