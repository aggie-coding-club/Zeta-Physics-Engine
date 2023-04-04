#include <iostream>
#include <string>
#include "../include/intersections.h"

// * =====================================
// * Primary Unit Testing Functions
// * =====================================

// Macro for unit testing.
#define UNIT_TEST(test, obtained, expected) \
({ \
    if ((obtained) == (expected)) { \
        std::cout << "[PASSED] " << (test) << "\n"; \
 \
    } else { \
        std::cout << "[FAILED] " << (test) << "\nExpected: " << (expected) << ". Obtained: " << (obtained) << ".\n"; \
    } \
\
    (obtained) != (expected); \
})

// Run a function of unit tests.
bool testCases(std::string test, bool (*func)()) {
    std::cout << "================== " << test << " Tests. ==================\n\n";

    if (func()) {
        std::cout << "\n================ [FAILED] " << test << ". ================\n\n";
        return 1;
    }

    std::cout << "\n================ [PASSED] " << test << ". ================\n\n";
    return 0;
};


// * ===================
// * Unit Tests
// * ===================

bool testPointAndLine() {
    // test 1
    ZMath::Vec3D point(-1, 0, -1);
    Primitives::Line3D line(ZMath::Vec3D(2, -4, -3), ZMath::Vec3D(-4, 4, 1));

    if (UNIT_TEST("Point on Line.", Collisions::PointAndLine(point, line), 1)) { return 1; }

    // test 2
    line.start = ZMath::Vec3D(-2.2f, -2, 0);
    line.end = ZMath::Vec3D(-2.2f, 3, 0);
    point.set(-2.2f, 2.5f, 0);

    if (UNIT_TEST("Point on Horizontal Line.", Collisions::PointAndLine(point, line), 1)) { return 1; }

    // test 3
    line.start = ZMath::Vec3D(3, -2, -5);
    line.end = ZMath::Vec3D(3, -2, 5);
    point.set(3, -2, 2.5f);
    
    if (UNIT_TEST("Point on Vertical Line.", Collisions::PointAndLine(point, line), 1)) { return 1; }

    // test 4
    line.start = ZMath::Vec3D(-1.2f, 9.8f, -204.5f);
    point.set(line.end);

    if (UNIT_TEST("Point on Endpoint of Line.", Collisions::PointAndLine(point, line), 1)) { return 1; }

    // test 5
    line.start = ZMath::Vec3D(6, -2, -4);
    line.end = ZMath::Vec3D(5, 8, -6);
    point.set(4.9f, 8, -6);

    if (UNIT_TEST("Point not on Line.", Collisions::PointAndLine(point, line), 0)) { return 1; }

    // test 6
    line.start = ZMath::Vec3D(2, -4, -3);
    line.end = ZMath::Vec3D(-4, 4, 1);
    point.set(-10, 12, 5);

    if (UNIT_TEST("Point not on Line, but would be if the line was infinite.", Collisions::PointAndLine(point, line), 0)) { return 1; }
    return 0;
}; 

bool testPointAndPlane() {
    // test 1
    ZMath::Vec3D point(0.866f, 1, 0.5f);
    Primitives::Plane plane1(ZMath::Vec2D(-2, -4), ZMath::Vec2D(2, 4), 0, 0.0f, 30.0f);

    if(UNIT_TEST("Point on XZ Rotated Plane.", Collisions::PointAndPlane(point, plane1), 1)) { return 1; }

    // test 2
    point.set(-3, 22.3f, 7.77f);
    Primitives::Plane plane2(ZMath::Vec2D(-3, -2), ZMath::Vec2D(20, 50), 7.77f);

    if (UNIT_TEST("Point on Edge of Unrotated Plane.", Collisions::PointAndPlane(point, plane2), 1)) { return 1; }

    // test 3
    point.set(0, 1.5f, 1.2f);
    Primitives::Plane plane3(ZMath::Vec2D(-2, -4), ZMath::Vec2D(2, 4), 0, 90.0f, 90.0f);

    if (UNIT_TEST("Point on Vertical Plane.", Collisions::PointAndPlane(point, plane3), 1)) { return 1; }

    // test 4
    point.set(-0.5f, 100, 20.0f);
    Primitives::Plane plane5(ZMath::Vec2D(-2, -4), ZMath::Vec2D(2, 4), 0, 0.0f, 30.0f);

    if(UNIT_TEST("Point not on Plane.", Collisions::PointAndPlane(point, plane5), 0)) { return 1; }

    // test 5
    point.set(5, 22.3f, 4.2f);
    Primitives::Plane plane6(ZMath::Vec2D(0, 0), ZMath::Vec2D(10, 20), 4.2f);

    if (UNIT_TEST("Point not on Plane, but would be if the plane was infinite.", Collisions::PointAndPlane(point, plane6), 0)) { return 1; }
    return 0;
};

