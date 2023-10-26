#pragma once

#include "intersections.h"

// todo determine an ideal value -- for now will have 64 as a placeholder
#define OCT_MAX_CAPACITY 64


// todo probably store the objects with a key
// todo will update to do so later
// todo probs will need to store ids for rigid bodies, static bodies, and kinematic bodies to ensure unique keys

namespace Zeta {
    // Data structure used for 3D partitioning
    template <typename T>
    class Octree {
        private:
            // Represent a node of the octree.
            // Check to ensure the count would not exceed the max capacity before adding.
            // To add an object, do objects[count++] = <object>;
            struct Node {
                Node* nodes[8] = { nullptr };
                AABB region;
                T objects[64];
                int count = 0;

                Node(ZMath::Vec3D const &min, ZMath::Vec3D const &max);
                Node(ZMath::Vec3D &&min, ZMath::Vec3D &&max);
                Node(AABB const &aabb);
                Node(AABB &&aabb);
            };

            Node* root;

        public:
            Octree(ZMath::Vec3D const &min, ZMath::Vec3D const &max);
            Octree(ZMath::Vec3D &&min, ZMath::Vec3D &&max);


            // * ===================
            // * Rule of 5 Stuff
            // * ===================

            Octree(Octree const &tree);
            Octree(Octree &&tree);

            Octree& operator = (Octree const &tree);
            Octree& operator = (Octree &&tree);

            ~Octree();


            // * ===================
            // * Normal Functions
            // * ===================

            bool contains();
            bool empty();

            T find();

            void clear();
            void insert();
            void remove();
    };
}
