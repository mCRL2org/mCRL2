#ifndef TRANSLATE_H
#define TRANSLATE_H

#include <string.h>
#include <math.h>
#include <iostream>
#include <aterm2.h>
#include "print/messaging.h"
#include "mcrl2/utilities/aterm_ext.h"
#include <cstdio>

class CAsttransform
{
  public:
  	bool translator(ATermAppl ast);
  private:
	std::string manipulateProcess(ATermAppl input);
//    std::string manipulateVariables();
//	string create_model();
    std::string mcrl2_result;
    bool StrcmpIsFun(const char* str, ATermAppl aterm);
    std::string variable_prefix;
    int scope_level;
    std::string manipulateDeclaredProcessVariables(ATermList input);
    std::string manipulateDeclaredProcessDefinition(ATermAppl input);
    std::string manipulateDeclaredProcessVariable(ATermAppl input); 

    std::string getVariablesNamesFromList(ATermList input);

    //Future implementation
    std::string manipulateExplicitTemplates(ATermList input);
    std::string manipulateDeclaredProcessChannels(ATermList input);
};

#endif
