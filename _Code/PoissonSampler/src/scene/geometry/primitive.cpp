#include "primitive.h"

bool Primitive::Intersect(const Ray &r, Intersection *isect) const
{
    if(!shape->Intersect(r, isect)) return false;
    isect->objectHit = this;

//    if (isnan(isect->bitangent.x) || isnan(isect->bitangent.y) || isnan(isect->bitangent.z)) {
//        //throw;
//    }
//    if (isnan(isect->normalGeometric.x) || isnan(isect->normalGeometric.y) || isnan(isect->normalGeometric.z)) {
//        //throw;
//    }
//    if (isnan(isect->tangent.x) || isnan(isect->tangent.y) || isnan(isect->tangent.z) ) {
//        //throw;
//    }

    // We create a BSDF for this intersection in our Integrator classes
    return true;
}

bool Primitive::ProduceBSDF(Intersection *isect) const
{
    if(material)
    {
        material->ProduceBSDF(isect);
        return true;
    }
    return false;
}

const AreaLight* Primitive::GetAreaLight() const
{
    return areaLight.get();
}


const Material* Primitive::GetMaterial() const
{
    return material.get();
}

Bounds3f Primitive::WorldBound() const {
    return shape->WorldBound();
}
