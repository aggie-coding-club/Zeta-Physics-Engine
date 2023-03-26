#include <iostream>
#include "../include/intersections.h"

// Point functions
#define POINT_AND_LINE_FAIL(point, line, expected) (Collisions::PointAndLine(point, line) != expected)
#define POINT_AND_PLANE_FAIL(point, plane, expected) (Collisions::PointAndPlane(point, plane) != expected)
#define POINT_AND_SPHERE_FAIL(point, sphere, expected) (Collisions::PointAndSphere(point, sphere) != expected)
#define POINT_AND_AABB_FAIL(point, aabb, expected) (Collisions::PointAndAABB(point, aabb) != expected)
#define POINT_AND_CUBE_FAIL(point, cube, expected) (Collisions::PointAndCube(point, cube) != expected)

// Line functions
#define LINE_AND_LINE_FAIL(line1, line2, expected) (Collisions::LineAndLine(line1, line2) != expected)
#define LINE_AND_PLANE_FAIL(line, plane, expected) (Collisions::LineAndPlane(line, plane) != expected)
#define LINE_AND_SPHERE_FAIL(line, sphere, expected) (Collisions::LineAndSphere(line, sphere) != expected)
#define LINE_AND_AABB_FAIL(line, aabb, expected) (Collisions::LineAndAABB(line, aabb) != expected)
#define LINE_AND_CUBE_FAIL(line, cube, expected) (Collisions::LineAndCube(line, cube) != expected)

// raycasting
#define RAYCAST_PLANE_FAIL(plane, ray, dist, expected, expectedDist) (Collisions::raycast(plane, ray, dist) != expected && dist != expectedDist)
#define RAYCAST_SPHERE_FAIL(sphere, ray, dist, expected, expectedDist) (Collisions::raycast(sphere, ray, dist) != expected && dist != expectedDist)
#define RAYCAST_AABB_FAIL(aabb, ray, dist, expected, expectedDist) (Collisions::raycast(aabb, ray, dist) != expected && dist != expectedDist)
#define RAYCAST_CUBE_FAIL(cube, ray, dist, expected, expectedDist) (Collisions::raycast(cube, ray, dist) != expected && dist != expectedDist)

// Plane functions
#define PLANE_AND_PLANE_FAIL(plane1, plane2, expected) (Collisions::PlaneAndPlane(plane1, plane2) != expected)
#define PLANE_AND_SPHERE_FAIL(plane, sphere, expected) (Collisions::PlaneAndSphere(plane, sphere) != expected)
#define PLANE_AND_AABB_FAIL(plane, aabb, expected) (Collisions::PlaneAndAABB(plane, aabb) != expected)
#define PLANE_AND_CUBE_FAIL(plane, cube, expected) (Collisions::PlaneAndCube(plane, cube) != expected)

// Sphere functions
/*#define SPHERE_AND_SPHERE_FAIL(sphere1, sphere2, expected) (Collisions::SphereAndSphere(sphere1, sphere2) != expected)
#define SPHERE_AND_AABB_FAIL(sphere, aabb, expected) (Collisions::SphereAndAABB(sphere, aabb) != expected)
#define SPHERE_AND_CUBE_FAIL(sphere, cube, expected) (Collisions::SphereAndCube(sphere, cube) != expected)

// AABB functions
#define AABB_AND_AABB_FAIL(aabb1, aabb2, expected) (Collisions::AABBAndAABB(aabb1, aabb2) != expected)
#define AABB_AND_CUBE_FAIL(aabb, cube, expected) (Collisions::AABBAndCube(aabb, cube) != expected)

// Cube function
#define CUBE_AND_CUBE_FAIL(cube1, cube2, expected) (Collisions::CubeAndCube(cube1, cube2) != expected)*/

