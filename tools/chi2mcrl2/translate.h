#ifndef TRANSLATE_H
#define TRANSLATE_H

#include <string.h>
#include <math.h>
#include <iostream>
#include <aterm2.h>
#include "print/messaging.h"
#include "mcrl2/utilities/aterm_ext.h"
#include <cstdio>
#include <vector>
#include <map>
#include <sstream>

#define RPV RecProcessVariable
#define RVT RecVariableType
#define RAT RecActionTransition

typedef struct
  {
    std::string Name;
    std::string Type; 
    std::string InitValue;
  } RecProcessVariable;

typedef struct
   {
     std::string Name;
     std::string Type;
   } RecVariableType;


typedef struct
  {
    int state;
    std::string guard;
    std::string action;
    std::map<std::string, std::string> vectorUpdate;
    int nextstate;
  } RecActionTransition;

template <class T>
inline std::string to_string (const T& t)
{
  std::stringstream ss;
  ss << t;
  return ss.str();
}

class CAsttransform
{
  public:
  	bool translator(ATermAppl ast);
    std::string getResult();
  private:
	std::string manipulateProcess(ATermAppl input);
//    std::string manipulateVariables();
//	string create_model();
    std::string mcrl2_result;
    bool StrcmpIsFun(const char* str, ATermAppl aterm);
    std::string variable_prefix;
    int scope_level;
    std::vector<RVT> manipulateDeclaredProcessDefinition(ATermAppl input);
    std::vector<RVT> manipulateDeclaredProcessVariables(ATermList input);

    std::vector<RPV> manipulateProcessSpecification(ATermAppl input);
    std::vector<RPV> manipulateProcessVariableDeclarations(ATermList input); 
    
    std::vector<std::string> getVariablesNamesFromList(ATermList input);
    std::vector<RVT> manipulateDeclaredProcessVariable(ATermAppl input); 
    std::string manipulateExpression(ATermAppl input);
    int manipulateStatements(ATermAppl input, int current, int next);


    //Future implementation
    std::string manipulateExplicitTemplates(ATermList input);
    std::string manipulateDeclaredProcessChannels(ATermList input);
    std::map<std::string, RecProcessVariable> ProcessVariableMap;

    std::vector<RAT> transitionSystem ;

    int statementorder;
    int statementlevel;

    std::vector<int> bypass;
}
;

#endif
