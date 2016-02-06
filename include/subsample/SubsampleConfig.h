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
#include <limits>
#include <stdexcept>
#include <exception>

#include "boost/serialization/serialization.hpp"
#include "persistence/PersistenceDiagram.h"
#include "persistence/WassersteinDistance.h"
#include "persistence/BottleneckDistance.h"
#include "persistence/BottleneckApproximateDistance.h"
#include "persistence/WassersteinApproximateDistance.h"

#include "tools/json.hpp"
using json = nlohmann::json;


namespace subsample
{

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
  Distance ( double p, double approx ) : p_(p), approx_(approx) {}
  double operator () ( Point const& p, Point const& q ) const {
    uint64_t N = p . pd . size ();
    double result = 0.0;
    if ( std::isinf(p_) ) {
      for ( uint64_t i = 0; i < N; ++ i ) {
        if ( approx_ == 0 ) {
          result = std::max(result, BottleneckDistance ( p.pd[i], q.pd[i] ) );
        } else {
          result = std::max(result, BottleneckApproximateDistance ( p.pd[i], q.pd[i], approx_ ) );
        }
      }
      return result;
    } else {
      for ( uint64_t i = 0; i < N; ++ i ) {
        if ( approx_ == 0 ) {
          result += std::pow ( WassersteinDistance ( p.pd[i], q.pd[i], p_), p_ );
        } else {
          result += std::pow ( WassersteinApproximateDistance ( p.pd[i], q.pd[i], p_, approx_), p_ );
        }
      }
      result = std::pow ( result, 1.0 / p_ );
      return result;
    }
  }
private:
  double p_;
  double approx_;
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
  std::vector<subsample::Point> const&
  getSamples ( void ) const;

  /// handleResults
  ///   Handle the results returned from the main program
  ///   (i.e. produce output from the subsample)
  void
  handleResults ( std::vector<subsample::Point> const& results ) const;

private:
  int argc_;
  char ** argv_;
  std::string samples_filename_;
  double delta_;
  double metric_;
  std::string subsample_filename_;
  Distance distance_;
  int64_t cohort_size_;
  std::vector<subsample::Point> samples_;
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
  distance_ = Distance ( metric_, 0 ); // Always initiate subsample with exact distance
  cohort_size_ = 1000;

  //std::cout << "Loading samples...\n";
  
  std::ifstream sample_infile ( samples_filename_ );
  json samples_json = json::parse ( sample_infile );
  sample_infile . close ();

  json sample_array = samples_json["sample"];
  std::string basepath = samples_json["path"];
  int64_t id = 0;
  for ( json const& tuple : sample_array ) {
    subsample::Point p;
    p . id = id ++;
    for ( std::string const& path : tuple ) {
      p.pd.push_back(PersistenceDiagram(basepath + "/" + path));
    }
    samples_.push_back(p);
  }
  //std::cout << "Finished loading samples.\n";
  std::random_shuffle ( samples_ . begin (), samples_ . end () );
  //std::cout << "There are " << samples_ . size () << " samples.\n";
  //std::cout << "Delta = " << delta_ << "\n";
  //std::cout << "Metric = " << metric_ << "\n";
}

inline Distance SubsampleConfig::
getDistanceFunctor ( void ) const {
  return distance_;
}

inline int64_t SubsampleConfig::
getCohortSize ( void ) const {
  return cohort_size_;
}

inline std::vector<subsample::Point> const& SubsampleConfig::
getSamples ( void ) const {
  return samples_;
}

inline double SubsampleConfig::
getDelta ( void ) const {
  return delta_;
}

inline void SubsampleConfig::
handleResults ( std::vector<subsample::Point> const& results ) const {
  //std::cout << "There were " << results . size () 
  //          << " points in the subsample.\n";
  std::vector<int64_t> subsample_indices;
  for ( int i = 0; i < results.size(); ++ i ) {
    subsample_indices . push_back ( results[i].id );
  }
  std::sort ( subsample_indices . begin (), subsample_indices . end () );

  json output;
  output["sample"] = samples_filename_;
  output["delta"] = delta_;
  if ( std::isinf ( metric_ ) ) {
    output["p"] = "inf";
  } else {
    output["p"] = metric_;
  }
  output["subsample"] = subsample_indices;
  std::ofstream ( subsample_filename_ ) << output;
#ifdef SUBSAMPLEDISTANCE_H
  //std::cout << "Distance calculations = " << global_distance_count << "\n";
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
  std::vector<subsample::Point> const&
  getSubsamples ( void ) const;

  /// getOutputFile
  ///   Return name of output file
  std::string const&
  getOutputFile ( void ) const;

  /// getApproximation ( void )
  ///   Return the approximation parameter (for auction algorithms)
  double
  getApproximationError ( void ) const;

  /// getDistanceFilter ( void )
  ///   Return the distance filter
  std::vector<int> const&
  getDistanceFilter ( void ) const;

private:
  std::string distance_filename_;

  double delta_;
  double metric_;
  double approx_;
  Distance distance_;
  std::vector<subsample::Point> subsamples_;

  std::vector<int> distance_filter_;
};

