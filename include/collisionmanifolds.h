#ifndef COLLISION_MANIFOLDS_H
#define COLLISION_MANIFOLDS_H

// ! This is mostly just for storing notes at this point

// 1st: We need to retrieve the collision normal and the penetration distance
    // ! Note: for planes we should attempt to use raycasting for detection by normalizing the kineticbody's velocity and testing
    // ! if there's an intersection as plane's are staticbodies in our engine and are unaffected by physics.
// 2nd: We need to identify contact points as it applies in collision detection

// ! look into what broad phase culling is
// ! use an enum for the type of collision being represented by the manifold

// We will determine collision manifolds using the clipping method
    // ! This is necessary for rotations

// Colision manifold is a collection of contact points
// Surface area between the two colliding objects

// * The clipping method
// * This is known as Sutherland-Hodgman Clipping
// Step 1: Identify significant faces (I.e. those involved in the collision)
    // We first select the vertex furthest along the collision normal
    // We select a face on each object which statisfies:
        // The face includes the selected vertex
        // The face's normal is closest to parallel with the collision normal of all faces which contain the selected vertex
            // ! The normal is inverted when selected the vertex of the second object
// Step 2: Calculating the incident and reference faces
    // ? The reference face becomes a point of reference when clipping occurs in subsequent stages of the check
    // ? The incident face will become a set of vertices that will be clipped
    // We compute which of the two significant faces have a normal that is closest to parallel with that of the collision normal
    // This face becomes the reference face
    // The other face then becomes the incident face
// Step 3: Adjacent face clipping
    // Do this step for all faces adjacent to the reference face
    // We take the adjacent face's normal and any vertex it contains to produce a plane equation
    // We then check both clipping planes for incident vertices. If any are found, we move it to be on the edge where our indicent face
    //  intersects the clipping plane.
// Step 4: Final clipping
    // The final clipping plane is the plane of the reference face itself.
    // After determing this clipping plane using the same steps as in determining the adjacent face clipping, we remove all
    //  the points that lie within this clipping plane.
    // This leaves us with only our contact points.

#endif // !COLLISION_MANIFOLDS_H
