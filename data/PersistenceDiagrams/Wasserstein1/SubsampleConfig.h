/// SubsampleConfig.h
/// Author: Shaun Harker
/// Date: July 16, 2014
///   Configures "Subsample" program to use W1 distance
///   sum on pairs of persistence diagrams.

#ifndef SUBSAMPLECONFIG_H
#define SUBSAMPLECONFIG_H

#define INPUTPREFIX "Out_"
#define INPUTSUFFIX0 "_0.txt"
#define INPUTSUFFIX1 "_1.txt"
#define SUBSAMPLEFILENAME "subsampleW1.txt"
#define DISTANCEFILENAME "distanceW1.txt"

#include "../DiagramPairSubsampleConfig.h"
#include "persistence/WassersteinDistance.h"

class Distance {
public:
  double operator () ( Point const& p, Point const& q ) const {
    return WassersteinDistance ( p . x, q . x, 1.0 ) + 
           WassersteinDistance ( p . y, q . y, 1.0 ); 
  }
};

#endif
