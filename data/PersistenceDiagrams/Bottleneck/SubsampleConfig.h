/// SubsampleConfig.h
/// Author: Shaun Harker
/// Date: July 16, 2014
///   Configures "Subsample" program to use Bottleneck distance
///   sum on pairs of persistence diagrams.

#ifndef SUBSAMPLECONFIG_H
#define SUBSAMPLECONFIG_H

#define INPUTPREFIX "Out_"
#define INPUTSUFFIX0 "_0.txt"
#define INPUTSUFFIX1 "_1.txt"
#define SUBSAMPLEFILENAME "subsampleB.txt"
#define DISTANCEFILENAME "distanceB.txt"

#include "../DiagramPairSubsampleConfig.h"
#include "persistence/BottleneckDistance.h"

class Distance {
public:
  double operator () ( Point const& p, Point const& q ) const {
    return std::max ( BottleneckDistance ( p . x, q . x ), 
                      BottleneckDistance ( p . y, q . y ) ); 
  }
};

#endif