bool testPointAndLine() {
    // test 1
    ZMath::Vec3D point(-1, 0, -1);
    Primitives::Line3D line(ZMath::Vec3D(2, -4, -3), ZMath::Vec3D(-4, 4, 1));

    if(POINT_AND_LINE_FAIL(point, line, 1)) {
        std::cout << 
        "[FAILED] Point on Line.\nExpected: true. Obtained: false." << std::endl;
        return 1;
    }

    std::cout << "[PASSED] Point on Line.\n";

    // test 2
    line.start = ZMath::Vec3D(-2.2f, -2, 0);
    line.end = ZMath::Vec3D(-2.2f, 3, 0);
    point.set(-2.2f, 2.5f, 0);

    if (POINT_AND_LINE_FAIL(point, line, 1)) {
        std::cout << "[FAILED] Point on Horizontal Line.\nExpected: true. Obtained: false." << std::endl;
        return 1;
    }

    std::cout << "[PASSED] Point on Horizontal Line.\n";

    // test 3
    line.start = ZMath::Vec3D(3, -2, -5);
    line.end = ZMath::Vec3D(3, -2, 5);
    point.set(3, -2, 2.5f);
    
    if (POINT_AND_LINE_FAIL(point, line, 1)) {
        std::cout << "[FAILED] Point on Vertical Line.\nExpected: true. Obtained: false." << std::endl;
        return 1;
    }

    std::cout << "[PASSED] Point on Vertical Line.\n";

    // test 4
    line.start = ZMath::Vec3D(-1.2f, 9.8f, -204.5f);
    point.set(line.end);

    if (POINT_AND_LINE_FAIL(point, line, 1)) {
        std::cout << "[FAILED] Point on Endpoint of Line.\nExpected: true. Obtained: false." << std::endl;
        return 1;
    }

    std::cout << "[PASSED] Point on Endpoint of Line.\n";

    // test 5
    line.start = ZMath::Vec3D(6, -2, -4);
    line.end = ZMath::Vec3D(5, 8, -6);
    point.set(4.9f, 8, -6);

    if (POINT_AND_LINE_FAIL(point, line, 0)) {
        std::cout << "[FAILED] Point not on Line.\nExpected: false. Obtained: true." << std::endl;
        return 1;
    }

    std::cout << "[PASSED] Point not on Line.\n";

    // test 6
    line.start = ZMath::Vec3D(2, -4, -3);
    line.end = ZMath::Vec3D(-4, 4, 1);
    point.set(-10, 12, 5);

    if (POINT_AND_LINE_FAIL(point, line, 0)) {
        std::cout << "[FAILED] Point not on Line, but would be if the line was infinite.\nExpected: false. Obtained: true." << std::endl;
        return 1;
    }

    std::cout << "[PASSED] Point not on Line, but would be if the line was infinite.\n";
    return 0;
};

bool testPointAndPlane() {
    // test 1
    ZMath::Vec3D point(0.866f, 1, 0.5f);
    Primitives::Plane plane1(ZMath::Vec2D(-2, -4), ZMath::Vec2D(2, 4), 0, 0.0f, 30.0f);

    if(POINT_AND_PLANE_FAIL(point, plane1, 1)) {
        std::cout << "[FAILED] Point on XZ Rotated Plane.\nExpected: true. Obtained: false." << std::endl;
        return 1;
    }

    std::cout << "[PASSED] Point on XZ Rotated Plane.\n";

    // test 2
    point.set(-3, 22.3f, 7.77f);
    Primitives::Plane plane2(ZMath::Vec2D(-3, -2), ZMath::Vec2D(20, 50), 7.77f);

    if (POINT_AND_PLANE_FAIL(point, plane2, 1)) {
        std::cout << "[FAILED] Point on Edge of Unrotated Plane.\nExpected: true. Obtained: false." << std::endl;
        return 1;
    }

    std::cout << "[PASSED] Point on Edge of Unrotated Plane.\n";

    // test 3
    point.set(0, 1.5f, 1.2f);
    Primitives::Plane plane3(ZMath::Vec2D(-2, -4), ZMath::Vec2D(2, 4), 0, 90.0f, 90.0f);

    if(POINT_AND_PLANE_FAIL(point, plane3, 1)) {
        std::cout << "[FAILED] Point on Vertical Plane.\nExpected: true. Obtained: false." << std::endl;
        return 1;
    }

    std::cout << "[PASSED] Point on Vertical Plane.\n";

    // test 4
    point.set(-0.5f, 100, 20.0f);
    Primitives::Plane plane5(ZMath::Vec2D(-2, -4), ZMath::Vec2D(2, 4), 0, 0.0f, 30.0f);

    if(POINT_AND_PLANE_FAIL(point, plane5, 0)) {
        std::cout << "[FAILED] Point not on Plane.\nExpected: false. Obtained: true." << std::endl;
        return 1;
    }

    std::cout << "[PASSED] Point not on Plane.\n";

    // test 5
    point.set(5, 22.3f, 4.2f);
    Primitives::Plane plane6(ZMath::Vec2D(0, 0), ZMath::Vec2D(10, 20), 4.2f);

    if (POINT_AND_PLANE_FAIL(point, plane6, 0)) {
        std::cout << "[FAILED] Point not on Plane, but would be if the plane was infinite.\nExpected: false. Obtained: true." << std::endl;
        return 1;
    }

    std::cout << "[PASSED] Point not on Plane, but would be if the plane was infinite.\n";

    return 0;
};

