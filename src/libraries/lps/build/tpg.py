#!/usr/bin/env python

"""Toy Parser Generator is a lexical and syntactic parser generator
for Python. This generator was born from a simple statement: YACC
is too complex to use in simple cases (calculators, configuration
files, small programming languages, ...).

TPG can very simply write parsers that are usefull for most every
day needs (even if it can't make your coffee). With a very clear
and simple syntax, you can write an attributed grammar that is
translated into a recursive descendant parser. TPG generated code
is very closed to the original grammar. This means that the parser
works "like" the grammar. A grammar rule can be seen as a method
of the parser class, symbols as method calls, attributes as method
parameters and semantic values as return values. You can also add
Python code directly into grammar rules and build abstract syntax
trees while parsing.
"""

# Toy Parser Generator: A Python parser generator
# Copyright (C) 2002 Christophe Delord
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#
# For further information about TPG you can visit
# http://christophe.delord.free.fr/en/tpg

# TODO:
#   - indent and dedent preprocessor
#

from __future__ import generators

__tpgname__ = 'TPG'
__version__ = '3.0.6'
__date__ = '2005-07-08'
__description__ = "A Python parser generator"
__long_description__ = __doc__
__license__ = 'LGPL'
__author__ = 'Christophe Delord'
__email__ = 'christophe.delord@free.fr'
__url__ = 'http://christophe.delord.free.fr/en/tpg/'

import parser
import re
import sre_parse
import sys

try:
    enumerate
except NameError:
    enumerate = lambda seq: zip(xrange(sys.maxint), seq)

_id = lambda x: x
tab = " "*4

class Error(Exception):
    """ Error((line, row), msg)
    
    Error is the base class for TPG exceptions.

    Attributes:
        line : line number from where the error has been raised
        row  : row number from where the error has been raised
        msg  : message associated to the error
    """
    def __init__(self, (line, row), msg):
        self.line, self.row = line, row
        self.msg = msg
    def __str__(self):
        return "%s at line %s, row %s: %s"%(self.__class__.__name__, self.line, self.row, self.msg)

class WrongToken(Error):
    """ WrongToken()
    
    WrongToken is raised when the parser can not continue in order to backtrack.
    """
    def __init__(self):
        Exception.__init__(self)

class LexicalError(Error):
    """ LexicalError((line, row), msg)

    LexicalError is raised by lexers when a lexical error is encountered.

    Attributes:
        line : line number from where the error has been raised
        row  : row number from where the error has been raised
        msg  : message associated to the error
    """
    pass

class SyntacticError(Error):
    """ SyntacticError((line, row), msg)

    SyntacticError is raised by parsers when they fail.

    Attributes:
        line : line number from where the error has been raised
        row  : row number from where the error has been raised
        msg  : message associated to the error
    """
    pass

class SemanticError(Error):
    """ SemanticError(msg)

    SemanticError is raised by user actions when an error is detected.

    Attributes:
        msg  : message associated to the error
    """
    def __init__(self, msg):
        Exception.__init__(self)
        self.msg = msg
    def __str__(self):
        return "%s: %s"%(self.__class__.__name__, self.msg)

class LexerOptions:
    """ LexerOptions(word_bounded, compile_options)

    LexerOptions is a base class for lexers holding lexers' options.

    Parameters:
        word_bounded    : if True identifier like regular expressions are added word boundaries
        compile_options : options given to re.compile to compile regular expressions
    """

    word_re = re.compile(r"^\w+$")

    def __init__(self, wb, compile_options):
        if not wb:
            self.word_bounded = self.not_word_bounded
        self.compile_options = compile_options

    def re_compile(self, expr):
        """ compile expr using self.compile_options as re.compile options
        """
        return re.compile(expr, self.compile_options)

    def word_bounded(self, expr):
        """ add word boundaries (\\b) to expr if it looks like an identifier
        """
        if self.word_re.match(expr):
            return r"\b%s\b"%expr
        else:
            return expr

    def not_word_bounded(self, expr):
        """ return expr without change. Used to replace word_bounded when wb is False
        """
        return expr

class NamedGroupLexer(LexerOptions):
    r""" NamedGroupLexer(word_bounded, compile_options)

    NamedGroupLexer is a TPG lexer:
        - use named group regular expressions (faster but limited to 100 tokens)

    Attributes:
        token_re : regular expression containing the whole lexer
        tokens   : dictionnary name -> (value, is_real_token)
                        name is a token name
                        value is a function that compute the value of a token from its text
                        is_real_token is a boleean. True for tokens, False for separators
    Once the lexer is started more attributes are defined:
        input      : input string being parsed
        max_pos    : maximum position reached in the input string
        last_token : last token reached in the input string
        pos        : position in the input string of the current token
        line       : line of the current token
        row        : row of the current token
        cur_token  : current token
    """

    def __init__(self, wb, compile_options):
        LexerOptions.__init__(self, wb, compile_options)
        self.token_re = []              # [named_regexp] and then regexp
        self.tokens = {}                # name -> value, is_real_token

    def def_token(self, name, expr, value=_id):
        """ add a new token to the lexer
        
        Parameters:
            name : name of the token
            expr : regular expression of the token
            value : function to compute the token value from its text

        The default for value is the identity function. If value is not callable
        it is returned whatever the text of the token.
        """
        if not callable(value):
            value = lambda _, value=value: value
        if name not in self.tokens:
            self.token_re.append("(?P<%s>%s)"%(name, self.word_bounded(expr)))
            self.tokens[name] = value, True
        else:
            raise SemanticError("Duplicate token definition (%s)"%name)

    def def_separator(self, name, expr, value=_id):
        """ add a new separator to the lexer
        
        Parameters:
            name : name of the separator
            expr : regular expression of the separator
            value : function to compute the separator value from its text

        The default for value is the identity function. If value is not callable
        it is returned whatever the text of the separator. Note that separator
        values are ignored.
        """
        if not callable(value):
            value = lambda _, value=value: value
        if name not in self.tokens:
            self.token_re.append("(?P<%s>%s)"%(name, self.word_bounded(expr)))
            self.tokens[name] = value, False
        else:
            raise SemanticError("Duplicate token definition (%s)"%name)

    def build(self):
        """ build the token_re attribute from the tokens and separators
        """
        if isinstance(self.token_re, list):
            self.token_re = self.re_compile("|".join(self.token_re))

    def start(self, input):
        """ start a lexical analysis

        Parameters:
            input : input string to be parsed
        """
        self.input = input
        self.max_pos = 0
        self.last_token = None
        self.build()
        self.back(None)
        self.next()

    def eof(self):
        """ True if the current position of the lexer is the end of the input string
        """
        return self.pos >= len(self.input) and isinstance(self.cur_token, EOFToken)

    def back(self, token):
        """ change the current token to token (used for backtracking)
        """
        if token is None:
            self.pos = 0
            self.line, self.row = 1, 1
            self.cur_token = None
        else:
            self.pos = token.stop
            self.line, self.row = token.end_line, token.end_row
            self.cur_token = token

    def next(self):
        """ return the next token

        Tokens are Token instances. Separators are ignored.
        """
        if self.cur_token is None:
            prev_stop = 0
        else:
            prev_stop = self.cur_token.stop
        while True:
            if self.pos >= len(self.input):
                self.cur_token = EOFToken(self.line, self.row, self.pos, prev_stop)
                return self.cur_token
            tok = self.token_re.match(self.input, self.pos)
            if tok:
                name = tok.lastgroup
                text = tok.group()
                value, real_token = self.tokens[name]
                try:
                    value = value(text)
                except WrongToken:
                    raise LexicalError((self.line, self.row), "Lexical error in %s"%text)
                start, stop = tok.span()
                self.pos = stop
                tok_line, tok_row = self.line, self.row
                if '\n' in text:
                    self.line += text.count('\n')
                    self.row = len(text) - text.rfind('\n')
                else:
                    self.row += len(text)
                if real_token:
                    self.cur_token = Token(name, text, value, tok_line, tok_row, self.line, self.row, start, stop, prev_stop)
                    if self.pos > self.max_pos:
                        self.max_pos = self.pos
                        self.last_token = self.cur_token
                    return self.cur_token
            else:
                w = 20
                nl = self.input.find('\n', self.pos, self.pos+w)
                if nl > -1:
                    err = self.input[self.pos:nl]
                else:
                    err = self.input[self.pos:self.pos+w]
                raise LexicalError((self.line, self.row), "Lexical error near %s"%err)

    def token(self):
        """ return the current token
        """
        return self.cur_token

    def __getitem__(self, item):
        """ extract text from the input string

        Parameters:
            item : slice delimiting the text to extract
                   item.start is the token from which the extraction starts
                   item.stop is the token where the extraction stops
        """
        return self.input[item.start.start:item.stop.prev_stop]