inline DistanceMatrixConfig::
DistanceMatrixConfig ( void ) {}

inline DistanceMatrixConfig::
DistanceMatrixConfig ( int argc, char * argv [] ) {
  assign ( argc, argv );
}

inline void DistanceMatrixConfig::
assign ( int argc, char * argv [] ) {
  if ( argc < 4 ) {
    std::cout << "Give at least three arguments: distance_approximation /path/to/subsample.json /path/to/distance.txt /path/to/distance_filter.txt\n";
    std::cout << " (Note: the fourth argument is the optional path to a distance filter.)\n";
    throw std::logic_error ( "Bad arguments." );
  }
  std::cout << "Loading subsamples...\n";
  approx_ = std::stod ( argv[1] );
  std::string subsample_filename = argv[2];
  distance_filename_ = argv[3];
  
  std::ifstream subsample_infile ( subsample_filename );
  json subsamples_json = json::parse ( subsample_infile );
  subsample_infile . close ();

  std::string samples_filename = subsamples_json["sample"];

  std::ifstream sample_infile ( samples_filename );
  json samples_json = json::parse ( sample_infile );
  sample_infile . close ();

  json sample_array = samples_json["sample"];
  std::string basepath = samples_json["path"];
  json subsample_array = subsamples_json["subsample"];
  try { 
    metric_ = subsamples_json [ "p" ];
  } catch ( ... ) {
    metric_ = std::numeric_limits<double>::infinity();
  }
  for ( int64_t const& i : subsample_array ) {
    subsample::Point p;
    p . id = i;
    json tuple = sample_array[i];
    for ( std::string const& path : tuple ) {
      p.pd.push_back(PersistenceDiagram(basepath + "/" + path));
    }
    subsamples_.push_back(p);
  }
  std::cout << "Finished loading subsamples.\n";


  // Initialize distance filter
  std::cout << "Initializing distance filter...\n";
  int64_t N;
  N = subsamples_ . size ();
  distance_filter_ . resize ( (N * N - N) / 2 , 1);
  distance_filter_[ distance_filter_.size() - 1 ] = 1;
  // Load distance filter if applicable
  std::cout << "No. of arguments: " << argc << "\n";
  if ( argc == 5 ){

    std::cout << "Loading distance filter. \n";

    int i;
    i = 0;
    std::ifstream infile ( argv[4] );
    if (not infile . good ()) { 
      std::cout << "DistanceMatrixConfig::assign. Distance filter file not found. \n";
      throw std::runtime_error("DistanceMatrixConfig::assign. Distance filter file not found.");
    }
    std::string entry;
    while ( std::getline ( infile, entry, ' ' ) ) {
      std::stringstream ss ( entry );
      std::cout << " " << entry;
      ss >> distance_filter_[i++];
      if ( ss . fail () ) { 
        throw std::string("DistanceMatrixConfig::assign. Distance filter unexpected entry: ") + entry;
      }
    }
    std::cout << "\n";
    infile . close ();

  }
  std::cout << "Distance filter loaded. Size: " << distance_filter_ . size() << ". First entry: " << distance_filter_[0] << ".\n";


}

inline Distance DistanceMatrixConfig::
getDistanceFunctor ( void ) const {
  return Distance ( metric_, approx_ );
}

inline std::vector<subsample::Point> const& DistanceMatrixConfig::
getSubsamples ( void ) const {
  return subsamples_;
}

inline std::string const& DistanceMatrixConfig::
getOutputFile ( void ) const {
  return distance_filename_;
}

inline double DistanceMatrixConfig::
getApproximationError ( void ) const {
  return approx_;
}

inline std::vector<int> const& DistanceMatrixConfig::
getDistanceFilter ( void ) const {
  return distance_filter_;
}

}

#endif
