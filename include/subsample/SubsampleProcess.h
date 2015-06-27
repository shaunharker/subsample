/// SubsampleProcess.h
/// Author: Shaun Harker 
/// July 2, 2014

#ifndef SUBSAMPLEPROCESS_H
#define SUBSAMPLEPROCESS_H

#include "geometry/MetricTree.h"
#include <exception>
#include <stdexcept>
#include "boost/foreach.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/thread/thread.hpp"
#include "boost/thread/mutex.hpp"
#include "SubsampleConfig.h"

#include "delegator/delegator.h"

template < class T, class D >
class SubsampleProcess : public Coordinator_Worker_Process {
public:
  void command_line ( int argc, char * argv [] );
  void initialize ( void ); 
  int  prepare ( Message & job ); 
  void work ( Message & result, const Message & job ) const;
  void accept ( const Message &result ); 
  void finalize ( void ); 
private:
  int argc_;
  char ** argv_;
  MetricTree<T,D> mt_;
  std::vector<T> samples_;
  double delta_;
  std::stack<int64_t> ready_;
  boost::mutex mutex_;
  boost::shared_ptr<D> distance_;
  bool all_done_;
  std::stack<std::pair<int64_t,std::pair<T,T> > > work_items_;
  boost::shared_ptr<boost::thread> thread_ptr;
  mutable int64_t time_delay_;
  int64_t cohort_size_;
  SubsampleConfig config_;
};

template < class T, class D >
class AspirationFunctor {
public:
  typedef bool ReturnType;
  typedef typename MetricTree<T,D>::AspirationException Exception;
  AspirationFunctor ( MetricTree<T,D> * mt, 
                      std::vector<T> const& samples, 
                      double delta ) 
    : mt_(mt), samples_(samples), delta_(delta) {}
  ReturnType operator () ( int64_t i ) { 
    return ( mt_ -> aspiration ( samples_ [ i ], delta_ ) == mt_ -> end () ); 
  }
  ReturnType operator () ( Exception & e ) { 
    return ( mt_ -> aspiration ( e ) == mt_ -> end () ); 
  }
private:
  MetricTree<T,D> * mt_;
  std::vector<T> const& samples_;
  double delta_;
};

template < class T, class D >
class InsertFunctor {
public:
  typedef int64_t ReturnType;
  typedef typename MetricTree<T,D>::InsertException Exception;
  InsertFunctor ( MetricTree<T,D> * mt, 
                  std::vector<T> const& samples )
    : mt_(mt), samples_(samples) {}
  ReturnType operator () ( int64_t i ) {
    return mt_ -> index ( mt_ -> insert ( samples_ [ i ] ) ); 
  }
  ReturnType operator () ( Exception & e ) { 
    return mt_ -> index ( mt_ -> insert ( e ) ); 
  }
private:
  MetricTree<T,D> * mt_;
  std::vector<T> const& samples_;
};

template < class T, class D >
class DeltaCloseFunctor {
public:
  typedef std::vector< typename MetricTree<T,D>::iterator> ReturnType;
  typedef typename MetricTree<T,D>::DeltaCloseException Exception;
  DeltaCloseFunctor ( MetricTree<T,D> * mt, 
                      std::vector<T> const& samples, 
                      double delta ) 
    : mt_(mt), samples_(samples), delta_(delta) {}
  ReturnType operator () ( int64_t i ) { 
    return mt_ -> deltaClose ( samples_ [ i ], delta_); 
  }
  ReturnType operator () ( Exception & e ) { 
    return mt_ -> deltaClose ( e ); 
  }
private:
  MetricTree<T,D> * mt_;
  std::vector<T> const& samples_;
  double delta_;
};


