#pragma once
#include <QList>
#include "raytracing/intersection.h"
#include "raytracing/ray.h"
#include "scene/bounds.h"
#include "openGL/drawable.h"

class Triangle
{
public:

    static pcg32 colorRNG;

    Triangle(const glm::vec3 &p1, const glm::vec3 &p2, const glm::vec3 &p3);
    Triangle(const glm::vec3 &p1, const glm::vec3 &p2, const glm::vec3 &p3,
             const glm::vec3 &n1, const glm::vec3 &n2, const glm::vec3 &n3);
    Triangle(const glm::vec3 &p1, const glm::vec3 &p2, const glm::vec3 &p3,
             const glm::vec3 &n1, const glm::vec3 &n2, const glm::vec3 &n3,
             const glm::vec2 &t1, const glm::vec2 &t2, const glm::vec2 &t3);


    bool Intersect(const Ray &ray, Intersection *isect, glm::mat4 viewProj) const;
    bool Intersect_PBVH(const Ray &ray, Intersection *isect, glm::mat4 viewProj) const;
    void InitializeIntersection(Intersection *isect, float t, Point3f pLocal) const;
    void ComputeTBN(const Point3f &P, Normal3f *nor, Vector3f *tan, Vector3f *bit) const;
    Point2f GetUVCoordinates(const Point3f &point) const;
    Normal3f GetNormal(const Point3f &position) const;
    float Area() const;


    Bounds3f WorldBound() const;

    Point3f points[3];
    Normal3f normals[3];
    Point2f uvs[3];
    Normal3f planeNormal;

//    void create();//This does nothing because individual triangles are not rendered with OpenGL;
//                  //they are rendered all together in their Mesh.


};

//A mesh's primary purpose is to store VBOs for rendering the triangles in OpenGL.
class Mesh : public Drawable
{
public:
    virtual void create();
    void LoadOBJ(const QStringRef &filename, const QStringRef &local_path, const Transform& transform);
    virtual GLenum drawMode() const;

    QList<std::shared_ptr<Triangle>> faces;
};






