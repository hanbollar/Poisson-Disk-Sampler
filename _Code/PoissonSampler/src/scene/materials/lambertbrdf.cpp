#include "lambertbrdf.h"

Color3f LambertBRDF::f(const Vector3f &wo, const Vector3f &wi) const
{
    //TODO - DONE
    return R / Pi;
}

Color3f LambertBRDF::Sample_f(const Vector3f &wo, Vector3f *wi, const Point2f &u,
                        Float *pdf, BxDFType *sampledType) const
{
    //TODO - DONE
    // filling in input value not prev declared
    *sampledType = this->type;

    // generate wi ray based on warp functions from hw3 for uniform distribution within the hemisphere
    *wi = WarpFunctions::squareToHemisphereCosine(u);
    // *wi = WarpFunctions::squareToHemisphereUniform(u);
    *pdf = this->Pdf(wo, *wi);

    if (wo.z < 0) { wi->z *= -1.0f; }

    return this->f(wo, *wi);
}

float LambertBRDF::Pdf(const Vector3f &wo, const Vector3f &wi) const
{
    //TODO - DONE
    float result = WarpFunctions::squareToHemisphereCosinePDF(wi);
    if(std::isnan(result))
    {
        std::cout << "";
    }
    // return WarpFunctions::squareToHemisphereUniformPDF(wi);
    return result;
}
