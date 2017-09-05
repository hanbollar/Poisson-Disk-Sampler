#pragma once
#include "integrator.h"
#include "directlightingintegrator.h"
#include "scene/photonmap.h"
#include "raytracing/intersection.h"

#include "scene/geometry/ignoreGeoLight.h"

class PhotonIntegrator : public Integrator
{
public:
    PhotonIntegrator(Bounds2i bounds, Scene* s, std::shared_ptr<Sampler> sampler, int recursionLimit,
                    PhotonMap* pMap, DirectLightingIntegrator* dirInt)
        : Integrator(bounds, s, sampler, recursionLimit), p_Map(pMap), dir_Int(dirInt)
    {}

    PhotonMap* p_Map;
    DirectLightingIntegrator* dir_Int;

    // Evaluate the energy transmitted along the ray back to
    // its origin, e.g. the camera or an intersection in the scene
    virtual Color3f Li(const Ray &ray, const Scene &scene, std::shared_ptr<Sampler> sampler, int depth,
                       Color3f compoundColor) const;
};
