#pragma once
#include "scene/scene.h"
#include "raytracing/ray.h"
#include "globals.h"
#include <iostream>

#include "eigen/Eigen/Dense"
#include "scene/geometry/mesh.h"

class P_BVHNode;

class PoissonBVH
{
public:
    PoissonBVH(Mesh* m) { buildBVH(m); }
    ~PoissonBVH() {}

    P_BVHNode* root;

    void buildBVH(Mesh* m);
};

class P_BVHNode
{
public:
    P_BVHNode() : l(nullptr), r(nullptr), tris(QList<std::shared_ptr<Triangle>>()), bbox(nullptr) {}
    P_BVHNode(P_BVHNode* left, P_BVHNode* right)
        : l(left), r(right), tris(QList<std::shared_ptr<Triangle>>()), bbox(nullptr) {}

    QList<std::shared_ptr<Triangle>> tris; //this should only contain items if leafnode

    void buildSelfAsChild(QList<std::shared_ptr<Triangle>> t, int minNum);
    Bounds3f* buildBoundingBox(QList<std::shared_ptr<Triangle>> t);

    Bounds3f* bbox;

    P_BVHNode* l;
    P_BVHNode* r;
};
