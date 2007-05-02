// --- parser.h -----------------------------------------------------
// (c) 2006  -  A.J. Pretorius  -  Eindhoven University of Technology
// ---------------------------  *  ----------------------------------

#ifndef PARSER_H
#define PARSER_H

#include <cstddef>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>
using namespace std;
#include "colleague.h"
#include "diagram.h"
#include "dof.h"
#include "graph.h"
#include "tinyxml.h"
#include "utils.h"

class Mediator;

class Parser : public Colleague
{
public:
    // -- constructors and destructor -------------------------------
    Parser( Mediator* m );
    virtual ~Parser();

    // -- parsing functions -----------------------------------------
    int getFileSize( const string &path );
    
    // fsm files
    void parseFSMFile( 
        const string &path,
        Graph* graph );
    void writeFSMFile(
        const string &path,
        Graph* graph );

    // dgc files
    void parseAttrConfig(
        const string &path,
        Graph* graph,
        map< int, int > &attrIdxFrTo,
        map< int, vector< string > > &attrCurDomains,
        map< int, map< int, int  > > &attrOrigToCurDomains );
    void writeAttrConfig(
        const string &path,
        Graph* graph );

    // dgd files
    void parseDiagram(
        const string &path,
        Graph* graph,
        Diagram* dgrmOld,
        Diagram* dgrmNew );
    void writeDiagram(
        const string &path,
        Graph* graph,
        Diagram* diagram );
            
private:
    // -- private utility functions ---------------------------------
    // fsm files
    void parseStateVarDescr( 
        const string &nextLine,
        Graph* graph );
    void parseStates( 
        const string &nextLine,
        Graph* graph );
    void parseTransitions( 
        const int &lineNumber,
        const string &nextLine,
        Graph* graph );

    // dgc files
    void parseAttrConfig(
        Graph* graph,
        map< int, int > &attrIdxFrTo,
        map< int, vector< string > > &attrCurDomains,
        map< int, map< int, int  > > &attrOrigToCurDomains,
        TiXmlElement* curNode );
    void parseAttr(
        Graph* graph,
        map< int, int > &attrIdxFrTo,
        map< int, vector< string > > &attrCurDomains,
        map< int, map< int, int  > > &attrOrigToCurDomains,
        TiXmlElement* curNode );

    // dgd files
    void parseDiagram(
        Graph* graph,
        Diagram* dgrmOld,
        Diagram* dgrmNew,
        TiXmlElement* curNode );
    void parseShape(
        Graph* graph,
        Diagram* dgrmOld,
        Diagram* dgrmNew,
        TiXmlElement* curNode );

    // -- data members ----------------------------------------------
    string delims;
};

#endif

// -- end -----------------------------------------------------------
