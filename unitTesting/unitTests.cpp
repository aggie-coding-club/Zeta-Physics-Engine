#include <iostream>
#include "../include/intersections.h"

// Point functions
#define POINT_AND_LINE_FAIL(point, line, expected) (Primitives::PointAndLine(point, line) != expected)
#define POINT_AND_PLANE_FAIL(point, plane, expected) (Primitives::PointAndPlane(point, plane) != expected)
#define POINT_AND_SPHERE_FAIL(point, sphere, expected) (Primitives::PointAndSphere(point, sphere) != expected)
#define POINT_AND_AABB_FAIL(point, aabb, expected) (Primitives::PointAndAABB(point, aabb) != expected)
#define POINT_AND_CUBE_FAIL(point, cube, expected) (Primitives::PointAndCube(point, cube) != expected)

// Line functions
#define LINE_AND_LINE_FAIL(line1, line2, expected) (Primitives::LineAndLine(line1, line2) != expected)
#define LINE_AND_PLANE_FAIL(line, plane, expected) (Primitives::LineAndPlane(line, plane) != expected)
#define LINE_AND_SPHERE_FAIL(line, sphere, expected) (Primitives::LineAndSphere(line, sphere) != expected)
#define LINE_AND_AABB_FAIL(line, aabb, expected) (Primitives::LineAndAABB(line, aabb) != expected)
#define LINE_AND_CUBE_FAIL(line, cube, expected) (Primitives::LineAndCube(line, cube) != expected)

// raycasting
#define RAYCAST_PLANE_FAIL(plane, ray, dist, expected, expectedDist) (Primitives::raycast(plane, ray, dist) != expected && dist != expectedDist)
#define RAYCAST_SPHERE_FAIL(sphere, ray, dist, expected, expectedDist) (Primitives::raycast(sphere, ray, dist) != expected && dist != expectedDist)
#define RAYCAST_AABB_FAIL(aabb, ray, dist, expected, expectedDist) (Primitives::raycast(aabb, ray, dist) != expected && dist != expectedDist)
#define RAYCAST_CUBE_FAIL(cube, ray, dist, expected, expectedDist) (Primitives::raycast(cube, ray, dist) != expected && dist != expectedDist)

// Plane functions
#define PLANE_AND_PLANE_FAIL(plane1, plane2, expected) (Primitives::PlaneAndPlane(plane1, plane2) != expected)
#define PLANE_AND_SPHERE_FAIL(plane, sphere, expected) (Primitives::PlaneAndSphere(plane, sphere) != expected)
#define PLANE_AND_AABB_FAIL(plane, aabb, expected) (Primitives::PlaneAndAABB(plane, aabb) != expected)
#define PLANE_AND_CUBE_FAIL(plane, cube, expected) (Primitives::PlaneAndCube(plane, cube) != expected)

// Sphere functions
#define SPHERE_AND_SPHERE_FAIL(sphere1, sphere2, expected) (Primitives::SphereAndSphere(sphere1, sphere2) != expected)
#define SPHERE_AND_AABB_FAIL(sphere, aabb, expected) (Primitives::SphereAndAABB(sphere, aabb) != expected)
#define SPHERE_AND_CUBE_FAIL(sphere, cube, expected) (Primitives::SphereAndCube(sphere, cube) != expected)

// AABB functions
#define AABB_AND_AABB_FAIL(aabb1, aabb2, expected) (Primitives::AABBAndAABB(aabb1, aabb2) != expected)
#define AABB_AND_CUBE_FAIL(aabb, cube, expected) (Primitives::AABBAndCube(aabb, cube) != expected)

// Cube function
#define CUBE_AND_CUBE_FAIL(cube1, cube2, expected) (Primitives::CubeAndCube(cube1, cube2) != expected)

bool testPointAndLine() {
    // test 1
    ZMath::Vec3D point(-1, 0, -1);
    Primitives::Line3D line(ZMath::Vec3D(2, -4, -3), ZMath::Vec3D(-4, 4, 1));

    if(POINT_AND_LINE_FAIL(point, line, 1)) {
        std::cout << 
        "[FAILED] Point on Line.\nExpected: true. Obtained: false." << std::endl;
        return 0;
    }

    std::cout << "[PASSED] Point on Line.\n";

    // test 2
    line.start = ZMath::Vec3D(-2.2f, -2, 0);
    line.end = ZMath::Vec3D(-2.2f, 3, 0);
    point.set(-2.2f, 2.5f, 0);

    if (POINT_AND_LINE_FAIL(point, line, 1)) {
        std::cout << "[FAILED] Point on Horizontal Line.\nExpected: true. Obtained: false." << std::endl;
        return 0;
    }

    std::cout << "[PASSED] Point on Horizontal Line.\n";

    // test 3
    line.start = ZMath::Vec3D(3, -2, -5);
    line.end = ZMath::Vec3D(3, -2, 5);
    point.set(3, -2, 2.5f);
    
    if (POINT_AND_LINE_FAIL(point, line, 1)) {
        std::cout << "[FAILED] Point on Vertical Line.\nExpected: true. Obtained: false." << std::endl;
        return 0;
    }

    std::cout << "[PASSED] Point on Vertical Line.\n";

    // test 3
    line.start = ZMath::Vec3D(-1.2f, 9.8f, -204.5f);
    point.set(line.end);

    if (POINT_AND_LINE_FAIL(point, line, 1)) {
        std::cout << "[FAILED] Point on Endpoint of Line.\nExpected: true. Obtained: false." << std::endl;
        return 0;
    }

    std::cout << "[PASSED] Point on Endpoint of Line.\n";

    // test 4
    line.start = ZMath::Vec3D(6, -2, -4);
    line.end = ZMath::Vec3D(5, 8, -6);
    point.set(4.9f, 8, -6);

    if (POINT_AND_LINE_FAIL(point, line, 0)) {
        std::cout << "[FAILED] Point not on Line.\nExpected: false. Obtained: true." << std::endl;
        return 0;
    }

    std::cout << "[PASSED] Point not on Line.\n";

    // test 5
    line.start = ZMath::Vec3D(2, -4, -3);
    line.end = ZMath::Vec3D(-4, 4, 1);
    point.set(-10, 12, 5);

    if (POINT_AND_LINE_FAIL(point, line, 0)) {
        std::cout << "[FAILED] Point not on Line, but would be if the line was infinite.\nExpected: false. Obtained: true." << std::endl;
        return 0;
    }

    std::cout << "[PASSED] Point not on Line, but would be if the line was infinite.\n";
    return 1;
};

int main() {
    // Point vs Line
    std::cout << "================== PointAndLine Tests. ==================\n\n";

    if (!testPointAndLine()) {
        std::cout << "\n================ [FAILED] PointAndLine. ================\n\n";
        return 1;
    }

    std::cout << "\n================ [PASSED] PointAndLine. ================\n\n";
    return 0;
};
