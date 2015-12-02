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

#include <algorithm>
#include <cfloat>
#include "basic_defs.h"

// Point

bool Point::operator==(const Point& other) const
{
    return ((this->x == other.x) and (this->y == other.y));
}

bool Point::operator!=(const Point& other) const
{
    return !(*this == other);
}

std::ostream& operator<<(std::ostream& output, const Point p)
{
    output << "(" << p.x << ", " << p.y << ")";
    return output;
}

std::ostream& operator<<(std::ostream& output, const PointSet& ps)
{
    output << "{ ";
    for(auto& p : ps) {
        output << p << ", ";
    }
    output << "\b\b }";
    return output;
}

double sqrDist(const Point& a, const Point& b)
{
    return (a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y);
}

double dist(const Point& a, const Point& b)
{
    return sqrt(sqrDist(a, b));
}

// DiagramPoint

// compute l-inf distance between two diagram points
double distLInf(const DiagramPoint& a, const DiagramPoint& b)
{
    if ( DiagramPoint::DIAG == a.type &&
         DiagramPoint::DIAG == b.type ) {
        // distance between points on the diagonal is 0
        return 0.0;
    } 
    // otherwise distance is a usual l-inf distance
    return std::max(fabs(a.getRealX() - b.getRealX()), fabs(a.getRealY() - b.getRealY()));
}

bool DiagramPoint::operator==(const DiagramPoint& other) const
{
    assert(this->id >= MIN_VALID_ID);
    assert(other.id >= MIN_VALID_ID);
    bool areEqual{ this->id == other.id };
    assert(!areEqual or  ((this->x == other.x) and (this->y == other.y) and (this->type == other.type)));
    return areEqual;
}

bool DiagramPoint::operator!=(const DiagramPoint& other) const
{
    return !(*this == other);
}

std::ostream& operator<<(std::ostream& output, const DiagramPoint p)
{
    if ( p.type == DiagramPoint::DIAG ) {
        output << "(" << p.x << ", " << p.y << ", " <<  0.5 * (p.x + p.y) << ", "  << p.id << " DIAG )";
    } else {
        output << "(" << p.x << ", " << p.y << ", " << p.id << " NORMAL)";
    }
    return output;
}

std::ostream& operator<<(std::ostream& output, const DiagramPointSet& ps)
{
    output << "{ ";
    for(auto pit = ps.cbegin(); pit != ps.cend(); ++pit) {
        output << *pit << ", ";
    }
    output << "\b\b }";
    return output;
}

DiagramPoint::DiagramPoint(double xx, double yy, Type ttype, size_t uid) : 
    x(xx),
    y(yy),
    type(ttype),
    id(uid)
{
    //if ( xx < 0 )
        //throw "Negative x coordinate";
    //if ( yy < 0)
        //throw "Negative y coordinate";
    //if ( yy < xx )
        //throw "Point is below the diagonal";
    if ( yy == xx and ttype != DiagramPoint::DIAG)
        throw "Point on the main diagonal must have DIAG type";

}

void DiagramPointSet::insert(const DiagramPoint p)
{
    points.insert(p);
}

// erase should be called only for the element of the set
void DiagramPointSet::erase(const DiagramPoint& p, bool doCheck)
{
    auto it = points.find(p);
    if (it != points.end()) {
        points.erase(it);
    } else {
        assert(!doCheck);
    }
}

void DiagramPointSet::reserve(const size_t newSize)
{
    points.reserve(newSize);
}


void DiagramPointSet::erase(const std::unordered_set<DiagramPoint, DiagramPointHash>::const_iterator it)
{
    points.erase(it);
}

void DiagramPointSet::clear()
{
    points.clear();
}

size_t DiagramPointSet::size() const
{
    return points.size();
}

bool DiagramPointSet::empty() const
{
    return points.empty();
}

bool DiagramPointSet::hasElement(const DiagramPoint& p) const
{
    return points.find(p) != points.end();
}
