#ifndef CLUSTER_H
#define CLUSTER_H
#include <vector>
#include <set>
#include <math.h>
#include "utils.h"

#ifndef STATE_H
  #include "state.h"
#else
  class State;
#endif

using namespace Utils;

// forward declaration
class Cluster;

// class for cluster comparison based on cluster sizes
class Comp_ClusterSize
{
  public:
    bool operator()( const Cluster*, const Cluster* ) const;
};

class Cluster
{
  
  private:
    Cluster*		ancestor;
    float		baseRadius;
    vector< Cluster* >	descendants;
    float		position;
    float		size;
    vector< State* >	states;
    float		topRadius;
  
  public:
    Cluster();
    ~Cluster();
    void      addDescendant( Cluster* c );
    void      addState( State* s );
    void      computeSizeAndDescendantPositions();
    Cluster*  getAncestor() const;
    float     getBaseRadius() const;
    void      getDescendants( vector< Cluster* > &cs ) const;
    float     getPosition() const;
    float     getTopRadius() const;
    float     getSize() const;
    void      getStates( vector< State* > &ss ) const;
    bool      hasDescendants() const;
    void      setAncestor( Cluster* c );
    void      setPosition( float p );

  /*
    int rank;
    vector< State* > states;
    RGBAColor FColor;
    vector< Cluster* > descendants;
//    vector< Cluster* > FDescendantsSorted;
    float scaleFactor;
    bool hasSmallest;
    bool hasLargest;
    bool hidden;
    bool selected;
    bool marked;
    float colorMapValue;
    bool centered;
    short slotCount;
    Point3D location;
    short indexInRank;
    short indexWithParent;
    short radialPosition;
    void Draw(bool LayoutOnly);
    void LayoutStates();
    void DoStatesWithSingleCenteredDescendant();
    int AverageSlot( vector< int > SlotList,  int SlotCount);
    void SpreadStates();
    vector< State* > GetStatesWithSlot(int Slot);
    vector< State* > GetStatesInCenterSlot();
    int CountTotalDescendantStates();
  
public:
    Cluster(int Rank);
    ~Cluster;
    Cluster* Ancestor;
    State* GetState(int Index);
    Cluster* GetDescendant(int Index);
//    Cluster* GetDescendantSorted(int Index);
    void SetColor();
    void RecursiveDraw(bool TopDown, bool LayoutOnly);
    float CalculateSize();
    float GetSimpleRadius();
    void DoStateLayoutForBranch();
    void NormalSelect();
    void CtrlSelect();
    void ShiftSelect();
    int NoNonHiddenDescendants();    
    RGBAColor GetColor();
    bool GetCentered();
    float GetColorMapValue();
    void SetColorMapValue();
    bool GetMarked();
    void SetMarked();
    bool GetHasLargest();
    bool GetHasSmallest();
    bool GetHidden();
    void SetHidden();
    vector< State* > GetStates();
    void SetStates();
    vector< Cluster* > GetDescendants();
    void SetDescendants();
    vector< Cluster* > GetDescendantsSorted();
    void SetDescendantsSorted();
    int GetRank();
    void SetRank();
    short GetRadialPosition();
    void SetRadialPosition();
    float GetSize();
    void SetSize();
    bool GetSelected();
    void SetSelected();
    short GetSlotCount();
    void SetSlotCount();
    short GetIndexInRank();
    void SetIndexInRank();
    short GetIndexWithParent();
    void SetIndexWithParent();
    Point3D GetLocation();
    void SetLocation();
  */
};

#endif
