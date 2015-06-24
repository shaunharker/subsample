/// Subsample.cpp
/// Author: Shaun Harker
/// Date: July 3, 2014
#include "delegator/delegator.h" 
#include "subsample/SubsampleDistance.h"
#include "subsample/SubsampleProcess.h" 

#include "SubsampleConfig.h" // Defines class Point, class Distance

int main ( int argc, char * argv [] ) {
  typedef SubsampleProcess<Point,SubsampleDistance<Point, Distance> > Process;
  delegator::Start ();
  delegator::Run<Process> (argc, argv);
  delegator::Stop ();
  return 0;
}