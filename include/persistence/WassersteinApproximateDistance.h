#ifndef WASSERSTEINAPPROXIMATEDISTANCE_H
#define WASSERSTEINAPPROXIMATEDISTANCE_H
#include <cmath>
#include <cstring>
#include <vector>
#include <algorithm>

#include "persistence/PersistenceDiagram.h"
#include "persistence/approximatedistances/geom_matching/wasserstein/src/include/basic_defs.h"
#include "persistence/approximatedistances/geom_matching/wasserstein/src/include/wasserstein.h"

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
    bool populateDiagramPointSets(geom_ws::DiagramPointSet& A,
                              geom_ws::DiagramPointSet& B)
    {
        
      A.clear();
      B.clear();

      size_t uniqueId = geom_ws::MIN_VALID_ID;
      size_t projId;
      for ( auto const& gen : Generators1 ) {
        double x = gen . birth;
        double y = gen . death;
        // normal point, its projection will be added next, so +1
        projId = uniqueId+1;
        geom_ws::DiagramPoint dpA {x, y, DiagramPoint::NORMAL, uniqueId++, projId};
        // diagonal point, its parent has been added, so -1
        projId = uniqueId-1;
        geom_ws::DiagramPoint dpB {x, y, DiagramPoint::DIAG, uniqueId++, projId};
        A.insert(dpA);
        B.insert(dpB);
      }
      for ( auto const& gen : Generators2 ) {
        double x = gen . birth;
        double y = gen . death;
        // normal point, its projection will be added next, so +1
        projId = uniqueId+1;
        geom_ws::DiagramPoint dpB {x, y, DiagramPoint::NORMAL, uniqueId++, projId};
        // diagonal point, its parent has been added, so -1
        projId = uniqueId-1;
        geom_ws::DiagramPoint dpA {x, y, DiagramPoint::DIAG, uniqueId++, projId};
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
    distance = geom_ws::wassersteinDist(A, B, p, epsilon, p);
    std::cout << "distance: " << distance << "\n";
    return distance;

}

#endif