bool testPointAndSphere() {
    // test 1
    ZMath::Vec3D point(2);
    Primitives::Sphere sphere(5.0f, ZMath::Vec3D(4, 3, 2));

    if (UNIT_TEST("Point in Sphere.", Collisions::PointAndSphere(point, sphere), 1)) { return 1; }

    std::cout << "[PASSED] Point in Sphere.\n";

    // test 2
    point.set(1, 0, 0);
    sphere.rb.pos.zero();
    sphere.r = 1.0f;

    if (UNIT_TEST("Point on Circumference of Sphere.", Collisions::PointAndSphere(point, sphere), 1)) { return 1; }

    // test 3
    point.set(1);
    
    if (UNIT_TEST("Point not in Sphere.", Collisions::PointAndSphere(point, sphere), 0)) { return 1; }
    return 0;
};

bool testPointAndAABB() {
    // test 1
    ZMath::Vec3D point(2, 3, 2);
    Primitives::AABB aabb1(ZMath::Vec3D(-1), ZMath::Vec3D(5));

    if (UNIT_TEST("Point in AABB.", Collisions::PointAndAABB(point, aabb1), 1)) { return 1; }

    // test 2
    point.zero();
    Primitives::AABB aabb2(ZMath::Vec3D(-2), ZMath::Vec3D());

    if (UNIT_TEST("Point on Vertex of AABB.", Collisions::PointAndAABB(point, aabb2), 1)) { return 1; }

    // test 3
    point.set(5);
    Primitives::AABB aabb3(ZMath::Vec3D(), ZMath::Vec3D(5, 6, 6));

    if (UNIT_TEST("Point on Edge of AABB.", Collisions::PointAndAABB(point, aabb3), 1)) { return 1; }

    // test 4
    point.zero();
    Primitives::AABB aabb4(ZMath::Vec3D(-2), ZMath::Vec3D(-1));

    if (UNIT_TEST("Point not in AABB.", Collisions::PointAndAABB(point, aabb4), 0)) { return 1; }
    return 0;
};

bool testPointAndCube() {
    // test 1
    ZMath::Vec3D point(1);
    Primitives::Cube cube1(ZMath::Vec3D(-100), ZMath::Vec3D(100), 10.0f, 20.0f);

    if (UNIT_TEST("Point in Cube.", Collisions::PointAndCube(point, cube1), 1)) { return 1; }

    // test 2
    point.set(-4.24f, 8.48f, 4.24f);
    Primitives::Cube cube2(ZMath::Vec3D(-6), ZMath::Vec3D(6), 45.0f, 45.0f);

    if (UNIT_TEST("Point on Cube Vertex.", Collisions::PointAndCube(point, cube2), 1)) { return 1; }
 
    // test 3
    point.set(10.0f);
    Primitives::Cube cube3(ZMath::Vec3D(-6), ZMath::Vec3D(6), 45.0f, 45.0f);

    if (UNIT_TEST("Point not in Cube.", Collisions::PointAndCube(point, cube3), 0)) { return 1; }

    // test 4
    point.set(-5.9f);
    Primitives::Cube cube4(ZMath::Vec3D(-6), ZMath::Vec3D(6), 45.0f, 45.0f);

    if (UNIT_TEST("Point not in Cube, but would be if the cube was an AABB.", Collisions::PointAndCube(point, cube4), 0)) { return 1; }
    return 0;
};

