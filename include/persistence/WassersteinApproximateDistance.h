#ifndef WASSERSTEINAPPROXIMATEDISTANCE_H
#define WASSERSTEINAPPROXIMATEDISTANCE_H
#include <cmath>
#include <cstring>
#include <vector>
#include <algorithm>

#include "persistence/PersistenceDiagram.h"
#include "persistence/approximatedistances/geom_matching/wasserstein/src/basic_defs.h"
#include "persistence/approximatedistances/geom_matching/wasserstein/src/wasserstein.cpp"

double 
WassersteinApproximateDistance( subsample::PersistenceDiagram const& diagram_1, 
                     subsample::PersistenceDiagram const& diagram_2,
                     const double p,
                     const double epsilon );


namespace WassersteinApproximateDistance_wrapper {

struct WassersteinApproximationWrapper {
    /* Generators for two persistence diagrams which are going to be compared */
    std::vector<subsample::Generator> Generators1;
    std::vector<subsample::Generator> Generators2;

    /* Read the generators into the approximate algorithm class. */
    bool populateDiagramPointSets(DiagramPointSet& A,
                              DiagramPointSet& B)
    {
        A.clear();
        B.clear();
        size_t uniqueId {MIN_VALID_ID};
        for ( std::vector<subsample::Generator>::const_iterator cur = Generators1.begin(); 
          cur != Generators1.end(); ++cur ) {
            DiagramPoint dpA {cur->birth, cur->death, DiagramPoint::NORMAL, uniqueId++};
            DiagramPoint dpB {0.5*(cur->birth +cur->death), 0.5 *(cur->birth + cur->death), DiagramPoint::DIAG, uniqueId++};
            A.insert(dpA);
            B.insert(dpB);
        }
        for ( std::vector<subsample::Generator>::const_iterator cur = Generators2.begin(); 
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
WassersteinApproximateDistance( subsample::PersistenceDiagram const& diagram_1, 
                    subsample::PersistenceDiagram const& diagram_2,
                    const double p,
                    const double epsilon ) {

    using namespace WassersteinApproximateDistance_wrapper;

    WassersteinApproximationWrapper ww;

    std::vector<subsample::Generator> & Generators1 = ww . Generators1;
    std::vector<subsample::Generator> & Generators2 = ww . Generators2;
    Generators1 . assign ( diagram_1 . begin (), diagram_1 . end () );
    Generators2 . assign ( diagram_2 . begin (), diagram_2 . end () );
  
    DiagramPointSet A, B;
    if (!ww.populateDiagramPointSets(A, B)) {
        std::cout << "Could not convert PersistenceDiagrams to DiagramPointSets.\n";
        return -1;
    }


    double distance;
    distance = wassersteinDist(A, B, p, epsilon, p);
    std::cout << "distance: " << distance << "\n";
    return distance;

}

#endif
