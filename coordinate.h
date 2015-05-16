/*
 *  This file is a part of KNOSSOS.
 *
 *  (C) Copyright 2007-2013
 *  Max-Planck-Gesellschaft zur Foerderung der Wissenschaften e.V.
 *
 *  KNOSSOS is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 of
 *  the License as published by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * For further information, visit http://www.knossostool.org or contact
 *     Joergen.Kornfeld@mpimf-heidelberg.mpg.de or
 *     Fabian.Svara@mpimf-heidelberg.mpg.de
 */

#ifndef COORDINATE_H
#define COORDINATE_H

#include <QList>
#include <QMetaType>

#include <cstddef>

#include <boost/functional/hash.hpp>

template<typename ComponentType = int, std::size_t = 0>//tag
class Coord {
public:
    using Coordinate = Coord<int>;
    using CoordOfCube = Coord<int, 1>;
    using CoordInCube = Coord<int, 2>;
    using floatCoordinate = Coord<float>;
    ComponentType x;
    ComponentType y;
    ComponentType z;

    constexpr Coord(ComponentType every = 0) : x(every), y(every), z(every) {}
    constexpr Coord(QList<ComponentType> l) : x(l[0]), y(l[1]), z(l[2]) {}
    constexpr Coord(QVector<ComponentType> l) : x(l[0]), y(l[1]), z(l[2]) {}
    constexpr Coord(ComponentType x, ComponentType y, ComponentType z) : x(x), y(y), z(z) {}
    template<typename T, typename = typename std::enable_if<std::is_convertible<ComponentType, T>::value>::type>
    constexpr Coord(const Coord<T> & rhs) : x(rhs.x), y(rhs.y), z(rhs.z) {}
    constexpr ComponentType sum() const {
        return x+y+z;
    }
    constexpr QList<ComponentType> list() const {
        return {x, y, z};
    }
    constexpr QVector<ComponentType> vector() const {
        return {x, y, z};
    }
    constexpr bool operator==(const Coord & rhs) const {
        return x == rhs.x && y == rhs.y && z == rhs.z;
    }
    constexpr bool operator!=(const Coord & rhs) const {
        return !(*this == rhs);
    }

    constexpr Coord operator+(const Coord & rhs) const {
        return Coord(x + rhs.x, y + rhs.y, z  + rhs.z);
    }
    Coord & operator+=(const Coord & rhs) {
        return *this = *this + rhs;
    }

    constexpr Coord operator-(const Coord & rhs) const {
        return Coord(x - rhs.x, y - rhs.y, z  - rhs.z);
    }
    Coord & operator-=(const Coord & rhs) {
        return *this = *this - rhs;
    }

    constexpr Coord operator*(const Coord & rhs) const {
        return Coord(x * rhs.x, y * rhs.y, z  * rhs.z);
    }
    Coord & operator*=(const Coord & rhs) {
        return *this = *this * rhs;
    }

    constexpr Coord operator/(const Coord & rhs) const {
        return Coord(x / rhs.x, y / rhs.y, z  / rhs.z);
    }
    Coord & operator/=(const Coord & rhs) {
        return *this = *this / rhs;
    }

    constexpr CoordOfCube cube(const int size, const int mag) const {
        return {x / size / mag, y / size / mag, z / size / mag};
    }

    constexpr CoordInCube insideCube(const int size) const {
        return {x % size, y % size, z % size};
    }

    /**
     * This function calculates the coordinates of the datacube from pixel coordinates
     */
    static Coordinate Px2DcCoord(Coordinate pxCoordinate, int cubeEdgeLength) {
        const auto cubeCoord = pxCoordinate.cube(cubeEdgeLength, 1);
        return {cubeCoord.x, cubeCoord.y, cubeCoord.z};
    }

    constexpr Coordinate global2Legacy(int cubeEdgeLength, const int magnification) const {
        return cube(cubeEdgeLength, magnification);
    }

    constexpr Coordinate legacy2Global(const int cubeEdgeLength, const int magnification) const {
        return {x * cubeEdgeLength * magnification, y * cubeEdgeLength * magnification, z * cubeEdgeLength * magnification};
    }

    constexpr Coordinate cube2Legacy() const {
        return {x, y, z};
    }

    constexpr Coordinate capped(const Coordinate & min, const Coordinate & max) const {
        return {
            std::max(min.x, std::min(x, max.x))
            , std::max(min.y, std::min(y, max.y))
            , std::max(min.z, std::min(z, max.z))
        };
    }
};
using Coordinate = Coord<>::Coordinate;
using CoordOfCube = Coord<>::CoordOfCube;
using CoordInCube = Coord<>::CoordInCube;
using floatCoordinate = Coord<>::floatCoordinate;

Q_DECLARE_METATYPE(Coordinate)
Q_DECLARE_METATYPE(CoordOfCube)
Q_DECLARE_METATYPE(floatCoordinate)

namespace std {
template<typename T, std::size_t x>
struct hash<Coord<T, x>> {
    std::size_t operator()(const Coord<T, x> & cord) const {
        return boost::hash_value(std::make_tuple(cord.x, cord.y, cord.z));
    }
};
}

#endif