template < class T, class D >
class SubsampleThread {
public:
  SubsampleThread ( MetricTree<T,D> * mt,
                    std::vector<T> const& samples, 
                    double delta, 
                    std::stack<int64_t> * ready, 
                    boost::mutex * mutex,
                    bool * all_done, 
                    std::stack<std::pair<int64_t,std::pair<T,T > > > * work_items,
                    boost::shared_ptr<D> distance, 
                    int64_t cohort_size ) 
    : mt_(mt), samples_(samples), delta_(delta), 
      ready_(ready), mutex_(mutex), all_done_(all_done), 
      work_items_(work_items), distance_(distance), cohort_size_(cohort_size) {}
  void operator () ( void );
  template < class FunctionObject > void
  parallel ( std::vector<typename FunctionObject::ReturnType> * results,
             std::vector<int64_t> const& arguments,
             FunctionObject & F );
private:
  MetricTree<T,D> * mt_;
  double delta_;
  std::vector<T> const& samples_;
  std::stack<int64_t> * ready_;
  boost::mutex * mutex_;
  bool * all_done_;
  std::stack<std::pair<int64_t,std::pair<T,T > > > * work_items_;
  boost::shared_ptr<D> distance_;
  int64_t time_delay_;
  int64_t cohort_size_;
};

template < class T, class D >
void SubsampleThread<T,D>::
operator () ( void ) {
  typedef typename MetricTree<T,D>::iterator iterator;
  int64_t N = 0;
  while ( N < samples_ . size () ) {
    // Stage 1. Aspiration Search Stage (identify candidates)
    //std::cout << "Stage 1. N = " << N << "\n";
    //std::cout << "cohort_size_ = " << cohort_size_ << "\n";
    std::vector<int64_t> candidates;
    /* Stage 1 */ {
      AspirationFunctor<T,D> functor ( mt_, samples_, delta_ );
      while ( N < samples_ . size () && candidates . size () < cohort_size_ ) {
        std::vector<int64_t> arguments;
        while ( N < samples_ . size () && arguments . size () < cohort_size_ ) {
          arguments . push_back ( N );
          ++ N;
        }
        std::vector<bool> results;
        parallel ( &results, arguments, functor );
        for ( int i = 0; i < results . size (); ++ i ) {
          if ( results [ i ] ) {
            candidates . push_back ( arguments [ i ] );
          }
        }
      }
    }
    // Stage 2. Build candidate Metric Tree.
    //std::cout << "Stage 2. N = " << N << "\n";
    MetricTree<T,D> candidate_mt;
    candidate_mt . assign ( distance_ );
    boost::unordered_map<int64_t, int64_t> iterator_to_candidate_number;
    /* Stage 2 */ {
      InsertFunctor<T,D> functor ( &candidate_mt, samples_ );
      std::vector<int64_t> results;
      std::vector<int64_t> arguments;
      for ( int i = 0; i < candidates . size (); ++ i ) {
        arguments . push_back ( candidates [ i ] );
        //std::cout << candidates[i] << " ";
      }
      parallel ( &results, arguments, functor );
      for ( int i = 0; i < candidates . size (); ++ i ) {
        iterator_to_candidate_number [ results [ i ] ] = i;
      }
    }
    
    // DEBUG 
    //std::cout << "candidate_mt . size () == " << candidate_mt . size () << "\n";
    // END DEBUG

    // Stage 3. Build adjacency lists for delta-closeness in 
    //          candidate metric tree.
    //std::cout << "Stage 3. N = " << N << "\n";
    std::vector< std::vector<int64_t> > adjacency_structure ( candidates . size () );
    /* Stage 3 */ {
      DeltaCloseFunctor<T,D> functor ( &candidate_mt, samples_, delta_ );
      std::vector<int64_t> arguments;
      for ( int i = 0; i < candidates . size (); ++ i ) {
        arguments . push_back ( candidates [ i ] );
      }
      std::vector< std::vector<iterator> > results;
      parallel ( &results, arguments, functor );
      //std::cout << "Building adjacency structure.\n";
      for ( int i = 0; i < results . size (); ++ i ) {
        //std::cout << "adjacency_structure[" << i << "] = ";
        for ( int j = 0; j < results [ i ] . size (); ++ j ) {
          adjacency_structure [ i ] . 
            push_back ( iterator_to_candidate_number 
              [ candidate_mt . index ( results [ i ] [ j ] ) ] );
          //std::cout << adjacency_structure [ i ] . back () << " ";
        }
        //std::cout << "\n";
      }
    }

    // Stage 4. Compute a maximal independent set of the graph on 
    //          candidates described by adjacency_structure. We use 
    //          a serial greedy algorithm.
    //std::cout << "Stage 4. N = " << N << "\n";
    std::vector<bool> accepted ( candidates . size (), true );
    for ( int i = 0; i < candidates . size (); ++ i ) {
      if ( accepted [ i ] ) {
        std::vector<int64_t> & adjacency_list = 
          adjacency_structure [ i ];
        for ( int64_t j : adjacency_list ) {
          if ( i == j ) continue;
          accepted [ j ] = false;
        }
      }
    }

    // Stage 5. Insert accepted candidates.
    //std::cout << "Stage 5. N = " << N << "\n";
    /* Stage 5 */ { 
      InsertFunctor<T,D> functor ( mt_, samples_ );
      std::vector<int64_t> results;
      std::vector<int64_t> arguments;
      for ( int i = 0; i < candidates . size (); ++ i ) {
        if ( accepted [ i ] ) {
          arguments . push_back ( candidates [ i ] );
        }
      }
      parallel ( &results, arguments, functor );
    }
  }
  mutex_ -> lock ();
  //std::cout << "All done! \n";
  * all_done_ = true;
  mutex_ -> unlock ();
}

