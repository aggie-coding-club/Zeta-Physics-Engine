// ? Define functions from our intersections header here.

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
        return !((point - start).cross(end - start).magSq());
    };

    bool PointAndPlane(ZMath::Vec3D const &point, Plane const &plane) {};

    bool PointAndSphere(ZMath::Vec3D const &point, Sphere const &sphere) {
        float r = sphere.getRaidus();
        return sphere.getCenter().distSq(point) <= r*r;
    };

    bool PointAndAABB(ZMath::Vec3D const &point, AABB const &aabb) {};

    bool PointAndCube(ZMath::Vec3D const &point, Cube const &cube) {};

    // * ====================================================================================================================

    // * ===================
    // * Line3D
    // * ===================

    bool LineAndPoint(Line3D const &line, ZMath::Vec3D const &point) { return PointAndLine(point, line); };

    bool LineAndLine(Line3D const &line1, Line3D const &line2) {};

    bool LineAndPlane(Line3D const &line, Plane const &plane) {};

    bool LineAndSphere(Line3D const &line, Sphere const &sphere) {
        // ? Use the parametric equations for a 3D line.
        // ? Relate the parametric equations with the distance squared to the center of the sphere.
        // ? Since we define our start point as the point at t_0 and end point as the point at t_1,
        // ?  we know that if either solution of the quadratic constructed is between 0 to 1 inclusive.

        float r = sphere.getRaidus();
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
        float sq = sqrt(B*B - 4*A*C);
        float t1 = (-B - sq)/2*A, t2 = (-B + sq)/2*A;

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

    bool SphereAndSphere(Sphere const &sphere1, Sphere const &sphere2) {};

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

    bool AABBAndAABB(AABB const &aabb1, AABB const &aabb2) {};

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