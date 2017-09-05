#pragma once

#include "light.h"

class PointLight : public Light
{
public:
    PointLight(const Transform &t, const Color3f& Le)
        : Light(t), emittedLight(Le),
          pLight(Vector3f(t.T() * glm::vec4(glm::vec3(0.0f), 1.0f) ))
    {}

    // Given a point of intersection on some surface in the scene and a pair
    // of uniform random variables, generate a sample point on the surface
    // of our shape and evaluate the light emitted along the ray from
    // our Shape's surface to the reference point.
    Color3f Sample_Li(const Intersection &ref, const Point2f &xi,
                                         Vector3f *wiW, Float *pdf) const;

    virtual float Pdf_Li(const Intersection &ref, const Vector3f &wi) const;

    virtual Color3f L(const Intersection &isect, const Vector3f &w) const;

    virtual Color3f powerVal();
    virtual Point3f getOrig(std::shared_ptr<Sampler> samp);
    virtual Vector3f getDir(std::shared_ptr<Sampler> samp, Intersection* i);

    // Member variables
    const Color3f emittedLight;
    Point3f pLight; // where the point light is in space
};
