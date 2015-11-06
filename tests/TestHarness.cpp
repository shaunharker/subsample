

/// TestHarness.cpp
/// Author: Rachel Levanger
/// Date: October 31, 2015


#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <cstdlib>
#include <cmath>
#include <limits>
#include <stdexcept>
#include <exception>

#include "boost/serialization/serialization.hpp"
#include "../include/persistence/PersistenceDiagram.h"
#include "../include/auction_algorithms/include/basic_defs.h"
#include "../include/auction_algorithms/bottleneck/bound_match.h"


class PersistPoint {
public:
  int64_t id;
  std::vector<PersistenceDiagram> pd;
  PersistPoint ( void ) {}
  PersistPoint ( std::vector<PersistenceDiagram> const& pd ) : pd(pd) {}

};


int main ( int argc, char * argv [] ) {

    std::cout << "SHAUN'S CLASSES\n";

    std::string p_path_1;
    std::string p_path_2;
    PersistPoint p_point_1;
    PersistPoint p_point_2;
    p_path_1 = argv[1];
    p_path_2 = argv[2];
    p_point_1.id = 1;
    p_point_1.pd.push_back(PersistenceDiagram(p_path_1));
    p_point_2.id = 2;
    p_point_2.pd.push_back(PersistenceDiagram(p_path_2));

    std::cout << "Persistence diagrams loaded.\n";

    std::vector<Generator> Generators1;
    Generators1 . assign ( p_point_1.pd[0] . begin (), p_point_1.pd[0] . end () );
    std::vector<Generator> Generators2;
    Generators2 . assign ( p_point_2.pd[0] . begin (), p_point_2.pd[0] . end () );
  
    std::cout << "Number of generators: " << Generators1.size() << ", " << Generators2.size() << "\n\n";


    std::cout << "AUCTION CLASSES\n";

    DiagramPointSet A, B;

    std::ifstream fA(p_path_1);
    if (!fA.good()) {
        std::cerr << "Cannot open file " << p_path_1 << std::endl;
    }
    std::ifstream fB(p_path_2);
    if (!fB.good()) {
        std::cerr << "Cannot open file " << p_path_2 << std::endl;
    }
    A.clear();
    B.clear();
    Point p;
    size_t uniqueId {MIN_VALID_ID};
    while(fA >> p.x >> p.y) {
        DiagramPoint dpA {p.x, p.y, DiagramPoint::NORMAL, uniqueId++};
        DiagramPoint dpB {0.5*(p.x +p.y), 0.5 *(p.x +p.y), DiagramPoint::DIAG, uniqueId++};
        A.insert(dpA);
        B.insert(dpB);
    }
    fA.close();
    while(fB >> p.x >> p.y) {
        DiagramPoint dpB {p.x, p.y, DiagramPoint::NORMAL, uniqueId++};
        DiagramPoint dpA {0.5 * (p.x + p.y), 0.5 * (p.x + p.y), DiagramPoint::DIAG, uniqueId++};
        B.insert(dpB);
        A.insert(dpA);
    }
    fB.close();

    std::cout << "DiagramPointSets loaded.\n";

    std::cout << "Number of generators: " << A.size() << ", " << B.size() << "\n";


    return 0;
};