class Lexer(NamedGroupLexer):
    r""" Lexer(word_bounded, compile_options)

    Lexer is a TPG lexer:
        - based on NamedGroupLexer
        - doesn't use named group regular expressions (slower but not limited to 100 tokens)
        - select the longuest match so the order of token definitions doesn't mater

    Attributes:
        tokens : list (name, regexp, value, is_real_token)
                        name is a token name
                        regexp is the regular expression of the token
                        value is a function that computes the value of a token from its text
                        is_real_token is a boleean. True for tokens, False for separators
    Once the lexer is started more attributes are defined:
        input      : input string being parsed
        max_pos    : maximum position reached in the input string
        last_token : last token reached in the input string
        pos        : position in the input string of the current token
        line       : line of the current token
        row        : row of the current token
        cur_token  : current token
    """

    def __init__(self, wb, compile_options):
        LexerOptions.__init__(self, wb, compile_options)
        self.tokens = []        # [(name, regexp, value, is_real_token)]

    def def_token(self, name, expr, value=_id):
        """ adds a new token to the lexer
        
        Parameters:
            name : name of the token
            expr : regular expression of the token
            value : function to compute the token value from its text

        The default for value is the identity function. If value is not callable
        it is returned whatever the text of the token.
        """
        if not callable(value):
            value = lambda _, value=value: value
        if name not in self.tokens:
            self.tokens.append((name, self.re_compile(self.word_bounded(expr)), value, True))
        else:
            raise SemanticError("Duplicate token definition (%s)"%name)
    
    def def_separator(self, name, expr, value=_id):
        """ add a new separator to the lexer
        
        Parameters:
            name : name of the separator
            expr : regular expression of the separator
            value : function to compute the separator value from its text

        The default for value is the identity function. If value is not callable
        it is returned whatever the text of the separator. Note that separator
        values are ignored.
        """
        if not callable(value):
            value = lambda _, value=value: value
        if name not in self.tokens:
            self.tokens.append((name, self.re_compile(self.word_bounded(expr)), value, False))
        else:
            raise SemanticError("Duplicate token definition (%s)"%name)

    def start(self, input):
        """ start a lexical analysis

        Parameters:
            input : input string to be parsed
        """
        self.input = input
        self.max_pos = 0
        self.last_token = None
        self.back(None)
        self.next()

    def next(self):
        """ return the next token

        Tokens are Token instances. Separators are ignored.
        """
        if self.cur_token is None:
            prev_stop = 0
        else:
            prev_stop = self.cur_token.stop
        while True:
            if self.pos >= len(self.input):
                self.cur_token = EOFToken(self.line, self.row, self.pos, prev_stop)
                return self.cur_token
            tok = None
            text = ""
            for _name, _regexp, _value, _is_real_token in self.tokens:
                _tok = _regexp.match(self.input, self.pos)
                if _tok:
                    _text = _tok.group()
                    if len(_text) > len(text):
                        tok = _tok
                        name = _name
                        text = _text
                        value = _value
                        real_token = _is_real_token
            if tok:
                try:
                    value = value(text)
                except WrongToken:
                    raise LexicalError((self.line, self.row), "Lexical error in %s"%text)
                start, stop = tok.span()
                self.pos = stop
                tok_line, tok_row = self.line, self.row
                if '\n' in text:
                    self.line += text.count('\n')
                    self.row = len(text) - text.rfind('\n')
                else:
                    self.row += len(text)
                if real_token:
                    self.cur_token = Token(name, text, value, tok_line, tok_row, self.line, self.row, start, stop, prev_stop)
                    if self.pos > self.max_pos:
                        self.max_pos = self.pos
                        self.last_token = self.cur_token
                    return self.cur_token
            else:
                w = 20
                nl = self.input.find('\n', self.pos, self.pos+w)
                if nl > -1:
                    err = self.input[self.pos:nl]
                else:
                    err = self.input[self.pos:self.pos+w]
                raise LexicalError((self.line, self.row), "Lexical error near %s"%err)

class CacheNamedGroupLexer(NamedGroupLexer):
    r""" CacheNamedGroupLexer(word_bounded, compile_options)

    CacheNamedGroupLexer is a TPG lexer:
        - based on NamedGroupLexer
        - the complete token list is built before parsing
          (faster with very ambigous grammars but needs more memory)

    Attributes:
        token_re : regular expression containing the whole lexer
        tokens   : dictionnary name -> (value, is_real_token)
                        name is a token name
                        value is a function that computes the value of a token from its text
                        is_real_token is a boleean. True for tokens, False for separators
        cache    : token list
    Once the lexer is started more attributes are defined:
        input      : input string being parsed
        max_pos    : maximum position reached in the input string
        last_token : last token reached in the input string
        pos        : position in the input string of the current token
        line       : line of the current token
        row        : row of the current token
        cur_token  : current token
    """

    def __init__(self, wb, compile_options):
        NamedGroupLexer.__init__(self, wb, compile_options)

    def start(self, input):
        """ start a lexical analysis

        Parameters:
            input : input string to be parsed
        """
        self.cache = []
        self.input = input
        self.max_pos = 0
        self.last_token = None
        self.build()
        self.back(None)
        while True:
            token = NamedGroupLexer.next(self)
            token.index = len(self.cache)
            self.cache.append(token)
            if isinstance(token, EOFToken):
                break
        self.max_pos = 0
        self.last_token = None
        self.back(None)
        self.next()

    def next(self):
        """ return the next token

        Tokens are Token instances. Separators are ignored.
        """
        if self.cur_token is None:
            index = 0
        else:
            index = self.cur_token.index+1
        token = self.cache[index]
        self.pos = token.stop
        self.line, self.row = token.line, token.row
        self.cur_token = token
        if self.pos > self.max_pos:
            self.max_pos = self.pos
            self.last_token = self.cur_token
        return self.cur_token

class CacheLexer(Lexer):
    r""" CacheLexer(word_bounded, compile_options)

    CacheLexer is a TPG lexer:
        - based on Lexer
        - doesn't use named group regular expressions (slower but not limited to 100 tokens)
        - select the longuest match so the order of token definitions doesn't mater
        - the complete token list is built before parsing
          (faster with very ambigous grammars but needs more memory)

    Attributes:
        tokens : list (name, regexp, value, is_real_token)
                        name is a token name
                        regexp is the regular expression of the token
                        value is a function that computes the value of a token from its text
                        is_real_token is a boleean. True for tokens, False for separators
        cache  : token list
    Once the lexer is started more attributes are defined:
        input      : input string being parsed
        max_pos    : maximum position reached in the input string
        last_token : last token reached in the input string
        pos        : position in the input string of the current token
        line       : line of the current token
        row        : row of the current token
        cur_token  : current token
    """

    def __init__(self, wb, compile_options):
        Lexer.__init__(self, wb, compile_options)

    def start(self, input):
        """ start a lexical analysis

        Parameters:
            input : input string to be parsed
        """
        self.cache = []
        self.input = input
        self.max_pos = 0
        self.last_token = None
        self.back(None)
        while True:
            token = Lexer.next(self)
            token.index = len(self.cache)
            self.cache.append(token)
            if isinstance(token, EOFToken):
                break
        self.max_pos = 0
        self.last_token = None
        self.back(None)
        self.next()

    def next(self):
        """ return the next token

        Tokens are Token instances. Separators are ignored.
        """
        if self.cur_token is None:
            index = 0
        else:
            index = self.cur_token.index+1
        token = self.cache[index]
        self.pos = token.stop
        self.line, self.row = token.line, token.row
        self.cur_token = token
        if self.pos > self.max_pos:
            self.max_pos = self.pos
            self.last_token = self.cur_token
        return self.cur_token

