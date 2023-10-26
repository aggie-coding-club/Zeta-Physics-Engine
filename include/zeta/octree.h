#pragma once

#include "intersections.h"

// todo determine an ideal value -- for now will have 16 as a placeholder
// todo maybe whip up a quick OpenGL thing that I can use to stress test the engine
#define OCT_MAX_CAPACITY 16

// todo test with a benchmark OpenGL program to determine an ideal depth
#define OCT_MAX_DEPTH 8

// todo test to see if caching the AABBs or computing them on the fly is faster when scenes are developed
// todo computing on the fly may only be faster due to cahce misses when traversing the tree
// todo but I feel computing AABBs that use floats will take longer as we can't use out bitwise operators

// todo store children so that they are contiguous in memory
// firstChild == index to first child
// firstChild + 1 == index to second child
// ...

// todo deferred cleanup and constant time removal
// todo look through the stack overflow post about this
// todo apparently this makes moving objects simple, too


namespace Zeta {
    template <typename T>
    class FreeList {

    };

    // Data structure used for 3D spatial partitioning.
    // This Octree only stores indices.
    // It is expected for you to store the list of objects where you use this Octree.
    class Octree {
        private:
            struct Node {
                // index of the first child if this is a node or the first element if this is a leaf.
                uint16_t firstChild;

                // Stores the number of elements in the leaf or -1 if this node is not a leaf.
                int32_t count;

                // // the region the node encompasses
                // AABB aabb;
            };

            struct Element {
                // Stores the element's ID
                int id;

                // Stores the AABB for the element
                ZMath::Vec3D min, max;
            };

            struct ElementNode {
                // Points to the next element in the leaf node.
                // -1 indicates the end of the list
                int next;

                // stores the element index
                int element;
            };


        public:
            FreeList<Element> elements; // Stores every element in the octree.
            FreeList<ElementNode> elmNodes; // Stores every element node in the octree.

            // Stores every node in the octree.
            // The first node will always be the root.
            Node* nodes;
            int capacity;
            int count;

            // The bounds of the entire octree.
            AABB bounds;

            // ? For free node, we store the index for the first freed node in the octree.
            // ? This allows us to implement a deferred cleanup approach, which reduces redundant operations.
            // ? We will free nodes of the octree as 8 contiguous nodes at once.
            // ? Whenever freeNode is -1, we will just insert 8 nodes to the back of the array.

            // Stores the first free node in the octree.
            // -1 indicates that the free list is empty.
            int freeNode;
    };
}
