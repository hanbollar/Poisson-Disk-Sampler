#include "diffusearealight.h"

#define 	M_PI   3.14159265358979323846	/* pi */

Color3f DiffuseAreaLight::L(const Intersection &isect, const Vector3f &w) const
{
    // if not two sided and  w not in same dir as surface normal then return zero color
    if (! twoSided) {
        if (glm::dot(w, isect.normalGeometric) < 0) {
            return Color3f(0.0f);
        }
    }

    // otherwise return color based on intersection
    return emittedLight;
}

Color3f DiffuseAreaLight::powerVal() {
    return emittedLight * area * (float)M_PI;
}

Point3f DiffuseAreaLight::getOrig(std::shared_ptr<Sampler> samp) {
    Intersection isx = Intersection();
    float pdf = 0.0f;

    isx = this->shape.get()->Sample(samp->Get2D(), &pdf);
    return isx.point;

    //return this->transform.position() * Vector3f(samp->Get2D(), 1.0f) * this->area;
}

Vector3f DiffuseAreaLight::getDir(std::shared_ptr<Sampler> samp, Intersection* i) {
    *i = this->shape.get()->Sample(samp->Get2D(), new Float());
    Vector3f dir = (this->transform.T3() * WarpFunctions::squareToHemisphereCosine(samp->Get2D()) ) - i->point;
    return i->SpawnRay(dir).direction;
}

Color3f DiffuseAreaLight::Sample_Li(const Intersection &ref, const Point2f &xi,
                                     Vector3f *wiW, Float *pdf) const {

    // Get an Intersection on the surface of its Shape by invoking shape->Sample.
    Intersection isxOnObj = shape->Sample(ref, xi, pdf);

    // pdf is 0 or found intersection is same as given
    if (pdf == 0 || (ref.point == isxOnObj.point)) {
        return Color3f(0.0f);
    }

    *wiW = glm::normalize(isxOnObj.point - ref.point);

    // evaluate light emitted along the ray from our Shape's surface to the reference point
    //    Return the light emitted along ωi from our intersection point.
    return L(isxOnObj, -(*wiW));
}

float DiffuseAreaLight::Pdf_Li(const Intersection &ref, const Vector3f &wi) const {
    // returns the PDF of the light's Shape given an Intersection from which the light is being viewed
    // and a viewing direction wi.
//    float pdf = 0.0f;

    Ray rayAlongWi = ref.SpawnRay(wi);
    Intersection isxLightToObj = Intersection();
    this->shape->Intersect(rayAlongWi, &isxLightToObj);

    Vector3f lightToObj = isxLightToObj.point - ref.point;

    float rSqrd = glm::length2(lightToObj);
    lightToObj = glm::normalize(lightToObj);
    float cosTheta = glm::abs(glm::dot(isxLightToObj.normalGeometric, -lightToObj));

    //std::cout<<"cosTheta: " << cosTheta<<std::endl;
    if (cosTheta == 0) {
        // to prevent dividing by zero
        return 0.0f; // pdf == 0
    } else {
        return rSqrd / (cosTheta * this->area); // pdf == rSqrd / cosTheta
    }
}
