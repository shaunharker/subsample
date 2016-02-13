#ifndef WASSERSTEINDISTANCE_H
#define WASSERSTEINDISTANCE_H
#include <cmath>
#include <cstring>
#include <vector>
#include <algorithm>
#include "PersistenceDiagram.h"

double 
WassersteinDistance( subsample::PersistenceDiagram const& diagram_1, 
                     subsample::PersistenceDiagram const& diagram_2,
                     double p );


namespace WassersteinDistance_detail {

// What follows is adapted from notes given in
// http://community.topcoder.com/tc?module=Static&d1=tutorials&d2=hungarianAlgorithm
  class Hungarian {
  public:
    double * cost_;
    int n, max_match;        //n workers and n jobs
    std::vector< double > lx, ly;
    std::vector<int> xy, yx;
    std::vector<bool> S, T;
    std::vector<double> slack, slackx;
    std::vector<int> prev;
    std::vector<int> q;

    double cost ( int i, int j ) const {
      return -cost_ [ j + i * n ];
    }

    Hungarian ( int n_in, double * cost_in ) : n ( n_in ), cost_ ( cost_in ) {
      max_match = 0;
      xy . assign ( n, -1 );
      yx . assign ( n, -1 );
      prev . resize ( n, 0 );
      q . resize ( n );
      slack . resize ( n , 0.0 );
      slackx . resize ( n , 0.0 );
      lx . resize ( n, 0.0 );
      ly . resize ( n, 0.0 );
      for (int x = 0; x < n; ++ x )
        for (int y = 0; y < n; ++ y )
          lx[x] = std::max(lx[x], cost(x,y) );
    }

    void update_labels( void ) {
      int x, y;
      double delta = std::numeric_limits<double>::infinity();
      for (y = 0; y < n; ++ y ) if (!T[y]) delta = std::min(delta, slack[y]);
      for (x = 0; x < n; ++ x ) if (S[x]) lx[x] -= delta;
      for (y = 0; y < n; ++ y ) if (T[y]) ly[y] += delta;
      for (y = 0; y < n; ++ y ) if (!T[y]) slack[y] -= delta;
    }

    void add_to_tree(int x, int prevx) {
      S[x] = true;
      prev[x] = prevx;
      for (int y = 0; y < n; ++ y ) {
        if (lx[x] + ly[y] - cost(x,y) < slack[y]) {
          slack[y] = lx[x] + ly[y] - cost(x,y);
          slackx[y] = x;
        }
      }
    }

    int augment( void ) {
      if (max_match == n) return 0;
      int x, y, root;
      int wr = 0;
      int rd = 0;
      S . assign ( n, false );
      T . assign ( n, false );      
      prev . assign ( n, -1 );      
      for (x = 0; x < n; ++ x) {
        if (xy[x] == -1) {
          q[wr++] = root = x;
          prev[x] = -2;
          S[x] = true;
          break;
        }
      }
      for (y = 0; y < n; ++ y ) {
        slack[y] = lx[root] + ly[y] - cost(root,y);
        slackx[y] = root;
      }
      while (true) {
        while (rd < wr) {
          x = q[rd++];
          for ( y = 0; y < n; ++ y ) { 
            if (cost(x,y) == lx[x] + ly[y] &&  !T[y]) {
              if (yx[y] == -1) break;
              T[y] = true;
              q[wr++] = yx[y];
              add_to_tree(yx[y], x);
            }
          }
          if (y < n) break;
        }
        if (y < n) break;
        update_labels();
        wr = rd = 0;                
        for ( y = 0; y < n; ++ y ) {
          if (!T[y] &&  slack[y] == 0 ) {
            if (yx[y] == -1) {
              x = slackx[y];
              break;
            } else {
              T[y] = true; 
              if (!S[yx[y]]) {
                q[wr++] = yx[y];
                add_to_tree(yx[y], slackx[y]);
              }
            }
          }
        }
        if (y < n) break;
      }
      if (y < n) {
        max_match++;
        for (int cx = x, cy = y, ty; cx != -2; cx = prev[cx], cy = ty) {
          ty = xy[cx];
          yx[cy] = cx;
          xy[cx] = cy;
        }
        return 1; //augment();
      }
      std::cout << "Unexpected point of execution. " << max_match << "\n";
      return 0;
    }

    double getPrice ( void ) {
      double ret = 0;
      while ( augment() );
      for (int x = 0; x < n; x++) ret += cost(x,xy[x]);
      return -ret;
    }
  };

}

inline double 
WassersteinDistance( subsample::PersistenceDiagram const& diagram_1, 
                     subsample::PersistenceDiagram const& diagram_2,
                     double p ) {
  using namespace WassersteinDistance_detail;
  subsample::Generator::Distance distance; 
  // Distance matrix the generators in diagram_1 and diagram_2 
  double* distanceMatrix;
  // Distance matrix has (diagram_1.size +  diagram_2.size())^2 elements 
  unsigned int matrixSize = diagram_1.size() + diagram_2.size();
  // Allocate the matrix 
  distanceMatrix = new double[matrixSize * matrixSize];
  // If both persistence diagrams are empty than their distance is zero 
  if ( !matrixSize ) return 0;
  // Prepare distance matrix (if there is different number of generators, 
  // then we add extra rows (columns)
  //   with distance of the generator from the diagonal 
  for( unsigned int  i = 0; i < matrixSize; ++ i ) {
    for( unsigned int j = 0; j < matrixSize; ++ j ) {
      if( i >= diagram_1.size() && j >=  diagram_2.size() ){
        distanceMatrix[ i * matrixSize + j ] = 0;
      } else if ( i >= diagram_1.size() ) {
        distanceMatrix[ i * matrixSize + j ] = 
          std::pow ( distance . diagonal ( diagram_2[ j ] ), p );
      } else if ( j >=  diagram_2.size() ) {
        distanceMatrix[ i * matrixSize + j ] = 
          std::pow ( distance . diagonal ( diagram_1[ i ] ), p );
      } else {
        distanceMatrix[ i * matrixSize + j ] = 
          std::pow ( distance( diagram_1[ i ], diagram_2[ j ] ), p );
      }
    }
  }
  // Class with Munkres algorithm for computing minimal price of the matrix 
  Hungarian munkres ( matrixSize, distanceMatrix );
  double result = munkres . getPrice ();
  delete[] distanceMatrix;
  if( p == 1) return result;
  if ( p == 2 ) return std::sqrt((double) ( result ));
  return std::pow( result, 1.0 / p );
}

#endif
