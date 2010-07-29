#ifndef ABC_H
#define ABC_H

#include "mcrl2/atermpp/aterm.h"
#include <iostream>
#include <vector>

// global variables
extern ATerm ABC_tree;                    // the parse tree
extern ATermIndexedSet ABC_protect_table; // table to protect parsed ATerms

// global functions
int ABC_lex();
void ABC_error(const char *s);
ATerm ABC_parse(std::vector<std::istream*> &streams, bool print_parse_errors);

#endif // ABC_H