template < class T, class D >
template < class FunctionObject > void SubsampleThread<T,D>::
parallel ( std::vector<typename FunctionObject::ReturnType> * results,
           std::vector<int64_t> const& arguments,
           FunctionObject & F ) {
  typedef typename FunctionObject::ReturnType ReturnType;
  typedef typename FunctionObject::Exception Exception;
  time_delay_ = 1;
  results -> resize ( arguments . size () );

  boost::unordered_map <int64_t, Exception> exceptions;
  int64_t num_to_compute = arguments . size ();    
  int64_t computed = 0;
  if ( not ready_ -> empty () ) {
    throw std::logic_error ( "Did not finish previous stage.\n");
  }
  mutex_ -> lock ();
  for ( int64_t n = 0; n < num_to_compute; ++ n ) {
    ready_ -> push ( n );
  }
  mutex_ -> unlock ();
  
  while ( computed < num_to_compute ) {
    // DEBUG BEGIN
    /*
    for ( int i = 0; i < num_to_compute; ++ i ) {
      if ( exceptions . count ( i ) != 0 ) {
        try {
          typename MetricTree<T,D>::InsertException& ie =  
          dynamic_cast<typename MetricTree<T,D>::InsertException&>(exceptions [i ]);
          std::cout << "insertion exception " << i << " inspected: e.index = " << ie.index << "\n";
        } catch ( std::bad_cast& bc ) { std::cout << "not an insert exception inspected (0)\n";}
      }
    }
    */
    // DEBUG END

    mutex_ -> lock ();
    if ( ready_ -> empty () ) {
      mutex_ -> unlock ();
      //std::cout << "Waiting " << time_delay_ << " microseconds.\n";
      boost::this_thread::sleep( boost::posix_time::microseconds(time_delay_) );
      if ( time_delay_ < 1000000 ) time_delay_ = time_delay_ * 2;
      continue;
    }
    time_delay_ = 1;
    //std::cout << "Processing something.\n";
    int64_t n = ready_ -> top ();
    //std::cout << "Processing " << n << "\n";

    ready_ -> pop ();
    mutex_ -> unlock ();
    try {
      if ( exceptions . count ( n ) == 0 ) {
        //std::cout << "Initializing operation " << n << "\n";
        (*results)[n] = F ( arguments [ n ] );
      } else {
        //std::cout << "Resuming operation " << n << "\n";
        /*
        try {
          typename MetricTree<T,D>::InsertException& ie =  
          dynamic_cast<typename MetricTree<T,D>::InsertException&>(exceptions [ n ]);
          std::cout << "insertion exception " << n << " resumed: e.index = " << ie.index << "\n";
        } catch ( std::bad_cast& bc ) { std::cout << "not an insert exception resumed (1).\n";}
        */
        (*results)[n] = F ( exceptions [ n ] );
        exceptions . erase ( n );
      }
      ++ computed;
    } catch ( typename MetricTree<T,D>::Exception & e ) {
      //std::cout << "parallel. Caught the exception!\n";
      mutex_ -> lock ();
      while ( not e . calculations -> empty () ) {
        std::pair < int64_t, std::pair < T, T > > item;
        item . first = n;
        item . second = e . calculations -> top ();
        //std::cout << "pushing work_item ( " << n << ", " << item.second.first <<
        //  ", " << item.second.second << ")\n";
        e . calculations -> pop ();
        work_items_ -> push ( item );
      }
      mutex_ -> unlock ();
      exceptions [ n ] = dynamic_cast<Exception&>(e);
      /*
      try {
          typename MetricTree<T,D>::InsertException& ie =  
          dynamic_cast<typename MetricTree<T,D>::InsertException&>(exceptions [ n ]);
          std::cout << "insertion exception " << n << " created: e.index = " << ie.index << "\n";
        } catch ( std::bad_cast& bc ) { std::cout << "not an insert exception created (1)\n";}
      */
    }
  }
}

