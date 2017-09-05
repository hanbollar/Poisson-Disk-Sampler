#include "mattematerial.h"
#include "lambertbrdf.h"

void MatteMaterial::ProduceBSDF(Intersection *isect) const
{
    // Important! Must initialize the intersection's BSDF!
    isect->bsdf = std::make_shared<BSDF>(*isect);

    Color3f color = Kd;
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
//        if (isnan(nor.x) || isnan(nor.y) || isnan(nor.z) ) {
//            throw;
//        }
//        if (isnan(tangent.x) || isnan(tangent.y) || isnan(tangent.z) ) {
//            throw;
//        }
//        if (isnan(bitangent.x) || isnan(bitangent.y) || isnan(bitangent.z) ) {
//            throw;
//        }

        isect->bsdf->UpdateTangentSpaceMatrices(isect->bsdf->normal, tangent, bitangent);
    }

    if(sigma == 0.f)
    {
        isect->bsdf->Add(new LambertBRDF(color));
    }
    //Else do Oren-Nayar (not required implementation)
}
