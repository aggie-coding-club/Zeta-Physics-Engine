#pragma once

#include <stdexcept>
#include <utility>
#include "primitives.h"

// todo determine an ideal value -- for now will have 16 as a placeholder
// todo maybe whip up a quick OpenGL thing that I can use to stress test the engine
// todo probs make a parameter
#define OCT_MAX_CAPACITY 16

// todo test with a benchmark OpenGL program to determine an ideal depth
#define OCT_MAX_DEPTH 8

// todo implement deferred cleanup in the octree
// todo do not have enough time to properly optimize it and figure it out currently


namespace Zeta {
    // todo update to use unit32_ts instead of int32_ts
    // Array allowing removal of elements from anywhere with O(1) without invalidating indices.
    // This can only be used for datatypes that are trivially constructible and destructible.
    template <typename T>
    class FreeList {
        private:
            union FreeElement {
                T element;
                uint32_t next;
            };

            FreeElement* data;
            uint32_t capacity;

            uint32_t freeFirst;

            // Helper grow function for ease of use.
            inline void grow() {
                capacity *= 2;
                FreeElement* temp = new FreeElement[capacity];

                for (int i = 0; i < count; ++i) { temp[i] = std::move(data[i]); }

                delete[] data;
                data = temp;
            };

        public:
            // Uint32 value corresponding to there not being a value present.
            const static uint32_t npos = -1;

            // Number of elements currently in the FreeList.
            uint32_t count;

