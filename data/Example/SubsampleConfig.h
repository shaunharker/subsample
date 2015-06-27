/// SubsampleConfig.h
/// Author: Shaun Harker
/// Date: July 16, 2014
///    An example SubsampleConfig.h file
///    for use with the "Subsample" program.
///    This file must define the following things:
///
///     + class Point, which should be serializable and
///                    have a int64_t field named "id"
///
///     + class Distance, which should be default constructible
///                       and provide a method
///         double Distance::operator () ( Point const& p, Point const& q ) const;
///
///     + A function
///        std::vector<Point> getSamples ( int argc, char * argv [] );
///       which returns the points to subsample from.
///
///     + A function
///         Distance getDistanceFunctor ( int argc, char * argv [] );
///       which returns a distance object to be used for the calculations.
///
///     + A function 
///        int64_t getCohortSize ( int argc, char * argv [] );
///       which returns the number of subsamples we should attempt to deal 
///       with in parallel. (Suggestion: use the number of processor cores
///       on the cluster).
///
///     + A function "getDelta" which returns the double "delta" used by
///       the subsample algorithm
///        double getDelta ( int argc, char * argv [] );
///
///     + A function 
///        void handleResults ( int argc, char * argv [], std::vector<Point> const& results );
///       which receives the "Point"s in the computed subsample and 
///       produces the desired output.

#ifndef SUBSAMPLECONFIG_H
#define SUBSAMPLECONFIG_H

#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include "boost/serialization/serialization.hpp"
#include "boost/functional/hash.hpp"


class Point {
public:
  int64_t id;
  int x;
  int y;
  Point ( void ) {}
  Point ( int x, int y ) : x(x), y(y) {}
  bool
  operator == ( Point const& rhs ) const {
    if ( x == rhs . x && y == rhs . y ) return true;
    return false;
  }
  friend std::size_t hash_value( Point const& p) {
    std::size_t seed = 0;
    boost::hash_combine(seed, p.x);
    boost::hash_combine(seed, p.y);
    return seed;
  }
  friend std::ostream & operator << ( std::ostream & stream, 
                                      Point const& p ) {
    stream << "(" << p.x << ", " << p.y << ")";
    return stream;
  }
private:
  friend class boost::serialization::access; 
  template<class Archive>
  void serialize (Archive & ar, const unsigned int version) {
    ar & id;
    ar & x;
    ar & y;
  }
};

class Distance {
public:
  double operator () ( Point const& p, Point const& q ) const {
    double dx = p.x - q.x;
    double dy = p.y - q.y;
    return std::sqrt ( dx * dx + dy * dy );
  }
};

inline Distance 
getDistanceFunctor ( int argc, char * argv [] ) {
  return Distance ();
}

inline int64_t 
getCohortSize ( int argc, char * argv [] ) {
  return 1000;
}

inline std::vector<Point> 
getSamples ( int argc, char * argv [] ) {
  std::vector<Point> samples;
  std::ofstream outfile ( "samples.txt" );
  for ( int i = 0; i < 10000; ++ i ) {
    Point p ( rand () % 1000, rand () % 1000 );
    p . id = i;
    samples . push_back ( p );
    outfile << p << "\n";
  }
  outfile . close ();
  return samples;
}

inline double
getDelta ( int argc, char * argv [] ) {
  return 20.0;
}

inline void
handleResults ( int argc, char * argv [], std::vector<Point> const& results ) {
  std::cout << "There were " << results . size () 
            << " points in the subsample.\n";
  std::ofstream outfile ( "subsample.txt" );
  for ( int i = 0; i < results.size(); ++ i ) {
    outfile << results[i] << "\n";
  }
  outfile . close ();
#ifdef SUBSAMPLEDISTANCE_H
  std::cout << "Distance calculations = " << global_distance_count << "\n";
#endif
}

/// Functions for ComputeDistanceMatrix.cpp
#define DISTANCEFILENAME "distance.txt"

inline std::vector<Point> 
getSubsamples ( int argc, char * argv [] ) {
  std::vector<Point> subsamples;
  std::ifstream infile ( "subsample.txt" );
  std::string line;
  while ( std::getline ( infile, line ) ) {
    std::stringstream ss ( line );
    char c; int x; int y;
    ss >> c >> x >> c >> y;
    subsamples . push_back ( Point (x, y) );
  }
  std::cout << "Finished loading subsamples.\n";
  return subsamples;
}

#endif
