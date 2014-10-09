/// MetricTree.h
/// Author(s): Shaun Harker
/// Date: June 29, 2014
#include <limits>
#include <vector>
#include <set>
#include <stack>
#include <fstream>
#include <typeinfo>
#include "boost/shared_ptr.hpp"
#include "boost/foreach.hpp"

/// Forward declarations
namespace MetricTree_detail {
  template < class T, class D > class MetricTreeException;
  template < class T, class D > class InsertException;
  template < class T, class D > class SearchException;
  template < class T, class D > class NearestException;
  template < class T, class D > class KNearestException;
  template < class T, class D > class AspirationException;
  template < class T, class D > class DeltaCloseException;
}

/// class MetricTree
///    MetricTree is a binary tree structure that organizes points in a
///    metric space in order to provide more efficient search algorithms.
///    The only information used is distance between two points.
///    Searches provided:
///       Nearest Search: Given x, report the closest point to x.
///       Aspiration Search: Given delta, report any p such that d(p,x) < delta
///       Delta-Close Search: Given delta, report all delta-close points to x
///    Template parameters: 
///            T is Point type. 
///            D is a distance functor
///    Notes. This class is designed to handle the case where distances
///           might not be available immediately, in which case D may
///           "throw". The code handles this by throwing an exception
///           which contains information necessary to restart the operation
///           being executed where it left off. Because of this, the
///           insert method and the various search methods have overloaded
///           versions which accept "Exceptions" as their arguments. These
///           are used to restart the operation once the distance has been
///           computed and the function object D can provide it (rather than
///           throw)
template < class T, class D >
class MetricTree {
public:
  typedef MetricTree_detail::MetricTreeException<T,D> Exception;
  typedef MetricTree_detail::InsertException<T,D> InsertException;
  typedef MetricTree_detail::SearchException<T,D> SearchException;
  typedef MetricTree_detail::NearestException<T,D> NearestException;
  typedef MetricTree_detail::KNearestException<T,D> KNearestException;
  typedef MetricTree_detail::AspirationException<T,D> AspirationException;
  typedef MetricTree_detail::DeltaCloseException<T,D> DeltaCloseException;
  typedef typename std::vector<T>::const_iterator iterator;
  typedef iterator const_iterator;
  typedef int64_t size_type;
  typedef T value_type;

  /// MetricTree
  ///   Contructs and empty tree
  MetricTree ( void );

  /// assign
  ///    assign a Distance functor to the Metric Tree
  void
  assign ( boost::shared_ptr<D> distance );

  /// begin
  ///   Return "begin" iterator, as in STL containers.
  ///   If container is empty, return "end()"
  ///   Note: returns the same as "root()"
  iterator 
  begin ( void ) const;
  
  /// end
  ///   Return "one-past-the-end" iterator, as in STL containers
  iterator 
  end ( void ) const;
  
  /// size
  ///   Return number of points in metric tree
  size_type 
  size ( void ) const;
  
  /// insert
  ///   Insert the point "x" into the metric tree
  ///   Return an iterator pointing to where "x" 
  ///   was inserted.
  iterator
  insert ( T const& x );

  /// insert (exceptional version)
  ///   If "insert" throws an InsertException due to
  ///   missing distance data, once the distance data
  ///   is found the insert operation may be resumed by
  ///   calling this overloaded version with the exception
  ///   object.
  iterator 
  insert ( InsertException & e );

  /// nearest
  ///   Find closest point to x, and return
  ///   an iterator pointing to it.
  iterator 
  nearest ( T const& x ) const;

  /// nearest (exceptional)
  ///   Used for resuming "nearest" after it throws
  iterator 
  nearest ( NearestException & e ) const;

  /// knearest
  ///   Find k closest points to x, and return
  ///   a vector of iterators pointing to them.
  std::vector<iterator> 
  knearest ( T const& x, int64_t k ) const;

  /// knearest (exceptional)
  ///   Used for resuming "knearest" after it throws
  std::vector<iterator> 
  knearest ( KNearestException & e ) const;

  /// aspiration
  ///   Given: a point x and a number delta
  ///   Try to find a point within delta of x.
  ///   Return the first such point found. If there
  ///   are no such points, return the end() iterator.
  iterator 
  aspiration ( T const& x, double delta ) const;

  /// aspiration (exceptional)
  ///   Used for resuming "aspiration" after it throws
  iterator 
  aspiration ( AspirationException & e ) const;

  /// deltaClose
  ///   Given: a point x and a number delta
  ///   Return the collection of iterators for points
  ///   in the metric tree within distance delta of x
  std::vector<iterator> 
  deltaClose ( T const& x, double delta ) const;

