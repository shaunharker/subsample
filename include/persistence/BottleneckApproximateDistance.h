/// BottleneckApproximateDistance.h
/// Author: Rachel Levanger
/// Date: Nov 6, 2015
/// Edit history: 
///   - Created from BottleneckDistance.h to interface approximate algorithms with Subsample code (Rache Levanger, Nov 6, 2015)


#ifndef BOTTLENECKAPPROXIMATEDISTANCE_H
#define BOTTLENECKAPPROXIMATEDISTANCE_H

#include <vector>
#include "persistence/PersistenceDiagram.h"
#include "persistence/approximatedistances/geom_bottleneck/bottleneck/include/bottleneck.h"

double 
BottleneckApproximateDistance( subsample::PersistenceDiagram const& diagram_1, 
                    subsample::PersistenceDiagram const& diagram_2,
                    const double epsilon );


namespace BottleneckApproximateDistance_wrapper {

struct BottleneckApproximationWrapper {

    /* Generators for two persistence diagrams which are going to be compared */
    std::vector<subsample::Generator> Generators1;
    std::vector<subsample::Generator> Generators2;

    /* Read the generators into the approximate algorithm class. */
    bool populateDiagramPointSets(std::vector<std::pair<double, double>>& A,
                              std::vector<std::pair<double, double>>& B)
    {
        A.clear();
        B.clear();
        /* Read in generators from Generators1 to A */
        for ( std::vector<subsample::Generator>::const_iterator cur = Generators1.begin(); 
          cur != Generators1.end(); ++cur ) {
            A.push_back(std::make_pair(cur->birth, cur->death));
        }
        /* Read in generators from Generators2 to B */
        for ( std::vector<subsample::Generator>::const_iterator cur = Generators2.begin(); 
          cur != Generators2.end(); ++cur ) {
            B.push_back(std::make_pair(cur->birth, cur->death));
        }
        return true;
    }  

};

} //namespace

inline double 
BottleneckApproximateDistance( subsample::PersistenceDiagram const& diagram_1, 
                    subsample::PersistenceDiagram const& diagram_2,
                    const double epsilon ) {

    using namespace BottleneckApproximateDistance_wrapper;

    BottleneckApproximationWrapper bw;

    std::vector<subsample::Generator> & Generators1 = bw . Generators1;
    std::vector<subsample::Generator> & Generators2 = bw . Generators2;
    Generators1 . assign ( diagram_1 . begin (), diagram_1 . end () );
    Generators2 . assign ( diagram_2 . begin (), diagram_2 . end () );
  
    std::vector<std::pair<double, double>> A, B;
    if (!bw.populateDiagramPointSets(A, B)) {
        std::cout << "Could not convert PersistenceDiagrams to DiagramPointSets.\n";
        return -1;
    }


    double distance;
    if (epsilon > 0) {
        // the third parameter is epsilon,
        // return approximate distance (faster)
        distance = geom_bt::bottleneckDistApprox(A, B, epsilon);
    } else {
        // only filenames have been supplied, return exact distance
        distance = geom_bt::bottleneckDistExact(A, B);
        //res = bottleneckDistSlow(A, B);
    }
    return distance;

}

#endif
