#pragma once
#include "geometry/primitive.h"

#include <QTime>


// Forward declarations of structs used by our BVH tree
// They are defined in the .cpp file
struct BVHBuildNode;
struct BVHPrimitiveInfo;
struct MortonPrimitive;
struct LinearBVHNode;

/**
 * Forward declaration of class for my implementation of the BVH tree
 */
class BVHNode;

class BVHAccel : public Primitive
{
    //Functions
public:

    BVHAccel(const std::vector<std::shared_ptr<Primitive>> &p,
             int maxPrimsInNode = 1);
    Bounds3f WorldBound() const;
    ~BVHAccel();
    bool Intersect(const Ray &ray, Intersection *isect) const;

    // line 350 in myGL
    void fillInBoundingBoxes();
    // note if this function is no longer being used also uncomment the clearing lines
    //      152 and 155
    void fillBox(BVHNode* r);

private:
    BVHBuildNode *recursiveBuild(
        std::vector<BVHPrimitiveInfo> &primitiveInfo,
        int start, int end, int *totalNodes,
        std::vector<std::shared_ptr<Primitive>> &orderedPrims);

    BVHBuildNode *buildUpperSAH(std::vector<BVHBuildNode *> &treeletRoots,
                                int start, int end, int *totalNodes) const;

    int flattenBVHTree(BVHBuildNode *node, int *offset);

    //Members
    const int maxPrimsInNode;
    std::vector<std::shared_ptr<Primitive>> primitives;
    LinearBVHNode *nodes = nullptr;

    /**
     * HB: the following info was created for my own implementation of
     *     the BVH tree creation
     */
    BVHNode* root;
    bool buildTree(BVHNode* r);
    float computeSplit(BVHNode* r, int axis,
                       std::vector<std::shared_ptr<Primitive>>& set0,
                       std::vector<std::shared_ptr<Primitive>>& set1);
    Bounds3f buildBBoxFromPrim(BVHNode* r);
    bool checkForIntersect(const Ray &ray, Intersection *isect, BVHNode* r) const;
};

/**
 * @brief The BVHNode struct
 *
 * HB: class I created to implement BVHAccel
 */
class BVHNode {

public:
    BVHNode(const std::vector<std::shared_ptr<Primitive>> &prim)
        : bBox(Bounds3f()), child0(nullptr), child1(nullptr), p(prim) {}

    Bounds3f bBox;
    BVHNode* child0;
    BVHNode* child1;

    ~BVHNode();

    std::vector<std::shared_ptr<Primitive>> p;
    // note can only be leaf node if both children are nullptrs
    // can only be valid BVHNode that is not a leaf if both children exist
};
