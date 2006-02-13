#include "cluster.h"

bool Comp_ClusterSize::operator()(const Cluster* c1, const Cluster* c2) const
{
  return ( c1->getSize() < c2->getSize() );
}

Cluster::Cluster()
{
  ancestor = NULL;
  position = 0;
  size = 0.0;
}

Cluster::~Cluster()
{
}

void Cluster::addState( State* s )
{
  states.push_back( s );
}

void Cluster::setAncestor( Cluster* c )
{
  ancestor = c;
}

void Cluster::setPosition( float p )
{
  position = p;
}

void Cluster::addDescendant( Cluster* c )
{
  descendants.push_back( c );
}

void Cluster::getStates( vector< State* > &ss ) const
{
  ss = states;
}

Cluster* Cluster::getAncestor() const
{
  return ancestor;
}

void Cluster::getDescendants( vector< Cluster* > &cs ) const
{
  cs = descendants;
}

bool Cluster::hasDescendants() const
{
  return ( descendants.size() != 0 );
}

float Cluster::getSize() const
{
  return size;
}

float Cluster::getPosition() const
{
  return position;
}

void Cluster::computeSizeAndDescendantPositions()
// pre: size of every descendant is known (and assumed to be correct)
{
  topRadius = states.size() / (2 * PI);
  if ( descendants.size() == 0 )
  {
    baseRadius = topRadius;
    size = topRadius;
  }
  else if ( descendants.size() == 1 )
  {
    baseRadius = (**descendants.begin()).getTopRadius();
    size = max( topRadius, (**descendants.begin()).getSize() );
    (**descendants.begin()).setPosition( -1.0 );
  }
  else // descendants.size() > 1
  {
    // sort descendants by size in ascending order
    sort( descendants.begin(), descendants.end(), Comp_ClusterSize() );

    // determine whether a unique smallest descendant exists
    Cluster* smallest = descendants[0];
    Cluster* nextSmallest = descendants[1];
    bool uniqueSmallest = ( nextSmallest->getSize() - smallest->getSize() > 0.1 );
    
    // determine whether a unique largest descendant exists
    Cluster* largest = descendants[ descendants.size()-1 ];
    Cluster* nextLargest = descendants[ descendants.size()-2 ];
    bool uniqueLargest = ( largest->getSize() - nextLargest->getSize() > 0.1 );

    // invariant: descendants in range [noPosBegin, noPosEnd) have not been
    // assigned a position yet
    int noPosBegin = 0;
    int noPosEnd = descendants.size();
    
    float centerSize = 0.0f;	// size of largest descendant in center
    float rimSize = 0.0f;	// size of largest descendant on rim
    if ( uniqueLargest )
    {
      // center the largest descendant
      largest->setPosition( -1.0 );
      --noPosEnd;

      centerSize = largest->getSize();
      rimSize = nextLargest->getSize();
    }
    else
    {
      // no unique largest descendant, so largest will be placed on the rim
      rimSize = largest->getSize();
      
      if ( uniqueSmallest )
      {
	// center the smallest descendant
	centerSize = smallest->getSize();
	smallest->setPosition( -1.0 );
	++noPosBegin;
      }
    }
    
    // compute the size of the cluster
    float minRimRadius = (float)( rimSize / sin(PI / (noPosEnd - noPosBegin)) );
    baseRadius = max( centerSize + rimSize + 0.01f, minRimRadius );
    size = max( topRadius, baseRadius );

    // now correct the baseRadius for the case in which a unique largest
    // descendant having descendants exists and all other descendants have no
    // descendants
    if ( uniqueLargest && largest->hasDescendants() )
    {
      // unique largest cluster exists (is centered) and has descendants
      int i = noPosBegin;
      while ( i < noPosEnd && ! descendants[i]->hasDescendants() )
	++i;
      if ( i == noPosEnd )
      {
	// none of the clusters on the rim has descendants
	baseRadius = max( largest->getTopRadius() + rimSize + 0.01f,
	    minRimRadius );
      }
    }

    // divide the remaining descendants over the rim of the circle:
    // suppose the list noPosBegin,...,noPosEnd-1 is: [ 0, 1, 2, 3, 4, 5 ]
    // then the clusters are positioned on the rim in the following order
    // (counter-clockwise): ( 0, 2, 4, 1, 3, 5 )
    int noPosit;
    int i = 0;
    int j = ( noPosEnd - noPosBegin ) / 2 + ( noPosEnd - noPosBegin ) % 2;
    float angle = 360.0 / ( noPosEnd - noPosBegin );
    for ( noPosit = noPosBegin ; noPosit != noPosEnd ; ++noPosit )
    {
      if ( (noPosit - noPosBegin) % 2 == 0 )
      {
	descendants[noPosit]->setPosition( i*angle );
	++i;
      }
      else
      {
	descendants[noPosit]->setPosition( j*angle );
	++j;
      }
    }
  }
}

