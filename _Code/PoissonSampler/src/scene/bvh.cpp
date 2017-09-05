#include "bvh.h"

#include <qlist.h>
#include <QList>

// Feel free to ignore these structs entirely!
// They are here if you want to implement any of PBRT's
// methods for BVH construction.

struct BVHPrimitiveInfo {
    BVHPrimitiveInfo() {}
    BVHPrimitiveInfo(size_t primitiveNumber, const Bounds3f &bounds)
        : primitiveNumber(primitiveNumber),
          bounds(bounds),
          centroid(.5f * bounds.min + .5f * bounds.max) {}
    int primitiveNumber;
    Bounds3f bounds;
    Point3f centroid;
};

struct BVHBuildNode {
    // BVHBuildNode Public Methods
    void InitLeaf(int first, int n, const Bounds3f &b) {
        firstPrimOffset = first;
        nPrimitives = n;
        bounds = b;
        children[0] = children[1] = nullptr;
    }
    void InitInterior(int axis, BVHBuildNode *c0, BVHBuildNode *c1) {
        children[0] = c0;
        children[1] = c1;
        bounds = Union(c0->bounds, c1->bounds);
        splitAxis = axis;
        nPrimitives = 0;
    }
    Bounds3f bounds;
    BVHBuildNode *children[2];
    int splitAxis, firstPrimOffset, nPrimitives;
};

struct MortonPrimitive {
    int primitiveIndex;
    unsigned int mortonCode;
};

struct LBVHTreelet {
    int startIndex, nPrimitives;
    BVHBuildNode *buildNodes;
};

struct LinearBVHNode {
    Bounds3f bounds;
    union {
        int primitivesOffset;   // leaf
        int secondChildOffset;  // interior
    };
    unsigned short nPrimitives;  // 0 -> interior node, 16 bytes
    unsigned char axis;          // interior node: xyz, 8 bytes
    unsigned char pad[1];        // ensure 32 byte total size
};


BVHAccel::~BVHAccel()
{
    delete [] nodes;
}

// Constructs an array of BVHPrimitiveInfos, recursively builds a node-based BVH
// from the information, then optimizes the memory of the BVH
BVHAccel::BVHAccel(const std::vector<std::shared_ptr<Primitive> > &p, int maxPrimsInNode)
    : maxPrimsInNode(std::min(255, maxPrimsInNode)), primitives(p)
{
    QTime timer;
    timer.start();

    // constructor
    root = new BVHNode(primitives);
    root->bBox = buildBBoxFromPrim(root);

//    return;

    // base case
    if (primitives.size() == 0) {
        return;
    }
    buildTree(root);

    int nMilliseconds = timer.elapsed();
    std::cout<<"Build time of BVH : " <<nMilliseconds<<std::endl;
}

/**
 * @brief BVHAccel::buildTree
 * HB: Helper method created to recursively build the BVH tree that I store
 *
 * precondition: the given node already has its bounding box and primitives created
 *
 * @param r: the parent node for this recursion -> except for the first iteration through
 *           in which r is actually the root node of the whole tree [so starts as nullptr]
 * @return bool: true if children added to this node, false otherwise
 */
bool BVHAccel::buildTree(BVHNode* r) {
    // note: two cases for this to stop the recursion [leaf]
    //      (1) num prims in node is <= maxPrimsInNode
    //      (2) check based on SAH cost > just intersection cost

    int len = r->p.size();

    // (1) LEAF STOP RECURSION CHECK: num prims in node is <= maxPrimsInNode
    if ((int)(len) < this->maxPrimsInNode) {
        return false;
    }

    // sort primitives across split by centroid
    std::vector<std::shared_ptr<Primitive>> set0;
    std::vector<std::shared_ptr<Primitive>> set1;

    // find split loc
    int axis = r->bBox.MaximumExtent();
    float locForSplit = computeSplit(r, axis, set0, set1);

//    for (int i = 0; i < len; i++) {
//        Point3f pmax = r->p[i]->WorldBound().max;
//        Point3f pmin = r->p[i]->WorldBound().min;
//        float indexVal = pmin[axis] + (pmax[axis] - pmin[axis])/2.0f;
//        // left of split
//        if (indexVal < locForSplit) {
//            set0.push_back(r->p[i]);
//        // right of split
//        } else {
//            set1.push_back(r->p[i]);
//        }
//    }

    // create children and set up their primitives and bounding boxes
    BVHNode* c0 = new BVHNode(set0);
    BVHNode* c1 = new BVHNode(set1);
    c0->bBox = buildBBoxFromPrim(c0);
    c1->bBox = buildBBoxFromPrim(c1);

    // (2) LEAF STOP RECURSION CHECK: check if this should be leaf based on SAH cost
    // SAH cost of a subdivision = (SA b1 * number of primitives in b1 + SA b2 * number of primitives in b2 ) / (SA).
    float SA_0 = c0->bBox.SurfaceArea();
    float numPrim_0 = c0->p.size();
    float SA_1 = c1->bBox.SurfaceArea();
    float numPrim_1 = c1->p.size();
    float SAH = (SA_0 * numPrim_0 + SA_1 * numPrim_1) / r->bBox.SurfaceArea();
    // intersection cost = number of primitives in this node
    float intersectionCost = len;
    // the check
    if (SAH >= intersectionCost) {
        return false;
    }

    // connect to parent node
    r->child0 = c0;
    r->child1 = c1;

    // recursive step also only storing primitives in leaves
    if (buildTree(c0)) {
        c0->p.clear();
    }
    if(buildTree(c1)) {
        c1->p.clear();
    }

    return true;
}

