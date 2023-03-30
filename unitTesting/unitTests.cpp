#include <iostream>
#include "../include/intersections.h"

// Macro for unit testing.
#define UNIT_TEST(test, obtained, expected) \
({ \
    if (obtained == expected) { \
        std::cout << "[PASSED] " << test << "\n"; \
 \
    } else { \
        std::cout << "[FAILED] " << test << "\nExpected: " << expected << ". Obtained: " << obtained << "."; \
    } \
\
    obtained != expected; \
})


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

    // Line vs Line
    std::cout << "================== LineAndLine Tests. ==================\n\n";

    if (testLineAndLine()) {
        std::cout << "\n================ [FAILED] LineAndLine. ================\n\n";
        return 1;
    }

    std::cout << "\n================ [PASSED] LineAndLine. ================\n\n";

    return 0;
};
