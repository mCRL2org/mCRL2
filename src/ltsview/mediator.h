#ifndef MEDIATOR_H
#define MEDIATOR_H
#include <string>
#include "utils.h"

class Mediator {
  public:
    virtual ~Mediator() {}
    virtual void      activateMarkRule(const int index,const bool activate) = 0;
    virtual void      addMarkRule() = 0;
    virtual void      applyMarkStyle(Utils::MarkStyle ms) = 0;
    virtual void      editMarkRule(const int index) = 0;
    virtual void      markAction(std::string label) = 0;
    virtual void      notifyRenderingFinished() = 0;
    virtual void      notifyRenderingStarted() = 0;
    virtual void      openFile(std::string fileName) = 0;
    virtual void      removeMarkRule(const int index) = 0;
    virtual void      setMatchAnyMarkRule(bool b) = 0;
    virtual void      setRankStyle(Utils::RankStyle rs) = 0;
    virtual void      setVisStyle(Utils::VisStyle vs) = 0;
    virtual void      unmarkAction(std::string label) = 0;
};

#endif