  /// deltaClose (exceptional)
  ///   Used for resuming "deltaClose" after it throws
  std::vector<iterator> 
  deltaClose ( DeltaCloseException & e ) const;

  /// getDistance 
  ///    Return the distance between x and y
  ///    If an errors occurs, put (x,y) in e
  ///    and throw e
  double 
  getDistance ( T const& x,
                T const& y, 
                Exception & e ) const;

  /// search
  ///   Used as a helper method by various search 
  ///   routines (nearest, aspiration, deltaClose)
  void 
  search ( SearchException & e ) const;

  /// radius
  ///    Given an iterator, return the maximum distance
  ///    between the point it refers to and all points
  ///    in its subtree
  double
  radius ( iterator it ) const;

  /// root
  ///   Return the iterator pointing to the root of the tree
  ///   If tree is empty, return end()
  ///   Note: returns the same as "begin()"
  iterator 
  root ( void ) const;

  /// left
  ///   Return the left child of tree node pointed at by
  ///   "x". If none exists, return "end()".
  iterator 
  left ( iterator x ) const;

  /// right
  ///   Return the right child of tree node pointed at by
  ///   "x". If none exists, return "end()".
  iterator 
  right ( iterator x ) const;

  /// parent
  ///   Return the parent of tree node pointed at by
  ///   "x". If none exists, return "end()".
  iterator 
  parent ( iterator x ) const;

  /// isLeaf
  ///   Return true if x points to a leaf in the tree.
  ///   Otherwise return false.
  bool
  isLeaf ( iterator x ) const;

  /// index
  ///   Return a number for the iterator
  ///   which tells us which order it was inserted in
  int64_t 
  index ( iterator it ) const;

  /// node
  ///    Return an iterator corresponding the the "index"
  ///    (i.e. the inverse of the "index" method)
  iterator
  node ( int64_t i ) const;

  /// insertAsLeft
  ///    Insert x as the left child of n
  iterator
  insertAsLeft ( iterator n, T const& x );

  /// insertAsRight
  ///   insert x as the right child of n
  iterator
  insertAsRight ( iterator n, T const& x ); 

  /// graphVizDebug
  ///    Create a .gv file illustrating the data structure
  void
  graphVizDebug ( const char * filename );

private:
  std::vector< T > points_;
  std::vector<int64_t> left_;
  std::vector<int64_t> right_;
  std::vector<int64_t> parent_;
  std::vector<double> radius_;
  boost::shared_ptr<D> distance_;
};

template < class T, class D >
MetricTree<T,D>::
MetricTree ( void ) {
  distance_ . reset ( new D );
}

template < class T, class D >
void MetricTree<T,D>::
assign ( boost::shared_ptr<D> distance ) { 
  distance_ = distance; 
  //std::cout << "MetricTree::assign. this = " << this << " and distance_ = " << distance_ << "\n";
}

template < class T, class D >
typename MetricTree<T,D>::iterator MetricTree<T,D>::
begin ( void ) const { 
  return points_ . begin (); 
}

template < class T, class D >
typename MetricTree<T,D>::iterator MetricTree<T,D>::
end ( void ) const { 
  return points_ . end (); 
}

template < class T, class D >
typename MetricTree<T,D>::size_type MetricTree<T,D>::
size ( void ) const { 
  return (size_type) points_ . size (); 
}

template < class T, class D > double MetricTree<T,D>::
getDistance ( T const& x,
              T const& y, 
              Exception & e ) const {
  double result;
  //std::cout << "getDistance. x = " << x << " y = " << y << "\n";
  try {
    //std::cout << "MetricTree::getDistance. this = " << this << " and distance_ = " << distance_ << "\n";
    result = (*distance_) ( x, y );
    //std::cout << "result = " << result << "\n";
    //abort ();
  } catch (...) {
    //std::cout << "MetricTree::getDistance. Caught the exception.\n";
    //std::cout << "MetricTree::getDistance. Need (" << x << "; " << y << ")\n";
    e . calculations -> push ( std::make_pair ( x, y ) );
    e . raise ();
  }
  return result;
}

template < class T, class D >
typename MetricTree<T,D>::iterator
MetricTree<T,D>::
insert ( T const& x ) { 
  InsertException e;
  e . x . reset ( new T (x) );
  e . index = index ( root () );
  return insert ( e );
}