class ContextSensitiveLexer(LexerOptions):
    r""" ContextSensitiveLexer(word_bounded, compile_options)

    ContextSensitiveLexer is a TPG lexer:
        - context sensitive means that each regular expression is matched when required by the parser.
          Different tokens can be found at the same position if the parser uses different grammar rules.

    Attributes:
        tokens     : dictionnary name -> (regexp, value)
                        name is a token name
                        regexp is the regular expression of the token
                        value is a function that computes the value of a token from its text
        separators : list (name, regexp, value)
                        name is a token name
                        regexp is the regular expression of the token
                        value is a function that computes the value of a token from its text
    Once the lexer is started more attributes are defined:
        input      : input string being parsed
        max_pos    : maximum position reached in the input string
        last_token : last token reached in the input string
        pos        : position in the input string of the current token
        line       : line of the current token
        row        : row of the current token
        cur_token  : current token
    """

    def __init__(self, wb, compile_options):
        LexerOptions.__init__(self, wb, compile_options)
        self.tokens = {}                # name -> (regexp, value)
        self.separators = []            # [(name, regexp, value)]

    def def_token(self, name, expr, value=_id):
        """ add a new token to the lexer
        
        Parameters:
            name : name of the token
            expr : regular expression of the token
            value : function to compute the token value from its text

        The default for value is the identity function. If value is not callable
        it is returned whatever the text of the token.
        """
        if not callable(value):
            value = lambda _, value=value: value
        if name not in self.tokens and name not in self.separators:
            self.tokens[name] = self.re_compile(self.word_bounded(expr)), value
        else:
            raise SemanticError("Duplicate token definition (%s)"%name)

    def def_separator(self, name, expr, value=_id):
        """ add a new separator to the lexer
        
        Parameters:
            name : name of the separator
            expr : regular expression of the separator
            value : function to compute the separator value from its text

        The default for value is the identity function. If value is not callable
        it is returned whatever the text of the separator. Note that separator
        values are ignored.
        """
        if not callable(value):
            value = lambda _, value=value: value
        if name not in self.tokens and name not in self.separators:
            self.separators.append((name, self.re_compile(self.word_bounded(expr)), value))
        else:
            raise SemanticError("Duplicate token definition (%s)"%name)

    def start(self, input):
        """ start a lexical analysis

        Parameters:
            input : input string to be parsed
        """
        self.input = input
        self.max_pos = 0
        self.last_token = None
        self.back(None)

    def eof(self):
        """ True if the current position of the lexer is the end of the input string
        """
        return self.pos >= len(self.input)

    def back(self, token):
        """ change the current token to token (used for backtracking)
        """
        if token is None:
            self.pos = 0
            self.line, self.row = 1, 1
            self.cur_token = SOFToken()
        else:
            self.pos = token.stop
            self.line, self.row = token.end_line, token.end_row
            self.cur_token = token
        self.eat_separators()
        self.cur_token.next_start = self.pos

    def eat_separators(self):
        """ skip separators in the input string from the current position
        """
        done = False
        while not done:
            done = True
            for name, regexp, value in self.separators:
                sep = regexp.match(self.input, self.pos)
                if sep:
                    start, stop = sep.span()
                    text = self.input[start:stop]
                    value = value(text)
                    self.pos = stop
                    if '\n' in text:
                        self.line += text.count('\n')
                        self.row = len(text) - text.rfind('\n')
                    else:
                        self.row += len(text)
                    done = False

    def eat(self, name):
        """ return the next token value if it matches the expected token name
        """
        regexp, value = self.tokens[name]
        tok = regexp.match(self.input, self.pos)
        if tok is None:
            raise WrongToken
        else:
            if self.cur_token is None:
                prev_stop = 0
            else:
                prev_stop = self.cur_token.stop
            start, stop = tok.span()
            text = self.input[start:stop]
            value = value(text)
            self.pos = stop
            tok_line, tok_row = self.line, self.row
            if '\n' in text:
                self.line += text.count('\n')
                self.row = len(text) - text.rfind('\n')
            else:
                self.row += len(text)
            self.cur_token = Token(name, text, value, tok_line, tok_row, self.line, self.row, start, stop, prev_stop)
            if self.pos > self.max_pos:
                self.max_pos = self.pos
                self.last_token = self.cur_token
            self.eat_separators()
            self.cur_token.next_start = self.pos
            return self.cur_token

    def token(self):
        """ return the current token
        """
        return self.cur_token

    def __getitem__(self, item):
        """ extract text from the input string

        Parameters:
            item : slice delimiting the text to extract
                   item.start is the token from which the extraction starts
                   item.stop is the token where the extraction stops
        """
        start = item.start and item.start.next_start or 0
        stop = item.stop and item.stop.stop or -1
        return self.input[start:stop]

class Token:
    """ Token(name, text, value, line, row, end_line, end_row, start, stop, prev_stop)

    Token object used by lexers

    Attributes:
        name      : name of the token
        text      : text matched by the regular expression
        value     : value computed from the text
        line      : line of the token in the input string
        row       : row of the token in the input string
        end_line  : line of the end of the token
        end_row   : row of the end of the token
        start     : position of the start in the input string of the token
        stop      : position of the end in the input string of the token
        prev_stop : position of the end of the previous token
    """

    def __init__(self, name, text, value, line, row, end_line, end_row, start, stop, prev_stop):
        self.name = name
        self.text = text
        self.value = value
        self.line, self.row = line, row
        self.end_line, self.end_row = end_line, end_row
        self.start, self.stop = start, stop
        self.prev_stop = prev_stop

    def match(self, name):
        """ return True is the token name is the name of the expected token

        Parameters:
            name : name of the expected token
        """
        return name == self.name

    def __str__(self):
        return "line %s, row %s: %s %s %s"%(self.line, self.row, self.name, self.text, self.value)

class EOFToken(Token):
    """ EOFToken(line, row, pos, prev_stop)

    Token for the end of file (end of the input string).
    EOFToken is a Token object.

    Attributes:
        name      : name of the token
        text      : text matched by the regular expression
        value     : value computed from the text
        line      : line of the token in the input string
        row       : row of the token in the input string
        end_line  : line of the end of the token
        end_row   : row of the end of the token
        start     : position of the start in the input string of the token
        stop      : position of the end in the input string of the token
        prev_stop : position of the end of the previous token
    """

    def __init__(self, line, row, pos, prev_stop):
        Token.__init__(self, "EOF", "EOF", None, line, row, line, row, pos, pos, prev_stop)

class SOFToken(Token):
    """ SOFToken()

    Token for the start of file (start of the input string).
    SOFToken is a Token object.

    Attributes:
        name      : name of the token
        text      : text matched by the regular expression
        value     : value computed from the text
        line      : line of the token in the input string
        row       : row of the token in the input string
        end_line  : line of the end of the token
        end_row   : row of the end of the token
        start     : position of the start in the input string of the token
        stop      : position of the end in the input string of the token
        prev_stop : position of the end of the previous token
    """

    def __init__(self):
        Token.__init__(self, "SOF", "SOF", None, 1, 1, 1, 1, 0, 0, 0)

class Py:
    def __init__(self, level=0):
        frame = sys._getframe(1+level)
        self.globals = frame.f_globals
        self.locals = frame.f_locals
    def __getitem__(self, item):
        return eval(item%self, self.globals, self.locals)


class ParserMetaClass(type):
    """ ParserMetaClass is the metaclass of Parser objects.

    When a ParserMetaClass class in defined, its doc string should contain
    a grammar. This grammar is parsed by TPGParser and the generated code
    is added to the class.
    If the class doesn't have a doc string, nothing is generated
    """

    def __init__(cls, name, bases, dict):
        super(ParserMetaClass, cls).__init__(name, bases, dict)
        try:
            grammar = dict['__doc__']
        except KeyError:
            pass
        else:
            parser = TPGParser(sys._getframe(1).f_globals)
            for attribute, source, code in parser(grammar):
                setattr(cls, attribute, code)

