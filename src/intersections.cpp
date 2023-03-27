// ? Define functions from our intersections header here.

// todo unit tests
// todo probably need to add in comparison function calls to account for floating point errors
// todo go through each rotation and make sure it rotates XZ before XY when taking something into the local plane and XY before XZ when taking something out
// todo for each rotation do 360 - when converting it back

#include "intersections.h"
#include <cmath>
//#include <iostream> // ! for debugging

namespace Collisions {
    // * ===================
    // * Point
    // * ===================

    bool PointAndLine(ZMath::Vec3D const &point, Primitives::Line3D const &line) {
        // ? Determine the distance between the line and point using the projection of that point to a point on the line.
        // ? D = ||PQ x u||/||u||
        // ? If this distance is 0, we know it lies on the line.

        // obtain the max and min points for the bounds
        ZMath::Vec3D max(ZMath::max(line.start.x, line.end.x), ZMath::max(line.start.y, line.end.y), ZMath::max(line.start.z, line.end.z));
        ZMath::Vec3D min(ZMath::min(line.start.x, line.end.x), ZMath::min(line.start.y, line.end.y), ZMath::min(line.start.z, line.end.z));

        // ensure the point is within the start and end of the line segment
        if (point.x < min.x || point.x > max.x || point.y < min.y || point.y > max.y || point.z < min.z || point.z > max.z) { return 0; }

        // We don't need to divide by ||u|| to know if it will evaluate to 0.
        return ZMath::compare((point - line.start).cross((line.end - line.start)).magSq(), 0);
    };

    bool PointAndPlane(ZMath::Vec3D const &point, Primitives::Plane const &plane) {
        ZMath::Vec3D min = plane.getLocalMin(), max = plane.getLocalMax();
        ZMath::Vec3D p(point); // allows for rotation

        // must rotate it in the proper order
        ZMath::rotateXZ(p, plane.sb.pos, 360 - plane.sb.phi);
        ZMath::rotateXY(p, plane.sb.pos, 360 - plane.sb.theta);

        return p.x >= min.x && p.y >= min.y && p.x <= max.x && p.y <= max.y && ZMath::compare(p.z, min.z);
    };

    bool PointAndSphere(ZMath::Vec3D const &point, Primitives::Sphere const &sphere) { return sphere.rb.pos.distSq(point) <= sphere.r*sphere.r; };

    bool PointAndAABB(ZMath::Vec3D const &point, Primitives::AABB const &aabb) {
        ZMath::Vec3D min = aabb.getMin(), max = aabb.getMax();
        return point.x <= max.x && point.y <= max.y && point.z <= max.z && point.x >= min.x && point.y >= min.y && point.z >= min.z;
    };

    bool PointAndCube(ZMath::Vec3D const &point, Primitives::Cube const &cube) {
        ZMath::Vec3D min = cube.getLocalMin(), max = cube.getLocalMax();
        ZMath::Vec3D p = point; // create a copy so we can rotate into our local cords

        // rotate into our UVW cords
        ZMath::rotateXZ(p, cube.rb.pos, 360 - cube.rb.phi);
        ZMath::rotateXY(p, cube.rb.pos, 360 - cube.rb.theta);

        return p.x <= max.x && p.y <= max.y && p.z <= max.z && p.x >= min.x && p.y >= min.y && p.z >= min.z;
    };

    // * ====================================================================================================================

    // * ===================
    // * Line3D
    // * ===================

    bool LineAndPoint(Primitives::Line3D const &line, ZMath::Vec3D const &point) { return PointAndLine(point, line); };

