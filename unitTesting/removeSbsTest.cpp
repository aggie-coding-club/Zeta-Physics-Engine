#include <iostream>

// Note this is missing for all non-glibc platforms. This will exclusively be run in an ubuntu environment as a result
#include <mcheck.h>

// cursed define directive to allow for testing on the private members of Handler
#define private public
#include <zeta/physicshandler.h>

void no_op(enum mcheck_status status) {};


// functions so that I can be lazy
inline void startTest(int num) { std::cout << "\n========================= [TEST " << num <<"] =========================\n\n"; };
inline void endTest() { std::cout << "\n============================================================\n\n"; };
inline void passedTest(const char* testName) { std::cout << "[PASSED] " << testName << ".\n"; };


// note do not add the period to the test name in here
inline bool compareSize(const char* test, int expected, int obtained) {
    if (expected != obtained) {
        std::cout << "[FAILED] " << test << ".\n\tCount does not match.\n\tExpected: "
                    << expected << "\n\tObtained: " << obtained << "\n";
        std::cout << "\n============================================================\n\n";
        return 1;
    }

    return 0;
};

// Note: this does not work perfectly and often will only be detected from the handler's destructor directly
//       although, it still prints out its own message when this is detected
inline bool checkPtrState(const char* test, Zeta::StaticBody3D** list, int size) {
    mcheck(&no_op);

    for (int i = 0; i < size; ++i) {
        if (mprobe(list[i]) == MCHECK_FREE) { // a pointer to freed memory persists
            std::cout << "[FAILED] " << test << ".\n\tHandler contains freed pointers."
                        << "\n\tBe sure you remove any pointers you free from the list.\n";
            std::cout << "\n============================================================\n\n";
            return 1;
        }
    }

    return 0;
};

inline bool checkResult(const char* test, bool expected, bool obtained) {
    if (expected != obtained) {
        std::cout << "[FAILED] " << test << ".\n\tFunction does not return expected value.\n\tExpected: "
                    << expected << "\n\tObtained: " << obtained << "\n";
        std::cout << "\n============================================================\n\n";
        return 1;
    }

    return 0;
};


