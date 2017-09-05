#include "transmissivematerial.h"
#include "specularbtdf.h"

void TransmissiveMaterial::ProduceBSDF(Intersection *isect) const
{
    // Important! Must initialize the intersection's BSDF!
    isect->bsdf = std::make_shared<BSDF>(*isect, indexOfRefraction);

    Color3f color = Kt;
    if(this->textureMap)
    {
        color *= Material::GetImageColor(isect->uv, this->textureMap.get());
    }
    if(this->normalMap)
    {
        isect->bsdf->normal = isect->bsdf->tangentToWorld *  Material::GetImageColor(isect->uv, this->normalMap.get());
        //Update bsdf's TBN matrices to support the new normal
        Vector3f tangent, bitangent;
        CoordinateSystem(isect->bsdf->normal, &tangent, &bitangent);

        glm::vec3 nor = isect->bsdf->normal;
        if (std::isnan(nor.x) || std::isnan(nor.y) || std::isnan(nor.z) ) {
            throw;
        }
        if (std::isnan(tangent.x) || std::isnan(tangent.y) || std::isnan(tangent.z) ) {
            throw;
        }
        if (std::isnan(bitangent.x) || std::isnan(bitangent.y) || std::isnan(bitangent.z) ) {
            throw;
        }


        isect->bsdf->UpdateTangentSpaceMatrices(isect->bsdf->normal, tangent, bitangent);
    }

    isect->bsdf->Add(new SpecularBTDF(color, 1.f, indexOfRefraction, new FresnelNoReflect()));
}
