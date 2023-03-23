#include <iostream>
#include "../include/intersections.h"

bool testPointAndLine() {
    // test 1
    ZMath::Vec3D point(-1, 0, -1);
    Primitives::Line3D line(ZMath::Vec3D(2, -4, -3), ZMath::Vec3D(-4, 4, 1));

    if(!Primitives::PointAndLine(point, line)) {
        std::cout << "[FAILED] Point on Line.\nExpected: true. Obtained: false." << std::endl;
        return 0;
    }

    std::cout << "[PASSED] Point on Line.\n";

    // test 2
    line.start = ZMath::Vec3D(-2.2f, -2, 0);
    line.end = ZMath::Vec3D(-2.2f, 3, 0);
    point.set(-2.2f, 2.5f, 0);

    if (!Primitives::PointAndLine(point, line)) {
        std::cout << "[FAILED] Point on Horizontal Line.\nExpected: true. Obtained: false." << std::endl;
        return 0;
    }

    std::cout << "[PASSED] Point on Horizontal Line.\n";

    // test 3
    line.start = ZMath::Vec3D(3, -2, -5);
    line.end = ZMath::Vec3D(3, -2, 5);
    point.set(3, 2, 2.5f);
    
    if (!Primitives::PointAndLine(point, line)) {
        std::cout << "[FAILED] Point on Vertical Line.\nExpected: true. Obtained: false." << std::endl;
        return 0;
    }

    std::cout << "[PASSED] Point on Vertical Line.\n";

    // test 3
    line.start = ZMath::Vec3D(-1.2f, 9.8f, -204.5f);
    point.set(line.end);

    if (!Primitives::PointAndLine(point, line)) {
        std::cout << "[FAILED] Point on Endpoint of Line.\nExpected: true. Obtained: false." << std::endl;
        return 0;
    }

    std::cout << "[PASSED] Point on Endpoint of Line.\n";

    // test 4
    line.start = ZMath::Vec3D(6, -2, -4);
    line.end = ZMath::Vec3D(5, 8, -6);
    point.set(4.9f, 8, -6);

    if (Primitives::PointAndLine(point, line)) {
        std::cout << "[FAILED] Point not on Line.\nExpected: false. Obtained: true." << std::endl;
        return 0;
    }

    std::cout << "[PASSED] Point not on Line.\n";

    // test 5
    line.start = ZMath::Vec3D(2, -4, -3);
    line.end = ZMath::Vec3D(-4, 4, 1);
    point.set(-10, 12, 5);

    if (Primitives::PointAndLine(point, line)) {
        std::cout << "[FAILED] Point not on Line, but would be if the line was infinite.\nExpected: false. Obtained: true." << std::endl;
        return 0;
    }

    std::cout << "[PASSED] Point not on Line, but would be if the line was infinite.\n";
    return 1;
};

int main() {
    if (!testPointAndLine()) { return 1; }
    std::cout << "[PASSED] PointAndLine.\n\n";
    return 0;
};