            // By default, allocate 16 data slots.
            // Cap must be strictly greater than 0.
            inline FreeList(int cap = 16) : data(new FreeElement[cap]), capacity(cap), count(0), freeFirst(npos) {
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
                if (freeFirst != npos) {
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
                if (freeFirst != npos) {
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
                freeFirst = npos;
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
                // index of the first child if this is a node or the first element if this is a leaf.
                uint32_t firstChild;

                // Stores the number of elements in the leaf or npos16 if this node is not a leaf.
                uint16_t count;
            };


            // ? We split up Element and ElementNode as then we can insert Element nodes a singular time and let the smaller
            // ?  struct be inserted more frequently to reduce cache misses.

            struct Element {
                // The index of the element in the main list of bodies.
                uint32_t index;

                // The point corresponding to the position of the body.
                ZMath::Vec3D pos;
            };

            struct ElementNode {
                // Points to the next element in the leaf node.
                // npos32 indicates the end of the list
                uint32_t next;

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
            // Uint16 value corresponding to there not being a value present.
            const static uint16_t npos16 = -1;

            // Uint32 value corresponding to there not being a value present.
            const static uint32_t npos32 = -1;

            // Stores every ElementNode in the octree.
            // ElementNodes store an index corresponding to the element in the elements list and the next element node.
            FreeList<ElementNode> elmNodes;

            // Stores every element node in the octree.
            // Elements are stored as the indices of each index in the original list of elements and a point.
            FreeList<Element> elements;

            // Stores every node in the octree.
            // The first node will always be the root.
            Node* nodes;
            uint32_t capacity;
            uint32_t count;

            // The bounds of the entire octree.
            ZMath::Vec3D center;
            ZMath::Vec3D halfsize;

            // ? For free node, we store the index for the first freed node in the octree.
            // ? This allows us to implement a deferred cleanup approach, which reduces redundant operations.
            // ? We will free nodes of the octree as 8 contiguous nodes at once.
            // ? Whenever freeNode is -1, we will just insert 8 nodes to the back of the array.

            // // Stores the first free node in the octree.
            // // -1 indicates that the free list is empty.
            // int freeNode = -1;

            // Maximum number of elements allowed at each leaf node.
            uint16_t maxElementCapacity;

            // Maximum depth of the octree allowed.
            uint16_t maxDepth;


            // * ===============
            // * Constructors
            // * ===============

            // todo constructors don't work because it'll think the firstChild and count are weird when comparing to root node later
            // todo fix this

            /**
             * @brief Construct a new Octree object
             * 
             * @param min The minimum vertex of the region encompassed by the octree.
             * @param max The maximum vertex of the region encompassed by the octree.
             * @param maxElementCapacity The maximum number of elements allowed at each leaf node. Default of 16.
             * @param maxDepth The maximum depth of the octree allowed. Default of 8.
             * @param nodeCap The initial capacity of the node array. Must be a multiple of 8. Default of 32.
             */
            Octree(ZMath::Vec3D const &min, ZMath::Vec3D const &max, uint16_t maxElementCapacity = OCT_MAX_CAPACITY,
                    uint16_t maxDepth = OCT_MAX_DEPTH) : maxDepth(maxDepth), maxElementCapacity(maxElementCapacity)
            {
                // todo ask josh how this would affect the vectors

                center = std::move(max - min);
                halfsize = std::move(center - min);

                capacity = 17;
                count = 1;
                nodes = new Node[capacity];

                // initialize the root node
                nodes[0].firstChild = 0;
                nodes[0].count = 0;
            };

            /**
             * @brief Construct a new Octree object
             * 
             * @param aabb An AABB representing the region encompassed by the octree.
             * @param maxElementCapacity The maximum number of elements allowed at each leaf node. Default of 16.
             * @param maxDepth The maximum depth of the octree allowed. Default of 8.
             */
            Octree(AABB const &aabb, int maxElementCapacity = OCT_MAX_CAPACITY, int maxDepth = OCT_MAX_DEPTH) {
                center = aabb.pos;
                halfsize = aabb.getHalfSize();

                capacity = 17;
                count = 1;
                nodes = new Node[capacity];

                // initialize the root node
                nodes[0].firstChild = 0;
                nodes[0].count = 0;

                this->maxDepth = maxDepth;
                this->maxElementCapacity = maxElementCapacity;
            };

            /**
             * @brief Construct a new Octree object
             * 
             * @param aabb An AABB representing the region encompassed by the octree.
             * @param maxElementCapacity The maximum number of elements allowed at each leaf node. Default of 16.
             * @param maxDepth The maximum depth of the octree allowed. Default of 8.
             */
            Octree(AABB &&aabb, int maxElementCapacity = OCT_MAX_CAPACITY, int maxDepth = OCT_MAX_DEPTH) {
                center = std::move(aabb.pos);
                halfsize = std::move(aabb.getHalfSize());

                capacity = 17;
                count = 1;
                nodes = new Node[capacity];

                // initialize the root node
                nodes[0].firstChild = 0;
                nodes[0].count = 0;

                this->maxDepth = maxDepth;
                this->maxElementCapacity = maxElementCapacity;
            };


            // * ===================
            // * Rule of 5 Stuff
            // * ===================

            inline Octree(Octree const &tree) {
                capacity = tree.capacity;
                count = tree.count;
                // freeNode = tree.freeNode;
                maxDepth = tree.maxDepth;
                maxElementCapacity = tree.maxElementCapacity;

                center = tree.center;
                halfsize = tree.halfsize;

                nodes = new Node[capacity];
                for (int i = 0; i < count; ++i) { nodes[i] = tree.nodes[i]; }

                elements = tree.elements;
                elmNodes = tree.elmNodes;
            };

            inline Octree(Octree &&tree) {
                nodes = tree.nodes;
                capacity = tree.capacity;
                count = tree.count;
                // freeNode = tree.freeNode;
                maxDepth = tree.maxDepth;
                maxElementCapacity = tree.maxElementCapacity;

                center = std::move(tree.center);
                halfsize = std::move(tree.halfsize);

                elements = std::move(tree.elements);
                elmNodes = std::move(tree.elmNodes);

                tree.nodes = nullptr;
            };

            inline Octree& operator = (Octree const &tree) {
                if (this != &tree) {
                    if (nodes) { delete[] nodes; }

                    capacity = tree.capacity;
                    count = tree.count;
                    // freeNode = tree.freeNode;
                    maxDepth = tree.maxDepth;
                    maxElementCapacity = tree.maxElementCapacity;

                    center = tree.center;
                    halfsize = tree.halfsize;

                    nodes = new Node[capacity];
                    for (int i = 0; i < count; ++i) { nodes[i] = tree.nodes[i]; }

                    elements = tree.elements;
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
                    // freeNode = tree.freeNode;
                    maxDepth = tree.maxDepth;
                    maxElementCapacity = tree.maxElementCapacity;

                    center = std::move(tree.center);
                    halfsize = std::move(tree.halfsize);

                    elements = std::move(tree.elements);
                    elmNodes = std::move(tree.elmNodes);

                    tree.nodes = nullptr;
                }

                return *this;
            };

            inline ~Octree() { delete[] nodes; };


            // * ===================
            // * Normal Functions
            // * ===================

            // Determine if the given point is contained within the Octree.
            // If so, this will return 1.
            bool contains(ZMath::Vec3D const &point, uint32_t index) const {
                // ? We go through until we find the region the point would be in.
                // ? We then check if that point is within the region.

                // preliminary check to ensure the point is within the octree's bounds
                if (ZMath::clamp(point, center - halfsize, center + halfsize) != point) { return 0; }

                int region = 0; // start with the root node
                ZMath::Vec3D center = this->center; // centerpoint of the region

                for (;;) {
                    if (nodes[region].count == npos16) { // not a leaf node
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

                        // ensure there are elements contained within this region
                        if (!nodes[region].count) { return 0; } // there is no match if there are no elements

                        // traverse the singly linked list
                        for (int32_t i = nodes[region].firstChild; i != npos32; i = elmNodes[i].next) {
                            if (elements[elmNodes[i].element].index == index) { return 1; } // we've found our match
                        }

                        return 0; // there is no possible match if this point is reached
                    }
                }

                // just for the compiler
                // the code will never reach this point
                return 0;
            };

            // Insert the given point into the octree.
            // Returns the index of the element inserted.
            int insert(ZMath::Vec3D const &point, uint32_t index) {
                // ? We are guarenteed to have at least the root node by construction.
                // ? We will then recursively find the leaf node the point is in.

                // preliminary check to ensure the point is within the octree's bounds
                if (ZMath::clamp(point, center - halfsize, center + halfsize) != point) { return 0; }

                int region = 0; // start with the root node
                int depth = 1; // track the depth
                ZMath::Vec3D center = this->center; // store the centerpoint of the region

                for (;;++depth) {
                    if (nodes[region].count == npos16) { // not a leaf node
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
                            if (count+8 >= capacity) { grow(); }
                            
                            // ? determine the points that should be added to each new region
                            // ? do this by first determining the region each node should occupy
                            // ? then, check if the point is within a certain region
                            // ? if it is within that certain region, add it to the region's linked list of elements
                            // ? insert it at the front most likely to ensure O(1) operation

                            // value to be used as the point being added to a new region
                            // stored for readability
                            ZMath::Vec3D p;

                            // cache the indices for each new region
                            // note: octant 1 will be accessed using count
                            uint32_t oct2 = count + 1, oct3 = count + 2, oct4 = count + 3, oct5 = count + 4;
                            uint32_t oct6 = count + 5, oct7 = count + 6, oct8 = count + 7;

                            // * Initialize the new nodes to be ready for adding the points to each region
                            // octant 1
                            nodes[count].firstChild = 0;
                            nodes[count].count = npos16;

                            // octant 2
                            nodes[oct2].firstChild = 0;
                            nodes[oct2].count = npos16;

                            // octant 3
                            nodes[oct3].firstChild = 0;
                            nodes[oct3].count = npos16;

                            // octant 4
                            nodes[oct4].firstChild = 0;
                            nodes[oct4].count = npos16;

                            // octant 5
                            nodes[oct5].firstChild = 0;
                            nodes[oct5].count = npos16;

                            // octant 6
                            nodes[oct6].firstChild = 0;
                            nodes[oct6].count = npos16;

                            // octant 7
                            nodes[oct7].firstChild = 0;
                            nodes[oct7].count = npos16;

                            // octant 8
                            nodes[oct8].firstChild = 0;
                            nodes[oct8].count = npos16;
                            

                            // * Loop through each point in the region and determine its new region

                            for (int32_t curr = nodes[region].firstChild; elmNodes[curr].next != -1; curr = elmNodes[curr].next) {
                                // Determine the value of p
                                // This is only stored as a variable for readability as it is technically slightly less efficient
                                // todo may update in the future to save the 3 clock cycles per loop
                                // todo (realistically they're done concurrently so more like 1 clock cycle)
                                p = elements[elmNodes[curr].element].pos;

                                // Determine the region the point falls in and add it to that region
                                if (p.x < center.x && p.y < center.y && p.z < center.z) { // octant 1
                                    if (nodes[count].count != npos16) { // the node already has a child
                                        elmNodes[curr].next = nodes[count].firstChild;
                                        ++nodes[count].count;

                                    } else { // the node has no children
                                        elmNodes[curr].next = npos32;
                                        nodes[count].count = 1;
                                    }

                                    nodes[count].firstChild = curr;

                                } else if (p.x < center.x && p.y >= center.y && p.z < center.z) { // octant 2
                                    if (nodes[oct2].count != npos16) { // the node already has a child
                                        elmNodes[curr].next = nodes[oct2].firstChild;
                                        ++nodes[oct2].count;

                                    } else { // the node has no children
                                        elmNodes[curr].next = npos32;
                                        nodes[oct2].count = 1;
                                    }

                                    nodes[oct2].firstChild = curr;

                                } else if (p.x < center.x && p.y < center.y && p.z >= center.z) { // octant 3
                                    if (nodes[oct3].count != npos16) { // the node already has a child
                                        elmNodes[curr].next = nodes[oct3].firstChild;
                                        ++nodes[oct3].count;

                                    } else { // the node has no children
                                        elmNodes[curr].next = npos32;
                                        nodes[oct3].count = 1;
                                    }

                                    nodes[oct3].firstChild = curr;

                                } else if (p.x < center.x && p.y >= center.y && p.z >= center.z) { // octant 4
                                    if (nodes[oct4].count != npos16) { // the node already has a child
                                        elmNodes[curr].next = nodes[oct4].firstChild;
                                        ++nodes[oct4].count;

                                    } else { // the node has no children
                                        elmNodes[curr].next = npos32;
                                        nodes[oct4].count = 1;
                                    }

                                    nodes[oct4].firstChild = curr;

                                } else if (p.x >= center.x && p.y < center.y && p.z < center.z) { // octant 5
                                    if (nodes[oct5].count != npos16) { // the node already has a child
                                        elmNodes[curr].next = nodes[oct5].firstChild;
                                        ++nodes[oct5].count;

                                    } else { // the node has no children
                                        elmNodes[curr].next = npos32;
                                        nodes[oct5].count = 1;
                                    }

                                    nodes[oct5].firstChild = curr;

                                } else if (p.x >= center.x && p.y >= center.y && p.z < center.z) { // octant 6
                                    if (nodes[oct6].count != npos16) { // the node already has a child
                                        elmNodes[curr].next = nodes[oct6].firstChild;
                                        ++nodes[oct6].count;

                                    } else { // the node has no children
                                        elmNodes[curr].next = npos32;
                                        nodes[oct6].count = 1;
                                    }

                                    nodes[oct6].firstChild = curr;

                                } else if (p.x >= center.x && p.y < center.y && p.z >= center.z) { // octant 7
                                    if (nodes[oct7].count != npos16) { // the node already has a child
                                        elmNodes[curr].next = nodes[oct7].firstChild;
                                        ++nodes[oct7].count;

                                    } else { // the node has no children
                                        elmNodes[curr].next = npos32;
                                        nodes[oct7].count = 1;
                                    }

                                    nodes[oct7].firstChild = curr;

                                } else { // octant 8
                                    if (nodes[oct8].count != npos16) { // the node already has a child
                                        elmNodes[curr].next = nodes[oct8].firstChild;
                                        ++nodes[oct8].count;

                                    } else { // the node has no children
                                        elmNodes[curr].next = npos32;
                                        nodes[oct8].count = 1;
                                    }

                                    nodes[oct8].firstChild = curr;
                                }
                            }
                            
                            nodes[region].firstChild = count;
                            nodes[region].count = npos16; // todo maybe should be set to 0

                            count += 8;

                            // insert the new point
                            uint32_t curr = elmNodes.count;
                            elmNodes.insert({npos32, elements.count});
                            elements.insert({index, point});

                            // determine the region to place the point in
                            if (point.x < center.x && point.y < center.y && point.z < center.z) { // octant 1
                                if (nodes[count].count != -1) { // the node already has a child
                                    elmNodes[curr].next = nodes[count].firstChild;
                                    ++nodes[count].count;

                                } else { // the node has no children
                                    elmNodes[curr].next = -1;
                                    nodes[count].count = 1;
                                }

                                nodes[count].firstChild = curr;

                            } else if (point.x < center.x && point.y >= center.y && point.z < center.z) { // octant 2
                                if (nodes[oct2].count != -1) { // the node already has a child
                                    elmNodes[curr].next = nodes[oct2].firstChild;
                                    ++nodes[oct2].count;

                                } else { // the node has no children
                                    elmNodes[curr].next = -1;
                                    nodes[oct2].count = 1;
                                }

                                nodes[oct2].firstChild = curr;

                            } else if (point.x < center.x && point.y < center.y && point.z >= center.z) { // octant 3
                                if (nodes[oct3].count != -1) { // the node already has a child
                                    elmNodes[curr].next = nodes[oct3].firstChild;
                                    ++nodes[oct3].count;

                                } else { // the node has no children
                                    elmNodes[curr].next = -1;
                                    nodes[oct3].count = 1;
                                }

                                nodes[oct3].firstChild = curr;

                            } else if (point.x < center.x && point.y >= center.y && point.z >= center.z) { // octant 4
                                if (nodes[oct4].count != -1) { // the node already has a child
                                    elmNodes[curr].next = nodes[oct4].firstChild;
                                    ++nodes[oct4].count;

                                } else { // the node has no children
                                    elmNodes[curr].next = -1;
                                    nodes[oct4].count = 1;
                                }

                                nodes[oct4].firstChild = curr;

                            } else if (point.x >= center.x && point.y < center.y && point.z < center.z) { // octant 5
                                if (nodes[oct5].count != -1) { // the node already has a child
                                    elmNodes[curr].next = nodes[oct5].firstChild;
                                    ++nodes[oct5].count;

                                } else { // the node has no children
                                    elmNodes[curr].next = -1;
                                    nodes[oct5].count = 1;
                                }

                                nodes[oct5].firstChild = curr;

                            } else if (point.x >= center.x && point.y >= center.y && point.z < center.z) { // octant 6
                                if (nodes[oct6].count != -1) { // the node already has a child
                                    elmNodes[curr].next = nodes[oct6].firstChild;
                                    ++nodes[oct6].count;

                                } else { // the node has no children
                                    elmNodes[curr].next = -1;
                                    nodes[oct6].count = 1;
                                }

                                nodes[oct6].firstChild = curr;

                            } else if (point.x >= center.x && point.y < center.y && point.z >= center.z) { // octant 7
                                if (nodes[oct7].count != -1) { // the node already has a child
                                    elmNodes[curr].next = nodes[oct7].firstChild;
                                    ++nodes[oct7].count;

                                } else { // the node has no children
                                    elmNodes[curr].next = -1;
                                    nodes[oct7].count = 1;
                                }

                                nodes[oct7].firstChild = curr;

                            } else { // octant 8
                                if (nodes[oct8].count != -1) { // the node already has a child
                                    elmNodes[curr].next = nodes[oct8].firstChild;
                                    ++nodes[oct8].count;

                                } else { // the node has no children
                                    elmNodes[curr].next = -1;
                                    nodes[oct8].count = 1;
                                }

                                nodes[oct8].firstChild = curr;
                            }

                            return; // insertion is complete
                        }

                        // * Insert the element as the new head of the node's linked list

                        if (!nodes[region].count) { // no elements contained in the region
                            nodes[region].firstChild = elmNodes.count;
                            elmNodes.insert({npos32, elements.count});
                        
                        } else { // elements already in the region
                            // preliminary
                            uint32_t temp = elmNodes.count;
                            elmNodes.insert({npos32, elements.count});

                            // insert the element's index into the linked list
                            elmNodes[temp].next = nodes[region].firstChild;
                            nodes[region].firstChild = temp;
                        }

                        ++nodes[region].count;
                        elements.insert({index, point});

                        return; // insertion is complete
                    }
                }
            };

            // Remove an element from the octree.
            // Returns 1 if the element was successfully found and removed.
            bool remove(ZMath::Vec3D const &point, uint32_t index) {
                // ? Search through each region of the octree until we find the one the point belongs to.
                // ? Once we find that region, traverse the linked list until we find the matching element.
            
                // preliminary check to ensure the point is within the octree's bounds
                if (ZMath::clamp(point, center - halfsize, center + halfsize) != point) { return 0; }

                int region = 0;
                ZMath::Vec3D center = this->center;

                for (;;) {
                    if (nodes[region].count == npos16) { // not a leaf node
                        // * Determine the new region

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
                        // * Check each element contained within this leaf node.
                        // * If we find the element to be removed, remove it from the list and return 1.

                        // ensure there is an element in the region to remove
                        if (!nodes[region].count) { return 0; } // there was nothing to remove

                        // first we want to traverse the singly linked list until we find the element we are searching for
                        // once we find that element, we wanna make sure we were storing the previous element
                        // we wanna set the previous element's next to the current element's next
                        // we then remove the current guy from our list and return 1

                        // track the previous element
                        uint32_t prev = npos32;

                        // todo traversal currently fails in the same way it does for in the insert function
                        // traverse the singly linked list
                        for (uint32_t curr = nodes[region].firstChild; curr != npos32; curr = elmNodes[curr].next) {
                            if (elements[elmNodes[curr].element].index == index) {
                                // update the next values in the linked list
                                if (prev == npos32) { nodes[region].firstChild = elmNodes[curr].next; } // curr is head
                                else { elmNodes[prev].next = elmNodes[curr].next; } // curr is not head

                                // remove the current element
                                elements.remove(elmNodes[curr].element);
                                elmNodes.remove(curr);
                                --nodes[region].count;

                                return 1;
                            }

                            prev = curr;
                        }

                        return 0; // there is no possible match if this point is reached
                    }
                }

                // just for the compiler
                // this part of the code will never be reached
                return 0;
            };

            // Update the regions after adding or removing elements.
            // This should be called each physics frame after your physics handler updates the positions of the objects.
            void update() {

            };

            // Clear the octree.
            inline void clear() { // todo reinitialize the root node
                delete[] nodes;
                capacity = 17;
                count = 1;
                nodes = new Node[capacity];
                // freeNode = -1;
                elements.clear();
                elmNodes.clear();
            };

            // Determine if the octree is empty.
            inline bool empty() const { return !elmNodes.count; };
    };
}