template < class T, class D >
void SubsampleProcess<T,D>::
command_line ( int argc, char * argv [] ) {
  config_ . assign ( argc, argv );
  argc_ = argc;
  argv_ = argv;
  time_delay_ = 1;
  distance_ . reset ( new D ( config_ . getDistanceFunctor () ) );
  cohort_size_ = config_ . getCohortSize ();
}

template < class T, class D >
void SubsampleProcess<T,D>::
initialize ( void ) {
  all_done_ = false;
  samples_ = config_ . getSamples ();
  delta_   = config_ . getDelta ();
  mt_ . assign ( distance_ );
  thread_ptr . reset ( new boost::thread 
    ( SubsampleThread<T,D> ( &mt_, samples_, delta_, &ready_, &mutex_, 
                             &all_done_, &work_items_, distance_, cohort_size_ ) ) );
}

template < class T, class D >
int SubsampleProcess<T,D>::
prepare ( Message & job ) {
  mutex_ . lock ();
  if ( all_done_ ) { 
    //std::cout << "prepare. All done!\n";
    mutex_ . unlock ();
    return 1;
  }
  if ( work_items_ . empty () ) {
    job << (int64_t) 0;
    mutex_ . unlock ();
    return 0;
  }
  std::pair < int64_t, std::pair < T, T > > & item =
    work_items_ . top ();
  job << (int64_t) 1;
  job << item . first;
  job << item . second . first;
  job << item . second . second;
  //std::cout << "popping work_item ( " << item . first << ", " << item.second.first <<
  //        ", " << item.second.second << ")\n";
  work_items_ . pop ();
  mutex_ . unlock ();
  return 0;
}

template < class T, class D >
void SubsampleProcess<T,D>::
work ( Message & result, 
       const Message & job ) const {
  int64_t t;
  job >> t;
  if ( t == 0 ) {
    // Timer Job.
    boost::this_thread::sleep( boost::posix_time::microseconds(time_delay_) );
    if ( time_delay_ < 1000000 ) time_delay_ = time_delay_ * 2;
    result << (int64_t) 0;
  } else {
    time_delay_ = 1;
    // Distance Job.
    int64_t n;
    T p;
    T q;
    job >> n;
    job >> p;
    job >> q;
    result << (int64_t) 1;
    result << n;
    result << p;
    result << q;
    result << distance_ -> compute (p, q);
    //std::cout << "Computed distance between " << p << " and " << q << "\n";
  }
}

template < class T, class D >
void SubsampleProcess<T,D>::
accept ( const Message &result ) {
  int64_t t;
  result >> t;
  if ( t == 0 ) return;
  int64_t n;
  T p;
  T q;
  double dist;
  result >> n;
  result >> p;
  result >> q;
  result >> dist;
  //std::cout << "Received distance between " << p << " and " << q << "\n";

  distance_ -> cache ( p, q, dist );
  mutex_ . lock ();
  ready_ . push ( n );
  mutex_ . unlock ();
}

template < class T, class D >
void SubsampleProcess<T,D>::
finalize ( void ) {
  //std::cout << "finalize.\n";
  std::vector<T> results;
  for ( T const& p : mt_ ) {
    results . push_back ( p );
  }
  config_ . handleResults ( results );
}

#endif
