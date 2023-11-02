#pragma once

#include <stdexcept>
// todo probs change this to a different header
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

    // todo will probs need to increase the size of the ints and uints used to allow for a greater number of objects in Zeta

    // Data structure used for 3D spatial partitioning.
    // This Octree only stores indices.
    // It is expected for you to store the list of objects where you use this Octree.
    class Octree {
        private:
            struct Node {
                // ? We can store firstChild as a uint16_t since the next index in element stores a 32bit int

                // index of the first child if this is a node or the first element if this is a leaf.
                uint16_t firstChild;

                // Stores the number of elements in the leaf or -1 if this node is not a leaf.
                int32_t count;
            };

            struct ElementNode {
                // Points to the next element in the leaf node.
                // -1 indicates the end of the list
                int32_t next;

                // stores the element index
                uint32_t element;
            };

            // grow function for ease of use
            inline void grow() {
                capacity *= 2;
                Node* temp = new Node[capacity];

                for (int i = 0; i < count; ++i) { temp[i] = std::move(nodes[i]); }

                delete[] nodes;
                nodes = temp;
            };


        public:
            // todo we'll need an easy way to update the tree after a physics object moves into a different portion
            // todo probs just do that directly in the physicshandler with the remove and insert functions

            // Stores every element in the octree.
            // Elements are stored as the indices of each index in the original list of elements.
            FreeList<ElementNode> elmNodes;

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

            // todo initialize the root node

            /**
             * @brief Construct a new Octree object
             * 
             * @param min The minimum vertex of the region encompassed by the octree.
             * @param max The maximum vertex of the region encompassed by the octree.
             * @param maxElementCapacity The maximum number of elements allowed at each leaf node. Default of 16.
             * @param maxDepth The maximum depth of the octree allowed. Default of 8.
             * @param nodeCap The initial capacity of the node array. Must be a multiple of 8. Default of 32.
             */
            Octree(ZMath::Vec3D const &min, ZMath::Vec3D const &max, int maxElementCapacity = OCT_MAX_CAPACITY,
                    int maxDepth = OCT_MAX_DEPTH, int nodeCap = 32) : maxDepth(maxDepth), maxElementCapacity(maxElementCapacity)
            {
                if (nodeCap&7) { throw std::runtime_error("NodeCap must be a multiple of 8."); }

                bounds = std::move(AABB(min, max));
                capacity = nodeCap;
                count = 0;
                nodes = new Node[capacity];
            };

            /**
             * @brief Construct a new Octree object
             * 
             * @param aabb An AABB representing the region encompassed by the octree.
             * @param maxElementCapacity The maximum number of elements allowed at each leaf node. Default of 16.
             * @param maxDepth The maximum depth of the octree allowed. Default of 8.
             * @param nodeCap The initial capacity of the node array. Must be a multiple of 8. Default of 32.
             */
            Octree(AABB const &aabb, int maxElementCapacity = OCT_MAX_CAPACITY, int maxDepth = OCT_MAX_DEPTH, int nodeCap = 32) {
                if (nodeCap&7) { throw std::runtime_error("NodeCap must be a multiple of 8."); }

                bounds = aabb;
                capacity = nodeCap;
                count = 0;
                nodes = new Node[capacity];

                this->maxDepth = maxDepth;
                this->maxElementCapacity = maxElementCapacity;
            };

            /**
             * @brief Construct a new Octree object
             * 
             * @param aabb An AABB representing the region encompassed by the octree.
             * @param maxElementCapacity The maximum number of elements allowed at each leaf node. Default of 16.
             * @param maxDepth The maximum depth of the octree allowed. Default of 8.
             * @param nodeCap The initial capacity of the node array. Must be a multiple of 8. Default of 32.
             */
            Octree(AABB &&aabb, int maxElementCapacity = OCT_MAX_CAPACITY, int maxDepth = OCT_MAX_DEPTH, int nodeCap = 32) {
                if (nodeCap&7) { throw std::runtime_error("NodeCap must be a multiple of 8."); }
                
                bounds = std::move(aabb);
                capacity = nodeCap;
                count = 0;
                nodes = new Node[capacity];

                this->maxDepth = maxDepth;
                this->maxElementCapacity = maxElementCapacity;
            };


            // * ===================
            // * Rule of 5 Stuff
            // * ===================

            inline Octree(Octree const &tree) {
                capacity = tree.capacity;
                count = tree.count;
                freeNode = tree.freeNode;
                maxDepth = tree.maxDepth;
                maxElementCapacity = tree.maxElementCapacity;

                bounds = tree.bounds;

                nodes = new Node[capacity];
                for (int i = 0; i < count; ++i) { nodes[i] = tree.nodes[i]; }

                // elements = tree.elements;
                elmNodes = tree.elmNodes;
            };

            inline Octree(Octree &&tree) {
                nodes = tree.nodes;
                capacity = tree.capacity;
                count = tree.count;
                freeNode = tree.freeNode;
                maxDepth = tree.maxDepth;
                maxElementCapacity = tree.maxElementCapacity;

                bounds = std::move(tree.bounds);

                // elements = std::move(tree.elements);
                elmNodes = std::move(tree.elmNodes);

                tree.nodes = nullptr;
            };

            inline Octree& operator = (Octree const &tree) {
                if (this != &tree) {
                    if (nodes) { delete[] nodes; }

                    capacity = tree.capacity;
                    count = tree.count;
                    freeNode = tree.freeNode;
                    maxDepth = tree.maxDepth;
                    maxElementCapacity = tree.maxElementCapacity;

                    bounds = tree.bounds;

                    nodes = new Node[capacity];
                    for (int i = 0; i < count; ++i) { nodes[i] = tree.nodes[i]; }

                    // elements = tree.elements;
                    elmNodes = tree.elmNodes;
                }

                return *this;
            };

            inline Octree& operator = (Octree &&tree) {
                if (this != &tree) {
                    if (nodes) { delete[] nodes; }

                    nodes = tree.nodes;
                    capacity = tree.capacity;
                    count = tree.count;
                    freeNode = tree.freeNode;
                    maxDepth = tree.maxDepth;
                    maxElementCapacity = tree.maxElementCapacity;

                    bounds = std::move(tree.bounds);

                    // elements = std::move(tree.elements);
                    elmNodes = std::move(tree.elmNodes);

                    tree.nodes = nullptr;
                }

                return *this;
            };

            inline ~Octree() { delete[] nodes; };


            // * ===================
            // * Normal Functions
            // * ===================

            bool contains(ZMath::Vec3D const &point, uint32_t index) const {
                // ? We go through until we find the region the point would be in.
                // ? We then check if that point is within the region.

                int region = 0; // start with the root node
                ZMath::Vec3D center = bounds.pos; // centerpoint of the region

                for (;;) {
                    if (nodes[region].count == -1) { // not a leaf node
                        // * Find the next region to search

                        if (point.x < center.x && point.y < center.y && point.z < center.z) { // octant 1
                            region = nodes[region].firstChild;
                            center *= 0.5f;

                        } else if (point.x < center.x && point.y >= center.y && point.z < center.z) { // octant 2
                            region = nodes[region].firstChild + 1;
                            center.x *= 0.5f;
                            center.y *= 1.5f;
                            center.z *= 0.5f;

                        } else if (point.x < center.x && point.y < center.y && point.z >= center.z) { // octant 3
                            region = nodes[region].firstChild + 2;
                            center.x *= 0.5f;
                            center.y *= 0.5f;
                            center.z *= 1.5f;

                        } else if (point.x < center.x && point.y >= center.y && point.z >= center.z) { // octant 4
                            region = nodes[region].firstChild + 3;
                            center.x *= 0.5f;
                            center.y *= 1.5f;
                            center.z *= 1.5f;

                        } else if (point.x >= center.x && point.y < center.y && point.z < center.z) { // octant 5
                            region = nodes[region].firstChild + 4;
                            center.x *= 1.5f;
                            center.y *= 0.5f;
                            center.z *= 0.5f;

                        } else if (point.x >= center.x && point.y >= center.y && point.z < center.z) { // octant 6
                            region = nodes[region].firstChild + 5;
                            center.x *= 1.5f;
                            center.y *= 1.5f;
                            center.z *= 0.5f;

                        } else if (point.x >= center.x && point.y < center.y && point.z >= center.z) { // octant 7
                            region = nodes[region].firstChild + 6;
                            center.x *= 1.5f;
                            center.y *= 0.5f;
                            center.z *= 1.5f;

                        } else { // octant 8
                            region = nodes[region].firstChild + 7;
                            center *= 1.5f;
                        }

                    } else { // leaf node
                        // * Since this is a leaf node, we begin our search for the match
                        
                        // traverse the singly linked list
                        for (int32_t i = nodes[region].firstChild; i != -1; i = elmNodes[i].next) {
                            if (elmNodes[i].element == index) { return 1; } // we've found our match
                        }

                        return 0; // there is no possible match if this point is reached
                    }
                }

                // just for the compiler
                // the code will never reach this point
                return 0;
            };


            // todo factor in the free node

            // Insert the given point into the octree.
            // Returns the index of the element inserted.
            int insert(ZMath::Vec3D const &point, uint32_t index) {
                // ? We are guarenteed to have at least the root node by construction.
                // ? We will then recursively find the leaf node the point is in.

                int region = 0; // start with the root node
                int depth = 1; // track the depth
                ZMath::Vec3D center = bounds.pos; // store the centerpoint of the region

                for (;;++depth) {
                    if (nodes[region].count == -1) { // not a leaf node
                        if (point.x < center.x && point.y < center.y && point.z < center.z) { // octant 1
                            region = nodes[region].firstChild;
                            center *= 0.5f;

                        } else if (point.x < center.x && point.y >= center.y && point.z < center.z) { // octant 2
                            region = nodes[region].firstChild + 1;
                            center.x *= 0.5f;
                            center.y *= 1.5f;
                            center.z *= 0.5f;

                        } else if (point.x < center.x && point.y < center.y && point.z >= center.z) { // octant 3
                            region = nodes[region].firstChild + 2;
                            center.x *= 0.5f;
                            center.y *= 0.5f;
                            center.z *= 1.5f;

                        } else if (point.x < center.x && point.y >= center.y && point.z >= center.z) { // octant 4
                            region = nodes[region].firstChild + 3;
                            center.x *= 0.5f;
                            center.y *= 1.5f;
                            center.z *= 1.5f;

                        } else if (point.x >= center.x && point.y < center.y && point.z < center.z) { // octant 5
                            region = nodes[region].firstChild + 4;
                            center.x *= 1.5f;
                            center.y *= 0.5f;
                            center.z *= 0.5f;

                        } else if (point.x >= center.x && point.y >= center.y && point.z < center.z) { // octant 6
                            region = nodes[region].firstChild + 5;
                            center.x *= 1.5f;
                            center.y *= 1.5f;
                            center.z *= 0.5f;

                        } else if (point.x >= center.x && point.y < center.y && point.z >= center.z) { // octant 7
                            region = nodes[region].firstChild + 6;
                            center.x *= 1.5f;
                            center.y *= 0.5f;
                            center.z *= 1.5f;

                        } else { // octant 8
                            region = nodes[region].firstChild + 7;
                            center *= 1.5f;
                        }

                    } else { // leaf node
                        // check if we should expand the tree
                        if (nodes[region].count >= maxElementCapacity && depth < maxDepth) {
                            // add the 8 new nodes
                            if (count >= capacity) { grow(); }

                            // todo figure out how to split up all of the rbs in the region when it splits
                            // todo adjust the firstChild and count values for each of the 8 nodes added

                            nodes[count++];
                            nodes[count++];
                            nodes[count++];
                            nodes[count++];
                            nodes[count++];
                            nodes[count++];
                            nodes[count++];
                            nodes[count++];

                            // todo after figuring out how to split up the region, see if we need the count-8 for nodes[region].firstChild

                            nodes[region].count = -1;
                            nodes[region].firstChild = count - 8;
                        }

                        // todo make sure this is right

                        elmNodes.insert({-1, index});
                        ++(nodes[region].count);
                    }
                }
            };

            void remove(ZMath::Vec3D const &point);

            // Clear the octree.
            inline void clear() {
                delete[] nodes;
                capacity = 32;
                count = 0;
                nodes = new Node[capacity];
                freeNode = -1;
                // elements.clear();
                elmNodes.clear();
            };

            // Determine if the octree is empty.
            inline bool empty() const { return !count; };
    };
}
