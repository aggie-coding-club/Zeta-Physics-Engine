// ? Define functions from our intersections header here.

// todo unit tests
// todo raycasting

#include "intersections.h"
#include <cmath>

namespace Primitives {
    // * ===================
    // * Point
    // * ===================

    bool PointAndLine(ZMath::Vec3D const &point, Line3D const &line) {
        // ? Determine the distance between the line and point using the projection of that point to a point on the line.
        // ? D = ||PQ x u||/||u||
        // ? If this distance is 0, we know it lies on the line.

        ZMath::Vec3D start = line.getStart(), end = line.getEnd();

        // obtain the max and min points for the bounds
        ZMath::Vec3D max(ZMath::max(start.x, end.x), ZMath::max(start.y, end.y), ZMath::max(start.z, end.z));
        ZMath::Vec3D min(ZMath::min(start.x, end.x), ZMath::min(start.y, end.y), ZMath::min(start.z, end.z));

        // ensure the point is within the start and end of the line segment
        if (point.x < min.x || point.x > max.x || point.y < min.y || point.y > max.y || point.z < min.z || point.z > max.z) { return 0; }

        // We don't need to divide by ||u|| to know if it will evaluate to 0.
        // ! normalize requires a slow sqrt -- optimize away if possible. Will probably just implement fast inv sqrt
        return !((point - start).cross((end - start).normalize()).magSq());
    };

    bool PointAndPlane(ZMath::Vec3D const &point, Plane const &plane) {
        ZMath::Vec3D p = plane.getMin(), r = plane.getMax();
        ZMath::Vec3D pq = plane.getCenter() - p;
        ZMath::Vec3D pr = r - p;
        ZMath::Vec3D n = pq.cross(pr);
        
        if (n.x*(point.x - p.x) + n.y*(point.y - p.y) + n.z*(point.z - p.z) != 0) { return 0; }

        return point.x <= r.x && point.y <= r.y && point.z <= r.z && point.x >= p.x && point.y >= p.y && point.z >= p.z;
    };

    bool PointAndSphere(ZMath::Vec3D const &point, Sphere const &sphere) {
        float r = sphere.getRadius();
        return sphere.getCenter().distSq(point) <= r*r;
    };

    bool PointAndAABB(ZMath::Vec3D const &point, AABB const &aabb) {
        ZMath::Vec3D min = aabb.getMin(), max = aabb.getMax();
        return point.x <= max.x && point.y <= max.y && point.z <= max.z && point.x >= min.x && point.y >= min.y && point.z >= min.z;
    };

    bool PointAndCube(ZMath::Vec3D const &point, Cube const &cube) {
        ZMath::Vec3D c = cube.getPos(), min = cube.getLocalMin(), max = cube.getLocalMax();
        ZMath::Vec3D p = point; // create a copy so we can rotate into our local cords

        // rotate into our UVW cords
        ZMath::rotateXY(p, c, cube.getTheta());
        ZMath::rotateXZ(p, c, cube.getPhi());

        return p.x <= max.x && p.y <= max.y && p.z <= max.z && p.x >= min.x && p.y >= min.y && p.z >= min.z;
    };

    // * ====================================================================================================================

    // * ===================
    // * Line3D
    // * ===================

    bool LineAndPoint(Line3D const &line, ZMath::Vec3D const &point) { return PointAndLine(point, line); };

    bool LineAndLine(Line3D const &line1, Line3D const &line2) {
        // ! unfinished

        // ? Use the parametric equations for a 3D line.
        // ? Solve for the point of intersection.
        // ? If the point of intersection exists and lies within the bounds of the two lines, then we have an intersection.

        // * ALTERNATIVE SOLUTION IDEA
        // ? For two lines to intersect, they must lie in the same plane.
        // ? The plane they must both lie in would be orthogonal to the cross of the directional vectors of both lines.
        // ? The displacement vector between a point on either line must then be orthogonal to the cross of the directional vectors.
        // ? In other words, $(dir1 x dir2) \cdot (p1 - p2)$.
        // ? Only thing to do to solve it using this solution would be to find a way to ensure the point of intersection lies within
        // ?  the segments of the Line3Ds.

        ZMath::Vec3D s1 = line1.getStart(), s2 = line2.getStart(), e1 = line1.getEnd(), e2 = line2.getEnd();
        ZMath::Vec3D v1 = e1 - s1, v2 = e2 - s2;

        // todo check for parallel lines

        // todo add checks for divisions by 0

        // determine the s and t values for when the line would intersect
        float s = (s1.y - s2.y + (v1.y * (s2.x - s1.x)/v1.x))/(v2.y - (v2.x * v1.y)/v1.x);
        float t = (s2.x + s*v2.x - s1.x)/v1.x;

        // ensure it satisfies the third equation
        if (s1.z + t*v1.z != s2.z + s*v2.z) { return 0; }

        // determine the point of intersection
        ZMath::Vec3D p = ZMath::Vec3D(s1.x + t*v1.x, s1.y + t*v1.y, s1.z + t*v1.z);

        // determine the min and max points for both lines
        ZMath::Vec3D max = ZMath::Vec3D(ZMath::min(ZMath::max(s1.x, e1.x), ZMath::max(s2.x, e2.x)), 
                                        ZMath::min(ZMath::max(s1.y, e1.y), ZMath::max(s2.y, e2.y)),
                                        ZMath::min(ZMath::max(s1.z, e1.z), ZMath::max(s2.z, e2.z)));

        ZMath::Vec3D min = ZMath::Vec3D(ZMath::max(ZMath::min(s1.x, e1.x), ZMath::min(s2.x, e2.x)),
                                        ZMath::max(ZMath::min(s1.y, e1.y), ZMath::min(s2.y, e2.y)),
                                        ZMath::max(ZMath::min(s1.z, e1.z), ZMath::min(s2.z, e2.z)));

        // ensure the point of intersection is between these bounds
        return p.x <= max.x && p.y <= max.y && p.z <= max.z && p.x >= min.x && p.y >= min.y && p.z >= min.z;
    };

