#include "specularbTdf.h"

Color3f SpecularBTDF::f(const Vector3f &wo, const Vector3f &wi) const
{
    return Color3f(0.f);
}


float SpecularBTDF::Pdf(const Vector3f &wo, const Vector3f &wi) const
{
    return 0.f;
}

Color3f SpecularBTDF::Sample_f(const Vector3f &wo, Vector3f *wi, const Point2f &sample, Float *pdf, BxDFType *sampledType) const
{
    //TODO-DONE

//    // since specular, wi is always wo refracted through the surface
//    // note this is always called in tangent space so the normal is always (0,0,1)
    *sampledType = this->type;
    *pdf = 1.0f; //because actually got the wi refracted properly this time

    // checking if need to switch indices of refraction
    float cosThetaI = wo.z;
    float ei = this->etaA;
    float et = this->etaB;
    if (cosThetaI <= 0.0f) {
        std::swap(ei, et);
    }
    float index = ei/et;

    // handling case of total internal reflection based on snell's law
    bool noInternalRef = Refract(wo, Faceforward(Normal3f(0.0f, 0.0f, 1.0f), wo), index, wi);//glm::refract(wo, Vector3f(0.0f, 0.0f, 1.0f));//Vector3f(-wo.x, -wo.y, wo.z); // bc refracting through the surface
    // if total internal reflection then return black
    if (!noInternalRef) {
        //*wi = glm::refract(wo, Vector3f(0.0f, 0.0f, 1.0f), index);
        return Color3f(0.0f);
    }

    Vector3f color(0.0f);
    // fspecular = (inherent color of material) * (1/cos(wi)) * Fr
    // ftrans = (1 - Fr)*(color)
    // float cosTheta = wi->z;
    Color3f currCol = this->T;
    float abs = fabsf(wi->z);

    Color3f fresnelEval = this->fresnel->Evaluate(wo.z);
    color = (Color3f(1.0f) - fresnelEval) * currCol / abs;

    return color;
}
