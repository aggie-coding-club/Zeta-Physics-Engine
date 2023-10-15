# **Zeta-Physics-Engine**

<img src="ZetaLogo.png" width="400" height="400"/>

___

## About
* 3D header-only mechanics physics engine.
* Created as an easy-to-use yet efficient physics engine for any 3D project.

___

## To Use
* Clone the Zeta repository.
* Copy the include/zeta folder into your include directory.
* `#include <zeta/physicshandler.h` in the file you want to use Zeta in.

___

## Getting Started
The following code snippet provides an example of how to get Zeta up and running in your program.

```c++
#include <ZETA/physicshandler.h>

int main() {
    // Create a physics handler with the default settings
    Zeta::Handler handler;

    // Create the colliders
    Zeta::Sphere* s1 = new Zeta::Sphere(ZMath::Vec3D(100.0f, 120.0f, 100.0f), 50.0f);
    Zeta::Sphere* s2 = new Zeta::Sphere(ZMath::Vec3D(350.0f, 500.0f, -340.0f), 200.0f);

    // Create some rigid bodies
    Zeta::RigidBody3D rb1(
        s1->c,                       // centerpoint
        50.0f,                       // mass
        0.9f,                        // coefficient of restitution
        0.975f,                      // linear damping
        Zeta::RIGID_SPHERE_COLLIDER, // collider type
        s1                           // collider
    );

    Zeta::RigidBody3D rb2(
        s2->c,                       // centerpoint
        100.0f,                      // mass
        0.95f,                       // coefficient of restitution
        0.8f,                        // linear damping
        Zeta::RIGID_SPHERE_COLLIDER, // collider type
        s2                           // collider
    );

    // Add the rigid bodies to the handler
    handler.addRigidBody(&rb1);
    handler.addRigidBody(&rb2);

    // Program's dt loop
    float dt = 0.0f;

    // Note: windowShouldNotClose would be replaced with the exit window condition in the user's graphics library
    while (windowShouldNotClose) {
        /* Rendering/Drawing code would go here */

        handler.update(dt); // The handler subtracts from dt for you
        // Note: getEllapsedTime() would be replaced with the equivalent in the user's graphics library
        dt += getEllapsedTime();
    }

    return 0;
};
```

___

## Resources for Contributing
* If you want to get started contributing to Zeta, check out our [resources page](https://github.com/Salamence064/Zeta-Resources) for some important information

___

## Bug Reports
* To report a bug open an issue detailing the bug with relevant screenshots and code snippets and tag it with the "bug" tag.

___

## A 2D Version
* [Zeta2D](https://github.com/Salamence064/Zeta2D) is a 2D version of this physics engine.
* Check it out if you're working on a 2D project.

___

## Planned Additions
* Kinematic Bodies
* Rotational Kinematics
* Add source files and move the header-only version to a branch
* Arbitrary Polyhedrons

___

## Contributors
 * Utsawb Lamichhane
 * Thomas Ducote
 * Lenny Tanui
 * Thomas Masha
 * Hailey Trinh
 * Franco Martinez
 * Adrian Cortes
 * Sai Sudarshan Barath
 * Colin Nerren
 * Junyi Wu
 * Andrey Pridgen
