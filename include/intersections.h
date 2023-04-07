#ifndef INTERSECTIONS_H
#define INTERSECTIONS_H

// todo unit tests

// todo any of these involving planes may have issues regarding the z in the local coords. Remember to use Compare to the z value the plane is at in local coords.

#include "primitives.h"
#include <iostream> // ! for debugging

namespace Collisions {
    // * ===================================
    // * Point vs Primitives
    // * ===================================

    // Determine if a point lies on a line.
    bool PointAndLine(ZMath::Vec3D const &point, Primitives::Line3D const &line) {
        // ? Determine the distance between the line and point using the projection of that point to a point on the line.
        // ? D = ||PQ x u||/||u||
        // ? If this distance is 0, we know it lies on the line.

        // obtain the max and min points for the bounds
        ZMath::Vec3D min = line.getMin(), max = line.getMax();

        // ensure the point is within the start and end of the line segment
        if (point.x < min.x || point.x > max.x || point.y < min.y || point.y > max.y || point.z < min.z || point.z > max.z) { return 0; }

        // We don't need to divide by ||u|| to know if it will evaluate to 0.
        return ZMath::compare((point - line.start).cross((line.end - line.start)).magSq(), 0);
    };

    // Determine if a point lies on a plane.
    bool PointAndPlane(ZMath::Vec3D const &point, Primitives::Plane const &plane) {
        ZMath::Vec2D min = plane.getLocalMin(), max = plane.getLocalMax();
        ZMath::Vec3D p(point); // allows for rotation

        // must rotate it in the proper order
        ZMath::rotateXZ(p, plane.sb.pos, 360 - plane.sb.phi);
        ZMath::rotateXY(p, plane.sb.pos, 360 - plane.sb.theta);

        return p.x >= min.x && p.y >= min.y && p.x <= max.x && p.y <= max.y && ZMath::compare(p.z, plane.sb.pos.z);
    };

    // Determine if a point lies within a sphere.
    bool PointAndSphere(ZMath::Vec3D const &point, Primitives::Sphere const &sphere) { return sphere.rb.pos.distSq(point) <= sphere.r*sphere.r; };

    // Determine if a point lies within an unrotated cube.
    bool PointAndAABB(ZMath::Vec3D const &point, Primitives::AABB const &aabb) {
        ZMath::Vec3D min = aabb.getMin(), max = aabb.getMax();
        return point.x <= max.x && point.y <= max.y && point.z <= max.z && point.x >= min.x && point.y >= min.y && point.z >= min.z;
    };

    // Determine if a point lies within a rotated cube.
    bool PointAndCube(ZMath::Vec3D const &point, Primitives::Cube const &cube) {
        ZMath::Vec3D min = cube.getLocalMin(), max = cube.getLocalMax();
        ZMath::Vec3D p = point; // create a copy so we can rotate into our local cords

        // rotate into our UVW cords
        ZMath::rotateXZ(p, cube.rb.pos, 360 - cube.rb.phi);
        ZMath::rotateXY(p, cube.rb.pos, 360 - cube.rb.theta);

        return p.x <= max.x && p.y <= max.y && p.z <= max.z && p.x >= min.x && p.y >= min.y && p.z >= min.z;
    };

    // * ===================================
    // * Line3D vs Primitives
    // * ===================================

    // Determine if a line intersects a point.
    bool LineAndPoint(Primitives::Line3D const &line, ZMath::Vec3D const &point) { return PointAndLine(point, line); };