bool testLineAndLine() {
    // test 1
    Primitives::Line3D line1(ZMath::Vec3D(0, 0, -1), ZMath::Vec3D(10, 10, 9));
    Primitives::Line3D line2(ZMath::Vec3D(10, 0, 9), ZMath::Vec3D(0, 10, -1));

    if (UNIT_TEST("Standard Line Intersection.", Collisions::LineAndLine(line1, line2), 1)) { return 1; }

    // test 2
    line1.start = ZMath::Vec3D(100, -1000, 77);
    line2.end = ZMath::Vec3D(100, -1000, 77);
    line2.start = ZMath::Vec3D(200, -1200, 100);

    if (UNIT_TEST("Line Intersects End Point.", Collisions::LineAndLine(line1, line2), 1)) { return 1; }

    // test 3
    line1.start = ZMath::Vec3D(-3, -2.5f, 0);
    line1.end = ZMath::Vec3D(8, 2.5f, 0);
    line2.start = ZMath::Vec3D(-3.2f, 2.5f, 0);
    line2.end = ZMath::Vec3D(10, -1.2f, 0);

    if (UNIT_TEST("Line and Line on XY Plane.", Collisions::LineAndLine(line1, line2), 1)) { return 1; }

    // test 4
    line1.start = ZMath::Vec3D();
    line1.end = ZMath::Vec3D(2);
    line2.start = ZMath::Vec3D(1);
    line2.end = ZMath::Vec3D(3);

    if (UNIT_TEST("Parallel Line Intersection.", Collisions::LineAndLine(line1, line2), 1)) { return 1; }

    // test 5
    line1.end.set(0, 0, 5);
    line2.start.set(0, 0, 4);
    line2.end.set(0, 0, 10);

    if (UNIT_TEST("Vertical Line Intersection.", Collisions::LineAndLine(line1, line2), 1)) { return 1; }

    // test 6
    line1.end.set(0, 0, 3);

    if (UNIT_TEST("Vertical Lines not Intersecting.", Collisions::LineAndLine(line1, line2), 0)) { return 1; }

    // test 7
    line1.start.set(3.2f);
    line1.end.set(3.3f);
    line2.start.set(-2.2f);
    line2.end.set(100, 200, -1.2f);

    if (UNIT_TEST("Lines not Intersecting.", Collisions::LineAndLine(line1, line2), 0)) { return 1; }

    // test 8
    line2.start.set(3.5f);
    line2.end.set(3.7f);

    if (UNIT_TEST("Parallel Lines not Intersecting.", Collisions::LineAndLine(line1, line2), 0)) { return 1; }

    return 0;
};

bool testLineAndPlane() {
    // test 1
    Primitives::Line3D line(ZMath::Vec3D(-0.4f, -0.1f, -2), ZMath::Vec3D(2));
    Primitives::Plane plane1(ZMath::Vec2D(), ZMath::Vec2D(2), 0);

    if (UNIT_TEST("Line and Unrotated Plane.", Collisions::LineAndPlane(line, plane1), 1)) { return 1; }

    // test 2
    Primitives::Plane plane4(ZMath::Vec2D(-2), ZMath::Vec2D(2), 0, 30, 60);
    line.start.set(0.5915f, -2.04904f, -0.97548f);
    line.end.set(-0.774519f, 0.68301f, 0.65849f);

    if (UNIT_TEST("Line and Rotated Plane.", Collisions::LineAndPlane(line, plane4), 1)) { return 1; }

    // test 3
    Primitives::Plane plane3(ZMath::Vec2D(-0.5f), ZMath::Vec2D(0.5f), 5, 55.6f, 1);
    line.start.set(-9.8f, -2.3f, -1.4f);
    line.end.set(9.7f, 9.7f, 2);

    if (UNIT_TEST("Not Line and Plane.", Collisions::LineAndPlane(line, plane3), 0)) { return 1; }

    return 0;
};

