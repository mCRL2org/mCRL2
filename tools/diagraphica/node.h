// --- node.h -------------------------------------------------------
// (c) 2007  -  A.J. Pretorius  -  Eindhoven University of Technology
// ---------------------------  *  ----------------------------------

#ifndef NODE_H
#define NODE_H

#include <cstddef>
#include <map>
#include <string>
#include <vector>
using namespace std;

class Cluster;
class Edge;

class Node
{
public:
	// -- constructors and destructors ------------------------------
    Node( const int &idx );
    Node( 
        const int &idx,
        const vector< double > &tpl );
	virtual ~Node();
	
    // -- set functions ---------------------------------------------
    void swapTupleVal( 
        const int &idx1,
        const int &idx2 );
    void moveTupleVal( 
        const int &idxFr,
        const int &idxTo );
    void moveTupleVals( map< int, int > &idcsFrTo );
    void addTupleVal( 
        const int &idx,
        const double &val );
    void delTupleVal( const int &idx );
    void addInEdge( Edge* e );
    void setInEdges( const vector< Edge* > e );
    void addOutEdge( Edge* e );
    void setOutEdges( const vector< Edge* > e );
    void setCluster( Cluster* c );
    
    // -- get functions ---------------------------------------------
    int getIndex();
    int getSizeTuple();
    double getTupleVal( const int &idx );
    int getSizeInEdges();
    Edge* getInEdge( const int &idx );
    int getSizeOutEdges();
    Edge* getOutEdge( const int &idx );
	Cluster* getCluster();

    // -- clear functions -------------------------------------------
    void clearInEdges();
    void clearOutEdges();
    void clearCluster();
    
protected:
    // -- data members ----------------------------------------------
    int index; // index in list of graph nodes
    vector< double >   tuple;
    vector< Edge* > inEdges;  // association
	vector< Edge* > outEdges; // association
    Cluster* cluster;
};

#endif

// -- end -----------------------------------------------------------
