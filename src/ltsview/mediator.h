#ifndef MEDIATOR_H
#define MEDIATOR_H

#include <vector>
#include <string>
#include <utils.h>

using namespace std;
using namespace Utils;

class Mediator
{
  public:
    virtual ~Mediator() {}
    virtual void      activateMarkRule( const int index, const bool activate ) = 0;
    virtual void      addMarkRule() = 0;
    virtual void      applyDefaultSettings() = 0;
    virtual void      applyMarkStyle( MarkStyle ms ) = 0;
    virtual void      applySettings() = 0;
    virtual void      drawLTS( Point3D viewpoint ) = 0;
    virtual void      editMarkRule( const int index ) = 0;
    virtual float     getHalfStructureHeight() const = 0;
    virtual void      markAction( string label ) = 0;
    virtual void      notifyRenderingFinished() = 0;
    virtual void      notifyRenderingStarted() = 0;
    virtual void      openFile( string fileName ) = 0;
    virtual void      removeMarkRule( const int index ) = 0;
    virtual void      setMatchAnyMarkRule( bool b ) = 0;
    virtual void      setRankStyle( RankStyle rs ) = 0;
    virtual void      toggleDisplayStates() = 0;
    virtual void      toggleDisplayWireframe() = 0;
    virtual void      unmarkAction( string label ) = 0;
};

#endif
