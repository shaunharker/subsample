/// BottleneckDistance.h
/// Author: Miro Kramar
/// Date: Feb 21, 2012
/// Edit history: 
///   - refactored to interface with Subsample code (Shaun Harker July 7, 2014)

#ifndef HOPKROFTKARPBOTTLENECKDISTANCE_H
#define HOPKROFTKARPBOTTLENECKDISTANCE_H

#include <vector>
#include <queue>
#include <algorithm>
#include "persistence/PersistenceDiagram.h"

double 
BottleneckDistance( subsample::PersistenceDiagram const& diagram_1, 
                    subsample::PersistenceDiagram const& diagram_2 );


namespace BottleneckDistance_detail {

class Edge{
public:
  int vertex_1;
  int vertex_2;
  double weight;
  Edge(int v1, int v2, double w ){ vertex_1 = v1; vertex_2 = v2; weight = w;}
  bool operator<(const Edge& other) const { return weight < other.weight; }
};

struct BottleneckProblem {

  /* Generators for two persistence diagrams which are going to be compared */
  std::vector<subsample::Generator> Generators1;
  std::vector<subsample::Generator> Generators2;
  /* Number of generators in Generator1 and Generator2 */
  unsigned int  Max_Size;
  /* Edges between all the nodes given by generators of the persistence diagrams */
  std::vector<Edge> Edges;
  /* Pairing between the vertices in the persistence diagrams
   * -1 means the vertex is not paired to any other vertex it is paired to special vertex NIL
   * non negative number is the index of the vertex to which the given vertex is paired*/
  std::vector< int > Pair;
  /*Connection matrix gives the edges between the vertexes in the first diagram and the second*/
  std::vector< std::vector < int > > Connections;
  /*Layers used in Hopcroft-Karp algorithm
   * the layer information is required for a NIL (special) vertex and all vertices in Generators1
   * Hence 0 slot is used for NIL and all the vertices in Generators1 are shifted by one. So to
   * read layer of the vertex 0 we access the Layers[1] */
  std::vector<int> Layers;

  void PrepareEdges( void ){
    subsample::Generator::Distance distance;
    /*Set the number of generators */
    Max_Size = Generators1.size() + Generators2.size();
    /*Clear edges */
    Edges.clear();
    /* Connect all diagonal points to each other */
    for (unsigned int i = Generators1.size(); i < Max_Size; ++i)
      for (unsigned int j = Max_Size + Generators2.size(); j < 2*Max_Size; ++j)
        Edges.push_back(Edge(i, j, 0));
    /* Edges between real points */
    unsigned int i = 0;
    for ( std::vector<subsample::Generator>::const_iterator cur1 = Generators1.begin(); 
          cur1 != Generators1.end(); ++cur1 ) {
      unsigned int j = Max_Size;
      for ( std::vector<subsample::Generator>::const_iterator cur2 = Generators2.begin(); 
            cur2 != Generators2.end(); ++cur2 ) {
        Edges.push_back(Edge(i,j++, distance(*cur1, *cur2)));
      }
      ++i;
    }
    /* Edges between real points and their corresponding diagonal points */
    i = 0;
    for ( std::vector<subsample::Generator>::const_iterator cur1 = Generators1.begin(); 
          cur1 != Generators1.end(); ++cur1, ++i)
      Edges.push_back( Edge( i, Max_Size + Generators2.size() + i, distance . diagonal ( *cur1 ) ) );
    i = Max_Size;
    for ( std::vector<subsample::Generator>::const_iterator cur2 = Generators2.begin(); 
          cur2 != Generators2.end(); ++cur2, ++i)
      Edges.push_back( Edge( Generators1.size() + (i - Max_Size), i, distance . diagonal ( *cur2 ) ) );
    std::sort(Edges.begin(), Edges.end());
  }