template < class T, class D >
typename MetricTree<T,D>::iterator 
MetricTree<T,D>::
insert ( InsertException & e ) { 
  iterator it = node ( e . index );
  T const& x = * e . x;

  if ( it == end () ) {
    points_ . push_back ( x );
    parent_ . push_back ( -1 );
    left_ . push_back ( -1 );
    right_ . push_back ( -1 );
    radius_ . push_back ( 0.0 );
    return root ();
  }
  //std::cout << "INSERT. x = " << x << " and *it = " << *it << "\n";
  //std::cout << "INSERT. index(it) == " << index(it) << "\n";
  if ( index(it) < 0 || index(it) >= size() ) {
    std::cout << "INSERT. Invalid iterator.\n";
    abort ();
  }

  double a, b;
  b = getDistance ( x, * it, e );
  //std::cout << "INSERT. Survived initial distance.\n";
  while ( 1 ) {
    radius_ [ index ( it ) ] = 
      std::max ( radius_ [ index ( it ) ], b );
    iterator L = left ( it );
    iterator R = right ( it );
    if ( L == end () && R == end () ) {
      //std::cout << "INSERT. insertAsLeft (1).\n";
      it = insertAsLeft ( it, x );
      e . index = index ( it );
      //std::cout << "INSERT. index(it) == " << index(it) << " (1)\n";
      return it;
    }
    if ( L == end () ) {
      a = b;
      b = getDistance ( x, * R, e );
      if ( a <= b ) {
        //std::cout << "INSERT. insertAsLeft (2).\n";
        it = insertAsLeft ( it, x );
        e . index = index ( it );
        return it;
      } else {
        //std::cout << "INSERT. move to right (2).\n";
        it = R;
        e . index = index ( it );
        continue;
      }
    }
    if ( R == end () ) {
      a = b;
      b = getDistance ( x, * L, e );
      if ( a <= b ) {
        //std::cout << "INSERT. insertAsRight (3).\n";
        it = insertAsRight ( it, x );
        e . index = index ( it );
        return it;
      } else {
        //std::cout << "INSERT. move to left. (3)\n";
        it = L;
        e . index = index ( it );
        continue;
      } 
    }
    a = getDistance ( x, * L, e );
    b = getDistance ( x, * R, e );
    if ( a <= b ) {
      //std::cout << "INSERT. move to left. (4)\n";
      it = L;
      e . index = index ( it );
      b = a;
    } else {
      //std::cout << "INSERT. move to right. (4)\n";
      it = R;
      e . index = index ( it );
    }
  }
}

template < class T, class D >
typename MetricTree<T,D>::iterator MetricTree<T,D>::
nearest ( T const& x ) const { 
  NearestException e;
  e . x . reset ( new T (x) );
  e . best = std::numeric_limits<double>::infinity();
  return nearest ( e );
}

template < class T, class D >
typename MetricTree<T,D>::iterator MetricTree<T,D>::
nearest ( NearestException & e ) const { 
  search ( e );
  return node ( e . best_index );
}

template < class T, class D >
std::vector<typename MetricTree<T,D>::iterator> MetricTree<T,D>::
knearest ( T const& x, int64_t k ) const { 
  KNearestException e;
  e . x . reset ( new T (x) );
  e . k = k;
  return knearest ( e );
}

template < class T, class D >
std::vector<typename MetricTree<T,D>::iterator> MetricTree<T,D>::
knearest ( KNearestException & e ) const { 
  search ( e );
  std::vector<iterator> results;
  typedef typename KNearestException::BestSet_t::reverse_iterator reverse_iterator;
  for ( reverse_iterator it = e . best . rbegin (); 
        it != e . best . rend (); ++ it ) {
    results . push_back ( node ( it -> second ) );
  }
  return results;
}

template < class T, class D >
typename MetricTree<T,D>::iterator MetricTree<T,D>::
aspiration ( T const& x, double delta ) const { 
  AspirationException e;
  e . x . reset ( new T (x) );
  e . delta = delta;
  return aspiration ( e );
}

template < class T, class D >
typename MetricTree<T,D>::iterator MetricTree<T,D>::
aspiration ( AspirationException & e ) const { 
  search ( e );
  if ( e . results -> empty () ) return end ();
  return node ( (* e . results) [ 0 ] );
}

template < class T, class D >
std::vector<typename MetricTree<T,D>::iterator> MetricTree<T,D>::
deltaClose ( T const& x, double delta ) const { 
  DeltaCloseException e;
  e . x . reset ( new T (x) );
  e . delta = delta;
  return deltaClose ( e );
}

template < class T, class D >
std::vector<typename MetricTree<T,D>::iterator> MetricTree<T,D>::
deltaClose ( DeltaCloseException & e ) const { 
  search ( e );
  std::vector<iterator> results;
  BOOST_FOREACH ( int64_t index, * e . results ) {
    results . push_back ( node ( index ) );
  }
  return results;
}

