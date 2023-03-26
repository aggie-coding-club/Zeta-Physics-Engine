#ifndef INTERSECTIONS_H
#define INTERSECTIONS_H

#include "primitives.h"

namespace Collisions {
    // * ===================================
    // * Point vs Primitives
    // * ===================================

    // Determine if a point lies on a line.
    bool PointAndLine(ZMath::Vec3D const &point, Primitives::Line3D const &line);

    // Determine if a point lies on a plane.
    bool PointAndPlane(ZMath::Vec3D const &point, Primitives::Plane const &plane);

    // Determine if a point lies within a sphere.
    bool PointAndSphere(ZMath::Vec3D const &point, Primitives::Sphere const &sphere);

    // Determine if a point lies within an unrotated cube.
    bool PointAndAABB(ZMath::Vec3D const &point, Primitives::AABB const &aabb);

    // Determine if a point lies within a rotated cube.
    bool PointAndCube(ZMath::Vec3D const &point, Primitives::Cube const &cube);

    // * ===================================
    // * Line3D vs Primitives
    // * ===================================

    // Determine if a line intersects a point.
    bool LineAndPoint(Primitives::Line3D const &line, ZMath::Vec3D const &point);

    // Determine if a line intersects another line.
    bool LineAndLine(Primitives::Line3D const &line1, Primitives::Line3D const &line2);

    // Determine if a line intersects a plane.
    bool LineAndPlane(Primitives::Line3D const &line, Primitives::Plane const &plane);

    // Determine if a line intersects a sphere.
    bool LineAndSphere(Primitives::Line3D const &line, Primitives::Sphere const &sphere);

    // Determine if a line intersects an unrotated cube.
    bool LineAndAABB(Primitives::Line3D const &line, Primitives::AABB const &aabb);

    // Determine if a line intersects a cube.
    bool LineAndCube(Primitives::Line3D const &line, Primitives::Cube const &cube);

    // * =================
    // * Raycasting
    // * =================

    // Determine if a ray intersects a plane.
    // dist will be modified to equal the distance from the ray it hits the plane.
    // dist is set to -1 if there is no intersection.
    bool raycast(Primitives::Plane const &plane, Primitives::Ray3D const &ray, float &dist);

    // Determine if a ray intersects a sphere.
    // dist will be modified to equal the distance from the ray it hits the sphere.
    // dist is set to -1 if there is no intersection.
    bool raycast(Primitives::Sphere const &sphere, Primitives::Ray3D const &ray, float &dist);

    // Determine if a ray intersects an AABB.
    // dist will be modified to equal the distance from the ray it hits the AABB.
    // dist is set to -1 if there is no intersection.
    bool raycast(Primitives::AABB const &aabb, Primitives::Ray3D const &ray, float &dist);

    // Determine if a ray intersects a cube.
    // dist will be modified to equal the distance from the ray it hits the cube.
    // dist is set to -1 if there is no intersection.
    bool raycast(Primitives::Cube const &cube, Primitives::Ray3D const &ray, float &dist);

    // * ===================================
    // * Plane vs Primitives
    // * ===================================

    // Determine if a plane intersects a point.
    bool PlaneAndPoint(Primitives::Plane const &plane, ZMath::Vec3D const &point);

    // Determine if a plane intersects a line.
    bool PlaneAndLine(Primitives::Plane const &plane, Primitives::Line3D const &line);

    // Determine if a plane intersects another plane.
    bool PlaneAndPlane(Primitives::Plane const &plane1, Primitives::Plane const &plane2);

    // Determine if a plane intersects a sphere.
    bool PlaneAndSphere(Primitives::Plane const &plane, Primitives::Sphere const &sphere);

    // Determine if a plane intersects an unrotated cube.
    bool PlaneAndAABB(Primitives::Plane const &plane, Primitives::AABB const &aabb);

    // Determine if a plane intersects a cube.
    bool PlaneAndCube(Primitives::Plane const &plane, Primitives::Cube const &cube);

    // * ===================================
    // * Sphere vs Primitives
    // * ===================================

    // Determine if a sphere intersects a point.
    bool SphereAndPoint(Primitives::Sphere const &sphere, ZMath::Vec3D const &point);

    // Determine if a sphere intersects a line.
    bool SphereAndLine(Primitives::Sphere const &sphere, Primitives::Line3D const &line);

    // Determine if a sphere intersects a plane.
    bool SphereAndPlane(Primitives::Sphere const &sphere, Primitives::Plane const &plane);

    // Determine if a sphere intersects another sphere.
    /*bool SphereAndSphere(Primitives::Sphere const &sphere1, Primitives::Sphere const &sphere2);

    // Determine if a sphere intersects an unrotated cube.
    bool SphereAndAABB(Primitives::Sphere const &sphere, Primitives::AABB const &aabb);

    // Determine if a sphere intersects a cube.
    bool SphereAndCube(Primitives::Sphere const &sphere, Primitives::Cube const &cube);*/

    // * ===================================
    // * AABB vs Primitives
    // * ===================================

    // Determine if an unrotated cube intersects a point.
    bool AABBAndPoint(Primitives::AABB const &aabb, ZMath::Vec3D const &point);

    // Determine if an unrotated cube intersects a line.
    bool AABBAndLine(Primitives::AABB const &aabb, Primitives::Line3D const &line);

    // Determine if an unrotated cube intersects a plane.
    bool AABBAndPlane(Primitives::AABB const &aabb, Primitives::Plane const &plane);

    // Determine if an unrotated cube intersects a sphere.
    /*bool AABBAndSphere(Primitives::AABB const &aabb, Primitives::Sphere const &sphere);

    // Determine if an unrotated cube intersects another unrotated cube.
    bool AABBAndAABB(Primitives::AABB const &aabb1, Primitives::AABB const &aabb2);

    // Determine if an unrotated cube intersects a cube.
    bool AABBAndCube(Primitives::AABB const &aabb, Primitives::Cube const &cube);*/

    // * ===================================
    // * Cube vs Primitives
    // * ===================================

    // Determine if a cube intersects a point.
    bool CubeAndPoint(Primitives::Cube const &cube, ZMath::Vec3D const &point);

    // Determine if a cube intersects a line.
    bool CubeAndLine(Primitives::Cube const &cube, Primitives::Line3D const &line);

    // Determine if a cube intersects a plane.
    bool CubeAndPlane(Primitives::Cube const &cube, Primitives::Plane const &plane);

    // Determine if a cube intersects a sphere.
    /*bool CubeAndSphere(Primitives::Cube const &cube, Primitives::Sphere const &sphere);

    // Determine if a cube intersects an unrotated cube.
    bool CubeAndAABB(Primitives::Cube const &cube, Primitives::AABB const &aabb);

    // Determine if a cube intersects another cube.
    bool CubeAndCube(Primitives::Cube const &cube1, Primitives::Cube const &cube2);*/
}

#endif // !INTERSECTIONS__H
