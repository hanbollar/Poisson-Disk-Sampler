#pragma once
#include <globals.h>
//#include <scene/materials/bsdf.h>
#include <raytracing/ray.h>
//#include <scene/geometry/primitive.h>
#include <QList>
//#include <scene/geometry/primitive.h>

//class Material;
//class BSDF;
//class Primitive;

class Intersection
{
public:
    Intersection();

    // Instantiate a Ray that originates from this Intersection and
    // travels in direction d.
    Ray SpawnRay(const Vector3f &d) const;

    Point3f point;          // The place at which the intersection occurred
    Normal3f normalGeometric; // The surface normal at the point of intersection, NO alterations like normal mapping applied
    Point2f uv;             // The UV coordinates computed at the intersection
    float t;                  // The parameterization for the ray (in world space) that generated this intersection.
                              // t is equal to the distance from the point of intersection to the ray's origin if the ray's direction is normalized.

    Vector3f tangent, bitangent; // World-space vectors that form an orthonormal basis with the surface normal.
};
