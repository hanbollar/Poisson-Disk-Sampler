#include "specularbrdf.h"

Color3f SpecularBRDF::f(const Vector3f &wo, const Vector3f &wi) const
{
    return Color3f(0.f);
}


float SpecularBRDF::Pdf(const Vector3f &wo, const Vector3f &wi) const
{
    return 0.f;
}

Color3f SpecularBRDF::Sample_f(const Vector3f &wo, Vector3f *wi, const Point2f &sample, Float *pdf, BxDFType *sampledType) const
{
    //TODO-DONE

    // since specular, wi is always wo reflected across the normal
    // note this is always called in tangent space so the normal is always (0,0,1)
    *sampledType = this->type;
    *wi = Vector3f(-wo.x, -wo.y, wo.z); // bc reflecting across the z axis
    //*wi = glm::reflect(wo, Vector3f(0.0, 0.0, 1.0));
    *pdf = 1.0f; //because actually got the wi reflected this time

    Vector3f color(0.0f);
    // fspecular = (inherent color of material) * (1/cos(wi)) * Fr
   // float cosTheta = wi->z;
    Color3f currCol = this->R;
    float abs = fabs(wi->z);
    Color3f fresnelEval = this->fresnel->Evaluate(wo.z);

//    if (abs == 0) {
//        color = 0.0f;
//    }

    color = fresnelEval * currCol / abs;

//    if (isnan(color.x) || isnan(color.y) || isnan(color.z)) {
//        std::cout<<std::endl;
//    }

    return color;
}