    bool LineAndLine(Primitives::Line3D const &line1, Primitives::Line3D const &line2) {
        // ? First check if the lines are parallel.
        // ? If the lines are parallel, we check to ensure overlap and that the start point of line2 lies on line1 if the lines were infinite.
        // ? If the lines are not parallel, we then solve for the point of intersection using the parametric equations for a 3D line.
        // ? We then ensure this point in time statisfies all 3 equations.
        // ? If it does and there's overlap, we have an intersection; otherwise we do not.

        ZMath::Vec3D s1 = line1.start, s2 = line2.start, e1 = line1.end, e2 = line2.end;
        ZMath::Vec3D v1 = e1 - s1, v2 = e2 - s2;

        // check for parallel lines
        if (v1.x == v2.x && v1.y == v2.y && v1.z == v2.z) {
            ZMath::Vec3D min1(ZMath::min(s1.x, e1.x), ZMath::min(s1.y, e1.y), ZMath::min(s1.z, e1.z));
            ZMath::Vec3D max1(ZMath::max(s1.x, e1.x), ZMath::max(s1.y, e1.y), ZMath::max(s1.z, e1.z));
            ZMath::Vec3D min2(ZMath::min(s2.x, e2.x), ZMath::min(s2.y, e2.y), ZMath::min(s2.z, e2.z));
            ZMath::Vec3D max2(ZMath::max(s2.x, e2.x), ZMath::max(s2.y, e2.y), ZMath::max(s2.z, e2.z));

            if (v1.x) {
                // ! unsure if we actually need to check for every interval or just x. We can test once we get unit tests.
                // ! same thing for the second case, too.

                float t = (s2.x - s1.x)/v1.x;
                return s2.y == s1.y + v1.y * t && s2.z == s1.z + v1.z * t &&
                        min1.x <= max2.x && min2.x <= max1.x &&
                        min1.y <= max2.y && min2.y <= max1.y &&
                        min1.z <= max2.z && min2.z <= max1.z;
            }

            if (v1.y) {
                return s1.x == s2.x && s2.z == s1.z + v1.z * ((s2.y - s1.y)/v1.y) &&
                        min1.y <= max2.y && min2.y <= max1.y &&
                        min1.z <= max2.z && min2.z <= max1.z;
            }

            return s1.x == s2.x && s1.y == s2.y && min1.z <= max2.z && min2.z <= max1.z;
        }

        float s, t;

        // determine the s and t values for when the line would intersect
        if (v2.x*v1.y != v1.x*v2.y) { s = (v1.x*(s2.y - s1.y) + v1.y*(s1.x - s2.x))/(v2.x*v1.y - v1.x*v2.y); }
        else if (v2.y*v1.z != v1.y*v2.z) { s = (v1.y*(s2.z - s1.z) + v1.z*(s1.y - s2.y))/(v2.y*v1.z - v1.y*v2.z); }
        else { s = (v1.x*(s2.z - s1.z) + v1.z*(s1.x - s2.x))/(v2.x*v1.z - v1.x*v2.z); }

        if (v1.x) { t = (s2.x - s1.x + v2.x*s)/v1.x; }
        else if (v1.y) { t = (s2.y - s1.y + v2.y*s)/v1.y; }
        else { t = (s2.z - s1.z + v2.z*s)/v1.z; }

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

    bool LineAndPlane(Primitives::Line3D const &line, Primitives::Plane const &plane) {
        // ? We can use the same approach to solve this problem as for the raycasting.
        // ? We just ensure the point of intersection also lies within the bounds of the line.

        ZMath::Vec3D dir = (line.end - line.start).normalize();
        float dot = plane.normal * dir;

        // check if the line is parallel to the plane
        if (!dot) { return 0; }

        float t = -((plane.normal * (line.start - plane.sb.pos))/dot);
        ZMath::Vec3D min = plane.getLocalMin(), max = plane.getLocalMax();
        ZMath::Vec3D p = line.start + dir*t;

        ZMath::rotateXY(p, plane.sb.pos, plane.sb.theta);
        ZMath::rotateXZ(p, plane.sb.pos, plane.sb.phi);

        // Make sure the point of intersection is within our bounds.
        // We don't need to check if it's greater than or equal to the start point on the line segment as t >= 0 ensures that already.
        return t >= 0 && p.x >= min.x && p.y >= min.y && p.z >= min.z && p.x <= max.x && p.y <= max.y && p.z <= max.z &&
                p.x <= line.end.x && p.y <= line.end.y && p.z <= line.end.z;
    };

    bool LineAndSphere(Primitives::Line3D const &line, Primitives::Sphere const &sphere) {
        // ? Use the parametric equations for a 3D line.
        // ? Relate the parametric equations with the distance squared to the center of the sphere.
        // ? Since we define our start point as the point at t_0 and end point as the point at t_1,
        // ?  we know that if either solution of the quadratic constructed is between 0 to 1 inclusive, we have a collision.

        float dx = line.end.x - line.start.x, dy = line.end.y - line.start.y, dz = line.end.z - line.start.z;
        float xc = line.start.x - sphere.rb.pos.x, yc = line.start.y - sphere.rb.pos.y, zc = line.start.z - sphere.rb.pos.z;

        float A = dx*dx + dy*dy + dz*dz;
        float B = 2*(dx*xc + dy*yc + dz*zc);
        float C = xc*xc + yc*yc + zc*zc - sphere.r*sphere.r;

        float D = B*B - 4*A*C;

        if (D < 0) { return 0; } // no intersections even if the line was infinite

        float sq = sqrtf(D);
        float div = 1.0f/(2.0f*A);
        float t1 = (-B - sq)*div, t2 = (-B + sq)*div;

        // if t1 or t2 falls between 0 and 1, the line intersects the sphere
        return (0.0f <= t1 && t1 <= 1.0f) || (0.0f <= t2 && t2 <= 1.0f);
    };

    bool LineAndAABB(Primitives::Line3D const &line, Primitives::AABB const &aabb) {
        // ? We can use the same logic as when raycasting an AABB.
        // ? Then we just have to make sure the distance to the AABB is less than the length of the line segment.

        ZMath::Vec3D dir = (line.end - line.start).normalize();
        ZMath::Vec3D dirfrac(1.0f/dir.x, 1.0f/dir.y, 1.0f/dir.z);
        ZMath::Vec3D min = aabb.getMin(), max = aabb.getMax();
        
        float t1 = (min.x - line.start.x)*dirfrac.x;
        float t2 = (max.x - line.start.x)*dirfrac.x;
        float t3 = (min.y - line.start.y)*dirfrac.y;
        float t4 = (max.y - line.start.y)*dirfrac.y;
        float t5 = (min.z - line.start.z)*dirfrac.z;
        float t6 = (max.z - line.start.z)*dirfrac.z;

        // tMin is the max of the mins and tMx is the min of the maxes
        float tMin = ZMath::max(ZMath::max(ZMath::min(t1, t2), ZMath::min(t3, t4)), ZMath::min(t5, t6));
        float tMax = ZMath::min(ZMath::min(ZMath::max(t1, t2), ZMath::max(t3, t4)), ZMath::max(t5, t6));

        float lengthSq = (line.end - line.start).magSq();

        // if tMax < 0 the ray is intersecting behind it. Therefore, we do not actually have a collision.
        if (tMax < 0) {
            // ! unsure if this part will cause errors. Will figure out after unit tests are developed.
            // ! If this part causes errors we will have to manually check for end point in the AABB.
            return tMax*tMax <= lengthSq;
        }

        // ray doesn't intersect the AABB.
        if (tMax < tMin) { return 0; }

        return tMin*tMin <= lengthSq;
    };

    bool LineAndCube(Primitives::Line3D const &line, Primitives::Cube const &cube) {
        // ? This will be the same as the AABB vs Line check after rotating the line into the cube's UVW coordinates.

        Primitives::Line3D l(line.start, line.end);

        ZMath::rotateXZ(l.start, cube.rb.pos, cube.rb.phi);
        ZMath::rotateXY(l.start, cube.rb.pos, cube.rb.theta);
        ZMath::rotateXZ(l.end, cube.rb.pos, cube.rb.phi);
        ZMath::rotateXY(l.end, cube.rb.pos, cube.rb.theta);

        return LineAndAABB(l, Primitives::AABB(cube.getLocalMin(), cube.getLocalMax()));
    };

    // * ====================================================================================================================

    // * =================
    // * Raycasting
    // * =================

    bool raycast(Primitives::Plane const &plane, Primitives::Ray3D const &ray, float &dist) {
        float dot = plane.normal * ray.dir;

        // check if the ray is parallel to the plane
        if (!dot) { dist = -1.0f; return 0; }

        dist = -((plane.normal * (ray.origin - plane.sb.pos))/dot);
        ZMath::Vec3D min = plane.getLocalMin(), max = plane.getLocalMax();
        ZMath::Vec3D p = ray.origin + ray.dir*dist;

        ZMath::rotateXY(p, plane.sb.pos, plane.sb.theta);
        ZMath::rotateXZ(p, plane.sb.pos, plane.sb.phi);

        // make sure the point of intersection is within our bounds and the intersection wouldn't occur behind the ray
        if (dist >= 0 && p.x >= min.x && p.y >= min.y && p.z >= min.z && p.x <= max.x && p.y <= max.y && p.z <= max.z) { return 1; }

        dist = -1.0f;
        return 0;
    };
    
    bool raycast(Primitives::Sphere const &sphere, Primitives::Ray3D const &ray, float &dist) {
        // todo at some point we may want to get the hit point.
        // todo if we do, we simply do hit = ray.origin + dist*ray.dir;

        // ? First we find the closest point on the ray to the sphere.
        // ? To find this point, we find the distance to it using dir * (center - origin).
        // ? Next we solve origin + t*origin to find the closest point.
        // ? If the distance of that closest point to the center is less than or equal to the radius, we have an intersection.

        float rSq = sphere.r*sphere.r;

        // determine the closest point and the distance to that point
        float t = ray.dir * (sphere.rb.pos - ray.origin);
        ZMath::Vec3D close = ray.origin + ray.dir * t;

        float dSq = sphere.rb.pos.distSq(close);

        // no intersection
        if (dSq > rSq) {
            dist = -1.0f;
            return 0;
        }

        // lands on the circumference
        if (dSq == rSq) {
            dist = sphere.r;
            return 1;
        }

        // standard intersection
        dist = t - sqrtf(rSq - dSq);
        return 1;
    };

    bool raycast(Primitives::AABB const &aabb, Primitives::Ray3D const &ray, float &dist) {
        // ? We can determine the distance from the ray to a certain edge by dividing a select min or max vector component
        // ?  by the corresponding component from the unit directional vector.
        // ? We know if tMin > tMax, then we have no intersection and if tMax is negative the AABB is behind us and we do not have a hit.

        ZMath::Vec3D dirfrac(1.0f/ray.dir.x, 1.0f/ray.dir.y, 1.0f/ray.dir.z);
        ZMath::Vec3D min = aabb.getMin(), max = aabb.getMax();

        float t1 = (min.x - ray.origin.x)*dirfrac.x;
        float t2 = (max.x - ray.origin.x)*dirfrac.x;
        float t3 = (min.y - ray.origin.y)*dirfrac.y;
        float t4 = (max.y - ray.origin.y)*dirfrac.y;
        float t5 = (min.z - ray.origin.z)*dirfrac.z;
        float t6 = (max.z - ray.origin.z)*dirfrac.z;

        // tMin is the max of the mins and tMx is the min of the maxes
        float tMin = ZMath::max(ZMath::max(ZMath::min(t1, t2), ZMath::min(t3, t4)), ZMath::min(t5, t6));
        float tMax = ZMath::min(ZMath::min(ZMath::max(t1, t2), ZMath::max(t3, t4)), ZMath::max(t5, t6));

        // if tMax < 0 the ray is intersecting behind it. Therefore, we do not actually have a collision.
        if (tMax < 0) {
            dist = -1.0f;
            return 0;
        }

        // ray doesn't intersect the AABB.
        if (tMax < tMin) {
            dist = -1.0f;
            return 0;
        }

        dist = tMin;
        return 1;
    };

    bool raycast(Primitives::Cube const &cube, Primitives::Ray3D const &ray, float &dist) {};

    // * ====================================================================================================================

    // * =================
    // * Plane
    // * =================

    bool PlaneAndPoint(Primitives::Plane const &plane, ZMath::Vec3D const &point) { return PointAndPlane(point, plane); };

    bool PlaneAndLine(Primitives::Plane const &plane, Primitives::Line3D const &line) { return LineAndPlane(line, plane); };

    bool PlaneAndPlane(Primitives::Plane const &plane1, Primitives::Plane const &plane2) {};

    bool PlaneAndSphere(Primitives::Plane const &plane, Primitives::Sphere const &sphere) {
        // ? A line from the center of the sphere along the normal of the plane will eventually
        // ?  intersect an infinite plane at the closest point of intersection.
        // ? Using this, we can take the distance to the plane from that point using the equation
        // ?  for the distance from a point to a plane.
        // ? If this distance squared is less than the radius squared, we have an intersection.
        // ? Since we are not dealing with infinite planes, we will first need to clamp the 
        // ?  sphere's center between the min and max vertices of the plane.

        ZMath::Vec3D closest(sphere.rb.pos);
        ZMath::Vec3D min = plane.getLocalMin(), max = plane.getLocalMax();

        ZMath::rotateXZ(closest, plane.sb.pos, 360 - plane.sb.phi);
        ZMath::rotateXY(closest, plane.sb.pos, 360 - plane.sb.theta);

        closest.x = ZMath::clamp(closest.x, min.x, max.x);
        closest.y = ZMath::clamp(closest.y, min.y, max.y);
        closest.z = ZMath::clamp(closest.z, min.z, max.z);

        return closest.distSq(sphere.rb.pos) <= sphere.r*sphere.r;
    };

    bool PlaneAndAABB(Primitives::Plane const &plane, Primitives::AABB const &aabb) {
        float r = abs(plane.normal.x * aabb.rb.pos.x) + abs(plane.normal.y * aabb.rb.pos.y) + abs(plane.normal.z * aabb.rb.pos.z);
        return PlaneAndSphere(plane, Primitives::Sphere(r, aabb.rb.pos));
    };

    bool PlaneAndCube(Primitives::Plane const &plane, Primitives::Cube const &cube) {
        Primitives::AABB aabb(cube.getLocalMin(), cube.getLocalMax());

        ZMath::Vec3D min = plane.getLocalMin(), max = plane.getLocalMax();
        Primitives::Plane p(ZMath::Vec2D(min.x, min.y), ZMath::Vec2D(max.x, max.y), plane.sb.pos.z, plane.sb.theta - cube.rb.theta, plane.sb.phi - cube.rb.phi);

        return PlaneAndAABB(p, aabb);
    };

    // * ====================================================================================================================

    // * =================
    // * Sphere
    // * =================

    bool SphereAndPoint(Primitives::Sphere const &sphere, ZMath::Vec3D const &point) { return PointAndSphere(point, sphere); };

    bool SphereAndLine(Primitives::Sphere const &sphere, Primitives::Line3D const &line) { return LineAndSphere(line, sphere); };

    bool SphereAndPlane(Primitives::Sphere const &sphere, Primitives::Plane const &plane) { return PlaneAndSphere(plane, sphere); };

    /*bool SphereAndSphere(Primitives::Sphere const &sphere1, Primitives::Sphere const &sphere2) {
        float r = sphere1.r + sphere2.r;
        return sphere1.rb.pos.distSq(sphere2.rb.pos) <= r*r;
    };

    bool SphereAndAABB(Primitives::Sphere const &sphere, Primitives::AABB const &aabb) {
        // ? We know a sphere and AABB would intersect if the distance from the closest point to the center on the AABB
        // ?  from the center is less than or equal to the radius of the sphere.
        // ? We can determine the closet point by clamping the value of the sphere's center between the min and max of the AABB.
        // ? From here, we can check the distance from this point to the sphere's center.

        ZMath::Vec3D closest(sphere.rb.pos);
        ZMath::Vec3D min = aabb.getMin(), max = aabb.getMax();

        closest.x = ZMath::clamp(closest.x, min.x, max.x);
        closest.y = ZMath::clamp(closest.y, min.y, max.y);
        closest.z = ZMath::clamp(closest.z, min.z, max.z);

        return closest.distSq(sphere.rb.pos) <= sphere.r*sphere.r;
    };

    bool SphereAndCube(Primitives::Sphere const &sphere, Primitives::Cube const &cube) {
        // ? We can use the same approach as for SphereAndAABB, just we have to rotate the sphere into the Cube's UVW coordinates.

        ZMath::Vec3D center = sphere.rb.pos;
        ZMath::Vec3D min = cube.getLocalMin(), max = cube.getLocalMax();

        // rotate the center of the sphere into the UVW coordinates of our cube
        ZMath::rotateXY(center, cube.rb.pos, cube.rb.theta);
        ZMath::rotateXZ(center, cube.rb.pos, cube.rb.phi);
        
        // perform the check as if it was an AABB vs Sphere
        ZMath::Vec3D closest(center);

        closest.x = ZMath::clamp(closest.x, min.x, max.x);
        closest.y = ZMath::clamp(closest.y, min.y, max.y);
        closest.z = ZMath::clamp(closest.z, min.z, max.z);

        return closest.distSq(center) <= sphere.r*sphere.r;
    };*/

    // * ====================================================================================================================

    // * =============
    // * AABB
    // * =============
    
    bool AABBAndPoint(Primitives::AABB const &aabb, ZMath::Vec3D const &point) { return PointAndAABB(point, aabb); };

    bool AABBAndLine(Primitives::AABB const &aabb, Primitives::Line3D const &line) { return LineAndAABB(line, aabb); };

    bool AABBAndPlane(Primitives::AABB const &aabb, Primitives::Plane const &plane) { return PlaneAndAABB(plane, aabb); };

    /*bool AABBAndSphere(Primitives::AABB const &aabb, Primitives::Sphere const &sphere) { return SphereAndAABB(sphere, aabb); };

    bool AABBAndAABB(Primitives::AABB const &aabb1, Primitives::AABB const &aabb2) {
        // ? Check if there's overlap for the AABBs on all three axes.
        // ? If there is, we know the two AABBs intersect.

        ZMath::Vec3D min1 = aabb1.getMin(), max1 = aabb1.getMax(), min2 = aabb2.getMin(), max2 = aabb2.getMax();

        // if both min points for a given interval are less than the other max points, we know there's an overlap on that axis
        return min2.x <= max1.x && min1.x <= max2.x && min2.y <= max1.y && min1.y <= max2.y && min2.z <= max1.z && min1.z <= max2.z;
    };

    bool AABBAndCube(Primitives::AABB const &aabb, Primitives::Cube const &cube) {
        // ? Rotate the AABB into the cube's UVW coordinates.
        // ? Afterwards, use the same logic as AABB vs AABB.

        ZMath::Vec3D min1 = aabb.getMin(), max1 = aabb.getMax(), min2 = cube.getLocalMin(), max2 = cube.getLocalMax();

        ZMath::rotateXY(min1, cube.rb.pos, cube.rb.theta);
        ZMath::rotateXZ(max1, cube.rb.pos, cube.rb.phi);

        return min2.x <= max1.x && min1.x <= max2.x && min2.y <= max1.y && min1.y <= max2.y && min2.z <= max1.z && min1.z <= max2.z;
    };*/

    // * ====================================================================================================================

    // * ===============
    // * Cube
    // * ===============

    bool CubeAndPoint(Primitives::Cube const &cube, ZMath::Vec3D const &point) { return PointAndCube(point, cube); };

    bool CubeAndLine(Primitives::Cube const &cube, Primitives::Line3D const &line) { return LineAndCube(line, cube); };

    bool CubeAndPlane(Primitives::Cube const &cube, Primitives::Plane const &plane) { return PlaneAndCube(plane, cube); };

    /*bool CubeAndSphere(Primitives::Cube const &cube, Primitives::Sphere const &sphere) { return SphereAndCube(sphere, cube); };

    bool CubeAndAABB(Primitives::Cube const &cube, Primitives::AABB const &aabb) { return AABBAndCube(aabb, cube); };

    bool CubeAndCube(Primitives::Cube const &cube1, Primitives::Cube const &cube2) {
        // ? We can make an AABB object from the first cube and create a new cube object by subtracting the angles
        // ?  the first cube was rotated by from the second cube and perform an AABB vs Cube check.

        Primitives::AABB aabb(cube1.getLocalMin(), cube2.getLocalMax());
        Primitives::Cube cube(cube2.getLocalMin(), cube2.getLocalMax(), cube2.rb.theta - cube1.rb.theta, cube2.rb.phi - cube1.rb.phi);

        return AABBAndCube(aabb, cube);
    };*/

    // * ====================================================================================================================
}