/*
    Copyrigth 2015, D. Morozov, M. Kerber, A. Nigmetov

    This file is part of GeomBottleneck.

    GeomBottleneck is free software: you can redistribute it and/or modify
    it under the terms of the Lesser GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    GeomBottleneck is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    Lesser GNU General Public License for more details.

    You should have received a copy of the Lesser GNU General Public License
    along with GeomBottleneck.  If not, see <http://www.gnu.org/licenses/>.

*/


#ifndef BASIC_DEFS_H
#define BASIC_DEFS_H

#include <vector>
#include <math.h>
#include <cstddef>
#include <unordered_map>
#include <unordered_set>
#include <iostream>
#include <string>
#include <assert.h>

#include "def_debug.h"

#define MIN_VALID_ID 10

namespace Auction
{

struct Point {
    double x, y;
    bool operator==(const Point& other) const;
    bool operator!=(const Point& other) const;
    Point(double ax, double ay) : x(ax), y(ay) {}
    Point() : x(0.0), y(0.0) {}
    friend std::ostream& operator<<(std::ostream& output, const Point p);
};



struct DiagramPoint 
{
    // Points above the diagonal have type NORMAL
    // Projections onto the diagonal have type DIAG
    // for DIAG points only x-coordinate is relevant
    // to-do: add getters/setters, checks in constructors, etc
    enum Type { NORMAL, DIAG};
    // data members
private:
    double x, y;
public:
    Type type;
    size_t id;
    // operators, constructors
    bool operator==(const DiagramPoint& other) const;
    bool operator!=(const DiagramPoint& other) const;
    DiagramPoint(double xx, double yy, Type ttype, size_t uid);
    bool isDiagonal(void) const { return type == DIAG; }
    bool isNormal(void) const { return type == NORMAL; }
    double inline getRealX() const // return the x-coord
    {
        return x;
        //if (DiagramPoint::NORMAL == type)
            //return x;
        //else 
            //return 0.5 * ( x + y);
    }    

    double inline getRealY() const // return the y-coord
    {
        return y;
        //if (DiagramPoint::NORMAL == type)
            //return y;
        //else 
            //return 0.5 * ( x + y);
    }

    friend std::ostream& operator<<(std::ostream& output, const DiagramPoint p);
};

struct PointHash {
    size_t operator()(const Point& p) const{
        return std::hash<double>()(p.x)^std::hash<double>()(p.y);
    }
};

struct DiagramPointHash {
    size_t operator()(const DiagramPoint& p) const{
        //return std::hash<double>()(p.x)^std::hash<double>()(p.y)^std::hash<bool>()(p.type == DiagramPoint::NORMAL);
        assert(p.id >= MIN_VALID_ID);
        return std::hash<int>()(p.id);
    }
};

double sqrDist(const Point& a, const Point& b);
double dist(const Point& a, const Point& b);
double distLInf(const DiagramPoint& a, const DiagramPoint& b);

typedef std::unordered_set<Point, PointHash> PointSet;

class DiagramPointSet {
public:
    void insert(const DiagramPoint p);
    void erase(const DiagramPoint& p, bool doCheck = true); // if doCheck, erasing non-existing elements causes assert
    void erase(const std::unordered_set<DiagramPoint, DiagramPointHash>::const_iterator it);
    size_t size() const;
    void reserve(const size_t newSize);
    void clear();
    bool empty() const;
    bool hasElement(const DiagramPoint& p) const;
    std::unordered_set<DiagramPoint, DiagramPointHash>::iterator find(const DiagramPoint& p) { return points.find(p); };
    std::unordered_set<DiagramPoint, DiagramPointHash>::const_iterator find(const DiagramPoint& p) const { return points.find(p); };
    std::unordered_set<DiagramPoint, DiagramPointHash>::iterator begin() { return points.begin(); };
    std::unordered_set<DiagramPoint, DiagramPointHash>::iterator end() { return points.end(); }
    std::unordered_set<DiagramPoint, DiagramPointHash>::const_iterator cbegin() const { return points.cbegin(); }
    std::unordered_set<DiagramPoint, DiagramPointHash>::const_iterator cend() const { return points.cend(); }
    friend std::ostream& operator<<(std::ostream& output, const DiagramPointSet& ps);
    void print();
private:
    std::unordered_set<DiagramPoint, DiagramPointHash> points;
};

template<typename DiagPointContainer>
double getFurthestDistance3Approx(DiagPointContainer& A, DiagPointContainer& B)
{
    double result { 0.0 };
    DiagramPoint begA = *(A.begin());
    DiagramPoint optB = *(B.begin());
    for(const auto& pointB : B) {
        if (distLInf(begA, pointB) > result) {
            result = distLInf(begA, pointB);
            optB = pointB;
        }
    }
    for(const auto& pointA : A) {
        if (distLInf(pointA, optB) > result) {
            result = distLInf(pointA, optB);
        }
    }
    return result;
}

}


#endif