bool testPointAndSphere() {
    // test 1
    ZMath::Vec3D point(2);
    Primitives::Sphere sphere(5.0f, ZMath::Vec3D(4, 3, 2));

    if (POINT_AND_SPHERE_FAIL(point, sphere, 1)) {
        std::cout << "[FAILED] Point in Sphere.\nExpected: true. Obtained: false." << std::endl;
        return 1;
    }

    std::cout << "[PASSED] Point in Sphere.\n";

    // test 2
    point.set(1, 0, 0);
    sphere.rb.pos.zero();
    sphere.r = 1.0f;

    if (POINT_AND_SPHERE_FAIL(point, sphere, 1)) {
        std::cout << "[FAILED] Point on Circumference of Sphere.\nExpected: true. Obtained: false." << std::endl;
        return 1;
    }

    std::cout << "[PASSED] Point on Circumference of Sphere.\n";

    // test 3
    point.set(1);
    
    if (POINT_AND_SPHERE_FAIL(point, sphere, 0)) {
        std::cout << "[FAILED] Point not in Sphere.\nExpected: false. Obtained: true." << std::endl;
        return 1;
    }

    std::cout << "[PASSED] Point not in Sphere.\n";
    return 0;
};

bool testPointAndAABB() {
    // test 1
    ZMath::Vec3D point(2, 3, 2);
    Primitives::AABB aabb1(ZMath::Vec3D(-1), ZMath::Vec3D(5));

    if (POINT_AND_AABB_FAIL(point, aabb1, 1)) {
        std::cout << "[FAILED] Point in AABB.\nExpected: true. Obtained: false." << std::endl;
        return 1;
    }

    std::cout << "[PASSED] Point in AABB.\n";

    // test 2
    point.zero();
    Primitives::AABB aabb2(ZMath::Vec3D(-2), ZMath::Vec3D());

    if (POINT_AND_AABB_FAIL(point, aabb2, 1)) {
        std::cout << "[FAILED] Point on Vertex of AABB.\nExpected: true. Obtained: false." << std::endl;
        return 1;
    }

    std::cout << "[PASSED] Point on Vertex of AABB.\n";

    // test 3
    point.set(5);
    Primitives::AABB aabb3(ZMath::Vec3D(), ZMath::Vec3D(5, 6, 6));

    if (POINT_AND_AABB_FAIL(point, aabb3, 1)) {
        std::cout << "[FAILED] Point on Edge of AABB.\nExpected: true. Obtained: false." << std::endl;
        return 1;
    }

    std::cout << "[PASSED] Point on Edge of AABB.\n";

    // test 5
    point.zero();
    Primitives::AABB aabb4(ZMath::Vec3D(-2), ZMath::Vec3D(-1));

    if (POINT_AND_AABB_FAIL(point, aabb4, 0)) {
        std::cout << "[FAILED] Point not in AABB.\nExpected: false. Obtained: true." << std::endl;
        return 1;
    }

    std::cout << "[PASSED] Point not in AABB.\n";
    return 0;
};

