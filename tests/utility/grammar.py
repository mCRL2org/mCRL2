#!/usr/bin/env python

#~ Copyright 2013, 2014 Mark Geelen.
#~ Distributed under the Boost Software License, Version 1.0.
#~ (See accompanying file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

import mcrl2_parser2

class AttributeRule:
    def __init__(self, rule):
        self.rule = rule

class AttributeValue:
    def __init__(self, name):
        self.name = name

class CFG:
    def __init__(self, grammar):
        self.T = []
        self.tokens = []
        self.G = []
       # ebnf_parser = mcrl2_parser.EBNFParser(mcrl2_parser.Mcrl2Actions())
        ebnf_parser = mcrl2_parser2.EBNFParser(mcrl2_parser2.Mcrl2Actions())
        self.rules = ebnf_parser(grammar)
        self.labels = dict()
       # self.attrs = dict()
       # self.guards = dict()
       # self.secondpass = []
        self.lefthandsides = []
       # self.args = dict()
       # self.lhs_args = []
        for lhs in self.rules:
            self.lefthandsides.append(lhs.name())
       #     name = lhs.name()
       #     self.lhs_args.append(lhs.args)
       #     if name[0] == '!':
        #        self.secondpass.append(name[1:])
        #        self.lefthandsides.append(name[1:])
        #    else:
        #        self.lefthandsides.append(name)
        self.G = self.TransformRules()
      #  self.RemoveEmptyProductions()
       # self.CopyGrammarWithoutAttrs()
   

    def addRepetition(self, a):
        lhs = len(self.rules) + 1
        lf = mcrl2_parser.Argument(lhs, '', a.args)
        rec = mcrl2_parser.Function('', [mcrl2_parser.Argument(a.expressions, '', a.args), lf], None)
        eps = mcrl2_parser.Function('', [mcrl2_parser.Argument('', '', a.args)], None)
        af = mcrl2_parser.Function('', [mcrl2_parser.Argument(a.expressions, '', a.args)], None)
        rhs = []
        if a.repetitions == '+':
            rhs = [af, rec]
        if a.repetitions == '*':
            rhs = [eps, rec]
        if a.repetitions == '?':
            rhs = [eps, af]
        self.rules.append(mcrl2_parser.Rule(lhs, rhs, [])) # todo
        return lhs

    def addRule(self, arg):
        lhs = len(self.rules) + 1
        rhs = arg.expressions
        self.rules.append(mcrl2_parser.Rule(lhs, rhs, [])) # todo
        return lhs

    def getFunctions(self, i, fs):
        result = []
        j = 0
        for f in fs:
            val = self.getFunction(i, j, f)
            if len(val) > 0 and isinstance(val[0], list):
                for v in val:
                    result.append(v)
            else:
                result.append(val)
            j = j + 1          
        return result

    def getFunction(self, i, j, f):
        dim = 1
        interm = []
        attr = ''
        k = 0
        for a in f.arguments:
            fs = []
            #self.args[(i+1, j+1, k+1)] = a.args
            if a.lbl != None:
                self.labels[(i+1, j+1)] = a.lbl
                continue
            if a.repetitions != '':
                fs = self.addRepetition(a)
            elif isinstance(a.expressions, list):
                fs = self.addRule(a)
            else:
                fs = a.expressions
                
                if fs in self.lefthandsides:
                    fs = self.lefthandsides.index(fs) + 1
                    
                
                elif isinstance(fs, str) and fs != '':
                    if fs[0] == '\'':
                        fs = fs[1:-1] # remove quotes
                    elif fs[0] == '"':
                        fs = fs[1:-1] # remove quotes
                        self.tokens.append(fs)
                    elif fs[0] == '{':
                        if (fs[1] == '?'):
                            self.guards[(i+1,j+1)] = fs[2:-1]
                            continue
                        if (fs[1] == '!'):
                            self.secondpass.append((i+1,j+1))
                            continue
                        ar = AttributeRule(fs[1:-1])
                        interm.append(ar)
                        k = k + 1
                        continue
                    elif fs not in self.lefthandsides: 
                        # this is an attribute used as non-terminal
                        ar = AttributeValue(fs)
                        interm.append(ar)
                        k = k + 1
                        continue
                    self.T.append(fs)
                elif fs == '':
                    self.T.append('')
            interm.append(fs)
            k = k + 1
        return interm

    def TransformRules(self):
        result = []
        i = 0
        for r in self.rules:
            result.append(self.getFunctions(i, r.rhs))
            i = i + 1
        return result

    def RemoveEmptyProductions(self):
        while True:
            Ns = [x for x in range(len(self.G)) if [''] in self.G[x]]
            if len(Ns) == 0:
                break
            A = Ns[0]   
            del self.G[A][self.G[A].index([''])]
            for p in range(len(self.G)):
                c = 0
                while c < len(self.G[p]):
                    for a in range(len(self.G[p][c])):
                        if self.G[p][c][a] == (A + 1):
                            rhs = self.G[p][c][:a] + self.G[p][c][(a+1):]
                            if len(self.G[p][c]) == 1:
                                rhs = ['']                          
                            if rhs not in self.G[p]:
                                self.G[p].append(rhs)
                    c = c + 1
    
    def CopyGrammarWithoutAttrs(self):
        self.G_ = []
        for i in range(len(self.G)):
            self.G_.append([])
            for j in range(len(self.G[i])):                                       
                self.G_[i].append([k for k in self.G[i][j] if not isinstance(k, AttributeRule)])

                       
    # -------------------------------------------
    # Getters for the random generation algorithm
    # -------------------------------------------

    def t_(self, i, j):
        return len([x for x in self.G_[i - 1][j - 1] if not isinstance(x, AttributeRule)])

    def x(self, i, j, k):
        return self.G[i - 1][j - 1][k - 1]

    def t(self, i, j):
        return len(self.G[i - 1][j - 1])

    def s(self, i):
        return len(self.G[i - 1])

    def r(self):
        return len(self.G)

    def index(self, sort_string):
        return self.lefthandsides.index(sort_string) + 1


    def parameters(self, i):
        # return set of names of variables in the parameterlist of non-terminal i
        # as keys for the attribute dictionary
        return []

        
        