class Parser:
    # Parser is the base class for parsers.
    #
    # This class can not have a doc string otherwise it would be considered as a grammar.
    # The metaclass of this class is ParserMetaClass.
    #
    # Attributes:
    #   lexer : lexer build from the grammar
    #
    # Methods added to the generated parsers:
    #   init_lexer(self) : return a lexer object to scan the tokens defined by the grammar
    #   <rule>           : each rule is translated into a method with the same name

    __metaclass__ = ParserMetaClass

    def __init__(self):
        """ Parser is the base class for parsers.
       
        This class can not have a doc string otherwise it would be considered as a grammar.
        The metaclass of this class is ParserMetaClass.
        
        Attributes:
            lexer : lexer build from the grammar

        Methods added to the generated parsers:
            init_lexer(self) : return a lexer object to scan the tokens defined by the grammar
            <rule>           : each rule is translated into a method with the same name
        """
        self.lexer = self.init_lexer()

    def eat(self, name):
        """ eat the current token if it matches the expected token

        Parameters:
            name : name of the expected token
        """
        token = self.lexer.token()
        if token.match(name):
            self.lexer.next()
            return token.value
        else:
            raise WrongToken

    def eatCSL(self, name):
        """ eat the current token if it matches the expected token

        This method replaces eat for context sensitive lexers.

        Parameters:
            name : name of the expected token
        """
        token = self.lexer.eat(name)
        return token.value

    def __call__(self, input, *args, **kws):
        """ parse a string starting from the default axiom

        The default axiom is START.

        Parameters:
            input : input string to parse
            *args : argument list to pass to START
            **kws : argument dictionnary to pass to START
        """
        return self.parse('START', input, *args, **kws)

    def parse(self, axiom, input, *args, **kws):
        """ parse a string starting from a given axiom

        Parameters:
            axiom : rule name where the parser starts
            input : input string to parse
            *args : argument list to pass to START
            **kws : argument dictionnary to pass to START
        """
        try:
            self.lexer.start(input)
            value = getattr(self, axiom)(*args, **kws)
            if not self.lexer.eof():
                raise WrongToken
        except WrongToken:
            if self.lexer.last_token is None:
                last_token = ""
                line, row = 1, 1
            else:
                last_token = self.lexer.last_token.text
                line, row = self.lexer.last_token.line, self.lexer.last_token.row
            raise SyntacticError((line, row), "Syntax error near %s"%last_token)
        return value

    def line(self, token=None):
        """ return the line number of a token

        Parameters:
            token : token object. If None, the current token line is returned.
        """
        if token is None:
            token = self.lexer.token()
            if token is None:
                return 1
        return token.line

    def row(self, token=None):
        """ return the row number of a token

        Parameters:
            token : token object. If None, the current token row is returned.
        """
        if token is None:
            token =self.lexer.token()
            if token is None:
                return 2
        return token.row

    def mark(self):
        """ return the current token

        This can be used to get the line or row number of a token
        or to extract text between two tokens.
        """
        return self.lexer.token()

    def extract(self, start, stop):
        """ return the text found between two tokens

        Parameters :
            start : token object as returned by mark
            stop  : token object as returned by mark
        """
        return self.lexer[start:stop]

    def check(self, cond):
        """ check a condition and backtrack when it is False

        Parameters:
            cond : condition to be checked
        """
        if not cond:
            raise WrongToken
        return cond

    def error(self, msg):
        """ stop the parser and raise a SemanticError exception

        Parameters:
            msg : error message to raise
        """
        raise SemanticError(msg)

class VerboseParser(Parser):
    # VerboseParser is the base class for debugging parsers.
    #
    # This class can not have a doc string otherwise it would be considered as a grammar.
    # The metaclass of this class is ParserMetaClass.
    # It extends the Parser class to log the activity of the lexer.
    #
    # Attributes:
    #   lexer   : lexer build from the grammar
    #   verbose : level of information
    #               0 : no information
    #               1 : print tokens successfully matched
    #               2 : print tokens matched and not matched
    #
    # Methods added to the generated parsers:
    #   init_lexer(self) : return a lexer object to scan the tokens defined by the grammar
    #   <rule>           : each rule is translated into a method with the same name

    verbose = 1

    def __init__(self):
        """ VerboseParser is the base class for debugging parsers.
       
        This class can not have a doc string otherwise it would be considered as a grammar.
        The metaclass of this class is ParserMetaClass.
        It extends the Parser class to log the activity of the lexer.
       
        Attributes:
          lexer   : lexer build from the grammar
          verbose : level of information
                      0 : no information
                      1 : print tokens successfully matched
                      2 : print tokens matched and not matched
       
        Methods added to the generated parsers:
          init_lexer(self) : return a lexer object to scan the tokens defined by the grammar
          <rule>           : each rule is translated into a method with the same name
        """
        Parser.__init__(self)
        self.eatcnt = 0

    def eat(self, name):
        """ eat the current token if it matches the expected token

        Parameters:
            name : name of the expected token
        """
        self.eatcnt += 1
        token = self.lexer.token()
        try:
            value = Parser.eat(self, name)
            if self.verbose >= 1:
                print self.token_info(token, "==", name)
            return value
        except WrongToken:
            if self.verbose >= 2:
                print self.token_info(token, "!=", name)
            raise

    def eatCSL(self, name):
        """ eat the current token if it matches the expected token

        This method replaces eat for context sensitive lexers.

        Parameters:
            name : name of the expected token
        """
        self.eatcnt += 1
        try:
            value = Parser.eatCSL(self, name)
            if self.verbose >= 1:
                token = self.lexer.token()
                print self.token_info(token, "==", name)
            return value
        except WrongToken:
            if self.verbose >= 2:
                token = Token("???", self.lexer.input[self.lexer.pos:self.lexer.pos+10].replace('\n', ' '), "???", self.lexer.line, self.lexer.row, self.lexer.line, self.lexer.row, self.lexer.pos, self.lexer.pos, self.lexer.pos)
                print self.token_info(token, "!=", name)
            raise

    def parse(self, axiom, input, *args, **kws):
        """ parse a string starting from a given axiom

        Parameters:
            axiom : rule name where the parser starts
            input : input string to parse
            *args : argument list to pass to START
            **kws : argument dictionnary to pass to START
        """
        self.axiom = axiom
        return Parser.parse(self, axiom, input, *args, **kws)

    def token_info(self, token, op, expected):
        """ return information about a token

        Parameters:
            token    : token read by the lexer
            op       : result of the comparison made by the lexer (== or !=)
            expected : name of the expected token
        """
        eatcnt = self.eatcnt
        callernames = []
        stackdepth = 0
        name = None
        while name != self.axiom:
            stackdepth += 1
            name = sys._getframe(stackdepth+1).f_code.co_name
            if len(callernames) < 10:
                callernames.insert(0, name)
        callernames = '.'.join(callernames)
        found = "(%d,%d) %s %s"%(token.line, token.row, token.name, token.text)
        return "[%3d][%2d]%s: %s %s %s"%(eatcnt, stackdepth, callernames, found, op, expected)

blank_line_re = re.compile("^\s*$")
indent_re = re.compile("^\s*")

class tpg:
    """ This class contains some TPG classes to make the parsers usable inside and outside the tpg module
    """
    NamedGroupLexer = NamedGroupLexer
    Lexer = Lexer
    CacheNamedGroupLexer = CacheNamedGroupLexer
    CacheLexer = CacheLexer
    ContextSensitiveLexer = ContextSensitiveLexer
    Parser = Parser
    WrongToken = WrongToken
    re = re

