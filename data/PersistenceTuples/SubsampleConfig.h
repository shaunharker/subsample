/// SubsampleConfig.h
/// Author: Shaun Harker
/// Date: June 27, 2015

#ifndef SUBSAMPLECONFIG
#define SUBSAMPLECONFIG

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <cstdlib>
#include <cmath>

#include "boost/serialization/serialization.hpp"
#include "persistence/PersistenceDiagram.h"
#include "persistence/WassersteinDistance.h"
#include "persistence/BottleneckDistance.h"

#include "json.h"

class Point {
public:
  int64_t id;
  std::vector<PersistenceDiagram> pd;
  Point ( void ) {}
  Point ( std::vector<PersistenceDiagram> const& pd ) : pd(pd) {}
private:
  friend class boost::serialization::access; 
  template<class Archive>
  void serialize (Archive & ar, const unsigned int version) {
    ar & id;
    ar & pd;
  }
};

class Distance {
public:
  Distance ( void ) {}
  Distance ( double p ) : p_(p) {}
  double operator () ( Point const& p, Point const& q ) const {
    uint64_t N = p . pd . size ();
    double result = 0.0;
    if ( std::isinf(p_) ) {
      for ( uint64_t i = 0; i < N; ++ i ) {
        result = std::max(result, BottleneckDistance ( p.pd[i], q.pd[i] ) );
      }
      return result;
    } else {
      for ( uint64_t i = 0; i < N; ++ i ) {
        result += std::pow ( WassersteinDistance ( p.pd[i], q.pd[i], p_), p_ );
      }
      result = std::pow ( result, 1.0 / p_ );
      return result;
    }
  }
private:
  double p_;
};



/// Functions for Subsample.cpp

class SubsampleConfig {
public:
  /// SubsampleConfig
  ///   Configure with command line arguments
  SubsampleConfig ( int argc, char * argv [] );
  SubsampleConfig ( void );

  /// assign
  ///   Delayed constructor
  void
  assign ( int argc, char * argv [] );

  /// getDistanceFunctor
  ///   Return distance function object
  Distance
  getDistanceFunctor ( void ) const;

  /// getCohortSize
  ///   Return cohort size
  int64_t 
  getCohortSize ( void ) const;

  /// getDelta ( void )
  ///   Return the delta parameter (for delta-dense, delta-sparse subsampling)
  double
  getDelta ( void ) const;

  /// getSamples
  ///   Return collection of samples (Points)
  std::vector<Point> const&
  getSamples ( void ) const;

  /// handleResults
  ///   Handle the results returned from the main program
  ///   (i.e. produce output from the subsample)
  void
  handleResults ( std::vector<Point> const& results ) const;

private:
  int argc_;
  char ** argv_;
  std::string samples_filename_;
  double delta_;
  double metric_;
  std::string subsample_filename_;
  Distance distance_;
  int64_t cohort_size_;
  std::vector<Point> samples_;
};

inline SubsampleConfig::
SubsampleConfig ( void ) {}

inline SubsampleConfig::
SubsampleConfig ( int argc, char * argv [] ) {
  assign ( argc, argv );
}

inline void SubsampleConfig::
assign ( int argc, char * argv [] ) {
  if ( argc != 5 ) {
    std::cout << "Give four arguments: /path/to/sample.json delta p /path/to/subsample.json \n";
    std::cout << " (Note: the last argument is the output file.)\n";
    throw std::logic_error ( "Bad arguments." );
  }
  argc_ = argc;
  argv_ = argv;
  samples_filename_ = argv[1];
  delta_ = std::stod ( argv[2] );
  metric_ = std::stod ( argv[3] );
  subsample_filename_ = argv[4];
  distance_ = Distance ( metric_ );
  cohort_size_ = 1000;

  std::cout << "Loading samples...\n";
  JSON::Value samples_json = JSON::load ( samples_filename_ );
  JSON::Array sample_array = samples_json["sample"];
  JSON::String basepath = samples_json["path"];
  for ( JSON::Array const& sample : sample_array ) {
    Point p;
    for ( JSON::String const& path : sample ) {
      p.pd.push_back(PersistenceDiagram(basepath + "/" + path));
    }
    samples_.push_back(p);
  }
  std::cout << "Finished loading samples.\n";
  std::random_shuffle ( samples_ . begin (), samples_ . end () );
}