bool testLineAndSphere() {
    // test 1
    Primitives::Sphere sphere(2.0f, ZMath::Vec3D());
    Primitives::Line3D line(ZMath::Vec3D(-10), ZMath::Vec3D(10));

    if (UNIT_TEST("Sphere and Line.", Collisions::LineAndSphere(line, sphere), 1)) { return 1; }

    // test 2
    sphere.r = 5.0f;
    sphere.rb.pos.set(0, 0, 0);
    line.start.set(-6, 0, 0);
    line.end.set(-4, 0, 0);

    if (UNIT_TEST("Sphere and Line on Circumference.", Collisions::LineAndSphere(line, sphere), 1)) { return 1; }

    // test 3
    sphere.r = 2.5f;

    if (UNIT_TEST("Sphere and not Line.", Collisions::LineAndSphere(line, sphere), 0)) { return 1; }

    // test 4
    sphere.r = 1;
    line.start.set(-0.5f, 0, 0);
    line.end.set(10);

    if (UNIT_TEST("Sphere and Endpoint of Line.", Collisions::LineAndSphere(line, sphere), 1)) { return 1; }

    // test 5
    line.start.set(2);

    if (UNIT_TEST("Sphere and not Line, but would be if the line was infinite.", Collisions::LineAndSphere(line, sphere), 0)) { return 1; }

    // test 6
    sphere.r = 5;
    sphere.rb.pos.set(-5, 0, 0);
    line.start.set(-6, 0, 0);
    line.end.set(-4, 0, 0);

    if (UNIT_TEST("Sphere and Contained Line.", Collisions::LineAndSphere(line, sphere), 1)) { return 1; }

    return 0;
};

