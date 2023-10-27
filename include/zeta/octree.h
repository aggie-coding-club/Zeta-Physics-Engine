#pragma once

#include "intersections.h"

// todo determine an ideal value -- for now will have 16 as a placeholder
// todo maybe whip up a quick OpenGL thing that I can use to stress test the engine
// todo probs make a parameter
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
    // Array allowing removal of elements from anywhere with O(1) without invalidating indices.
    // This can only be used for datatypes that are trivially constructible and destructible.
    template <typename T>
    class FreeList {
        private:
            union FreeElement {
                T element;
                int next;
            };

            FreeElement* data;
            int capacity;
            int count;

            int freeFirst;

            // Helper grow function for ease of use.
            inline void grow() {
                capacity *= 2;
                FreeElement* temp = new FreeElement[capacity];

                for (int i = 0; i < count; ++i) { temp[i] = std::move(data[i]); }

                delete[] data;
                data = temp;
            };

        public:
            // By default, allocate 16 data slots.
            // Cap must be strictly greater than 0.
            inline FreeList(int cap = 16) : data(new FreeElement[cap]), capacity(cap), count(0), freeFirst(-1) {
                static_assert(cap > 0, "The capacity must be strictly greater than 0.");
            };


            // * ==================
            // * Rule of 5 Stuff
            // * ==================

            inline FreeList(FreeList const &list) {
                capacity = list.capacity;
                count = list.count;
                freeFirst = list.freeFirst;

                data = new FreeElement[capacity];
                for (int i = 0; i < count; ++i) { data[i] = list.data[i]; }
            };

            inline FreeList(FreeList &&list) {
                data = list.data;
                capacity = list.capacity;
                count = list.count;
                freeFirst = list.freeFirst;
                list.data = nullptr;
            };
            
            inline FreeList& operator = (FreeList const &list) {
                if (this != &list) {
                    if (data) { delete[] data; }

                    capacity = list.capacity;
                    count = list.count;
                    freeFirst = list.freeFirst;

                    data = new FreeElement[capacity];
                    for (int i = 0; i < count; ++i) { data[i] = list.data[i]; }
                }

                return *this;
            };

            inline FreeList& operator = (FreeList &&list) {
                if (this != &list) {
                    if (data) { delete[] data; }

                    data = list.data;
                    capacity = list.capacity;
                    count = list.count;
                    freeFirst = list.freeFirst;
                    list.data = nullptr;
                }

                return *this;
            };

            inline ~FreeList() { delete[] data; };


            // * ===================
            // * Normal Functions
            // * ===================

            // Insert an element to the list.
            // Returns the index of the element inserted.
            inline int insert(T const &element) {
                if (freeFirst != -1) {
                    int index = freeFirst;
                    freeFirst = data[freeFirst].next;
                    data[index].element = element;
                    return index;
                }

                if (count == capacity) { grow(); }
                data[count].element = element;
                return count++;
            };

            // Insert an element to the list.
            // Returns the index of the element inserted.
            inline int insert(T &&element) {
                if (freeFirst != -1) {
                    int index = freeFirst;
                    freeFirst = data[freeFirst].next;
                    data[index].element = std::move(element);
                    return index;
                }

                if (count == capacity) { grow(); }
                data[count].element = std::move(element);
                return count++;
            };

            // Remove the nth element from the list.
            inline void remove(int n) {
                data[n].next = freeFirst;
                freeFirst = n;
            };

            // Clear all elements from the list.
            inline void clear() {
                delete[] data;

                capacity = 16;
                count = 0;
                data = new FreeElement[capacity];
                freeFirst = -1;
            };

            // Returns the range of valid indices.
            inline int range() const { return count; };

            // Returns the nth element.
            inline T& operator[] (int n) const { return data[n].element; };
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
            int freeNode = -1;

            // Maximum number of elements allowed at each leaf node.
            int maxElementCapacity;

            // Maximum depth of the octree allowed.
            int maxDepth;


            // * ===============
            // * Constructors
            // * ===============

            /**
             * @brief Construct a new Octree object
             * 
             * @param min 
             * @param max 
             * @param maxElementCapacity 
             * @param maxDepth 
             * @param nodeCap 
             */
            Octree(ZMath::Vec3D const &min, ZMath::Vec3D const &max, int maxElementCapacity = OCT_MAX_CAPACITY,
                    int maxDepth = OCT_MAX_DEPTH, int nodeCap = 32);

            /**
             * @brief Construct a new Octree object
             * 
             * @param min 
             * @param max 
             * @param maxElementCapacity 
             * @param maxDepth 
             * @param nodeCap 
             */
            Octree(ZMath::Vec3D &&min, ZMath::Vec3D &&max, int maxElementCapacity = OCT_MAX_CAPACITY,
                    int maxDepth = OCT_MAX_DEPTH, int nodeCap = 32);

            /**
             * @brief Construct a new Octree object
             * 
             * @param aabb 
             * @param maxElementCapacity 
             * @param maxDepth 
             * @param nodeCap 
             */
            Octree(AABB const &aabb, int maxElementCapacity = OCT_MAX_CAPACITY, int maxDepth = OCT_MAX_DEPTH, int nodeCap = 32);

            /**
             * @brief Construct a new Octree object
             * 
             * @param aabb 
             * @param maxElementCapacity 
             * @param maxDepth 
             * @param nodeCap 
             */
            Octree(AABB &&aabb, int maxElementCapacity = OCT_MAX_CAPACITY, int maxDepth = OCT_MAX_DEPTH, int nodeCap = 32);


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

            // todo I think we will probs need the id's of the elements

            bool contains(ZMath::Vec3D const &point) const;
            int find(ZMath::Vec3D const &point) const;
            int insert(ZMath::Vec3D const &point);
            void remove(ZMath::Vec3D const &point);
            void clear();
            bool empty() const;
    };
}