template < class T, class D >
void MetricTree<T,D>::
search ( SearchException & e ) const { 
  if ( e . work_stack -> empty () ) { 
    e . work_stack -> push ( index ( root () ) );
  }
  while ( not e . work_stack -> empty () ) {
    iterator it = node ( e . work_stack -> top () );
    if ( it == end () ) {
      e . work_stack -> pop ();
      continue;
    }
    double dist = getDistance ( * e . x, * it, e );
    double r = radius ( it );

    bool breakflag = false;
    switch ( e . type ) {
      case 3: // Nearest neighbor search.
      {
        NearestException & ne = dynamic_cast<NearestException&> ( e );
        if ( dist < ne . best ) {
          ne . best = dist;
          ne . best_index = index ( it );
        }
        if ( dist > ne . best + r ) { 
          e . work_stack -> pop ();
          continue;
        } 
        break;
      }
      case 4: // k Nearest neighbor search
      {
        KNearestException & kne = dynamic_cast<KNearestException&> ( e );
        double worst_of_best = std::numeric_limits<double>::infinity();
        if ( not kne . best . empty () ) {
          worst_of_best = kne . best . begin () -> first;
        }
        if ( kne . best . size () == kne . k ) {
          if ( dist > worst_of_best + r ) {
            e . work_stack -> pop ();
            continue;
          }
        }
        std::pair<double, int64_t> val ( dist, index ( it ) );
        if ( kne . best . size () < kne . k || dist <= worst_of_best ) { 
          kne . best . insert ( val );
        }
        if ( kne . best . size () > kne . k ) { 
          kne . best . erase ( kne . best . begin () );
        }
        break;
      }
      case 5: // Aspiration search
      {
        AspirationException & ae = dynamic_cast<AspirationException&> ( e );
        if ( dist < ae . delta ) {
          ae . results -> push_back ( index ( it ) );
          ae . work_stack . reset ( new std::stack<int64_t> () ); 
          breakflag = true;
          break;
        }
        if ( dist > ae . delta + r ) { 
          ae . work_stack -> pop ();
          continue;
        }
        break;
      }
      case 6: // Delta-close neighbors search
      {
        DeltaCloseException & de = dynamic_cast<DeltaCloseException&> ( e );
        if ( dist < de . delta ) {
          if ( de . results -> empty () || 
               de . results -> back () != index ( it ) ) {
            de . results -> push_back ( index ( it ) );
          }
        }
        if ( dist > de . delta + r ) { 
          de . work_stack -> pop ();
          continue;
        }
        break;
      }
      default:
        throw std::logic_error ( "MetricTree::search. Invalid search type.\n" );
    }

    if ( breakflag ) break;
    
    iterator L = left ( it );
    iterator R = right ( it );
    if ( L == end () && R == end () ) {
      e . work_stack -> pop ();
      continue;
    }
    if ( L == end () ) {
      e . work_stack -> pop ();
      e . work_stack -> push ( index ( R ) );
      continue;
    }
    if ( R == end () ) {
      e . work_stack -> pop ();
      e . work_stack -> push ( index ( L ) );
      continue;
    }
    double ldist = getDistance ( * e . x, *L, e );
    double rdist = getDistance ( * e . x, *R, e );
    e . work_stack -> pop ();
    if ( ldist < rdist ) {
      e . work_stack -> push ( index ( R ) );
      e . work_stack -> push ( index ( L ) );
    } else {
      e . work_stack -> push ( index ( L ) );
      e . work_stack -> push ( index ( R ) );   
    }   
  }
}

template < class T, class D >
double MetricTree<T,D>::
radius ( iterator it ) const {
  return radius_ [ index ( it ) ];
}

template < class T, class D >
typename MetricTree<T,D>::iterator MetricTree<T,D>::
root ( void ) const { 
  return begin ();
}

template < class T, class D >
typename MetricTree<T,D>::iterator MetricTree<T,D>::
left ( iterator x ) const { 
  return node ( left_ [ index ( x ) ] ); 
}

template < class T, class D >
typename MetricTree<T,D>::iterator MetricTree<T,D>::
right ( iterator x ) const { 
  return node ( right_ [ index ( x ) ] ); 
}

template < class T, class D >
typename MetricTree<T,D>::iterator MetricTree<T,D>::
parent ( iterator x ) const { 
  return node ( parent_ [ index ( x ) ] ); 
}

template < class T, class D >
bool MetricTree<T,D>::
isLeaf ( iterator x ) const { 
  return ( left ( x ) == end () &&
           right ( x ) == end () );
}

