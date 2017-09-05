#include "pointlight.h"
#include <math.h>

#define 	M_PI   3.14159265358979323846	/* pi */

Color3f PointLight::Sample_Li(const Intersection &ref, const Point2f &xi,
                                     Vector3f *wiW, Float *pdf) const {

    *wiW = glm::normalize(pLight - ref.point);
    *pdf = 1.f;

    Color3f col = emittedLight / glm::distance2(pLight, ref.point);

    return col;
}

float PointLight::Pdf_Li(const Intersection &ref, const Vector3f &wi) const {
    return 0.0f;
}

Color3f PointLight::L(const Intersection &isect, const Vector3f &w) const {
    return emittedLight;
}

Color3f PointLight::powerVal() {
    return 4.0f * (float)M_PI * emittedLight;
}

Point3f PointLight::getOrig(std::shared_ptr<Sampler> samp) {
    return this->pLight;
}

Vector3f PointLight::getDir(std::shared_ptr<Sampler> samp, Intersection* i) {
//    Point3f p = this->transform.position();
//    i->point = p;
//    Vector3f dir = (this->transform.T3() * WarpFunctions::squareToHemisphereCosine(samp->Get2D()) ) - i->point;
//    return i->SpawnRay(dir).direction;

    Vector3f dir = WarpFunctions::squareToSphereUniform(samp->Get2D());
    i->point = this->pLight;
    dir = (transform.T3() * dir) - i->point;

    return i->SpawnRay(dir).direction;
}
