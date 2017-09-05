#pragma once
#include "integrator.h"

#include "scene/geometry/ignoreGeoLight.h"

class NaiveIntegrator : public Integrator
{
public:
    NaiveIntegrator(Bounds2i bounds, Scene* s, std::shared_ptr<Sampler> sampler, int recursionLimit)
        : Integrator(bounds, s, sampler, recursionLimit)
    {}

    // Evaluate the energy transmitted along the ray back to
    // its origin, e.g. the camera or an intersection in the scene
    virtual Color3f Li(const Ray &ray, const Scene &scene, std::shared_ptr<Sampler> sampler, int depth,
                       Color3f compoundColor) const;
};
