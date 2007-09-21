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
#include <list>
#include <set>
#include "CArray.h"
#include <stack>

#define RPV RecProcessVariable
#define RVT RecVariableType
#define RAT RecActionTransition
#define RS RecStreams
#define RSP RecStreamPos
#define RPI RecParenthesisInfo

typedef CArray<int> IntArray ; 

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
     int holdsForState;
   } RecStreams;

typedef struct
   {
     int stream;
     int position;
   } RecStreamPos;

typedef struct
   {
     std::set<int> endstates;
     bool looped;
     bool guardedloop;
     int  begin_state;
     int  end_state;
   } RecParenthesisInfo;


typedef struct
  {
    int state;
    int stream;
    bool terminate;
    int parenthesis_level;
    bool looped_state;
    bool guardedloop;
  //  std::vector<RSP> proceedAfterSteams;
    std::set<int> procedingStreams;
    int proceedStreamState;
    std::string guard;
    std::string action;
    std::map<std::string, std::string> vectorUpdate; // First:  Identifier Variable
                                                     // Second: Expression
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
    int parenthesis_level;
    std::vector<RVT> manipulateDeclaredProcessDefinition(ATermAppl input);
    std::vector<RVT> manipulateDeclaredProcessVariables(ATermList input);

    std::vector<RPV> manipulateProcessSpecification(ATermAppl input);
    std::vector<RPV> manipulateProcessVariableDeclarations(ATermList input); 
    
    std::vector<std::string> getVariablesNamesFromList(ATermList input);
    std::vector<RVT> manipulateDeclaredProcessVariable(ATermAppl input); 
    std::string manipulateExpression(ATermAppl input);
    void manipulateStatements(ATermAppl input);
    std::map<std::string, std::string> manipulateAssignmentStat(ATermList input_id, ATermList input_exp);
    std::vector<std::string> getExpressionsFromList(ATermList input);
    bool onlyIdentifiersInExpression(ATermList input );
    std::map<int, std::set<int> > affectedStreamMap;

    //Future implementation
    std::string manipulateExplicitTemplates(ATermList input);
    std::string manipulateDeclaredProcessChannels(ATermList input);
    std::map<std::string, RecProcessVariable> ProcessVariableMap;

    int max_stream_lvl;  //Maximal number of streams
    int local_max_stream_lvl;  //Maximal number of streams
    int stream_lvl;      //Variable to indicate the number of steams
    int concurrent_streams; 

    IntArray streams_per_parenthesis_level;
    std::map<int, std::set<int> > bypass_per_parenthesis_level;

    std::map<int, std::list<int> >postProcessVector;
    std::map<int, int> postProcessGuards;

    std::vector<RAT> transitionSystem;

    std::vector<RS> holdsForStreamVector;

    int statementorder;
    int statementlevel;

    bool terminate;  //terminate per parenthesis level
    int state;
    int next_state;

    //Begin_state: used to deterime the beginstates per parenthesis level
    std::map<int, int> begin_state; //first:  parenthesis level
                                    //second: begin state
    std::map<int, int> end_state;   //first:  parenthesis level
                                    //second: end state

    std::map<int, std::set<int> > endstates_per_parenthesis_level;
    std::map<int, std::vector<RPI>  > info_per_parenthesis_level_per_parenthesis;
 
//    std::map<int, std::vector<bool> > looped_parenthesis_level_per_parenthesis;
//    std::map<int, std::vector<int> > begin_state_parenthesis_level_per_parenthesis;
 
    int determineEndState(std::set<int> org_set, int lvl);
    bool loop;
    bool guardedloop; 
    std::string guardedStarExpression;
    int guardedStarBeginState;
    // std::vector<int> bypass;
}
;


#endif
