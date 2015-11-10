/// Subsample.cpp
/// Author: Shaun Harker
/// Date: July 3, 2014
#include "cluster-delegator.hpp" 
#include "subsample/SubsampleDistance.h"
#include "subsample/SubsampleProcess.h" 
#include "subsample/SubsampleConfig.h" // Defines class Point, class Distance

int main ( int argc, char * argv [] ) {
  typedef SubsampleProcess<subsample::Point,SubsampleDistance<subsample::Point, Distance> > Process;
  delegator::Start ();
  delegator::Run<Process> (argc, argv);
  delegator::Stop ();
  return 0;
}
