#pragma once
#include "scene/scene.h"
#include "scene/geometry/mesh.h"
#include "raytracing/ray.h"
#include "globals.h"
#include "scene/bvh.h"
#include <iostream>

class Sample;

float RADIUS = 0.25f;

class PoissonSampler
{
public:
    //assuming only inputting primitives in scene that ALL need to be filled
    PoissonSampler(Mesh* mesh, Scene* scene, int dim)
        : m(mesh), s(scene), b(s->bvh), bbox(new Bounds3f(b->WorldBound())),
            nDimensions(dim) {}
    ~PoissonSampler() {}

    // all variables below are initialized in the constructor's list
    Mesh* m;
    Scene* s;
    BVHAccel* b;
    Bounds3f* bbox;
    int nDimensions;
\
    std::vector<Sample*> poissonAlg();

    // notes:
    // sampler only created by being inputted specified mesh from scene
    // create bvh of mesh [done in constructor]
    // create 3d grid based on mesh bbox
    // perform poisson alg on mesh [use bvh to check if actually valid sample]
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
