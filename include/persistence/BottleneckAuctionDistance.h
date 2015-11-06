/// BottleneckAuctionDistance.h
/// Author: Rachel Levanger
/// Date: Nov 6, 2015
/// Edit history: 
///   - Created from BottleneckDistance.h to interface auction algorithms with Subsample code (Rache Levanger, Nov 6, 2015)


#include <vector>
#include "persistence/PersistenceDiagram.h"
#include "persistence/AuctionDistances/bottleneck/src/basic_defs.h"
#include "persistence/AuctionDistances/bottleneck/src/bottleneck.cpp"

double 
BottleneckAuctionDistance( PersistenceDiagram const& diagram_1, 
                    PersistenceDiagram const& diagram_2,
                    const double epsilon );


namespace BottleneckAuctionDistance_detail {

struct BottleneckAuctionWrapper {

    /* Generators for two persistence diagrams which are going to be compared */
    std::vector<Generator> Generators1;
    std::vector<Generator> Generators2;

    /* Read the generators into the auction algorithm class. */
    bool populateDiagramPointSets(DiagramPointSet& A,
                              DiagramPointSet& B)
    {
        A.clear();
        B.clear();
        Point p;
        size_t uniqueId {MIN_VALID_ID};
        for ( std::vector<Generator>::const_iterator cur = Generators1.begin(); 
          cur != Generators1.end(); ++cur ) {
            DiagramPoint dpA {cur->birth, cur->death, DiagramPoint::NORMAL, uniqueId++};
            DiagramPoint dpB {0.5*(cur->birth +cur->death), 0.5 *(cur->birth + cur->death), DiagramPoint::DIAG, uniqueId++};
            A.insert(dpA);
            B.insert(dpB);
        }
        for ( std::vector<Generator>::const_iterator cur = Generators2.begin(); 
          cur != Generators2.end(); ++cur ) {
            DiagramPoint dpB {cur->birth, cur->death, DiagramPoint::NORMAL, uniqueId++};
            DiagramPoint dpA {0.5*(cur->birth +cur->death), 0.5 *(cur->birth + cur->death), DiagramPoint::DIAG, uniqueId++};
            B.insert(dpB);
            A.insert(dpA);
        }
        return true;
    }  

};

} //namespace

inline double 
BottleneckAuctionDistance( PersistenceDiagram const& diagram_1, 
                    PersistenceDiagram const& diagram_2,
                    const double epsilon ) {
    using namespace BottleneckAuctionDistance_detail;

    BottleneckAuctionWrapper bw;

    std::vector<Generator> & Generators1 = bw . Generators1;
    std::vector<Generator> & Generators2 = bw . Generators2;
    Generators1 . assign ( diagram_1 . begin (), diagram_1 . end () );
    Generators2 . assign ( diagram_2 . begin (), diagram_2 . end () );
  
    DiagramPointSet A, B;
    if (!bw.populateDiagramPointSets(A, B)) {
        std::cout << "Could not convert PersistenceDiagrams to DiagramPointSets.\n";
        return -1;
    }


    double distance;
    if (epsilon > 0) {
        // the third parameter is epsilon,
        // return approximate distance (faster)
        distance = bottleneckDistApprox(A, B, epsilon);
    } else {
        // only filenames have been supplied, return exact distance
        distance = bottleneckDistExact(A, B);
        //res = bottleneckDistSlow(A, B);
    }
    return distance;

}