/**
 * @brief BVHAccel::computeSplit
 * HB: Helper method created to calculate split of objects on a given axis
 *
 * @param r: the parent node whose
 * @param axis: the axis across which the split is occuring
 *
 * @return int: the float location on the axis where the split will occur
 */
float BVHAccel::computeSplit(BVHNode* r, int axis,
                             std::vector<std::shared_ptr<Primitive>>& set0,
                             std::vector<std::shared_ptr<Primitive>>& set1) {
    // doing split check along axisLength / log(num primitives)
    int len = r->p.size();
    float min = r->bBox.min[axis];
    float max = r->bBox.max[axis];

    if (min > max) { throw; } // not valid bb

    float splitCheckLen = r->bBox.Diagonal()[axis] / 10.0f; //( len / glm::log((float)len) );//r->bBox->Diagonal()[axis] / 10.0f;

    // find all binding boxes so dont have to do them n*log(n) times and instead just n + logn times
//    std::vector<Bounds3f> allBoxes;
//    std::vector<Point3f> allPos;
//    for (int i = 0; i < len; i++) {
//        Bounds3f b = r->p[i]->WorldBound();
//        allBoxes.push_back(b);

//        Point3f pmin = b.min;
//        allPos.push_back( pmin[axis] + (b.Diagonal()) /2.0f );
//    }

    float splitVal = min;
    float minSAH = INFINITY;
    // find minSAH difference and use that as the final split location
    // iterating through all the splits
    for (float atLoc = min + splitCheckLen; atLoc <= max - splitCheckLen; atLoc += splitCheckLen) {
        // sorting items across left / right of the split line

        std::vector<std::shared_ptr<Primitive>> temp_set0;
        std::vector<std::shared_ptr<Primitive>> temp_set1;

//        float rightCount = 0.0f;
//        float leftCount = 0.0f;

        Bounds3f leftB = Bounds3f();
        Bounds3f rightB = Bounds3f();
        for (int i = 0; i < len; i++) {
            Bounds3f b = r->p[i]->WorldBound();
            glm::vec3 loc = b.min[axis] + (b.Diagonal()) /2.0f;

            // left of split
            if (loc[axis] < atLoc) {
                leftB = Union(leftB, r->p[i]->WorldBound());
                temp_set0.push_back(r->p[i]);
            // right of split
            } else {
                rightB = Union(rightB, r->p[i]->WorldBound());
                temp_set0.push_back(r->p[i]);
            }
        }

        float outerB = r->bBox.SurfaceArea();
        float rightSA = rightB.SurfaceArea();
        float leftSA = leftB.SurfaceArea();

        float leftCount = temp_set0.size();
        float rightCount = temp_set1.size();

        bool lZero = leftCount == 0.0f;
        bool rZero = rightCount == 0.0f;

        bool skip = false;

//        if ( (lZero && !rZero) || (!lZero && rZero)) {
//            skip = true;
//        }

//        if (!skip) {
            // check for invalid bounding boxes. SA returns zero is min > max as in beg construction
            if (outerB == 0.0f) { /*throw;*/ }
            if (rightSA == 0.0f && rightCount != 0) {
                if (rightCount == 1) {
                    rightB = temp_set1[0]->WorldBound();
                    rightSA = rightB.SurfaceArea();
                } else { /*throw;*/ }
            }
            if (leftSA == 0.0f && leftCount != 0) {
                if (leftCount == 1) {
                    leftB = temp_set0[0]->WorldBound();
                    leftSA = leftB.SurfaceArea();
                } else { /*throw;*/ }
            }

            float SAdiff = fabs(rightSA * rightCount + leftSA * leftCount) / outerB;
            if (SAdiff < minSAH) {
                minSAH = SAdiff;
                splitVal = atLoc;
                set0 = temp_set0;
                set1 = temp_set1;
            } else {
                temp_set0.clear();
                temp_set1.clear();
            }
//        }
    }

    return splitVal;
}

