//
// Created by s152717 on 9-7-2020.
//

#ifndef MCRL2_DEBUGTIMER_H
#define MCRL2_DEBUGTIMER_H

#include <queue>
#include <string>
#include <map>
#include <list>
#include <stdexcept>
#include <chrono>

/** constant-time running average */
template<int CAPACITY>
class AveragingQueue
{
  double entries[CAPACITY] = {0};
  double sum = 0;
  int head = 0;
public:

  /**
   * add an item to this collection, deleting the last added entry.
   * @param entry a new float
   */
  void add(double entry)
  {
    sum -= entries[head];
    entries[head] = entry;
    sum += entry;
    head = (head + 1) % CAPACITY;
  }

  /**
   * @return the average of the last {@code capacity} items.
   */
  double average()
  {
    return sum / CAPACITY;
  }
};

class DebugTimer
{

private:
  const std::string NONE = "other";
  static const int QUEUE_SIZE = 16;

  AveragingQueue<QUEUE_SIZE> loopTimes;
  std::map<std::string, AveragingQueue<QUEUE_SIZE>> totalMeasures;
  std::map<std::string, double> thisLoopMeasures;
  std::chrono::system_clock::time_point thisLoopStart;
  std::chrono::system_clock::time_point currentStart;

  bool includeOther;
  std::string currentMeasure;

public:
  DebugTimer();

  DebugTimer(bool doIncludeOther);

  void startNewLoop();

  void startTiming(const std::string& identifier);

  void endTiming(const std::string& identifier);

  std::list<std::pair<std::string, double>> results();

  void printResultsTo(std::ostream& target);

private:
  void endTiming();
};

#endif //MCRL2_DEBUGTIMER_H
