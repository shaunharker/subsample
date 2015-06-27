/// DiagramPairSubsampleConfig.h
/// Author: Shaun Harker
/// Date: July 17, 2014
///   This file is part of a "SubsampleConfig.h" file
///   that can be used by configurations that want
///   to use pairs of persistence diagrams as the point
///   type.

#ifndef DIAGRAMPAIRSUBSAMPLECONFIG
#define DIAGRAMPAIRSUBSAMPLECONFIG

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <cstdlib>
#include "boost/serialization/serialization.hpp"
#include "persistence/PersistenceDiagram.h"

class Point {
public:
  int64_t id;
  PersistenceDiagram x;
  PersistenceDiagram y;
  Point ( void ) {}
  Point ( PersistenceDiagram const& x, 
          PersistenceDiagram const& y ) : x(x), y(y) {}
private:
  friend class boost::serialization::access; 
  template<class Archive>
  void serialize (Archive & ar, const unsigned int version) {
    ar & id;
    ar & x;
    ar & y;
  }
};

inline Distance 
getDistanceFunctor ( int argc, char * argv [] ) {
  return Distance ();
}

/// Functions for Subsample.cpp

inline int64_t 
getCohortSize ( int argc, char * argv [] ) {
  return 1000;
}

inline std::vector<Point> 
getSamples ( int argc, char * argv [] ) {
  std::vector<Point> samples;
  if ( argc != 4 ) {
    std::cout << "Give three arguments: /path/to/data number_of_samples delta \n";
    return samples;
  }
  std::cout << "Loading samples...\n";
  std::string folder_name = std::string ( argv[1] );
  int number_of_samples = atoi ( argv[2] );
  for ( int i = 1; i <= number_of_samples; ++ i ) {
    std::stringstream ss0, ss1;
    ss0 << folder_name << "/" << INPUTPREFIX << i << INPUTSUFFIX0;
    ss1 << folder_name << "/" << INPUTPREFIX << i << INPUTSUFFIX1;
    samples . push_back ( Point () );
    samples . back () . x . load ( ss0 . str () );
    samples . back () . y . load ( ss1 . str () );
    samples . back () . id = i;
  }
  std::cout << "Finished loading samples.\n";
  std::random_shuffle ( samples . begin (), samples . end () );
  return samples;
}

inline double
getDelta ( int argc, char * argv [] ) {
  return std::stod ( argv[3] );
}

inline void
handleResults ( int argc, char * argv [], std::vector<Point> const& results ) {
  std::cout << "There were " << results . size () 
            << " points in the subsample.\n";
  std::vector<int64_t> subsample_indices;
  for ( int i = 0; i < results.size(); ++ i ) {
    subsample_indices . push_back ( results[i].id );
  }
  std::sort ( subsample_indices . begin (), subsample_indices . end () );

  std::ofstream outfile ( SUBSAMPLEFILENAME );
  for ( int i = 0; i < subsample_indices.size(); ++ i ) {
    if ( i > 0 ) outfile << " ";
    outfile << subsample_indices[i];
  }
  outfile << "\n";
  outfile . close ();
#ifdef SUBSAMPLEDISTANCE_H
  std::cout << "Distance calculations = " << global_distance_count << "\n";
#endif
}

/// Functions for ComputeDistanceMatrix.cpp
inline std::vector<Point> 
getSubsamples ( int argc, char * argv [] ) {
  std::vector<Point> subsamples;
  if ( argc != 3 ) {
    std::cout << "Give two arguments: /path/to/data samplefile \n";
    return subsamples;
  }
  std::cout << "Loading subsamples...\n";
  std::string folder_name = std::string ( argv[1] );
  std::stringstream ss;
  ss << argv[2];
  std::ifstream infile ( ss . str () );
  std::vector<int64_t> sample_indices;
  while ( 1 ) {
    int64_t i;
    infile >> i;
    if ( not infile . good () ) break;
    sample_indices . push_back ( i );
    std::cout << i << " ";
  }
  std::cout << "\n";
  infile . close ();
  for ( int i = 0; i < sample_indices . size (); ++ i ) {
    int64_t sample_index = sample_indices [ i ];
    std::stringstream ss0, ss1;
    ss0 << folder_name << "/" << INPUTPREFIX << sample_index << INPUTSUFFIX0;
    ss1 << folder_name << "/" << INPUTPREFIX << sample_index << INPUTSUFFIX1;
    subsamples . push_back ( Point () );
    subsamples . back () . x . load ( ss0 . str () );
    subsamples . back () . y . load ( ss1 . str () );
    subsamples . back () . id = i;
  }
  std::cout << "Finished loading subsamples.\n";
  return subsamples;
}

#endif
