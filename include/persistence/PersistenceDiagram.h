/// PersistenceDiagram.h
/// Author: Shaun Harker
/// Date: July 7, 2014
#ifndef PERSISTENCEDIAGRAM
#define PERSISTENCEDIAGRAM

#include <algorithm>
#include <exception>
#include <stdexcept>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <cmath>
#include <vector>

#include "boost/functional/hash.hpp"

#include "boost/serialization/serialization.hpp"
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/vector.hpp>

namespace subsample
{

int64_t generator_distance_count = 0;
struct Generator {
  double birth;
  double death;
  Generator ( void ) {}
  Generator ( double birth, double death ) 
    : birth(birth), death(death) {}
  friend std::ostream & operator << ( std::ostream & stream, 
                                      Generator const& gen ) {
    stream << "(" << gen . birth << ", " << gen . death << ")";
    return stream;
  }
  bool
  operator == ( Generator const& rhs ) const {
    if ( birth == rhs . birth && death == rhs . death ) return true;
    return false;
  }
  friend std::size_t hash_value( Generator const& g) {
    std::size_t seed = 0;
    boost::hash_combine(seed, g.birth);
    boost::hash_combine(seed, g.death);
    return seed;
  }

  class Distance {
  public:
    double operator () ( Generator const& x, 
                         Generator const& y ) const {
      ++ generator_distance_count;
      double dx = std::abs ( x . birth - y . birth );
      double dy = std::abs ( x . death - y . death );
      return std::max ( dx, dy );
    }
    double diagonal ( Generator const& x ) const {
      return (x.death-x.birth)/2.0;
    }  
  };
  friend class boost::serialization::access; 
  template<class Archive>
  void serialize (Archive & ar, const unsigned int version) {
    ar & birth;
    ar & death;
  }
};

class PersistenceDiagram : private std::vector<Generator> {
public:
  typedef std::vector<Generator> Base;
  using Base::value_type;
  using Base::assign;
  using Base::begin;
  using Base::end;
  using Base::size;
  using Base::empty;
  using Base::clear;
  using Base::push_back;
  using Base::iterator;
  using Base::const_iterator;
  using Base::operator[];
  void load ( std::string const& filename ) {

    clear ();
    std::ifstream infile ( filename );
    if (not infile . good ()) { 
      std::cout << "PersistenceDiagram::load. File not found. \n";
      throw std::runtime_error("PersistenceDiagram::load. File not found.");
    }
    std::string line;
    while ( std::getline ( infile, line ) ) {
      std::stringstream ss ( line );
      Generator g;
      ss >> g . birth;
      ss >> g . death;
      if ( ss . fail () ) { 
        throw std::string("PersistenceDiagram::load. Unexpected line: ") + line;
      }
      push_back ( g );
    }
    infile . close ();
    // Replace -1's with something sensible

    double max_entry = 0;
    for ( int i = 0; i < size (); ++ i ) {
      max_entry = std::max ( (*this)[i].birth, max_entry );
      max_entry = std::max ( (*this)[i].death, max_entry );
    }
    for ( int i = 0; i < size (); ++ i ) {
      if ( (*this)[i].birth == -1.0 ) (*this)[i].birth = 100000.0;
      if ( (*this)[i].death == -1.0 ) (*this)[i].death = 100000.0;
    }
    //std::cout << "Loaded a persistence diagram of size " << size () << "\n";
  }
  PersistenceDiagram ( void ) {}
  PersistenceDiagram ( std::string const& filename ) {
    load ( filename );
  }
  friend class boost::serialization::access; 
  template<class Archive>
  void serialize (Archive & ar, const unsigned int version) {
    ar & boost::serialization::base_object<std::vector<Generator> > (*this);
   // for ( int i = 0; i < )
  }
};

}

#endif