inline Distance SubsampleConfig::
getDistanceFunctor ( void ) const {
  return distance_;
}

inline int64_t SubsampleConfig::
getCohortSize ( void ) const {
  return cohort_size_;
}

inline std::vector<Point> const& SubsampleConfig::
getSamples ( void ) const {
  return samples_;
}

inline double SubsampleConfig::
getDelta ( void ) const {
  return delta_;
}

inline void SubsampleConfig::
handleResults ( std::vector<Point> const& results ) const {
  std::cout << "There were " << results . size () 
            << " points in the subsample.\n";
  std::vector<int64_t> subsample_indices;
  for ( int i = 0; i < results.size(); ++ i ) {
    subsample_indices . push_back ( results[i].id );
  }
  std::sort ( subsample_indices . begin (), subsample_indices . end () );

  JSON::Object output;
  output["sample"] = samples_filename_;
  output["delta"] = delta_;
  output["p"] = metric_;
  output["subsample"] = subsample_indices;
  JSON::save( output, subsample_filename_ );
#ifdef SUBSAMPLEDISTANCE_H
  std::cout << "Distance calculations = " << global_distance_count << "\n";
#endif
}

/// Functions for ComputeDistanceMatrix.cpp

class DistanceMatrixConfig {
public:
  /// DistanceMatrixConfig
  ///   Configure with command line arguments
  DistanceMatrixConfig ( int argc, char * argv [] );
  DistanceMatrixConfig ( void );

  /// assign
  ///   Delayed constructor
  void
  assign ( int argc, char * argv [] );

  /// getDistanceFunctor
  ///   Return distance function object
  Distance
  getDistanceFunctor ( void ) const;

  /// getSubsamples
  ///   Return collection of samples (Points)
  std::vector<Point> const&
  getSubsamples ( void ) const;

  /// getOutputFile
  ///   Return name of output file
  std::string const&
  getOutputFile ( void ) const;

private:
  std::string samples_filename_;
  std::string subsample_filename_;
  std::string distance_filename_;

  double delta_;
  double metric_;
  Distance distance_;
  std::vector<Point> subsamples_;
};

inline DistanceMatrixConfig::
DistanceMatrixConfig ( void ) {}

inline DistanceMatrixConfig::
DistanceMatrixConfig ( int argc, char * argv [] ) {
  assign ( argc, argv );
}

inline void DistanceMatrixConfig::
assign ( int argc, char * argv [] ) {
  if ( argc != 4 ) {
    std::cout << "Give three arguments: /path/to/sample.json /path/to/subsample.json /path/to/distance.txt\n";
    std::cout << " (Note: the last argument is the output file.)\n";
    throw std::logic_error ( "Bad arguments." );
  }
  std::cout << "Loading subsamples...\n";
  samples_filename_ = argv[1];
  subsample_filename_ = argv[2];
  distance_filename_ = argv[3];
  JSON::Value samples_json = JSON::load ( samples_filename_ );
  JSON::Value subsamples_json = JSON::load ( subsample_filename_ );
  JSON::Array sample_array = samples_json["sample"];
  JSON::String basepath = samples_json["path"];
  JSON::Array subsample_array = subsamples_json["subsample"];
  metric_ = (JSON::Double) subsamples_json [ "p" ];
  for ( JSON::Value const& i : subsample_array ) {
    Point p;
    for ( JSON::String const& path : (JSON::Array)sample_array[(JSON::Integer)i] ) {
      p.pd.push_back(PersistenceDiagram(basepath + "/" + path));
    }
    subsamples_.push_back(p);
  }
  std::cout << "Finished loading subsamples.\n";
}

inline Distance DistanceMatrixConfig::
getDistanceFunctor ( void ) const {
  return Distance ( metric_ );
}

inline std::vector<Point> const& DistanceMatrixConfig::
getSubsamples ( void ) const {
  return subsamples_;
}

inline std::string const& DistanceMatrixConfig::
getOutputFile ( void ) const {
  return distance_filename_;
}

#endif