bool testLineAndAABB() {
    // test 1
    Primitives::AABB aabb1(ZMath::Vec3D(-1), ZMath::Vec3D(1));
    Primitives::Line3D line(ZMath::Vec3D(-2), ZMath::Vec3D(1.5f, 1.2f, 1));

    if (UNIT_TEST("Line and AABB.", Collisions::LineAndAABB(line, aabb1), 1)) { return 1; }

    // test 2
    Primitives::AABB aabb2(ZMath::Vec3D(-4), ZMath::Vec3D(-2));

    if (UNIT_TEST("Line and Vertex of AABB.", Collisions::LineAndAABB(line, aabb2), 1)) { return 1; }

    // test 3
    line.start.set(-0.2f, -0.3f, -9);
    line.end.set(-0.2f, -0.3f, 120);

    if (UNIT_TEST("AABB and Vertical Line.", Collisions::LineAndAABB(line, aabb1), 1)) { return 1; }

    // test 4
    line.start.set(-0.5f);

    if (UNIT_TEST("AABB and Line Starting in AABB.", Collisions::LineAndAABB(line, aabb1), 1)) { return 1; }

    // test 5
    line.start.set(8, 9, 10);

    if (UNIT_TEST("Line and not AABB.", Collisions::LineAndAABB(line, aabb2), 0)) { return 1; }

    // test 6
    line.start.set(2);
    line.end.set(-0.1f);

    if (UNIT_TEST("Line and not AABB, but would be if the line was infinite.", Collisions::LineAndAABB(line, aabb2), 0)) { return 1; }

    // test 7
    line.start.set(-0.5f);
    line.end.set(0.5f);

    if (UNIT_TEST("Line inside AABB.", Collisions::LineAndAABB(line, aabb1), 1)) { return 1; }

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

bool testSphereAndSphere() {
    // test 1
    Primitives::Sphere sphere1(3.5f, ZMath::Vec3D());
    Primitives::Sphere sphere2(2, ZMath::Vec3D(1, 0, 0));

    if (UNIT_TEST("Sphere and Sphere.", Collisions::SphereAndSphere(sphere1, sphere2), 1)) { return 1; }

    // test 2
    sphere1.r = 1;
    sphere2.r = 1;
    sphere2.rb.pos.set(2, 0, 0);

    if (UNIT_TEST("Sphere and Sphere on Circumference.", Collisions::SphereAndSphere(sphere1, sphere2), 1)) { return 1; }

    // test 3
    sphere1.r = 0.5f;

    if (UNIT_TEST("Sphere and not Sphere.", Collisions::SphereAndSphere(sphere1, sphere2), 0)) { return 1; }

    return 0;
};

bool testSphereAndAABB() {
    // test 1
    Primitives::Sphere sphere(2.0f, ZMath::Vec3D());
    Primitives::AABB aabb1(ZMath::Vec3D(-4), ZMath::Vec3D(4));

    if (UNIT_TEST("Sphere and AABB.", Collisions::SphereAndAABB(sphere, aabb1), 1)) { return 1; }

    // test 2
    Primitives::AABB aabb2(ZMath::Vec3D(2, 0, 0), ZMath::Vec3D(5));

    if (UNIT_TEST("AABB and Sphere circumference.", Collisions::SphereAndAABB(sphere, aabb2), 1)) { return 1; }

    // test 3
    Primitives::AABB aabb3(ZMath::Vec3D(-1), ZMath::Vec3D(1));
    sphere.r = 5.2f;

    if (UNIT_TEST("AABB inside Sphere.", Collisions::SphereAndAABB(sphere, aabb3), 1)) { return 1; }

    // test 4
    sphere.r = 0.2f;
    
    if (UNIT_TEST("Sphere inside AABB.", Collisions::SphereAndAABB(sphere, aabb3), 1)) { return 1; }

    // test 5
    sphere.rb.pos.set(-7);
    sphere.r = 2.2f;

    if (UNIT_TEST("Not Sphere and AABB.", Collisions::SphereAndAABB(sphere, aabb3), 0)) { return 1; }

    return 0;
};

bool testSphereAndCube() {
    // test 1
    Primitives::Sphere sphere(4.0f, ZMath::Vec3D());
    Primitives::Cube cube1(ZMath::Vec3D(1), ZMath::Vec3D(6), 45.0f, 30.0f);

    if (UNIT_TEST("Sphere and Cube.", Collisions::SphereAndCube(sphere, cube1), 1)) { return 1; }

    // test 2
    Primitives::Cube cube2(ZMath::Vec3D(-1), ZMath::Vec3D(1), 45.0f, 45.0f);
    sphere.r = 1.56f;

    if (UNIT_TEST("Sphere and Vertex of Cube.", Collisions::SphereAndCube(sphere, cube2), 1)) { return 1; }

    // test 3
    sphere.r = 20;

    if (UNIT_TEST("Cube inside Sphere.", Collisions::SphereAndCube(sphere, cube2), 1)) { return 1; }

    // test 4
    Primitives::Cube cube3(ZMath::Vec3D(-4.5f), ZMath::Vec3D(4.5f), 20, 20);
    sphere.r = 1;

    if (UNIT_TEST("Sphere inside Cube.", Collisions::SphereAndCube(sphere, cube3), 1)) { return 1; }

    // test 5
    sphere.rb.pos.set(10, 3, 2);

    if (UNIT_TEST("Not Sphere and Cube.", Collisions::SphereAndCube(sphere, cube2), 0)) { return 1; }

    return 0;
};

bool testAABBAndAABB() {
    // test 1
    Primitives::AABB aabb1(ZMath::Vec3D(-3), ZMath::Vec3D(4, 5, 6)), aabb2(ZMath::Vec3D(-5), ZMath::Vec3D(-2));

    if (UNIT_TEST("AABB and AABB.", Collisions::AABBAndAABB(aabb1, aabb2), 1)) { return 1; }

    // test 2
    Primitives::AABB aabb3(ZMath::Vec3D(), ZMath::Vec3D(1)), aabb4(ZMath::Vec3D(-1.5f), ZMath::Vec3D(3.24567f));

    if (UNIT_TEST("AABB inside of AABB.", Collisions::AABBAndAABB(aabb3, aabb4), 1)) { return 1; }

    // test 3
    Primitives::AABB aabb5(ZMath::Vec3D(1, 1, 0), ZMath::Vec3D(2, 2, 1));

    if (UNIT_TEST("AABB and edge of AABB.", Collisions::AABBAndAABB(aabb3, aabb5), 1)) { return 1; }

    // test 4
    if (UNIT_TEST("Not AABB and AABB.", Collisions::AABBAndAABB(aabb3, aabb2), 0)) { return 1; }

    return 0;
};

bool testAABBAndCube() {
    Primitives::AABB aabb1(ZMath::Vec3D(-2), ZMath::Vec3D(2));
    Primitives::Cube cube1(ZMath::Vec3D(-1), ZMath::Vec3D(4), 32.0f, 78.0f);

    if (UNIT_TEST("AABB and Cube.", Collisions::AABBAndCube(aabb1, cube1), 1)) { return 1; }

    Primitives::AABB aabb2(ZMath::Vec3D(-6), ZMath::Vec3D(6));
    Primitives::Cube cube2(ZMath::Vec3D(-2), ZMath::Vec3D(2), 45, 45);

    if (UNIT_TEST("Cube inside AABB.", Collisions::AABBAndCube(aabb2, cube2), 1)) { return 1; }

    Primitives::AABB aabb3(ZMath::Vec3D(-0.2f), ZMath::Vec3D(0.2f));

    if (UNIT_TEST("AABB inside Cube.", Collisions::AABBAndCube(aabb3, cube2), 1)) { return 1; }

    Primitives::AABB aabb4(ZMath::Vec3D(), ZMath::Vec3D(2));
    Primitives::Cube cube3(ZMath::Vec3D(-7), ZMath::Vec3D(-4), 10, 123.4);

    if (UNIT_TEST("Not AABB and Cube.", Collisions::AABBAndCube(aabb4, cube3), 0)) { return 1; }

    return 0;
};

bool testCubeAndCube() {
    return 0;
};

int main() {
    if (testCases("PointAndLine", &testPointAndLine)) { return 1; }
    if (testCases("PointAndPlane", &testPointAndPlane)) { return 1; }
    if (testCases("PointAndSphere", &testPointAndSphere)) { return 1; }
    if (testCases("PointAndAABB", &testPointAndAABB)) { return 1; }
    if (testCases("PointAndCube", &testPointAndCube)) { return 1; }
    if (testCases("LineAndLine", &testLineAndLine)) { return 1; }
    if (testCases("LineAndPlane", &testLineAndPlane)) { return 1; }
    if (testCases("LineAndSphere", &testLineAndSphere)) { return 1; }
    if (testCases("LineAndAABB", &testLineAndAABB)) { return 1; }
    if (testCases("LineAndCube", &testLineAndCube)) { return 1; }
    if (testCases("RaycastingVSPlane", &testRaycastingVSPlane)) { return 1; }
    if (testCases("RaycastingVSSphere", &testRaycastingVSSphere)) { return 1; }
    if (testCases("RaycastingVSAABB", &testRaycastingVSAABB)) { return 1; }
    if (testCases("RaycastingVSCube", &testRaycastingVSCube)) { return 1; }
    if (testCases("PlaneAndPlane", &testPlaneAndPlane)) { return 1; }
    if (testCases("PlaneAndSphere", &testPlaneAndSphere)) { return 1; }
    if (testCases("PlaneAndAABB", &testPlaneAndAABB)) { return 1; }
    if (testCases("PlaneAndCube", &testPlaneAndCube)) { return 1; }
    if (testCases("SphereAndSphere", &testSphereAndSphere)) { return 1; }
    if (testCases("SphereAndAABB", &testSphereAndAABB)) { return 1; }
    if (testCases("SphereAndCube", &testSphereAndCube)) { return 1; }
    if (testCases("AABBAndAABB", &testAABBAndAABB)) { return 1; }
    if (testCases("AABBAndCube", &testAABBAndCube)) { return 1; }
    if (testCases("CubeAndCube", &testCubeAndCube)) { return 1; }
    return 0;
};