    // Determine if a line intersects another line.
    bool LineAndLine(Primitives::Line3D const &line1, Primitives::Line3D const &line2) {
        // ? First check if the lines are parallel.
        // ? If the lines are parallel, we check to ensure overlap and that the start point of line2 lies on line1 if the lines were infinite.
        // ? If the lines are not parallel, we then solve for the point of intersection using the parametric equations for a 3D line.
        // ? We then ensure this point in time statisfies all 3 equations.
        // ? If it does and there's overlap, we have an intersection; otherwise we do not.

       // todo potentially could use the proportions method instead

        ZMath::Vec3D s1 = line1.start, s2 = line2.start, e1 = line1.end, e2 = line2.end;
        ZMath::Vec3D v1 = e1 - s1, v2 = e2 - s2;

        // ! This works but could probably take the ratios between the lines instead
        ZMath::Vec3D n1 = v1.normalize(), n2 = v2.normalize(); // used for checking for parallel lines

        // check for parallel lines
        if (n1.x == n2.x && n1.y == n2.y && n1.z == n2.z) {
            ZMath::Vec3D min1 = line1.getMin(), max1 = line1.getMax();
            ZMath::Vec3D min2 = line2.getMin(), max2 = line2.getMax();

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
        ZMath::Vec3D max(ZMath::min(ZMath::max(s1.x, e1.x), ZMath::max(s2.x, e2.x)), 
                         ZMath::min(ZMath::max(s1.y, e1.y), ZMath::max(s2.y, e2.y)),
                         ZMath::min(ZMath::max(s1.z, e1.z), ZMath::max(s2.z, e2.z)));

        ZMath::Vec3D min(ZMath::max(ZMath::min(s1.x, e1.x), ZMath::min(s2.x, e2.x)),
                         ZMath::max(ZMath::min(s1.y, e1.y), ZMath::min(s2.y, e2.y)),
                         ZMath::max(ZMath::min(s1.z, e1.z), ZMath::min(s2.z, e2.z)));

        // ensure the point of intersection is between these bounds
        return p.x <= max.x && p.y <= max.y && p.z <= max.z && p.x >= min.x && p.y >= min.y && p.z >= min.z;
    };

    // Determine if a line intersects a plane.
    bool LineAndPlane(Primitives::Line3D const &line, Primitives::Plane const &plane) {
        Primitives::Line3D l(line.start, line.end); // copy so we can rotate it

        // rotate the line into the plane's local coordinates
        ZMath::rotateXZ(l.start, plane.sb.pos, 360 - plane.sb.phi);
        ZMath::rotateXY(l.start, plane.sb.pos, 360 - plane.sb.theta);
        ZMath::rotateXZ(l.end, plane.sb.pos, 360 - plane.sb.phi);
        ZMath::rotateXY(l.end, plane.sb.pos, 360 - plane.sb.theta);

        // get the maxes and mins
        ZMath::Vec3D minL = l.getMin(), maxL = l.getMax();
        ZMath::Vec2D minP = plane.getLocalMin(), maxP = plane.getLocalMax();

        // check for a line laying on the same plane as the plane we're checking against
        if (ZMath::compare(minL.z, plane.sb.pos.z) && ZMath::compare(maxL.z, plane.sb.pos.z)) {
            return minL.x > maxP.x || minP.x > maxL.x || minL.y > maxP.y || minP.y > maxL.y;
        }

        // ensure an intersection is possible
        if (minL.z > plane.sb.pos.z || plane.sb.pos.z > maxL.z) { return 0; }

        // If there was an intersection, there would be a point in time where the line's z coord = minP.z.
        // We also know that the z component of our slope is not 0 as we checked for that case earlier.
        ZMath::Vec3D slope = l.end - l.start;
        float t = (plane.sb.pos.z - l.start.z)/slope.z;
        ZMath::Vec2D p(l.start.x + t*slope.x, l.start.y + t*slope.y); // point of intersection

        // Ensure the point of intersection is within the bounds of the plane.
        // We do not need to check if it is within the bounds of the line as the conditional above requires that.
        return minP.x <= p.x && p.x <= maxP.x && minP.y <= p.y && p.y <= maxP.y;
    };

    // Determine if a line intersects a sphere.
    bool LineAndSphere(Primitives::Line3D const &line, Primitives::Sphere const &sphere) {
        // ? Use the parametric equations for a 3D line.
        // ? Relate the parametric equations with the distance squared to the center of the sphere.
        // ? Since we define our start point as the point at t_0 and end point as the point at t_1,
        // ?  we know that if either solution of the quadratic constructed is between 0 to 1 inclusive, we have a collision.

        if (line.start.distSq(sphere.rb.pos) <= sphere.r * sphere.r || line.end.distSq(sphere.rb.pos) <= sphere.r * sphere.r) { return 1; }

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

    // Determine if a line intersects an unrotated cube.
    bool LineAndAABB(Primitives::Line3D const &line, Primitives::AABB const &aabb) {
        // ? Check if the line has any point within the AABB's bounds.

        ZMath::Vec3D minL = line.getMin(), maxL = line.getMax();
        ZMath::Vec3D minA = aabb.getMin(), maxA = aabb.getMax();

        return minL.x <= maxA.x && minA.x <= maxL.x && minL.y <= maxA.y && minA.y <= maxL.y && minL.z <= maxA.z && minA.z <= maxL.z;
    };

    // Determine if a line intersects a cube.
    bool LineAndCube(Primitives::Line3D const &line, Primitives::Cube const &cube) {
        // ? Rotate into the Cube's UVW coordinates.
        // ? Check to see if the line is within the cube's bounds.

        Primitives::Line3D l(line.start, line.end);

        ZMath::rotateXZ(l.start, cube.rb.pos, 360 - cube.rb.phi);
        ZMath::rotateXY(l.start, cube.rb.pos, 360 - cube.rb.theta);
        ZMath::rotateXZ(l.end, cube.rb.pos, 360 - cube.rb.phi);
        ZMath::rotateXY(l.end, cube.rb.pos, 360 - cube.rb.theta);

        ZMath::Vec3D minL = l.getMin(), maxL = l.getMax();
        ZMath::Vec3D minC = cube.getLocalMin(), maxC = cube.getLocalMax();

        return minL.x <= maxC.x && minC.x <= maxL.x && minL.y <= maxC.y && minC.y <= maxL.y && minL.z <= maxC.z && minC.z <= maxL.z;
    };

    // * =================
    // * Raycasting
    // * =================

    // Determine if a ray intersects a plane.
    // dist will be modified to equal the distance from the ray it hits the plane.
    // dist is set to -1 if there is no intersection.
    bool raycast(Primitives::Plane const &plane, Primitives::Ray3D const &ray, float &dist) {
        float dot = plane.normal * ray.dir;

        // check if the ray is parallel to the plane
        if (!dot) { dist = -1.0f; return 0; }

        dist = -((plane.normal * (ray.origin - plane.sb.pos))/dot);
        ZMath::Vec2D min = plane.getLocalMin(), max = plane.getLocalMax();
        ZMath::Vec3D p = ray.origin + ray.dir*dist;

        ZMath::rotateXZ(p, plane.sb.pos, 360 - plane.sb.phi);
        ZMath::rotateXY(p, plane.sb.pos, 360 - plane.sb.theta);

        // make sure the point of intersection is within our bounds and the intersection wouldn't occur behind the ray
        if (dist >= 0 && p.x >= min.x && p.y >= min.y && p.x <= max.x && p.y <= max.y && ZMath::compare(p.z, plane.sb.pos.z)) { return 1; }

        dist = -1.0f;
        return 0;
    };

    // Determine if a ray intersects a sphere.
    // dist will be modified to equal the distance from the ray it hits the sphere.
    // dist is set to -1 if there is no intersection.
    bool raycast(Primitives::Sphere const &sphere, Primitives::Ray3D const &ray, float &dist) {
        // todo at some point we may want to get the hit point.
        // todo if we do, we simply do hit = ray.origin + dist*ray.dir;

        // ? First we find the closest point on the ray to the sphere.
        // ? To find this point, we find the distance to it using dir * (center - origin).
        // ? Next we solve origin + t*origin to find the closest point.
        // ? If the distance of that closest point to the center is less than or equal to the radius, we have an intersection.

        // determine the closest point and the distance to that point
        float t = ray.dir * (sphere.rb.pos - ray.origin);

        // the sphere is behind the ray
        if (t < 0) {
            dist = -1.0f;
            return 0;
        }

        ZMath::Vec3D close = ray.origin + ray.dir * t;

        float dSq = sphere.rb.pos.distSq(close);
        float rSq = sphere.r*sphere.r;

        // no intersection
        if (dSq > rSq) {
            dist = -1.0f;
            return 0;
        }

        // lands on the circumference
        if (dSq == rSq) {
            dist = t;
            return 1;
        }

        // ray started in the sphere
        if (t < sphere.r) {
            dist = t + sqrtf(rSq - dSq);
            return 1;
        }

        // standard intersection
        dist = t - sqrtf(rSq - dSq);
        return 1;
    };

    // Determine if a ray intersects an AABB.
    // dist will be modified to equal the distance from the ray it hits the AABB.
    // dist is set to -1 if there is no intersection.
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

    // Determine if a ray intersects a cube.
    // dist will be modified to equal the distance from the ray it hits the cube.
    // dist is set to -1 if there is no intersection.
    bool raycast(Primitives::Cube const &cube, Primitives::Ray3D const &ray, float &dist) {};

    // * ===================================
    // * Plane vs Primitives
    // * ===================================

    // Determine if a plane intersects a point.
    bool PlaneAndPoint(Primitives::Plane const &plane, ZMath::Vec3D const &point) { return PointAndPlane(point, plane); };

    // Determine if a plane intersects a line.
    bool PlaneAndLine(Primitives::Plane const &plane, Primitives::Line3D const &line) { return LineAndPlane(line, plane); };

    // Determine if a plane intersects a sphere.
    bool PlaneAndSphere(Primitives::Plane const &plane, Primitives::Sphere const &sphere) {
        // ? A line from the center of the sphere along the normal of the plane will eventually
        // ?  intersect an infinite plane at the closest point of intersection.
        // ? Using this, we can take the distance to the plane from that point using the equation
        // ?  for the distance from a point to a plane.
        // ? If this distance squared is less than the radius squared, we have an intersection.
        // ? Since we are not dealing with infinite planes, we will first need to clamp the 
        // ?  sphere's center between the min and max vertices of the plane.

        ZMath::Vec3D closest(sphere.rb.pos);
        ZMath::Vec2D min = plane.getLocalMin(), max = plane.getLocalMax();
        // todo probably need to rotate the min and max, too

        ZMath::rotateXZ(closest, plane.sb.pos, 360 - plane.sb.phi);
        ZMath::rotateXY(closest, plane.sb.pos, 360 - plane.sb.theta);

        closest.x = ZMath::clamp(closest.x, min.x, max.x);
        closest.y = ZMath::clamp(closest.y, min.y, max.y);
        // if this doesn't work try replacing with ZMath::clamp(closest.z, min.z, max.z);
        closest.z = plane.sb.pos.z;

        return closest.distSq(sphere.rb.pos) <= sphere.r*sphere.r;
    };

    // Determine if a plane intersects an unrotated cube.
    bool PlaneAndAABB(Primitives::Plane const &plane, Primitives::AABB const &aabb) {
        float r = abs(plane.normal.x * aabb.rb.pos.x) + abs(plane.normal.y * aabb.rb.pos.y) + abs(plane.normal.z * aabb.rb.pos.z);
        return PlaneAndSphere(plane, Primitives::Sphere(r, aabb.rb.pos));
    };

    // Determine if a plane intersects a cube.
    bool PlaneAndCube(Primitives::Plane const &plane, Primitives::Cube const &cube) {
        Primitives::AABB aabb(cube.getLocalMin(), cube.getLocalMax());
        Primitives::Plane p(plane.getLocalMin(), plane.getLocalMax(), plane.sb.pos.z, plane.sb.theta - cube.rb.theta, plane.sb.phi - cube.rb.phi);
        return PlaneAndAABB(p, aabb);
    };

    // * ===================================
    // * Sphere vs Primitives
    // * ===================================

    // Determine if a sphere intersects a point.
    bool SphereAndPoint(Primitives::Sphere const &sphere, ZMath::Vec3D const &point) { return PointAndSphere(point, sphere); };

    // Determine if a sphere intersects a line.
    bool SphereAndLine(Primitives::Sphere const &sphere, Primitives::Line3D const &line) { return LineAndSphere(line, sphere); };

    // Determine if a sphere intersects a plane.
    bool SphereAndPlane(Primitives::Sphere const &sphere, Primitives::Plane const &plane) { return PlaneAndSphere(plane, sphere); };

    // Determine if a sphere intersects another sphere.
    bool SphereAndSphere(Primitives::Sphere const &sphere1, Primitives::Sphere const &sphere2) {
        float r = sphere1.r + sphere2.r;
        return sphere1.rb.pos.distSq(sphere2.rb.pos) <= r*r;
    };

    // Determine if a sphere intersects an unrotated cube.
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

    // Determine if a sphere intersects a cube.
    bool SphereAndCube(Primitives::Sphere const &sphere, Primitives::Cube const &cube) {
        // ? We can use the same approach as for SphereAndAABB, just we have to rotate the sphere into the Cube's UVW coordinates.

        ZMath::Vec3D center = sphere.rb.pos;
        ZMath::Vec3D min = cube.getLocalMin(), max = cube.getLocalMax();

        // rotate the center of the sphere into the UVW coordinates of our cube
        ZMath::rotateXZ(center, cube.rb.pos, 360 - cube.rb.phi);
        ZMath::rotateXY(center, cube.rb.pos, 360 - cube.rb.theta);
        
        // perform the check as if it was an AABB vs Sphere
        ZMath::Vec3D closest(center);

        closest.x = ZMath::clamp(closest.x, min.x, max.x);
        closest.y = ZMath::clamp(closest.y, min.y, max.y);
        closest.z = ZMath::clamp(closest.z, min.z, max.z);

        return closest.distSq(center) <= sphere.r*sphere.r;
    };

    // * ===================================
    // * AABB vs Primitives
    // * ===================================

    // Determine if an unrotated cube intersects a point.
    bool AABBAndPoint(Primitives::AABB const &aabb, ZMath::Vec3D const &point) { return PointAndAABB(point, aabb); };

    // Determine if an unrotated cube intersects a line.
    bool AABBAndLine(Primitives::AABB const &aabb, Primitives::Line3D const &line) { return LineAndAABB(line, aabb); };

    // Determine if an unrotated cube intersects a plane.
    bool AABBAndPlane(Primitives::AABB const &aabb, Primitives::Plane const &plane) { return PlaneAndAABB(plane, aabb); };

    // Determine if an unrotated cube intersects a sphere.
    bool AABBAndSphere(Primitives::AABB const &aabb, Primitives::Sphere const &sphere) { return SphereAndAABB(sphere, aabb); };

    // Determine if an unrotated cube intersects another unrotated cube.
    bool AABBAndAABB(Primitives::AABB const &aabb1, Primitives::AABB const &aabb2) {
        // ? Check if there's overlap for the AABBs on all three axes.
        // ? If there is, we know the two AABBs intersect.

        ZMath::Vec3D min1 = aabb1.getMin(), max1 = aabb1.getMax(), min2 = aabb2.getMin(), max2 = aabb2.getMax();

        // if both min points for a given interval are less than the other max points, we know there's an overlap on that axis
        return min2.x <= max1.x && min1.x <= max2.x && min2.y <= max1.y && min1.y <= max2.y && min2.z <= max1.z && min1.z <= max2.z;
    };

    // Determine if an unrotated cube intersects a cube.
    bool AABBAndCube(Primitives::AABB const &aabb, Primitives::Cube const &cube) {
        // ? Rotate the AABB into the cube's UVW coordinates.
        // ? Afterwards, use the same logic as AABB vs AABB.

        ZMath::Vec3D min1 = aabb.getMin(), max1 = aabb.getMax(), min2 = cube.getLocalMin(), max2 = cube.getLocalMax();

        ZMath::rotateXZ(max1, cube.rb.pos, 360 - cube.rb.phi);
        ZMath::rotateXY(min1, cube.rb.pos, 360 - cube.rb.theta);

        return min2.x <= max1.x && min1.x <= max2.x && min2.y <= max1.y && min1.y <= max2.y && min2.z <= max1.z && min1.z <= max2.z;
    };

    // * ===================================
    // * Cube vs Primitives
    // * ===================================

    // Determine if a cube intersects a point.
    bool CubeAndPoint(Primitives::Cube const &cube, ZMath::Vec3D const &point) { return PointAndCube(point, cube); };

    // Determine if a cube intersects a line.
    bool CubeAndLine(Primitives::Cube const &cube, Primitives::Line3D const &line) { return LineAndCube(line, cube); };

    // Determine if a cube intersects a plane.
    bool CubeAndPlane(Primitives::Cube const &cube, Primitives::Plane const &plane) { return PlaneAndCube(plane, cube); };

    // Determine if a cube intersects a sphere.
    bool CubeAndSphere(Primitives::Cube const &cube, Primitives::Sphere const &sphere) { return SphereAndCube(sphere, cube); };

    // Determine if a cube intersects an unrotated cube.
    bool CubeAndAABB(Primitives::Cube const &cube, Primitives::AABB const &aabb) { return AABBAndCube(aabb, cube); };

    // Determine if a cube intersects another cube.
    bool CubeAndCube(Primitives::Cube const &cube1, Primitives::Cube const &cube2) {
        // ? We can make an AABB object from the first cube and create a new cube object by subtracting the angles
        // ?  the first cube was rotated by from the second cube and perform an AABB vs Cube check.

        Primitives::AABB aabb(cube1.getLocalMin(), cube2.getLocalMax());
        Primitives::Cube cube(cube2.getLocalMin(), cube2.getLocalMax(), cube2.rb.theta - cube1.rb.theta, cube2.rb.phi - cube1.rb.phi);

        return AABBAndCube(aabb, cube);
    };
}

#endif // !INTERSECTIONS__H