class TPGParser(tpg.Parser):
    __grammar__ = r"""

    # This class parses TPG grammar
    # and generate the Python source and compiled code for the parser

    set lexer = NamedGroupLexer

    separator   spaces      '\s+'                                                       ;
    separator   comment     '#.*'                                                       ;

    token       string      "\"(\\.|[^\"\\]+)*\"|'(\\.|[^'\\]+)*'"                      ;
    token       code        "\{\{(\}?[^\}]+)*\}\}|\$[^\$\n]*\$|\$.*\n([ \t]*\$.*\n)*"   $ self.Code
    token       ident       "\w+"                                                       ;
    token       lcbra       '\{'                                                        ;
    token       rcbra       '\}'                                                        ;
    token       star2       '\*\*'                                                      ;
    token       star        '\*'                                                        ;

    START/$self.gen(options, tokens, rules)$ ->
        OPTIONS/options
        TOKENS/tokens
        RULES/rules
        ;

    OPTIONS/options ->
                                    $ options = self.Options(self)
        (   'set' ident/name
            (   '=' ident/value     $ options.set(name, value)
            |                       $ options.set(name, 'True')
            )
        )*
        ;

    TOKENS/ts ->
                        $ ts = []
        (   TOKEN/t     $ ts.append(t)
        )*
        ;

    TOKEN/$token_type(name, expr, code)$ ->
        (   'separator'     $ token_type = self.DefSeparator
        |   'token'         $ token_type = self.DefToken
        )
        ident/name ':'?
        @t string/expr      $ self.re_check(expr, t)
        (   PY_EXPR/code ';'?
        |   ';'             $ code = None
        )
        ;

    RULES/rs ->
                        $ rs = self.Rules()
        (   RULE/r      $ rs.append(r)
        )*
        ;

    RULE/$self.Rule(head, body)$ -> HEAD/head '->' OR_EXPR/body ';' ;

    HEAD/$self.Symbol(name, args, ret)$ -> ident/name OPT_ARGS/args RET/ret ;

    OR_EXPR/$self.balance(or_expr)$ ->
        AND_EXPR/a                  $ or_expr = [a]
        (   check $ not or_expr[-1].empty() $
            '\|' AND_EXPR/a         $ or_expr.append(a)
        )*
        ;

    AND_EXPR/$and_expr$ ->
                                    $ and_expr = self.And()
        (   ATOM_EXPR/a REP<a>/a    $ and_expr.append(a)
        )*
        ;

    ATOM_EXPR/a ->
            SYMBOL/a
        |   INLINE_TOKEN/a
        |   @t code/a               $ self.code_check(a, t)
        |   '\(' OR_EXPR/a '\)'
        |   'check' PY_EXPR/cond    $ a = self.Check(cond)
        |   'error' PY_EXPR/msg     $ a = self.Error(msg)
        |   '@' PY_EXPR/mark        $ a = self.Mark(mark)
        ;

    REP<a>/a ->
        (   '\*'        $ a = self.Rep(a, 0, None)
        |   '\+'        $ a = self.Rep(a, 1, None)
        |   '\?'        $ a = self.Rep(a, 0, 1)
        |   '\{'
                ( PY_EXPR/min | $ min = self.PY_Ident("0") $ )
                (   ',' ( PY_EXPR/max | $ max = self.PY_Ident("None") $ )
                |   $ max = min $
                )
            '\}'        $ a = self.Rep(a, min, max)
        )?
        ;

    SYMBOL/$self.Symbol(name, args, ret)$ -> ident/name OPT_ARGS/args RET/ret ;

    INLINE_TOKEN/$self.InlineToken(expr, ret)$ ->
        @t string/expr  $ self.re_check(expr, t)
        RET/ret
        ;

    OPT_ARGS/args -> ARGS/args | $ args = self.Args() $ ;

    ARGS/args ->
        '<'                         $ args = self.Args()
            (   ARG/arg             $ args.append(arg)
                (   ',' ARG/arg     $ args.append(arg)
                )*
                ','?
            )?
        '>'
        ;

    ARG/a ->
            ident/name '=' PY_EXPR/a    $ a = self.PY_KeywordArgument(name, a)
        |   PY_EXPR/a                   $ a = self.PY_PositionArgument(a)
        |   '\*' ident/name             $ a = self.PY_PositionArgumentList(name)
        |   '\*\*' ident/name           $ a = self.PY_KeywordArgumentList(name)
        ;

    RET/ret -> '/' PY_EXPR/ret | $ ret = None $ ;

    PY_EXPR/expr ->
            ident/name      $ expr = self.PY_Ident(name)
        |   string/st       $ expr = self.PY_Ident(st)
        |   code/expr
        |   ARGS/expr
        ;
    """

    def init_lexer(self):
        lexer = tpg.NamedGroupLexer(True, 0)
        lexer.def_token('_tok_1', r'set')
        lexer.def_token('_tok_2', r'=')
        lexer.def_token('_tok_3', r'separator')
        lexer.def_token('_tok_4', r'token')
        lexer.def_token('_tok_5', r':')
        lexer.def_token('_tok_6', r';')
        lexer.def_token('_tok_7', r'->')
        lexer.def_token('_tok_8', r'\|')
        lexer.def_token('_tok_9', r'\(')
        lexer.def_token('_tok_10', r'\)')
        lexer.def_token('_tok_11', r'check')
        lexer.def_token('_tok_12', r'error')
        lexer.def_token('_tok_13', r'@')
        lexer.def_token('_tok_14', r'\+')
        lexer.def_token('_tok_15', r'\?')
        lexer.def_token('_tok_16', r',')
        lexer.def_token('_tok_17', r'<')
        lexer.def_token('_tok_18', r'>')
        lexer.def_token('_tok_19', r'/')
        lexer.def_separator('spaces', r'\s+')
        lexer.def_separator('comment', r'#.*')
        lexer.def_token('string', r"\"(\\.|[^\"\\]+)*\"|'(\\.|[^'\\]+)*'")
        lexer.def_token('code', r"\{\{(\}?[^\}]+)*\}\}|\$[^\$\n]*\$|\$.*\n([ \t]*\$.*\n)*", self.Code)
        lexer.def_token('ident', r"\w+")
        lexer.def_token('lcbra', r'\{')
        lexer.def_token('rcbra', r'\}')
        lexer.def_token('star2', r'\*\*')
        lexer.def_token('star', r'\*')
        return lexer

    def START(self, ):
        r""" START -> OPTIONS TOKENS RULES """
        options = self.OPTIONS()
        tokens = self.TOKENS()
        rules = self.RULES()
        return self.gen(options, tokens, rules)

    def OPTIONS(self, ):
        r""" OPTIONS -> ('set' ident ('=' ident | ))* """
        options = self.Options(self)
        while True:
            _p1 = self.lexer.token()
            try:
                self.eat('_tok_1') # 'set'
                name = self.eat('ident')
                _p2 = self.lexer.token()
                try:
                    self.eat('_tok_2') # '='
                    value = self.eat('ident')
                    options.set(name, value)
                except tpg.WrongToken:
                    self.lexer.back(_p2)
                    options.set(name, 'True')
            except tpg.WrongToken:
                self.lexer.back(_p1)
                break
        return options

    def TOKENS(self, ):
        r""" TOKENS -> (TOKEN)* """
        ts = []
        while True:
            _p1 = self.lexer.token()
            try:
                t = self.TOKEN()
                ts.append(t)
            except tpg.WrongToken:
                self.lexer.back(_p1)
                break
        return ts

    def TOKEN(self, ):
        r""" TOKEN -> ('separator' | 'token') ident ':'? string (PY_EXPR ';'? | ';') """
        _p1 = self.lexer.token()
        try:
            self.eat('_tok_3') # 'separator'
            token_type = self.DefSeparator
        except tpg.WrongToken:
            self.lexer.back(_p1)
            self.eat('_tok_4') # 'token'
            token_type = self.DefToken
        name = self.eat('ident')
        _p2 = self.lexer.token()
        try:
            self.eat('_tok_5') # ':'
        except tpg.WrongToken:
            self.lexer.back(_p2)
        t = self.mark()
        expr = self.eat('string')
        self.re_check(expr, t)
        _p3 = self.lexer.token()
        try:
            code = self.PY_EXPR()
            _p4 = self.lexer.token()
            try:
                self.eat('_tok_6') # ';'
            except tpg.WrongToken:
                self.lexer.back(_p4)
        except tpg.WrongToken:
            self.lexer.back(_p3)
            self.eat('_tok_6') # ';'
            code = None
        return token_type(name, expr, code)

    def RULES(self, ):
        r""" RULES -> (RULE)* """
        rs = self.Rules()
        while True:
            _p1 = self.lexer.token()
            try:
                r = self.RULE()
                rs.append(r)
            except tpg.WrongToken:
                self.lexer.back(_p1)
                break
        return rs

    def RULE(self, ):
        r""" RULE -> HEAD '->' OR_EXPR ';' """
        head = self.HEAD()
        self.eat('_tok_7') # '->'
        body = self.OR_EXPR()
        self.eat('_tok_6') # ';'
        return self.Rule(head, body)

    def HEAD(self, ):
        r""" HEAD -> ident OPT_ARGS RET """
        name = self.eat('ident')
        args = self.OPT_ARGS()
        ret = self.RET()
        return self.Symbol(name, args, ret)

    def OR_EXPR(self, ):
        r""" OR_EXPR -> AND_EXPR ('\|' AND_EXPR)* """
        a = self.AND_EXPR()
        or_expr = [a]
        while True:
            _p1 = self.lexer.token()
            try:
                self.check(not or_expr[-1].empty())
                self.eat('_tok_8') # '\|'
                a = self.AND_EXPR()
                or_expr.append(a)
            except tpg.WrongToken:
                self.lexer.back(_p1)
                break
        return self.balance(or_expr)

    def AND_EXPR(self, ):
        r""" AND_EXPR -> (ATOM_EXPR REP)* """
        and_expr = self.And()
        while True:
            _p1 = self.lexer.token()
            try:
                a = self.ATOM_EXPR()
                a = self.REP(a)
                and_expr.append(a)
            except tpg.WrongToken:
                self.lexer.back(_p1)
                break
        return and_expr

    def ATOM_EXPR(self, ):
        r""" ATOM_EXPR -> SYMBOL | INLINE_TOKEN | code | '\(' OR_EXPR '\)' | 'check' PY_EXPR | 'error' PY_EXPR | '@' PY_EXPR """
        _p1 = self.lexer.token()
        try:
            try:
                a = self.SYMBOL()
            except tpg.WrongToken:
                self.lexer.back(_p1)
                try:
                    a = self.INLINE_TOKEN()
                except tpg.WrongToken:
                    self.lexer.back(_p1)
                    t = self.mark()
                    a = self.eat('code')
                    self.code_check(a, t)
        except tpg.WrongToken:
            self.lexer.back(_p1)
            try:
                try:
                    self.eat('_tok_9') # '\('
                    a = self.OR_EXPR()
                    self.eat('_tok_10') # '\)'
                except tpg.WrongToken:
                    self.lexer.back(_p1)
                    self.eat('_tok_11') # 'check'
                    cond = self.PY_EXPR()
                    a = self.Check(cond)
            except tpg.WrongToken:
                self.lexer.back(_p1)
                try:
                    self.eat('_tok_12') # 'error'
                    msg = self.PY_EXPR()
                    a = self.Error(msg)
                except tpg.WrongToken:
                    self.lexer.back(_p1)
                    self.eat('_tok_13') # '@'
                    mark = self.PY_EXPR()
                    a = self.Mark(mark)
        return a

    def REP(self, a):
        r""" REP -> ('\*' | '\+' | '\?' | '\{' (PY_EXPR | ) (',' (PY_EXPR | ) | ) '\}')? """
        _p1 = self.lexer.token()
        try:
            try:
                try:
                    self.eat('star') # '\*'
                    a = self.Rep(a, 0, None)
                except tpg.WrongToken:
                    self.lexer.back(_p1)
                    self.eat('_tok_14') # '\+'
                    a = self.Rep(a, 1, None)
            except tpg.WrongToken:
                self.lexer.back(_p1)
                try:
                    self.eat('_tok_15') # '\?'
                    a = self.Rep(a, 0, 1)
                except tpg.WrongToken:
                    self.lexer.back(_p1)
                    self.eat('lcbra') # '\{'
                    _p2 = self.lexer.token()
                    try:
                        min = self.PY_EXPR()
                    except tpg.WrongToken:
                        self.lexer.back(_p2)
                        min = self.PY_Ident("0") 
                    _p3 = self.lexer.token()
                    try:
                        self.eat('_tok_16') # ','
                        _p4 = self.lexer.token()
                        try:
                            max = self.PY_EXPR()
                        except tpg.WrongToken:
                            self.lexer.back(_p4)
                            max = self.PY_Ident("None") 
                    except tpg.WrongToken:
                        self.lexer.back(_p3)
                        max = min 
                    self.eat('rcbra') # '\}'
                    a = self.Rep(a, min, max)
        except tpg.WrongToken:
            self.lexer.back(_p1)
        return a

    def SYMBOL(self, ):
        r""" SYMBOL -> ident OPT_ARGS RET """
        name = self.eat('ident')
        args = self.OPT_ARGS()
        ret = self.RET()
        return self.Symbol(name, args, ret)

    def INLINE_TOKEN(self, ):
        r""" INLINE_TOKEN -> string RET """
        t = self.mark()
        expr = self.eat('string')
        self.re_check(expr, t)
        ret = self.RET()
        return self.InlineToken(expr, ret)

    def OPT_ARGS(self, ):
        r""" OPT_ARGS -> ARGS |  """
        _p1 = self.lexer.token()
        try:
            args = self.ARGS()
        except tpg.WrongToken:
            self.lexer.back(_p1)
            args = self.Args() 
        return args

    def ARGS(self, ):
        r""" ARGS -> '<' (ARG (',' ARG)* ','?)? '>' """
        self.eat('_tok_17') # '<'
        args = self.Args()
        _p1 = self.lexer.token()
        try:
            arg = self.ARG()
            args.append(arg)
            while True:
                _p2 = self.lexer.token()
                try:
                    self.eat('_tok_16') # ','
                    arg = self.ARG()
                    args.append(arg)
                except tpg.WrongToken:
                    self.lexer.back(_p2)
                    break
            _p3 = self.lexer.token()
            try:
                self.eat('_tok_16') # ','
            except tpg.WrongToken:
                self.lexer.back(_p3)
        except tpg.WrongToken:
            self.lexer.back(_p1)
        self.eat('_tok_18') # '>'
        return args

    def ARG(self, ):
        r""" ARG -> ident '=' PY_EXPR | PY_EXPR | '\*' ident | '\*\*' ident """
        _p1 = self.lexer.token()
        try:
            try:
                name = self.eat('ident')
                self.eat('_tok_2') # '='
                a = self.PY_EXPR()
                a = self.PY_KeywordArgument(name, a)
            except tpg.WrongToken:
                self.lexer.back(_p1)
                a = self.PY_EXPR()
                a = self.PY_PositionArgument(a)
        except tpg.WrongToken:
            self.lexer.back(_p1)
            try:
                self.eat('star') # '\*'
                name = self.eat('ident')
                a = self.PY_PositionArgumentList(name)
            except tpg.WrongToken:
                self.lexer.back(_p1)
                self.eat('star2') # '\*\*'
                name = self.eat('ident')
                a = self.PY_KeywordArgumentList(name)
        return a

    def RET(self, ):
        r""" RET -> '/' PY_EXPR |  """
        _p1 = self.lexer.token()
        try:
            self.eat('_tok_19') # '/'
            ret = self.PY_EXPR()
        except tpg.WrongToken:
            self.lexer.back(_p1)
            ret = None 
        return ret

    def PY_EXPR(self, ):
        r""" PY_EXPR -> ident | string | code | ARGS """
        _p1 = self.lexer.token()
        try:
            try:
                name = self.eat('ident')
                expr = self.PY_Ident(name)
            except tpg.WrongToken:
                self.lexer.back(_p1)
                st = self.eat('string')
                expr = self.PY_Ident(st)
        except tpg.WrongToken:
            self.lexer.back(_p1)
            try:
                expr = self.eat('code')
            except tpg.WrongToken:
                self.lexer.back(_p1)
                expr = self.ARGS()
        return expr


    def __init__(self, _globals=None):
        Parser.__init__(self)
        if _globals is not None:
            self.env = _globals
        else:
            self.env = {}

    def re_check(self, expr, tok):
        try:
            sre_parse.parse(eval('r'+expr))
        except Exception, e:
            raise LexicalError((tok.line, tok.row), "Invalid regular expression: %s (%s)"%(expr, e))

    def code_check(self, code, tok):
        try:
            parser.suite(code.code)
        except Exception, e:
            erroneous_code = "\n".join([ "%2d: %s"%(i+1, l) for (i, l) in enumerate(code.code.splitlines()) ])
            raise LexicalError((tok.line, tok.row), "Invalid Python code (%s): \n%s"%(e, erroneous_code))

    class Options:
        option_dict = {
        #   Option name          Accepted values                                        Default value
            'lexer':            ({'NamedGroupLexer': NamedGroupLexer,
                                  'Lexer': Lexer,
                                  'CacheNamedGroupLexer': CacheNamedGroupLexer,
                                  'CacheLexer': CacheLexer,
                                  'ContextSensitiveLexer': ContextSensitiveLexer,
                                 },                                                     'NamedGroupLexer'),
            'word_boundary':    ({'True': True, 'False': False},                        'True'),
            #'indent':           ({'True': True, 'False': False},                        'False'),
            'lexer_ignorecase': ({'True': "IGNORECASE", 'False': False},                'False'),
            'lexer_locale':     ({'True': "LOCALE",     'False': False},                'False'),
            'lexer_multiline':  ({'True': "MULTILINE",  'False': False},                'False'),
            'lexer_dotall':     ({'True': "DOTALL",     'False': False},                'False'),
            'lexer_verbose':    ({'True': "VERBOSE",    'False': False},                'False'),
            'lexer_unicode':    ({'True': "UNICODE",    'False': False},                'False'),
        }
        def __init__(self, parser):
            self.parser = parser
            for name, (values, default) in TPGParser.Options.option_dict.items():
                self.set(name, default)
        def set(self, name, value):
            try:
                options, default = TPGParser.Options.option_dict[name]
            except KeyError:
                opts = TPGParser.Options.option_dict.keys()
                opts.sort()
                self.parser.error("Unknown option (%s). Valid options are %s"%(name, ', '.join(opts)))
            try:
                value = options[value]
            except KeyError:
                values = options.keys()
                values.sort()
                self.parser.error("Unknown value (%s). Valid values for %s are %s"%(value, name, ', '.join(values)))
            setattr(self, name, value)
        def lexer_compile_options(self):
            options = [ self.lexer_ignorecase,
                        self.lexer_locale,
                        self.lexer_multiline,
                        self.lexer_dotall,
                        self.lexer_verbose,
                        self.lexer_unicode,
                      ]
            return "+".join([ "tpg.re.%s"%opt for opt in options if opt ]) or 0

    class Empty:
        def empty(self):
            return True

    class NotEmpty:
        def empty(self):
            return False

    class Code(NotEmpty):
        def __init__(self, code):
            if code.startswith('$'):
                if code.endswith('$'):
                    lines = code[1:-1].splitlines()
                else:
                    lines = [line.split('$', 1)[1] for line in code.splitlines()]
            elif code.startswith('{{') and code.endswith('}}'):
                lines = code[2:-2].splitlines()
            else:
                raise WrongToken
            while lines and blank_line_re.match(lines[0]): lines.pop(0)
            while lines and blank_line_re.match(lines[-1]): lines.pop(-1)
            if lines:
                indents = [len(indent_re.match(line).group(0)) for line in lines]
                indent = indents[0]
                if min(indents) < indent:
                    # Indentation incorrecte
                    raise WrongToken
                lines = [line[indent:] for line in lines]
            self.code = "".join([line+"\n" for line in lines])
        def get_inline_tokens(self):
            return
            yield None
        def gen_code(self, indent=None, counters=None, pos=None):
            if indent is None:
                return self.code.strip()
            else:
                return [indent+line for line in self.code.splitlines()]
        def links_symbols_to_tokens(self, tokens):
            pass
        def gen_doc(self, parent):
            return ""

    class DefToken:
        def_method = "def_token"
        def __init__(self, name, expr, code=None):
            self.name = name
            self.expr = expr
            if code is not None and code.gen_code().count('\n') > 1:
                raise WrongToken
            self.code = code
        def gen_def(self):
            expr = self.expr
            if self.code is None:
                return "lexer.%s('%s', r%s)"%(self.def_method, self.name, expr)
            else:
                code = self.code.gen_code().strip()
                return "lexer.%s('%s', r%s, %s)"%(self.def_method, self.name, expr, code)

    class DefSeparator(DefToken):
        def_method = "def_separator"

    class Rules(list):
        def get_inline_tokens(self):
            for rule in self:
                for token in rule.get_inline_tokens():
                    yield token
        def links_symbols_to_tokens(self, tokens):
            for rule in self:
                rule.links_symbols_to_tokens(tokens)
        def gen_code(self):
            for rule in self:
                yield rule.gen_code()

    class Rule:
        class Counters(dict):
            def __call__(self, name):
                n = self.get(name, 1)
                self[name] = n+1
                return "_%s%s"%(name, n)
        def __init__(self, head, body):
            self.head = head
            self.body = body
        def get_inline_tokens(self):
            for token in self.body.get_inline_tokens():
                yield token
        def links_symbols_to_tokens(self, tokens):
            if self.head.name in tokens:
                raise SemanticError("%s is both a token and a symbol"%self.head.name)
            else:
                self.body.links_symbols_to_tokens(tokens)
        def gen_code(self):
            counters = self.Counters()
            return self.head.name, [
                self.head.gen_def(),
                tab + 'r""" %s -> %s """'%(self.head.gen_doc(self), self.body.gen_doc(self)),
                self.body.gen_code(tab, counters, None),
                self.head.gen_ret(tab),
            ]

    class Symbol(NotEmpty):
        def __init__(self, name, args, ret):
            self.name = name
            self.args = args
            self.ret = ret
        def get_inline_tokens(self):
            return
            yield None
        def links_symbols_to_tokens(self, tokens):
            self.token = tokens.get(self.name, None)
            if self.token is not None and self.args:
                raise SemanticError("Token %s can not have arguments"%self.name)
        def gen_def(self):
            return "def %s(self, %s):"%(self.name, self.args.gen_code())
        def gen_ret(self, indent):
            return self.ret and indent + "return %s"%self.ret.gen_code() or ()
        def gen_code(self, indent, counters, pos):
            if self.token is not None:
                if self.ret is not None:
                    return indent + "%s = self.eat('%s')"%(self.ret.gen_code(), self.token.name)
                else:
                    return indent + "self.eat('%s')"%(self.token.name)
            else:
                if self.ret is not None:
                    return indent + "%s = self.%s(%s)"%(self.ret.gen_code(), self.name, self.args.gen_code())
                else:
                    return indent + "self.%s(%s)"%(self.name, self.args.gen_code())
        def gen_doc(self, parent):
            return self.name

    class InlineToken(NotEmpty):
        def __init__(self, expr, ret):
            self.expr = expr
            self.ret = ret
        def get_inline_tokens(self):
            yield self
        def set_explicit_token(self, token):
            self.explicit_token = token
        def gen_def(self):
            return self.explicit_token.gen_def()
        def links_symbols_to_tokens(self, tokens):
            pass
        def gen_code(self, indent, counters, pos):
            if self.ret is not None:
                return indent + "%s = self.eat('%s') # %s"%(self.ret.gen_code(), self.explicit_token.name, self.expr)
            else:
                return indent + "self.eat('%s') # %s"%(self.explicit_token.name, self.expr)
        def gen_doc(self, parent):
            return self.expr

    class Args(list):
        def gen_code(self):
            return ", ".join([a.gen_code() for a in self])

    class PY_PositionArgument:
        def __init__(self, arg):
            self.arg = arg
        def gen_code(self):
            return self.arg.gen_code()

    class PY_KeywordArgument:
        def __init__(self, name, arg):
            self.name = name
            self.arg = arg
        def gen_code(self):
            return "%s=%s"%(self.name, self.arg.gen_code())

    class PY_PositionArgumentList:
        def __init__(self, name):
            self.name = name
        def gen_code(self):
            return "*%s"%self.name

    class PY_KeywordArgumentList:
        def __init__(self, name):
            self.name = name
        def gen_code(self):
            return "**%s"%self.name

    class And(list):
        def empty(self):
            for a in self:
                if not a.empty():
                    return False
            return True
        def get_inline_tokens(self):
            for a in self:
                for token in a.get_inline_tokens():
                    yield token
        def links_symbols_to_tokens(self, tokens):
            for a in self:
                a.links_symbols_to_tokens(tokens)
        def gen_code(self, indent, counters, pos):
            return self and [
                self[0].gen_code(indent, counters, pos),
                [a.gen_code(indent, counters, None) for a in self[1:]],
            ]
        def gen_doc(self, parent):
            docs = []
            for a in self:
                doc = a.gen_doc(self)
                if doc:
                    docs.append(doc)
            return " ".join(docs)

    class Or(NotEmpty):
        def __init__(self, a, b):
            self.a = a
            self.b = b
        def get_inline_tokens(self):
            for token in self.a.get_inline_tokens():
                yield token
            for token in self.b.get_inline_tokens():
                yield token
        def links_symbols_to_tokens(self, tokens):
            self.a.links_symbols_to_tokens(tokens)
            self.b.links_symbols_to_tokens(tokens)
        def gen_code(self, indent, counters, pos):
            p = pos or counters("p")
            return [
                pos is None and indent + "%s = self.lexer.token()"%p or (),
                indent + "try:",
                self.a.gen_code(indent+tab, counters, p),
                indent + "except tpg.WrongToken:",
                indent + tab + "self.lexer.back(%s)"%p,
                self.b.gen_code(indent+tab, counters, p),
            ]
        def gen_doc(self, parent):
            doc = "%s | %s"%(self.a.gen_doc(self), self.b.gen_doc(self))
            if isinstance(parent, TPGParser.And) and len(parent) > 1:
                doc = "(%s)"%doc
            return doc

    def balance(self, xs):
        if len(xs) == 1:
            return xs[0]
        else:
            m = len(xs)//2
            return self.Or(self.balance(xs[:m]), self.balance(xs[m:]))

    class Rep(NotEmpty):
        def __init__(self, a, min, max):
            self.a = a
            self.min = min
            self.max = max
        def get_inline_tokens(self):
            for token in self.a.get_inline_tokens():
                yield token
        def links_symbols_to_tokens(self, tokens):
            self.a.links_symbols_to_tokens(tokens)
        def gen_code(self, indent, counters, pos):
            # A?
            if (self.min, self.max) == (0, 1):
                p = pos or counters("p")
                return [
                    pos is None and indent + "%s = self.lexer.token()"%p or (),
                    indent + "try:",
                    self.a.gen_code(indent+tab, counters, p),
                    indent + "except tpg.WrongToken:",
                    indent + tab + "self.lexer.back(%s)"%p,
                ]
            # A*
            elif (self.min, self.max) == (0, None):
                p = pos or counters("p")
                return [
                    indent + "while True:",
                    indent + tab + "%s = self.lexer.token()"%p,
                    indent + tab + "try:",
                    self.a.gen_code(indent+tab+tab, counters, p),
                    indent + tab + "except tpg.WrongToken:",
                    indent + tab + tab + "self.lexer.back(%s)"%p,
                    indent + tab + tab + "break",
                ]
            # A+
            elif (self.min, self.max) == (1, None):
                p = pos or counters("p")
                n = counters("n")
                return [
                    indent + "%s = 0"%n,
                    indent + "while True:",
                    indent + tab + "%s = self.lexer.token()"%p,
                    indent + tab + "try:",
                    self.a.gen_code(indent+tab+tab, counters, p),
                    indent + tab + tab + "%s += 1"%n,
                    indent + tab + "except tpg.WrongToken:",
                    indent + tab + tab + "if %s < 1: raise"%n,
                    indent + tab + tab + "self.lexer.back(%s)"%p,
                    indent + tab + tab + "break",
                ]
            # A{min, max}
            else:
                p = pos or counters("p")
                n = counters("n")
                min = self.min.gen_code()
                max = self.max.gen_code()
                return [
                    indent + "%s = 0"%n,
                    indent + "while %s:"%(max=="None" and "True" or "%s < %s"%(n, max)),
                    indent + tab + "%s = self.lexer.token()"%p,
                    indent + tab + "try:",
                    self.a.gen_code(indent+tab+tab, counters, p),
                    indent + tab + tab + "%s += 1"%n,
                    indent + tab + "except tpg.WrongToken:",
                    indent + tab + tab + "if %s < %s: raise"%(n, min),
                    indent + tab + tab + "self.lexer.back(%s)"%p,
                    indent + tab + tab + "break",
                ]
        def gen_doc(self, parent):
            doc = self.a.gen_doc(self)
            if isinstance(self.a, (TPGParser.And, TPGParser.Or)):
                doc = "(%s)"%doc
            if (self.min, self.max) == (0, 1):
                rep = "?"
            elif (self.min, self.max) == (0, None):
                rep = "*"
            elif (self.min, self.max) == (1, None):
                rep = "+"
            else:
                min = self.min.gen_code()
                max = self.max.gen_code()
                if min == max:
                    rep = "{%s}"%min
                else:
                    if min == "0": min = ""
                    if max == "None": max = ""
                    rep = "{%s,%s}"%(min, max)
            return "%s%s"%(doc, rep)

    class Check(NotEmpty):
        def __init__(self, cond):
            self.cond = cond
        def get_inline_tokens(self):
            return
            yield None
        def links_symbols_to_tokens(self, tokens):
            pass
        def gen_doc(self, parent):
            return ""
        def gen_code(self, indent, counters, pos):
            return indent + "self.check(%s)"%self.cond.gen_code()

    class Error(NotEmpty):
        def __init__(self, msg):
            self.msg = msg
        def get_inline_tokens(self):
            return
            yield None
        def links_symbols_to_tokens(self, tokens):
            pass
        def gen_doc(self, parent):
            return ""
        def gen_code(self, indent, counters, pos):
            return indent + "self.error(%s)"%self.msg.gen_code()

    class Mark(NotEmpty):
        def __init__(self, mark):
            self.mark = mark
        def get_inline_tokens(self):
            return
            yield None
        def links_symbols_to_tokens(self, tokens):
            pass
        def gen_doc(self, parent):
            return ""
        def gen_code(self, indent, counters, pos):
            return indent + "%s = self.mark()"%self.mark.gen_code()

    class PY_Ident(str):
        def gen_code(self):
            return str(self)

    def flatten_nl(self, *lines):
        for sublines in lines:
            if isinstance(sublines, (list, tuple)):
                for line in self.flatten_nl(*sublines):
                    yield line
            else:
                yield sublines + "\n"

    def make_code(self, attribute, *source):
        source = "".join(self.flatten_nl(*source))
        local_namespace = {}
        exec source in self.env, local_namespace
        code = local_namespace[attribute]
        return attribute, source, code

    def gen(self, options, tokens, rules):
        # building the lexer
        lexer = options.lexer
        word_bounded = options.word_boundary
        lexer_options = options.lexer_compile_options()
        explicit_tokens = {}
        for token in tokens:
            explicit_tokens[token.expr[1:-1]] = token
        token_number = 0
        inline_tokens = []
        for token in rules.get_inline_tokens():
            try:
                # If the token was already defined just link it to the first definition
                token.set_explicit_token(explicit_tokens[token.expr[1:-1]])
            except KeyError:
                # Otherwise create an explicit definition for the new inline token
                token_number += 1
                token.set_explicit_token(self.DefToken("_tok_%s"%token_number, token.expr))
                explicit_tokens[token.expr[1:-1]] = token.explicit_token
                inline_tokens.append(token)
        yield self.make_code("init_lexer",
            "def init_lexer(self):",
            lexer is ContextSensitiveLexer and [tab + "self.eat = self.eatCSL"] or (),
            tab + "lexer = tpg.%s(%s, %s)"%(lexer.__name__, word_bounded, lexer_options),
            [ tab + tok.gen_def() for tok in inline_tokens ],
            [ tab + tok.gen_def() for tok in tokens ],
            tab + "return lexer",
        )
        # building the parser
        tokens_from_name = {}
        for token in inline_tokens:
            tokens_from_name[token.explicit_token.name] = token
        for token in tokens:
            tokens_from_name[token.name] = token
        rules.links_symbols_to_tokens(tokens_from_name)
        for name, code in rules.gen_code():
            yield self.make_code(name, *code)