  /* Depth first search used by Hopf-Karp algorithm */
  bool DFS( int v ) {
    if( v < 0 ) return true;
    /* for every adjacent vertex u of v */
    for( unsigned int i = 0; i < Connections[ v ].size(); ++i ){
      int u = Connections[ v ][ i ];
      if( Layers[ Pair[ u ] + 1 ] == Layers[ v + 1 ] + 1 ){
        if( DFS( Pair[ u ] ) ){
          Pair[ u ] = v;
          Pair[ v ] = u;
          return true;
        }
      }
    }
    Layers[ v + 1 ] = -1;
    return false;
  }
  /* Breath first search used by Hopf-Karp algorithm */
  bool BFS( void ){
    std::queue< int > vertex_queue;

    /* For every vertex v given by Generators1 */
    for( unsigned int v = 0; v < Max_Size; ++v ){
      /* If its not paired to vertex in Generators2 */
      if ( Pair[ v ] < 0 ){
        /* Set its layer to 0 and put it in the queue */
        Layers[ v + 1 ] = 0;
        vertex_queue.push( v );
      }
      else{
        /* Otherwise mark it as matched (set Layer to NILL)*/
        Layers[ v + 1 ] = -1;
      }
    }
    /* Set layer for NIL */
    Layers[ 0 ] = -1;
    /* Search the vertices in the queue */
    while ( !vertex_queue.empty( ) ){
      int v = vertex_queue.front();
      vertex_queue.pop();
      if( Layers[ v + 1 ] > Layers[ 0 ] ){
      for( unsigned int i = 0; i < Connections[ v ].size(); ++i ){
          int u = Connections[ v ][ i ];
          /* Check if the vertex has an edge to the match vertex */
          if( Layers[ Pair[ u ] + 1 ] < 0 ){
            /* Set the layer of the vertex (it can be NILL) which is 
               matched to the matched vertex u  */
            Layers[ Pair[ u ] + 1 ] = Layers[ v + 1 ] + 1;
            /* If the pairing vertex is not NIL add it into the queue */
            if( Pair[ u ] != -1 ) vertex_queue.push( Pair[u] );
          }
        }
      }
    }
    return (Layers[ 0 ] != -1);
  }
  /*Hopf-Karp algorithm to find maximal matching*/
  void Hopcroft_Karp(unsigned int &matching ){
    while( BFS() == true )
      for( unsigned int vertex = 0; vertex < Max_Size; ++vertex ){
        if( Pair[ vertex ] == -1){
          if( DFS( vertex ) ){
            ++matching;
          }
        }
      }
  }

};

} //namespace

inline double 
BottleneckDistance( subsample::PersistenceDiagram const& diagram_1, 
                    subsample::PersistenceDiagram const& diagram_2 ) {
  using namespace BottleneckDistance_detail;
  BottleneckProblem bp;
  std::vector<int> & Pair = bp.Pair;
  std::vector<int> & Layers = bp.Layers;
  std::vector< std::vector < int > > & Connections = bp.Connections;
  std::vector<subsample::Generator> & Generators1 = bp . Generators1;
  std::vector<subsample::Generator> & Generators2 = bp . Generators2;
  unsigned int & Max_Size = bp . Max_Size;
  std::vector<Edge> & Edges = bp . Edges;
  Generators1 . assign ( diagram_1 . begin (), diagram_1 . end () );
  Generators2 . assign ( diagram_2 . begin (), diagram_2 . end () );
  /* If both diagrams are empty the distance is 0 */
  if( Generators1.size() == 0 &&  Generators2.size() == 0 ) return 0;
  bp.PrepareEdges ();

  /* Clear the pairing */
  Pair.clear( );
  Pair.assign( 2*Max_Size, -1);
  /* Clearing Layers */
  Layers.clear();
  Layers.resize( Max_Size + 1 );
  /* No vertices are matched */
  unsigned int matching = 0;
  /* Clear the connection matrix and set it to the right size */
  Connections.clear();
  Connections.resize( Max_Size );
  /* The maximal weight of the edges which are used for the matching */
  double current_weight = 0;
  /* First non added edge is an iterator pointing to the first edge 
     in Edges which was added to the Connections */
  unsigned int first_not_added_edge = 0;
  /* Repeat till all the vertices are matched */
  while( matching < Max_Size){
    /* Add the edges with the current weight (distance) to the connection matrix */
    while ( Edges[ first_not_added_edge ].weight == current_weight && 
            first_not_added_edge < Edges.size()){
      /*Add the edge to Connections*/
      Connections[ Edges[ first_not_added_edge ].vertex_1 ].
        push_back( Edges[ first_not_added_edge ].vertex_2);
      ++first_not_added_edge;
    }
    /* Do matching */
    bp.Hopcroft_Karp( matching );
    if( matching == Max_Size ){          
      return current_weight;
    }
    /* Check if we did not run out of edges. This should never happen. */
    if( first_not_added_edge == Edges.size() ){
      std::cout << "Serious problem - Not enough edges to find the matching! \n";
      return -1;
    }
    /* Increase the value of the current weight */
    current_weight = Edges[ first_not_added_edge ].weight;
  }
  return -1;
}

#endif