    bool LineAndPlane(Line3D const &line, Plane const &plane) {};

    bool LineAndSphere(Line3D const &line, Sphere const &sphere) {
        // ? Use the parametric equations for a 3D line.
        // ? Relate the parametric equations with the distance squared to the center of the sphere.
        // ? Since we define our start point as the point at t_0 and end point as the point at t_1,
        // ?  we know that if either solution of the quadratic constructed is between 0 to 1 inclusive, we have a collision.

        float r = sphere.getRadius();
        ZMath::Vec3D center = sphere.getCenter();
        ZMath::Vec3D start = line.getStart(), end = line.getEnd();

        float dx = end.x - start.x, dy = end.y - start.y, dz = end.z - start.z;
        float xc = start.x - center.x, yc = start.y - center.y, zc = start.z - center.z;

        float A = dx*dx + dy*dy + dz*dz;
        float B = 2*dx*xc + 2*dy*yc + 2*dz*zc;
        float C = xc*xc + yc*yc + zc*zc - r*r;

        float D = B*B - 4*A*C;

        if (D < 0) { return 0; } // no intersections even if the line was infinite

        // ! slow sqrt, optimize away if possible
        float sq = sqrt(D);
        float t1 = (-B - sq)/(2*A), t2 = (-B + sq)/(2*A);

        // if t1 or t2 falls between 0 and 1, the line intersects the spheres
        return (0.0f <= t1 && t1 <= 1.0f) || (0.0f <= t2 && t2 <= 1.0f);
    };

    bool LineAndAABB(Line3D const &line, AABB const &aabb) {};

    bool LineAndCube(Line3D const &line, Cube const &cube) {};

    // * ====================================================================================================================

    // * =================
    // * Plane
    // * =================

    bool PlaneAndPoint(Plane const &plane, ZMath::Vec3D const &point) { return PointAndPlane(point, plane); };

    bool PlaneAndLine(Plane const &plane, Line3D const &line) { return LineAndPlane(line, plane); };

    bool PlaneAndPlane(Plane const &plane1, Plane const &plane2) {};

    bool PlaneAndSphere(Plane const &plane, Sphere const &sphere) {};

    bool PlaneAndAABB(Plane const &plane, AABB const &aabb) {};

    bool PlaneAndCube(Plane const &plane, Cube const &cube) {};

    // * ====================================================================================================================

    // * =================
    // * Sphere
    // * =================

    bool SphereAndPoint(Sphere const &sphere, ZMath::Vec3D const &point) { return PointAndSphere(point, sphere); };

    bool SphereAndLine(Sphere const &sphere, Line3D const &line) { return LineAndSphere(line, sphere); };

    bool SphereAndPlane(Sphere const &sphere, Plane const &plane) { return PlaneAndSphere(plane, sphere); };

    bool SphereAndSphere(Sphere const &sphere1, Sphere const &sphere2) {
        float r = sphere1.getRadius() + sphere2.getRadius();
        return sphere1.getCenter().distSq(sphere2.getCenter()) <= r * r;
    };

    bool SphereAndAABB(Sphere const &sphere, AABB const &aabb) {};

    bool SphereAndCube(Sphere const &sphere, Cube const &cube) {};

    // * ====================================================================================================================

    // * =============
    // * AABB
    // * =============
    
    bool AABBAndPoint(AABB const &aabb, ZMath::Vec3D const &point) { return PointAndAABB(point, aabb); };

    bool AABBAndLine(AABB const &aabb, Line3D const &line) { return LineAndAABB(line, aabb); };

    bool AABBAndPlane(AABB const &aabb, Plane const &plane) { return PlaneAndAABB(plane, aabb); };

    bool AABBAndSphere(AABB const &aabb, Sphere const &sphere) { return SphereAndAABB(sphere, aabb); };

    bool AABBAndAABB(AABB const &aabb1, AABB const &aabb2) {
        // ? Check if there's overlap for the AABBs on all three axes.
        // ? If there is, we know the two AABBs intersect.

        ZMath::Vec3D min1 = aabb1.getMin(), max1 = aabb1.getMax(), min2 = aabb2.getMin(), max2 = aabb2.getMax();

        // if both min points for a given interval are less than the other max points, we know there's an overlap on that axis
        return min2.x <= max1.x && min1.x <= max2.x && min2.y <= max1.y && min1.y <= max2.y && min2.z <= max1.z && min1.z <= max2.z;
    };

    bool AABBAndCube(AABB const &aabb, Cube const &cube) {};

    // * ====================================================================================================================

    // * ===============
    // * Cube
    // * ===============

    bool CubeAndPoint(Cube const &cube, ZMath::Vec3D const &point) { return PointAndCube(point, cube); };

    bool CubeAndLine(Cube const &cube, Line3D const &line) { return LineAndCube(line, cube); };

    bool CubeAndPlane(Cube const &cube, Plane const &plane) { return PlaneAndCube(plane, cube); };

    bool CubeAndSphere(Cube const &cube, Sphere const &sphere) { return SphereAndCube(sphere, cube); };

    bool CubeAndAABB(Cube const &cube, AABB const &aabb) { return AABBAndCube(aabb, cube); };

    bool CubeAndCube(Cube const &cube1, Cube const &cube2) {};

    // * ====================================================================================================================
}