template < class T, class D >
int64_t MetricTree<T,D>:: 
index ( iterator it ) const {
  if ( it == end () ) return -1;
  return it - begin ();
}

template < class T, class D >
typename MetricTree<T,D>::iterator 
MetricTree<T,D>:: 
node ( int64_t i ) const {
  if ( i == -1 ) return end ();
  return begin () + i;
}

template < class T, class D >
typename MetricTree<T,D>::iterator 
MetricTree<T,D>::
insertAsLeft ( iterator n, T const& x ) { 
  int64_t child_index = points_ . size ();
  int64_t parent_index = index ( n );
  points_ . push_back ( x );
  left_ . push_back ( -1 );
  right_ . push_back ( -1 );
  radius_ . push_back ( 0.0 );
  parent_ . push_back ( parent_index );
  left_ [ parent_index ] = child_index;
  return node ( child_index );
}

template < class T, class D >
typename MetricTree<T,D>::iterator 
MetricTree<T,D>::
insertAsRight ( iterator n, T const& x ) { 
  int64_t child_index = points_ . size ();
  int64_t parent_index = index ( n );
  points_ . push_back ( x );
  left_ . push_back ( -1 );
  right_ . push_back ( -1 );
  radius_ . push_back ( 0.0 );
  parent_ . push_back ( parent_index );
  right_ [ parent_index ] = child_index;
  return node ( child_index );
}

template < class T, class D >
void MetricTree<T,D>::
graphVizDebug ( const char * filename ) {
  std::ofstream outfile ( filename );
  outfile << "digraph G {\n";
  for ( int64_t i = 0; i < size (); ++ i ) {
    outfile << i << " [label=\"" << points_[i] << "\\n" << radius_[i] << "\"]\n";
    if ( left_ [ i ] != -1 ) {
      outfile << i << " -> " << left_ [ i ] << "\n";
    }
    if ( right_ [ i ] != -1 ) {
      outfile << i << " -> " << right_ [ i ] << "\n";
    }    
  }
  outfile << "}\n";
}

namespace MetricTree_detail {
template < class T, class D >
class MetricTreeException {
public:
  boost::shared_ptr<T> x;
  boost::shared_ptr<std::stack<std::pair<T, T> > > calculations;
  int type;
  MetricTreeException ( void ) : type ( 0 )
    { calculations . reset ( new std::stack<std::pair<T,T> >); }
  virtual ~MetricTreeException ( void ) {}
  virtual void raise ( void ) { throw *this; }
};

template < class T, class D >
class InsertException : public MetricTreeException<T,D> {
public:
  using MetricTreeException<T,D>::type;
  InsertException ( void ) { type = 1; }
  virtual ~InsertException ( void ) {}
  virtual void raise ( void ) { throw *this; }
  int64_t index;
};

template < class T, class D >
class SearchException : public MetricTreeException<T,D> {
public:
  using MetricTreeException<T,D>::type;
  boost::shared_ptr<std::stack<int64_t> > work_stack;
  boost::shared_ptr<std::vector<int64_t> > results;
  SearchException ( void ) { 
    type = 2;
    work_stack . reset ( new std::stack<int64_t> ); 
    results . reset ( new std::vector<int64_t> ); 
  }
  virtual ~SearchException ( void ) {}
  virtual void raise ( void ) { throw *this; }
};

template < class T, class D >
class NearestException : public SearchException<T,D> {
public:
  using MetricTreeException<T,D>::type;
  NearestException ( void ) { type = 3; }
  virtual ~NearestException ( void ) {}
  virtual void raise ( void ) { throw *this; }
  int64_t best_index;
  double best;
};

template < class T, class D >
class KNearestException : public SearchException<T,D> {
public:
  using MetricTreeException<T,D>::type;
  KNearestException ( void ) { type = 4; }
  virtual ~KNearestException ( void ) {}
  virtual void raise ( void ) { throw *this; }
  typedef std::set<std::pair<double, int64_t>, 
    std::greater<std::pair<double, int64_t> > > BestSet_t;
  BestSet_t best;
  int64_t k;
};

template < class T, class D >
class AspirationException : public SearchException<T,D> {
public:
  using MetricTreeException<T,D>::type;
  AspirationException ( void ) { type = 5; }
  virtual ~AspirationException ( void ) {}
  virtual void raise ( void ) { throw *this; }
  double delta;
};

template < class T, class D >
class DeltaCloseException : public SearchException<T,D> {
public:
  using MetricTreeException<T,D>::type;
  DeltaCloseException ( void ) { type = 6; }
  virtual ~DeltaCloseException ( void ) {}
  virtual void raise ( void ) { throw *this; }
  double delta;
};

}