int main() {
    Zeta::Handler handler;
    ZMath::Vec3D halfSize(25.0f); // half the size for all of the colliders

    // create the set of rigid bodies to test on
    int setSize = 100;
    Zeta::StaticBody3D** sbs = new Zeta::StaticBody3D*[setSize];

    for (int i = 0; i < setSize; ++i) {
        ZMath::Vec3D origin(i*50.0f, 0.0f, 100.0f);

        sbs[i] = new Zeta::StaticBody3D(
            origin,
            Zeta::STATIC_AABB_COLLIDER,
            new Zeta::AABB(origin - halfSize, origin + halfSize)
        );
    }

    handler.addStaticBodies(sbs, setSize); // add the set


    // * create removal test sets

    // set 1 will have elements of index 80-91 inclusive contained
    int size1 = 12;
    Zeta::StaticBody3D** remSbs1 = new Zeta::StaticBody3D*[size1];
    for (int i = 0; i < size1; ++i) { remSbs1[i] = sbs[i+80]; }

    // set 2 will have elements of index 0-5 and index 9-12 inclusive contained (nonsequential)
    int size2 = 10;
    Zeta::StaticBody3D** remSbs2 = new Zeta::StaticBody3D*[size2];
    for (int i = 0; i < 6; ++i) { remSbs2[i] = sbs[i]; }
    for (int i = 6; i < size2; ++i) { remSbs2[i] = sbs[i+3]; }

    // set 3 will have elements of index 77-80 inclusive and should return 0
    int size3 = 4;
    Zeta::StaticBody3D** remSbs3 = new Zeta::StaticBody3D*[size3];
    for (int i = 0; i < size3; ++i) { remSbs3[i] = sbs[i+77]; }

    // set 4 will have elements of index 23-25 inslucive with a random rb at the end (should return 0) and not delete any before it breaks
    int size4 = 4;
    Zeta::StaticBody3D** remSbs4 = new Zeta::StaticBody3D*[size4];
    for (int i = 0; i < size4-1; ++i) { remSbs4[i] = sbs[i+23]; }
    remSbs4[3] = new Zeta::StaticBody3D(
        ZMath::Vec3D(-50.0f, 0.0f, 100.0f),
        Zeta::STATIC_SPHERE_COLLIDER,
        new Zeta::Sphere(ZMath::Vec3D(-50.0f, 0.0f, 100.0f), 12.5f)
    );

    // set 5 will simply call it for index 23 which should return 1
    Zeta::StaticBody3D* sb5 = sbs[23];

    // set 6 will simply call it for index 2 which should return 0
    Zeta::StaticBody3D* sb6 = sbs[2];


    // ! ---------- Test 1 ----------

    startTest(1);

    const char* testName = "Standard Removal of 12 Sequential Elements";
    bool result = handler.removeStaticBodies(remSbs1, size1);
    setSize -= size1;

    // check to see if the list size is as expected
    if (compareSize(testName, setSize, handler.sbs.count)) { return 1; }

    // ensure there are freed pointers
    if (checkPtrState(testName, handler.sbs.staticBodies, handler.sbs.count)) { return 2; } 

    // ensure the test returned the proper value
    if (checkResult(testName, 1, result)) { return 3; }

    passedTest(testName);
    endTest();

    // ! ----------------------------


    // ! ---------- Test 2 ----------

    startTest(2);

    const char* testName2 = "Standard Removal of 10 Non-sequential Elements";
    result = handler.removeStaticBodies(remSbs2, size2);
    setSize -= size2;

    // check to see if the list size is as expected
    if (compareSize(testName2, setSize, handler.sbs.count)) { return 1; }

    // ensure there are freed pointers
    if (checkPtrState(testName2, handler.sbs.staticBodies, handler.sbs.count)) { return 2; } 

    // ensure the test returned the proper value
    if (checkResult(testName2, 1, result)) { return 3; }

    passedTest(testName2);
    endTest();

    // ! ----------------------------


    // ! ---------- Test 3 ----------

    startTest(3);

    const char* testName3 = "Sequential with a Previously Removed Element";
    result = handler.removeStaticBodies(remSbs3, size3);

    // check to see if the list size is as expected
    if (compareSize(testName3, setSize, handler.sbs.count)) { return 1; }

    // ensure there are freed pointers
    if (checkPtrState(testName3, handler.sbs.staticBodies, handler.sbs.count)) { return 2; } 

    // ensure the test returned the proper value
    if (checkResult(testName3, 0, result)) { return 3; }

    passedTest(testName3);
    endTest();

    // ! ----------------------------


    // ! ---------- Test 4 ----------

    startTest(4);

    const char* testName4 = "Non-sequential with an Element not Contained in the Handler";
    result = handler.removeStaticBodies(remSbs4, size4);

    // check to see if the list size is as expected
    if (compareSize(testName4, setSize, handler.sbs.count)) { return 1; }

    // ensure there are freed pointers
    if (checkPtrState(testName4, handler.sbs.staticBodies, handler.sbs.count)) { return 2; } 

    // ensure the test returned the proper value
    if (checkResult(testName4, 0, result)) { return 3; }

    passedTest(testName4);
    endTest();

    // ! ----------------------------


    // ! ---------- Test 5 ----------

    startTest(5);

    const char* testName5 = "Element Should be in Handler";
    result = handler.removeStaticBody(sb5);
    --setSize;

    // check to see if the list size is as expected
    if (compareSize(testName5, setSize, handler.sbs.count)) { return 1; }

    // ensure there are freed pointers
    if (checkPtrState(testName5, handler.sbs.staticBodies, handler.sbs.count)) { return 2; } 

    // ensure the test returned the proper value
    if (checkResult(testName5, 1, result)) { return 3; }

    passedTest(testName5);
    endTest();

    // ! ----------------------------


    // ! ---------- Test 6 ----------

    startTest(6);

    const char* testName6 = "Element Shouldn't be in Handler";
    result = handler.removeStaticBody(sb6);

    // check to see if the list size is as expected
    if (compareSize(testName6, setSize, handler.sbs.count)) { return 1; }

    // ensure there are freed pointers
    if (checkPtrState(testName6, handler.sbs.staticBodies, handler.sbs.count)) { return 2; } 

    // ensure the test returned the proper value
    if (checkResult(testName6, 0, result)) { return 3; }

    passedTest(testName6);
    endTest();

    // ! ----------------------------


    return 0;
};