bool testPointAndCube() {
    // test 1
    ZMath::Vec3D point(1);
    Primitives::Cube cube1(ZMath::Vec3D(-100), ZMath::Vec3D(100), 10.0f, 20.0f);

    if (POINT_AND_CUBE_FAIL(point, cube1, 1)) {
        std::cout << "[FAILED] Point in Cube.\nExpected: true. Obtained: false." << std::endl;
        return 1;
    }

    std::cout << "[PASSED] Point in Cube.\n";

    // test 2
    point.set(-4.24f, 8.48f, 4.24f);
    Primitives::Cube cube2(ZMath::Vec3D(-6), ZMath::Vec3D(6), 45.0f, 45.0f);

    if (POINT_AND_CUBE_FAIL(point, cube2, 1)) {
        std::cout << "[FAILED] Point on Cube Vertex.\nExpected: true. Obtained: false." << std::endl;
        return 1;
    }

    std::cout << "[PASSED] Point on Cube Vertex.\n";

    // test 3
    point.set(10.0f);
    Primitives::Cube cube3(ZMath::Vec3D(-6), ZMath::Vec3D(6), 45.0f, 45.0f);

    if (POINT_AND_CUBE_FAIL(point, cube3, 0)) {
        std::cout << "[FAILED] Point not in Cube.\nExpected: false. Obtained: true." << std::endl;
        return 1;
    }

    std::cout << "[PASSED] Point not in Cube.\n";

    // test 4
    point.set(-5.9f);
    Primitives::Cube cube4(ZMath::Vec3D(-6), ZMath::Vec3D(6), 45.0f, 45.0f);

    if (POINT_AND_CUBE_FAIL(point, cube4, 0)) {
        std::cout << "[FAILED] Point not in Cube, but would be if the cube was an AABB.\nExpected: false. Obtained: true." << std::endl;
        return 1;
    }

    std::cout << "[PASSED] Point not in Cube, but would be if the cube was an AABB.\n";
    return 0;
};

bool testLineAndLine() {
    return 0;
};

bool testLineAndPlane() {
    return 0;
};

bool testLineAndSphere() {
    return 0;
};

bool testLineAndAABB() {
    return 0;
};

bool testLineAndCube() {
    return 0;
};

bool testRaycastingVSPlane() {
    return 0;
};

bool testRaycastingVSSphere() {
    return 0;
};

bool testRaycastingVSAABB() {
    return 0;
};

bool testRaycastingVSCube() {
    return 0;
};

bool testPlaneAndPlane() {
    return 0;
};

bool testPlaneAndSphere() {
    return 0;
};

bool testPlaneAndAABB() {
    return 0;
};

bool testPlaneAndCube() {
    return 0;
};

/*bool testSphereAndSphere() {
    return 0;
};

bool testSphereAndAABB() {
    return 0;
};

bool testSphereAndCube() {
    return 0;
};

bool testAABBAndAABB() {
    return 0;
};

bool testAABBAndCube() {
    return 0;
};

bool testCubeAndCube() {
    return 0;
};*/

int main() {
    // Point vs Line
    std::cout << "================== PointAndLine Tests. ==================\n\n";

    if (testPointAndLine()) {
        std::cout << "\n================ [FAILED] PointAndLine. ================\n\n";
        return 1;
    }

    std::cout << "\n================ [PASSED] PointAndLine. ================\n\n";

    // Point vs Plane
    std::cout << "================== PointAndPlane Tests. ==================\n\n";

    if (testPointAndPlane()) {
        std::cout << "\n================ [FAILED] PointAndPlane. ================\n\n";
        return 1;
    }

    std::cout << "\n================ [PASSED] PointAndPlane. ================\n\n";

    // Point vs Sphere
    std::cout << "================== PointAndSphere Tests. ==================\n\n";

    if (testPointAndSphere()) {
        std::cout << "\n================ [FAILED] PointAndSphere. ================\n\n";
        return 1;
    }

    std::cout << "\n================ [PASSED] PointAndSphere. ================\n\n";

    // Point vs AABB
    std::cout << "================== PointAndAABB Tests. ==================\n\n";

    if (testPointAndAABB()) {
        std::cout << "\n================ [FAILED] PointAndAABB. ================\n\n";
        return 1;
    }

    std::cout << "\n================ [PASSED] PointAndAABB. ================\n\n";

    // Point vs Cube
    std::cout << "================== PointAndCube Tests. ==================\n\n";

    if (testPointAndCube()) {
        std::cout << "\n================ [FAILED] PointAndCube. ================\n\n";
        return 1;
    }

    std::cout << "\n================ [PASSED] PointAndCube. ================\n\n";

    return 0;
};
