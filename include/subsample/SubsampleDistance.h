/// SubsampleDistance.h
/// Author: Shaun Harker
/// Date: July 16, 2014
///   This file provides a templated class "SubsampleDistance"
///   which wraps a distance function (object) with funtionality
///   used by the "Subsample" program.
///   In particular, it allows for caching answers and it "throws"
///   an exception when "Subsample" tries to access a distance not 
///   yet cached. The "Subsample" program will catch this exception
///   and compute the result using the template class.
///   We require the Point class have a field "id" which distinguishes it
///   (presumably this can be its index in the sample)

#ifndef SUBSAMPLEDISTANCE_H
#define SUBSAMPLEDISTANCE_H

#include <utility>
#include "boost/unordered_map.hpp"
#include "boost/thread/mutex.hpp"

int64_t global_distance_count = 0;

template < class Point, class Distance >
class SubsampleDistance {
public:
  SubsampleDistance ( void ) {}
  SubsampleDistance ( Distance const& distance ) : distance_ ( distance ) {}
  double compute ( Point const& p, Point const& q ) const {
    return distance_ ( p, q );
  }
  double operator () ( Point const& p, Point const& q ) {
    //std::cout << " () Looking for point pair (" << p << ", " << q << ")\n";
    //std::cout << " () Looking for id pair (" << p.id << ", " << q.id << ")\n";
    mutex_ . lock ();
    Cache_t::iterator it = cache_ . find ( std::make_pair(p.id,q.id) );
    if ( it == cache_ . end () ) { 
      mutex_ . unlock ();
      ++ global_distance_count;
      throw 0;
    }
    mutex_ . unlock ();
    return it -> second;
  }
  void cache ( Point const& p, Point const& q, double dist ) {
    mutex_ . lock ();
    cache_ [ std::make_pair(p.id,q.id) ] = dist;
    mutex_ . unlock ();
  }
private:
  typedef boost::unordered_map<std::pair<int64_t, int64_t>, double> Cache_t;
  Cache_t cache_;
  boost::mutex mutex_;
  Distance distance_;
};

#endif
