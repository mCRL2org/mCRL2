// Author(s): A.J. (Hannes) Pretorius
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./parser.h

#ifndef PARSER_H
#define PARSER_H

#include <cstddef>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include "colleague.h"
#include "diagram.h"
#include "dof.h"
#include "graph.h"
#include "tinyxml.h"
#include "utils.h"

template <class T>
inline std::string to_string (const T& t)
{
  std::stringstream ss;
  ss << t;
  return ss.str();
}

class Mediator;

class Parser : public Colleague
{
public:
    // -- constructors and destructor -------------------------------
    Parser( Mediator* m );
    virtual ~Parser();

    // -- parsing functions -----------------------------------------
    int getFileSize( const std::string &path );
    
    // fsm files
    void parseFSMFile( 
        const std::string &path,
        Graph* graph );
    void writeFSMFile(
        const std::string &path,
        Graph* graph );

    // dgc files
    void parseAttrConfig(
        const std::string &path,
        Graph* graph,
        std::map< int, int > &attrIdxFrTo,
        std::map< int, std::vector< std::string > > &attrCurDomains,
        std::map< int, std::map< int, int  > > &attrOrigToCurDomains );
    void writeAttrConfig(
        const std::string &path,
        Graph* graph );

    // dgd files
    void parseDiagram(
        const std::string &path,
        Graph* graph,
        Diagram* dgrmOld,
        Diagram* dgrmNew );
    void writeDiagram(
        const std::string &path,
        Graph* graph,
        Diagram* diagram );
            
private:
    // -- private utility functions ---------------------------------
    // fsm files
    void parseStateVarDescr( 
        const std::string &nextLine,
        Graph* graph );
    void parseStates( 
        const std::string &nextLine,
        Graph* graph );
    void parseTransitions( 
        const std::string &nextLine,
        Graph* graph );

    // dgc files
    void parseAttrConfig(
        Graph* graph,
        std::map< int, int > &attrIdxFrTo,
        std::map< int, std::vector< std::string > > &attrCurDomains,
        std::map< int, std::map< int, int  > > &attrOrigToCurDomains,
        TiXmlElement* curNode );
    void parseAttr(
        Graph* graph,
        std::map< int, int > &attrIdxFrTo,
        std::map< int, std::vector< std::string > > &attrCurDomains,
        std::map< int, std::map< int, int  > > &attrOrigToCurDomains,
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
    std::string delims;
};

#endif

// -- end -----------------------------------------------------------
