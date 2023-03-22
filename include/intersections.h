#ifndef INTERSECTIONS_H
#define INTERSECTIONS_H

#include "primitives.h"

namespace Primitives {
    // * ===================================
    // * Point vs Primitives
    // * ===================================

    // Determine if a point lies on a line.
    bool PointAndLine(ZMath::Vec3D const &point, Line3D const &line);

    // Determine if a point lies on a plane.
    bool PointAndPlane(ZMath::Vec3D const &point, Plane const &plane);

    // Determine if a point lies within a sphere.
    bool PointAndSphere(ZMath::Vec3D const &point, Sphere const &sphere);

    // Determine if a point lies within an unrotated cube.
    bool PointAndAABB(ZMath::Vec3D const &point, AABB const &aabb);

    // Determine if a point lies within a rotated cube.
    bool PointAndCube(ZMath::Vec3D const &point, Cube const &cube);

    // * ===================================
    // * Line3D vs Primitives
    // * ===================================

    // Determine if a line intersects a point.
    bool LineAndPoint(Line3D const &line, ZMath::Vec3D const &point);

    // Determine if a line intersects another line.
    bool LineAndLine(Line3D const &line1, Line3D const &line2);

    // Determine if a line intersects a plane.
    bool LineAndPlane(Line3D const &line, Plane const &plane);

    // Determine if a line intersects a sphere.
    bool LineAndSphere(Line3D const &line, Sphere const &sphere);

    // Determine if a line intersects an unrotated cube.
    bool LineAndAABB(Line3D const &line, AABB const &aabb);

    // Determine if a line intersects a cube.
    bool LineAndCube(Line3D const &line, Cube const &cube);

    // * =================
    // * Raycasting
    // * =================

    // Determine if a ray intersects a plane.
    // dist will be modified to equal the distance from the ray it hits the plane.
    // dist is set to -1 if there is no intersection.
    bool raycast(Plane const &plane, Ray3D const &ray, float &dist);

    // Determine if a ray intersects a sphere.
    // dist will be modified to equal the distance from the ray it hits the sphere.
    // dist is set to -1 if there is no intersection.
    bool raycast(Sphere const &sphere, Ray3D const &ray, float &dist);

    // Determine if a ray intersects an AABB.
    // dist will be modified to equal the distance from the ray it hits the AABB.
    // dist is set to -1 if there is no intersection.
    bool raycast(AABB const &aabb, Ray3D const &ray, float &dist);

    // Determine if a ray intersects a cube.
    // dist will be modified to equal the distance from the ray it hits the cube.
    // dist is set to -1 if there is no intersection.
    bool raycast(Cube const &cube, Ray3D const &ray, float &dist);

    // * ===================================
    // * Plane vs Primitives
    // * ===================================

    // Determine if a plane intersects a point.
    bool PlaneAndPoint(Plane const &plane, ZMath::Vec3D const &point);

    // Determine if a plane intersects a line.
    bool PlaneAndLine(Plane const &plane, Line3D const &line);

    // Determine if a plane intersects another plane.
    bool PlaneAndPlane(Plane const &plane1, Plane const &plane2);

    // Determine if a plane intersects a sphere.
    bool PlaneAndSphere(Plane const &plane, Sphere const &sphere);

    // Determine if a plane intersects an unrotated cube.
    bool PlaneAndAABB(Plane const &plane, AABB const &aabb);

    // Determine if a plane intersects a cube.
    bool PlaneAndCube(Plane const &plane, Cube const &cube);

    // * ===================================
    // * Sphere vs Primitives
    // * ===================================

    // Determine if a sphere intersects a point.
    bool SphereAndPoint(Sphere const &sphere, ZMath::Vec3D const &point);

    // Determine if a sphere intersects a line.
    bool SphereAndLine(Sphere const &sphere, Line3D const &line);

    // Determine if a sphere intersects a plane.
    bool SphereAndPlane(Sphere const &sphere, Plane const &plane);

    // Determine if a sphere intersects another sphere.
    bool SphereAndSphere(Sphere const &sphere1, Sphere const &sphere2);

    // Determine if a sphere intersects an unrotated cube.
    bool SphereAndAABB(Sphere const &sphere, AABB const &aabb);

    // Determine if a sphere intersects a cube.
    bool SphereAndCube(Sphere const &sphere, Cube const &cube);

    // * ===================================
    // * AABB vs Primitives
    // * ===================================

    // Determine if an unrotated cube intersects a point.
    bool AABBAndPoint(AABB const &aabb, ZMath::Vec3D const &point);

    // Determine if an unrotated cube intersects a line.
    bool AABBAndLine(AABB const &aabb, Line3D const &line);

    // Determine if an unrotated cube intersects a plane.
    bool AABBAndPlane(AABB const &aabb, Plane const &plane);

    // Determine if an unrotated cube intersects a sphere.
    bool AABBAndSphere(AABB const &aabb, Sphere const &sphere);

    // Determine if an unrotated cube intersects another unrotated cube.
    bool AABBAndAABB(AABB const &aabb1, AABB const &aabb2);

    // Determine if an unrotated cube intersects a cube.
    bool AABBAndCube(AABB const &aabb, Cube const &cube);

    // * ===================================
    // * Cube vs Primitives
    // * ===================================

    // Determine if a cube intersects a point.
    bool CubeAndPoint(Cube const &cube, ZMath::Vec3D const &point);

    // Determine if a cube intersects a line.
    bool CubeAndLine(Cube const &cube, Line3D const &line);

    // Determine if a cube intersects a plane.
    bool CubeAndPlane(Cube const &cube, Plane const &plane);

    // Determine if a cube intersects a sphere.
    bool CubeAndSphere(Cube const &cube, Sphere const &sphere);

    // Determine if a cube intersects an unrotated cube.
    bool CubeAndAABB(Cube const &cube, AABB const &aabb);

    // Determine if a cube intersects another cube.
    bool CubeAndCube(Cube const &cube1, Cube const &cube2);
}

#endif // !INTERSECTIONS__H