/// ComputeDistanceMatrix.cpp
/// Author: Shaun Harker
/// Date: July 17, 2014
#include <vector>
#include "cluster-delegator.hpp" 
#include "subsample/SubsampleConfig.h" // Defines class Point, class Distance


class ComputeMatrixProcess : public Coordinator_Worker_Process {
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
  int64_t job_num_;
  int64_t N_;
  int64_t last_job_;
  int64_t result_index_;
  subsample::DistanceMatrixConfig config_;
  std::vector<subsample::Point> subsamples_;
  std::vector<double> results_;
  subsample::Distance distance_;
  std::vector<int> distance_filter_;
};

void ComputeMatrixProcess::
command_line ( int argc, char * argv [] ) {
  config_ . assign ( argc, argv );
  argc_ = argc;
  argv_ = argv;
  distance_ = config_ . getDistanceFunctor ();
}

void ComputeMatrixProcess::
initialize ( void ) {
  subsamples_ = config_ . getSubsamples ();
  job_num_ = 0;
  result_index_ = 0;
  N_ = subsamples_ . size ();
  last_job_ = N_ * N_;
  results_ . resize ( (N_ * N_ - N_) / 2 );
  std::cout << "Result size: " << results_ . size() << ".\n";
  distance_filter_ = config_ . getDistanceFilter ();
}

int  ComputeMatrixProcess::
prepare ( Message & job ) {
  bool found_job = false;
  int64_t i, j;
  while ( not found_job ) {
    if ( ++ job_num_ >= last_job_ ) return 1;
    i = job_num_ / N_;
    j = job_num_ % N_;
    if ( i < j ) break;
  }
  std::cout << "prepare. preparing job (" << i << ", " << j << ")\n";
  std::cout << "index " << result_index_ << ": " << distance_filter_[result_index_] << ".\n";
  job << result_index_ ++;
  job << subsamples_[i];
  job << subsamples_[j];
  job << distance_filter_[result_index_ - 1];
  std::cout << "preparing complete.\n";
  return 0;
}

void ComputeMatrixProcess::
work ( Message & result, const Message & job ) const {
  std::cout << "working...\n";
  int64_t id;
  int distance_filter;
  subsample::Point x, y;
  job >> id;
  job >> x;
  job >> y;
  std::cout << "working index: " << id << "\n";
  job >> distance_filter;
  std::cout << id << ":distance filter: " << distance_filter << "\n";
  result << id;
  if ( distance_filter == 0 ) {
    result << 0.0;
    std::cout << id << ": result set to zero.\n";
  }
  else {
    result << distance_ ( x, y );
    std::cout << id << ": computed real distance.\n";
  }
  std::cout << id << ": working complete.\n";
}

void ComputeMatrixProcess::
accept ( const Message &result ) {
  std::cout << "accept.\n";
  int64_t id;
  double distance;
  result >> id;
  result >> distance;
  std::cout << "accepting result " << id << " " << distance << "\n";
  results_ [ id ] = distance;
  std::cout << "accepted.\n";
}

void ComputeMatrixProcess::
finalize ( void ) {
  std::cout << "finalize.\n";
  std::string filename = config_ . getOutputFile ();
  std::ofstream outfile ( filename );
  if ( not outfile ) throw std::runtime_error ( "Invalid output filename " + filename );
  for ( int64_t i = 0; i < results_ . size (); ++ i ) {
    if ( i > 0 ) outfile << " ";
    outfile << results_[i];
  }
  outfile << "\n";
  outfile . close ();
}

int main ( int argc, char * argv [] ) {
  typedef ComputeMatrixProcess Process;
  delegator::Start ();
  delegator::Run<Process> (argc, argv);
  delegator::Stop ();
  return 0;
}
