//
// Created by s152717 on 9-7-2020.
//

#include <ostream>
#include <iostream>
#include "debugTimer.h"

DebugTimer::DebugTimer(bool doIncludeOther)
{
  includeOther = doIncludeOther;
  currentMeasure = NONE;
  currentStart = std::chrono::system_clock::now();
  thisLoopStart = std::chrono::system_clock::now();
}

DebugTimer::DebugTimer() : DebugTimer(true)
{}

void DebugTimer::startNewLoop()
{
  endTiming();

  auto dt = currentStart - thisLoopStart;
  double loopDurationSeconds = (double) std::chrono::duration_cast<std::chrono::microseconds>(dt).count() / 1000000.0;

  // remove loop overhead if this should be ignored
  if (!includeOther) loopDurationSeconds -= thisLoopMeasures.erase(NONE);

  loopTimes.add(loopDurationSeconds);
  thisLoopStart = currentStart;

  // create default empty queue if it does not exist
  for (auto& elt : thisLoopMeasures)
  {
    if (totalMeasures.count(elt.first) == 0)
    {
      // i am definitely doing this wrong
      totalMeasures.insert(std::pair(elt.first, AveragingQueue<QUEUE_SIZE>()));
    }
  }

  // compute for all measures the fraction of the loop it took,
  // and add it to totalMeasures
  for (auto& elt : totalMeasures)
  {
    double duration = thisLoopMeasures[elt.first];
    elt.second.add(duration / loopDurationSeconds);
  }

  thisLoopMeasures.clear();
}

void DebugTimer::startTiming(const std::string& identifier)
{
  endTiming();
  currentMeasure = identifier;
}

void DebugTimer::endTiming(const std::string& identifier)
{
  if (currentMeasure != identifier)
  {
    std::string message = "Waiting for end of " + currentMeasure + " but received " + identifier;
    throw std::runtime_error(message);
  }
  endTiming();
}

std::list<std::pair<std::string, double>> DebugTimer::results()
{
  std::list<std::pair<std::string, double>> pairs;

  for (auto elt : totalMeasures)
  {
    std::string first = elt.first;
    double average = elt.second.average();
    pairs.push_back({first, average});
  }

  return pairs;
}

void DebugTimer::endTiming()
{
  std::chrono::system_clock::time_point end = std::chrono::system_clock::now();

  std::chrono::duration dt = end - currentStart;
  double seconds = (double) std::chrono::duration_cast<std::chrono::microseconds>(dt).count() / 1000000.0;

  thisLoopMeasures[currentMeasure] += seconds;


  currentMeasure = NONE;
  currentStart = end;
}

void DebugTimer::printResultsTo(std::ostream& target)
{
  target << std::to_string(totalMeasures.size()) << " time division averages of " << std::to_string(QUEUE_SIZE);
  target << " loops of " << std::to_string(loopTimes.average()) << " sec:" << std::endl;

  for (auto elt : totalMeasures)
  {
    std::string name = elt.first;
    double average = elt.second.average();
    char buffer[100];
    snprintf(buffer, 100, "| %-30s | %4.01f%% |", name.c_str(), average * 100);
    target << buffer << std::endl;
  }
}