/**
 * @brief BVHAccel::buildBBoxFromPrim
 * HB: Helper method created to recursively build the BVH tree that I store
 *
 * precondition: the given node already has its bounding box and primitives created
 *
 * @param r: passing the node instead of the parameters bc smaller amount of data to pass
 */
Bounds3f BVHAccel::buildBBoxFromPrim(BVHNode* r) {
    int len = r->p.size();
    if (len == 0) {
        return Bounds3f();
    }

    // union all the primitives to find overall bounding box
    // we know there's at least one primitive for this method to even be called

    Bounds3f bBox = Bounds3f(r->p[0]->WorldBound());
    for (int i = 1; i < len; i++) {
        bBox = Union(bBox, r->p[i]->WorldBound());
    }

    return bBox;
}

bool BVHAccel::Intersect(const Ray &ray, Intersection *isect) const
{
    if (primitives.size() == 0 || root == nullptr) {
        return false;
    }

    return checkForIntersect(ray, isect, root);
}

/**
 * Recursive helper method used in Intersect
 *
 * @brief BVHAccel::checkForIntersect
 * @param ray: the ray cast to check for an intersection
 * @param isect: to be filled in with the actual intersection if one is found
 * @param r: the parent node in which we are currently checking for an additional intersection
 * @return true/false: if an intersection ultimately occurs
 */
bool BVHAccel::checkForIntersect(const Ray &ray, Intersection *isect, BVHNode* r) const {
    bool n0 = (r->child0 == nullptr);
    bool n1 = (r->child1 == nullptr);

    // checking if leaf node
    if (n0 || n1) {
        // checking to make sure tree was created properly with two children or none
        if (n0 && n1) {

            // checking for intersection with primitives
            bool hit = false;
            Intersection tempIsect = Intersection();
            Intersection finalIntersect = Intersection();
            finalIntersect.t = INFINITY;
            for (std::shared_ptr<Primitive> prim : r->p) {

                tempIsect = Intersection();
                bool inter = prim->Intersect(ray, &tempIsect);
                if (inter) {
                    if (finalIntersect.objectHit == nullptr || finalIntersect.t > tempIsect.t) {
                        finalIntersect = tempIsect;
                        hit = true;
                    }
                }
            }
            if (finalIntersect.t < INFINITY && hit) {
                *isect = finalIntersect;
                return true;
            }
            return false;

        } else {
            // tree was set up improperly: has only one childnode -> this case cannot occur
            throw;
        }
    }

    float t_0 = INFINITY;
    float t_1 = INFINITY;
    // since not leaf node we check for intersections with BVHNode children
    bool leftIsect = (r->child0->bBox.Intersect(ray, &t_0));
    bool rightIsect = (r->child1->bBox.Intersect(ray, &t_1));

    Intersection isect0 = Intersection();
    Intersection isect1 = Intersection();
    if (leftIsect) {
        leftIsect = checkForIntersect(ray, &isect0, r->child0);
    }
    if (rightIsect) {
        rightIsect = checkForIntersect(ray, &isect1, r->child1);
    }

    // checking if have valid intersections
    if (leftIsect && isect0.objectHit == nullptr) {
        throw;
    }
    if (rightIsect && isect1.objectHit == nullptr) {
        throw;
    }

    if (leftIsect || rightIsect) {
        if (leftIsect && rightIsect) {
            if (isect0.t > isect1.t) {
                *isect = isect1;
            } else {
                *isect = isect0;
            }
        } else if (leftIsect) {
            *isect = isect0;
        } else if (rightIsect) {
            *isect = isect1;
        } else {
            // SHOULD NEVER REACH THIS CASE
            throw;
        }

        return true;
    }
    return false;
}

void BVHAccel::fillInBoundingBoxes() {
    fillBox(this->root);
}

void BVHAccel::fillBox(BVHNode *r) {
    bool n0 = (r->child0 == nullptr);
    bool n1 = (r->child1 == nullptr);

    r->bBox = buildBBoxFromPrim(r);

    // checking if leaf node
    if (n0) {
       fillBox(r->child0);
    }
    if (n1) {
        fillBox(r->child1);
    }
}
