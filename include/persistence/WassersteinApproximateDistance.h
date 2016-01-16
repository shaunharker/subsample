#ifndef WASSERSTEINAPPROXIMATEDISTANCE_H
#define WASSERSTEINAPPROXIMATEDISTANCE_H
#include <cmath>
#include <cstring>
#include <vector>
#include <algorithm>

#include "persistence/PersistenceDiagram.h"
#include "persistence/approximatedistances/geom_matching/wasserstein/include/wasserstein.h"

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
  
    std::vector<std::pair<double, double>> A, B;
    if (!ww.populateDiagramPointSets(A, B)) {
        std::cout << "Could not convert PersistenceDiagrams to DiagramPointSets.\n";
        return -1;
    }


    double distance;
    distance = geom_ws::wassersteinDist(A, B, p, epsilon, p);
    std::cout << "distance: " << distance << "\n";
    return distance;

}

#endif
