#pragma once
#include "scene/scene.h"
#include "scene/geometry/mesh.h"
#include "raytracing/ray.h"
#include "globals.h"
#include "scene/bvh.h"
#include <iostream>

#include "eigen/Eigen/Dense"

#include "poissonbvh.h"

class Sample;

class PoissonSampler
{
public:
    //assuming only inputting primitives in scene that ALL need to be filled
    PoissonSampler(Mesh* mesh, Scene* scene, bool isThreeDim)
        : m(mesh), s(scene), b(s->bvh), bbox(new Bounds3f(b->WorldBound())),
            threeDim(isThreeDim), voxelDim(glm::vec3(0.0f)) {

        initializeBackgroundGridsandBVH();
    }
    ~PoissonSampler() { /** ------- TO DO ------ **/ /** STILL NEED TO DESTROY BACKGROUND GRIDS & BVH**/ }

    // all variables below are initialized in the constructor's list
    Mesh* m;
    Scene* s;
    BVHAccel* b;
    Bounds3f* bbox;
    bool threeDim;
    glm::vec3 voxelDim;
    PoissonBVH* bvh;

    float RADIUS = 0.25f;

    std::vector<std::vector<std::vector<Sample*>>> backgroundGrid3D;
    std::vector<std::vector<Sample*>> backgroundGrid2D;

    void initializeBackgroundGridsandBVH();
\
    void poissonAlg();

    glm::vec3 posToGridLoc(glm::vec3 p);
    bool validLocWithinObj(glm::vec3 p);
};

class Sample {
    public:
        Sample(glm::vec3 gLoc, glm::vec3 wPos, int gId)
            : gridLoc(gLoc), pos(wPos), id(gId) {}
        ~Sample();

        glm::vec3 pos;
        glm::vec3 gridLoc;
        int id;
};
