#pragma once
#include "globals.h"
#include <iostream>

#include "eigen/Eigen/Dense"
#include "scene/geometry/mesh.h"
#include "scene/bounds.h"
#include "scene/scene.h"
#include "raytracing/intersection.h"
#include "samplers/poissonsampler.h"
#include "raytracing/ray.h"
#include "openGL/drawable.h"
#include "openGL/shaderprogram.h"
//#include "openGL/glwidget277.h"
#include "mygl.h"

class P_BVHNode;
//class QWidget277;
class ShaderProgram;
class MyGL;

class PoissonBVH : public Drawable
{
public:
    PoissonBVH(Mesh& m);
    ~PoissonBVH() {}

    P_BVHNode* root;

    void create();  // implemented for Drawable
    GLenum drawMode();

    void drawAll(MyGL& context, ShaderProgram& s);

    void buildBVH(Mesh& m);
    bool intersect(Ray &ray, Intersection *isect, int* numIntersections);
};

class P_BVHNode : public Drawable
{
public:
    P_BVHNode() : l(nullptr), r(nullptr), tris(QList<std::shared_ptr<Triangle>>()), bbox(nullptr) {}
    P_BVHNode(P_BVHNode* left, P_BVHNode* right)
        : l(left), r(right), tris(QList<std::shared_ptr<Triangle>>()), bbox(nullptr) {}

    ~P_BVHNode() {}

    QList<std::shared_ptr<Triangle>> tris; //this should only contain items if leafnode
    Bounds3f* bbox;
    P_BVHNode* l;
    P_BVHNode* r;

    void create();
    GLenum drawMode();
    void draw(MyGL& context, ShaderProgram& s);

    void buildSelfAsChild(QList<std::shared_ptr<Triangle>>& t, int minNum);

    Bounds3f* buildBoundingBox(QList<std::shared_ptr<Triangle>>& t);

    void splitTheTris(int axis, QList<std::shared_ptr<Triangle>>& t,
                      QList<std::shared_ptr<Triangle>>* &left, QList<std::shared_ptr<Triangle>>* &right,
                      Point3f min, Point3f max,
                      Bounds3f* leftBox, Bounds3f* rightBox,
                      float outerSA);

    bool intersect(Ray &ray, Intersection *isect, int* numIntersections);
};