float Cluster::getTopRadius() const
{
  return topRadius;
}

float Cluster::getBaseRadius() const
{
  return baseRadius;
}

/*
Cluster::Cluster(int Rank)
{
    rank = Rank;
    selected = false;
    marked = false;
}

Cluster::~Cluster()
{
}

State* Cluster::GetState(int Index)
{  
  return states[Index];
}

Cluster* Cluster::GetDescendant(int Index)
{
  return descendants[Index];
}

Cluster* Cluster::GetDescendantSorted(int Index)
{
  return descendantsSorted[Index];
}

float Cluster::CalculateSize()
// Calculate Radius for cone's base
{  
  float result; int i;
  float CalcRadius;
  float AvgNonCenteredSize;
  int NonCenteredCount;

  //Reset Data
  centered = false;
  hasLargest = false;
  hasSmallest = false;
  size = 0;
  
  if (descendants.size() == 0) size = min( 100, GetSimpleRadius() );
  
  if (descendants.size() == 1)
  {
    size = min( 100, max( GetSimpleRadius(), GetDescendant(0).CalculateSize() ) );
    if ( Vis.CenterSymmetry ) GetDescendant(0).centered = true;
  }
  
  if (descendants.size() > 1)
  {
    int DescendantCount = 0;
    for ( i = 0 ; i < descendants.size() ; ++i )
      GetDescendant(i).CalculateSize();
    for ( i = 0 ; i < descendants.size() ; ++i )
    {
      if ( GetDescendant(i).states.size() > 1 ) DescendantCount++;
    }
    
    descendantsSorted.Sort(ListSortCompare);
    
    if ( Vis.CenterSymmetry )
    {
      // Unique largest cluster among descendants?
      if ( GetDescendantSorted(0).FDescendants.size() == 0 && GetDescendantSorted(0).States.size() > 1 )
      //if ((GetDescendantSorted(1).FSize > GetDescendantSorted(0).FSize) or (GetDescendantSorted(1).FDescendants.size() > 0)) then
      {
	FHasSmallest = true;
	if ( Vis.CenterSymmetry )
	  GetDescendantSorted(0).FCentered = true;
      }
      // Unique largest cluster among descendants?
      if ( FDescendantsSorted.size() > 1 || FHasSmallest )
      {
	if ( GetDescendantSorted(FDescendantsSorted.size() - 1).FSize > GetDescendantSorted(FDescendantsSorted.size() - 2).FSize ||
	     ( GetDescendantSorted(FDescendantsSorted.size() - 1).FSize == GetDescendantSorted(FDescendantsSorted.size() - 2).FSize &&
	       GetDescendantSorted(FDescendantsSorted.size() - 1).CountTotalDescendantStates > GetDescendantSorted(FDescendantsSorted.size() - 2).CountTotalDescendantStates ) )
	{
	  FHasLargest = true;
	  if ( Vis.CenterSymmetry )
	    GetDescendantSorted(FDescendantsSorted.size() - 1).FCentered = true;
	}
      }
      if ( GetDescendantSorted(FDescendantsSorted.size() - 2).FDescendants.size() > 0 )
      {
	if ( FHasLargest )
	{
	  if ( FDescendantsSorted.size() - Ord(FHasLargest) - Ord(FHasSmallest) > 1 )
	  {
	    CalcRadius = max( GetDescendantSorted( FDescendantsSorted.size()
	    - 1 ).FSize + GetDescendantSorted( FDescendantsSorted.size() - 2
	      ).FSize,
	      static_cast< double >
	      (GetDescendantSorted(FDescendantsSorted.size() - 2).FSize) /
	      Sin( DegtoRad( static_cast< double > (180) /
	      (FDescendantsSorted.size() - Ord( FHasLargest ) - Ord( FHasSmallest )) ) ) );
	  }
	  else
	    CalcRadius = GetDescendantSorted(FDescendantsSorted.size() - 1).FSize + GetDescendantSorted(FDescendantsSorted.size() - 2).FSize;
	}
	else
	{
	  CalcRadius = static_cast<double>(GetDescendantSorted(FDescendantsSorted.size() - 1).FSize)/Sin(DegToRad(static_cast<double>(180)/(FDescendantsSorted.size() - Ord(FHasSmallest))));
	}
	FSize = min(100, max(CalcRadius, GetSimpleRadius));
      }
      else
      {
	FSize = GetDescendantSorted(FDescendantsSorted.size() - 1).GetSimpleRadius;
      }
    }
    else
    {
      FSize = max(static_cast<double>(GetDescendantSorted(FDescendantsSorted.size() - 1).FSize)/Sin(DegToRad(static_cast<double>(180)/FDescendantsSorted.size())), GetSimpleRadius);
    }
  }
  Result = FSize;
  return result;
}

float Cluster::GetSimpleRadius()
{
  return ( FStates.size() ) / ( 2 * pi );
}

void Cluster::SetColor()
{ 
  // Set transparencey
  float AlphaIndex = ( FSelected ) ? 1.0 : 1.0 - 0.01 * Vis.Transparency;

  if ( FMarked && !Vis.OnlySelectedClusters )
    FColor = ColortoRGBA( ColorToRGB( frmMain.pnlMarked.Color ), AlphaIndex );
  else
    FColor = ColortoRGBA( ColorToRGB( frmMain.pnlFamilies.Color ), AlphaIndex );
}


void Cluster::RecursiveDraw( bool TopDown, bool LayoutOnly )
{
  int i;

  // Position on circle

    if ( TopDown && FRank < frmMain.speClustersUpToRank.Value )
      Draw(LayoutOnly);
  
    if ( FDescendants.size() == 1 )
    {
        glTranslatef(0, -Vis.ClusterHeight, 0);
        if (LayoutOnly)
	{
	  getDescendant(0).FScaleFactor = FScaleFactor;
	  getDescendant(0).FRadialPosition = -1;
        }
        getDescendant(0).RecursiveDraw(TopDown, LayoutOnly);
        glTranslatef(0, Vis.ClusterHeight, 0);
    }
    if (FDescendants.size() > 1)
    {
        for ( i = 0; i < FDescendantsSorted.size() ; ++i )
	{
            if ( !GetDescendantSorted(i).Centered )
	    {
                glPushMatrix();
                glRotatef(Vis.RotateCenter, 0, 1, 0);
                glRotatef(static_cast<double>(i * 360)/(FDescendants.size() - Ord(FHasSmallest) - Ord(FHasLargest)), 0, 1, 0);
                if ( LayoutOnly )
		{
                    GetDescendantSorted(i).FRadialPosition = Round(static_cast<double>(i * 360)/(FDescendants.size() - Ord(FHasSmallest) - Ord(FHasLargest)));
                    getDescendantSorted(i).FScaleFactor = FScaleFactor * 0.01 * Vis.Scale;
                }
                glTranslatef(0.01 * Vis.Spread * FSize, 0, 0);
                glTranslatef(0, -Vis.ClusterHeight, 0);
                glRotatef(Vis.Tilt, 0, 0, 1);
                glScalef(0.01 * Vis.Scale, 0.01 * Vis.Scale, 0.01 * Vis.Scale);
                getDescendantSorted(i).RecursiveDraw(TopDown, LayoutOnly);
                glPopMatrix();
            }
            else
	    {
                glRotatef(Vis.RotateCenter, 0, 1, 0);
                glTranslatef(0, -Vis.ClusterHeight, 0);
                glScalef(0.01 * Vis.Scale, 0.01 * Vis.Scale, 0.01 * Vis.Scale);
                if (LayoutOnly)
		{
                    getDescendantSorted(i).FRadialPosition = -1;
                    getDescendantSorted(i).FScaleFactor = FScaleFactor * 0.01 * Vis.Scale;
                }
                getDescendantSorted(i).RecursiveDraw(TopDown, LayoutOnly);
                glScalef(static_cast<double>(1)/(0.01 * Vis.Scale), static_cast<double>(1)/(0.01 * Vis.Scale), static_cast<double>(1)/(0.01 * Vis.Scale));
                glTranslatef(0, Vis.ClusterHeight, 0);
                glRotatef(-Vis.RotateCenter, 0, 1, 0);
            }
        }
    }
    if ( !TopDown && FRank < frmMain.speClustersUpToRank.Value )
        Draw(LayoutOnly);
}

void Cluster::Draw(bool LayoutOnly)
{
    Point3D SpherePoint;
    RGBAColor FamilyColor;
    int i;
    Matrix4x4Array Matrix;

//and Vis.OnlySelectedClusters
//or not (Vis.OnlySelectedClusters)
// Draw sphere
// Draw cyclinder
// Draw black cylinder with small height indicating end of family
// Draw Family with multiple descendants
// Draw black cylinder with small height indicating end of family

    SetColor();
    if (!FHidden && Vis.DrawStates || LayoutOnly)
    {
        for (i = 0; i <= FStates.size() - 1; ++i)
	  GetState(i).Draw(LayoutOnly);
    }
    if (LayoutOnly)
    {
        glGetFloatv(GL_MODELVIEW_MATRIX, &Matrix);
        FLocation = ToPoint3D(Matrix[12], Matrix[13], Matrix[14]);
    }
    if (!FHidden && !LayoutOnly)
    {
        glPushName(2);
        glPushName(FRank);
        glPushName(FIndexInRank);
        if (FDescendants.size() == 0)
	{
            GLSphere(0, 0, 0, GetSimpleRadius, SPHERE_FACES, Vis.TextureOn, FColor, FColorMapValue, FColorMapValue);
        }
        if (FDescendants.size() == 1)
	{
            GLCylinder(0, -Vis.ClusterHeight, 0, GetDescendant(0).GetSimpleRadius, GetSimpleRadius, Vis.ClusterHeight, CYLINDER_FACES, Vis.TextureOn, FColor, GetDescendant(0).FColorMapValue, FColorMapValue);
            if (Vis.LevelDividers) {
                GLCylinder(0, -Vis.ClusterHeight, 0, GetDescendant(0).GetSimpleRadius + 0.01, GetDescendant(0).GetSimpleRadius + 0.01, 0.05, CYLINDER_FACES, False, ToRGBA(51, 51, 51, 1), 0, 0);
            }
        }
        if (FDescendants.size() > 1)
	{
            if (Ancestor != NULL) {
                GLCylinder(0, -Vis.ClusterHeight, 0, FSize, Min(GetSimpleRadius, Min(Ancestor.FSize, Ancestor.GetSimpleRadius)), Vis.ClusterHeight, CYLINDER_FACES, Vis.TextureOn, ToRGBA(FColor.r, FColor.g, FColor.b, static_cast<double>(FColor.a)/3), FColorMapValue, FColorMapValue);
            }
            else
	    {
                GLCylinder(0, -Vis.ClusterHeight, 0, FSize, 0, Vis.ClusterHeight, CYLINDER_FACES, Vis.TextureOn, ToRGBA(FColor.r, FColor.g, FColor.b, static_cast<double>(FColor.a)/3), FColorMapValue, FColorMapValue);
            }
            if (Vis.LevelDividers)
	    {
                GLCylinder(0, -Vis.ClusterHeight, 0, FSize + 0.01, FSize + 0.01, 0.05, CYLINDER_FACES, False, ToRGBA(51, 51, 51, 1), 0, 0);
            }
        }
        if (Ancestor != NULL)
	{
            if ((*Ancestor).Descendants.size() > 1 && Vis.LevelDividers)
	    {
                GLCylinder(0, 0, 0, GetSimpleRadius + 0.01, GetSimpleRadius + 0.01, 0.05, CYLINDER_FACES, False, ToRGBA(51, 51, 51, 1), 0, 0);
            }
        }
        glPopName();
        glPopName();
        glPopName();
    }
}

void Cluster::DoStateLayoutForBranch()
{ TCluster CurrentCluster;
    int i;
    bool EndOfBranch;


    CurrentCluster = self;
    EndOfBranch = false;
    while (!EndOfBranch)
    {
        for (i = 0; i < CurrentCluster.States.size() ; ++i)
	{
            CurrentCluster.GetState(i).Slot = -1;
            CurrentCluster.GetState(i).CenterSlot = False;
            CurrentCluster.GetState(i).StillToDo = False;
            CurrentCluster.GetState(i).Angle = 0;
        }
        CurrentCluster.LayoutStates();
        if (CurrentCluster.Ancestor == NULL)
	{
            EndOfBranch = true;
        }
        else
	{
            if (CurrentCluster.Ancestor.Descendants.size() == 1)
	    {
                CurrentCluster = *CurrentCluster.Ancestor;
            }
            else
	    {
                EndOfBranch = true;
            }
        }
    }
}

void Cluster::LayoutStates()
{ int i; int j; int VSlot;
    TList SlotList;//TSmallIntArray;
    TCluster DescendantCluster;

//always position node in center of cluster
//TSmallIntArray.Create(0,0);
// Still uses family's radialposition, should be adapted
//GetPosInLargestGap;

    if (FStates.size() == 1) {
        FSlotCount = 1;
        GetState(0).CenterSlot = True;
        GetState(0).Slot = -1;
    }
    else{
        SlotList = TList.Create;
        if (FDescendants.size() == 0) {
            FSlotCount = FStates.size();
            for (i = 0; i <= FStates.size() - 1; ++i) {
                GetState(i).Slot = AngleToSlot(GetState(i).BestGuess, FSlotCount);
            }
        }
        if (FDescendants.size() == 1) {
            FSlotCount = Min(Round(max(GetDescendant(0).FSlotCount * 2, FStates.size() * 2)), MAX_SLOT_COUNT);
            for (i = 0; i <= FStates.size() - 1; ++i) {
                SlotList.Clear();
                for (j = 0; j <= GetState(i).SubOrdinates.size() - 1; ++j) {
                    if (!GetState(i).GetSubordinate(j).CenterSlot) {
                        VSlot = AngleToSlot(GetState(i).GetSubordinate(j).Angle, FSlotCount);
                        SlotList.Add(Pointer(VSlot));
                    }
                }
                GetState(i).Slot = AverageSlot(SlotList, FSlotCount);
            }
        }
        if (FDescendants.size() > 1) {
            if (HasLargest) {
                FSlotCount = Min((FDescendants.size() - 1) * 2, MAX_SLOT_COUNT);
            }
            else{
                FSlotCount = Min(FDescendants.size() * 2, MAX_SLOT_COUNT);
            }
            for (i = 0; i <= FStates.size() - 1; ++i) {
                SlotList.Clear();
                for (j = 0; j <= GetState(i).Subordinates.size() - 1; ++j) {
                    DescendantCluster = *TPCluster(GetState(i).Getsubordinate(j).GetCluster);
                    if (!DescendantCluster.Centered) {
                        SlotList.Add(Pointer(AngleToSlot(DescendantCluster.FRadialPosition, FSlotCount)));
                    }
                    else{
                        if (!GetState(i).Getsubordinate(j).CenterSlot) {
                            SlotList.Add(Pointer(AngleToSlot(GetState(i).Getsubordinate(j).Angle, FSlotCount)));
                        }
                    }
                }
                GetState(i).Slot = AverageSlot(SlotList, FSlotCount);
            }
        }
        DoStatesWithSingleCenteredDescendant();
        if (Vis.SpreadStates) {
            SpreadStates();
        }
        SlotList.Free();
    }
}

void TCluster::DoStatesWithSingleCenteredDescendant()
// If node's only descendant is centered include in third pass
{ int i;


    if (FStates.size() > 1) {
        for (i = 0; i <= FStates.size() - 1; ++i) {
            if (GetState(i).Subordinates.size() == 1) {
                if (Getnode(i).GetSubordinate(0).CenterSlot == True) {
                    GetState(i).CenterSlot = False;
                    GetState(i).Slot = AngleToSlot(GetState(i).BestGuess, FSlotCount);
                }
            }
        }
    }
}

int TCluster::AverageSlot(TList SlotList,  int SlotCount)
// Calculate average slot
{   int result; TVector2D SumVector; TVector2D CurrentVector;
    int i; int CurrentResult;
    bool Center;

//if (Center and (FStates.size() > 2)) or (FStates.size() = 1) then Result := -1 else 

    SumVector = ToVector2D(0, 0);
    for (i = 0; i <= SlotList.size() - 1; ++i) {
        CurrentVector = AngleToVector(SlottoAngle(Integer(SlotList.Items[i]), SlotCount));
        SumVector = Add2DVectors(SumVector, CurrentVector);
    }
    CurrentResult = AngleToSlot(VectorToAngle(SumVector, Center), SlotCount);
    Result = CurrentResult;
return result;
}

void TCluster::SpreadStates()
{ int TotalStatesOnLevel;
    int AngleBetween; int CurAngle; int i; int j; int maxslot;
    TList StatesInSlot;

// Spread nodes equidistant
// Spread nodes according to slotcount
// Determine available space
// Spread nodes over available space
// Spread nodes in centerslot

    TotalStatesOnLevel = FStates.size() - GetStatesInCenterSlot.size();
    if (TotalStatesOnLevel > 0) {
        if (FStates.size() <= StrtoInt(frmMain.txtSpreadNo.Text)) {
            AngleBetween = Round(static_cast<double>(360)/FStates.size());
            StatesInSlot = TList.Create;
            CurAngle = -Round(static_cast<double>(GetStatesWithSlot(0).size() - 1)/2 * AngleBetween);
            for (i = 0; i <= FSlotCount - 1; ++i) {
                StatesInSlot = GetStatesWithSlot(i);
                for (j = 0; j <= StatesInSlot.size() - 1; ++j) {
                    (*TPState(StatesInSlot.Items[j])).Angle = Round(Modulo(CurAngle, 360));
                    CurAngle = CurAngle + AngleBetween;
                }
            }
        }
        else{
            for (i = 0; i <= FSlotCount - 1; ++i) {
                StatesInSlot = GetnodesWithSlot(i);
                if (StatesInSlot.size() > 0) {
                    MaxSlot = 1;
                    while (GetStatesWithSlot(Round(Modulo(i + MaxSlot, FSlotCount))).size() == 0 && GetStatesWithSlot(Round(Modulo(i - MaxSlot, FSlotCount))).size() == 0) {
                        MaxSlot = MaxSlot + 1;
                    }
                    AngleBetween = Round(static_cast<double>(static_cast<double>(360)/FSlotCount * MaxSlot)/StatesInSlot.size());
                    for (j = 0; j <= StatesInSlot.size() - 1; ++j) {
                        (*TPState(StatesInSlot.Items[j])).Angle = Round(Modulo(SlottoAngle(i, FSlotCount) - static_cast<double>(StatesInSlot.size() - 1)/2 * AngleBetween + j * AngleBetween, 360));
                    }
                }
            }
        }
        if (GetStatesInCenterSlot.size() > 1) {
            StatesInSlot = TList.Create;
            StatesInSlot = GetStatesInCenterSlot;
            for (i = 0; i <= StatesInSlot.size() - 1; ++i) {
                (*TPState(StatesInSlot.Items[i])).Angle = Round(i * (static_cast<double>(360)/StatesInSlot.size()));
            }
        }
        StatesInSlot.Free();
    }
}

TList TCluster::GetStatesWithSlot(int Slot)
// Returns a list of all nodes in slot <Slot>
{   TList result; int i;
    TList Templist;


    Templist = TList.Create;
    for (i = 0; i <= FStates.size() - 1; ++i) {
        if (GetState(i).Slot == Slot) {
            Templist.Add(GetPState(i));
        }
    }
    Result = TempList;
return result;
}

TList TCluster::GetStatesInCenterSlot()
// Returns a list of all nodes in the center slot
{   TList result; int i;
    TList Templist;


    Templist = TList.Create;
    for (i = 0; i <= FStates.size() - 1; ++i) {
        if (GetState(i).CenterSlot) {
            TempList.Add(GetPState(i));
        }
    }
    Result = TempList;
return result;
}

int TCluster::CountTotalDescendantStates()
{   int result; int TempTotal; int i;


    TempTotal = FStates.size();
    for (i = 0; i <= FDescendants.size() - 1; ++i) {
        TempTotal = TempTotal + GetDescendant(i).CountTotalDescendantStates;
    }
    Result = TempTotal;
return result;
}

void TCluster::NormalSelect()
// Select this one
{ int i; int j;


    FSelected = True;
}

void TCluster::CtrlSelect()
// Toggle select
{

    FSelected = !FSelected;
}

void TCluster::ShiftSelect()
{ int i;
// Select family + all descendants


    if (!FHidden) {
        FSelected = True;
    }
    for (i = 0; i <= FDescendants.size() - 1; ++i) {
        GetDescendant(i).ShiftSelect();
    }
}

int TCluster::NoNonHiddenDescendants()
{   int result; int NonHiddenDescendants; int i;


    NonHiddenDescendants = 0;
    if (!FHidden) {
        NonHiddenDescendants = NonHiddenDescendants + 1;
    }
    for (i = 0; i <= FDescendants.size() - 1; ++i) {
        NonHiddenDescendants = NonHiddenDescendants + GetDescendant(i).NoNonHiddenDescendants;
    }
    Result = NonHiddenDescendants;
return result;
}    


// Auxiliary functions ////

int ListSortCompare(Pointer Item1, Pointer Item2)
{   int result;

    if ((*TPCluster(Item1)).FSize > (*TPCluster(Item2)).FSize) {
        Result = 1;
    }
    if ((*TPCluster(Item1)).FSize < (*TPCluster(Item2)).FSize) {
        Result = -1;
    }
    if ((*TPCluster(Item1)).FSize == (*TPCluster(Item2)).FSize) {
        if ((*TPCluster(Item1)).CountTotalDescendantStates > (*TPCluster(Item2)).CountTotalDescendantStates) {
            Result = 1;
        }
        if ((*TPCluster(Item1)).CountTotalDescendantStates < (*TPCluster(Item2)).CountTotalDescendantStates) {
            Result = -1;
        }
        if ((*TPCluster(Item1)).CountTotalDescendantStates == (*TPCluster(Item2)).CountTotalDescendantStates) {
            if ((*TPCluster(Item1)).IndexWithParent < (*TPCluster(Item2)).IndexWithParent) {
                Result = 1;
            }
            if ((*TPCluster(Item1)).IndexWithParent > (*TPCluster(Item2)).IndexWithParent) {
                Result = -1;
            }
            if ((*TPCluster(Item1)).IndexWithParent == (*TPCluster(Item2)).IndexWithParent) {
                Result = 0;
            }
        }
    }
return result;
}

int AltListSortCompare(Pointer Item1, Pointer Item2)
{   int result;

    if ((*TPCluster(Item1)).GetSimpleRadius > (*TPCluster(Item2)).GetSimpleRadius) {
        Result = 1;
    }
    if ((*TPCluster(Item1)).GetSimpleRadius < (*TPCluster(Item2)).GetSimpleRadius) {
        Result = -1;
    }
    if ((*TPCluster(Item1)).GetSimpleRadius == (*TPCluster(Item2)).GetSimpleRadius) {
        Result = 0;
    }
return result;
}



//END
*/
