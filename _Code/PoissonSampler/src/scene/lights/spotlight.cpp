#include "spotlight.h"

#define 	M_PI   3.14159265358979323846	/* pi */

Color3f SpotLight::Sample_Li(const Intersection &ref, const Point2f &xi,
                                     Vector3f *wiW, Float *pdf) const {

    *wiW = glm::normalize(pLight - ref.point);
    *pdf = 1.f;

    float fallOff = Falloff(-1.0f * (*wiW));

    Color3f col = emittedLight * fallOff / glm::distance2(pLight, ref.point);

    return col;
}

float SpotLight::Pdf_Li(const Intersection &ref, const Vector3f &wi) const {
    return 0.0f;
}

Point3f SpotLight::getOrig(std::shared_ptr<Sampler> samp) {
    return this->transform.position();
}

Vector3f SpotLight::getDir(std::shared_ptr<Sampler> samp, Intersection* i) {
//    float rad = this->cosTotalWidth;
//    Vector3f dir = this->transform.T3() * Vector3f(samp->Get2D() * (float)M_PI * rad * rad, 1.0f);
//    return dir - orig; //this->getOrig(samp);

//    i->point = this->shape.get()->Sample(samp->Get2D(), new Float());
//    Vector3f dir = this->transform.T3() * Vector3f(samp->Get2D() * (float)M_PI * rad * rad, 1.0f) - i->point;
//    return i->SpawnRay(dir).direction;

    Vector3f dir = WarpFunctions::squareToSphereUniform(samp->Get2D());
    i->point = this->pLight;
    dir = (transform.T3() * dir) - i->point;
    dir *= this->Falloff(dir);

    return i->SpawnRay(dir).direction;
}

float SpotLight::Falloff(const Vector3f &w) const {
    glm::mat4x4 worldToLight = transform.invT();

    Vector3f wl = glm::vec3( glm::normalize(worldToLight * glm::vec4(w, 0.0f)) );
    Float cosTheta = wl.z;
    if (cosTheta < cosTotalWidth) {
        return 0.0f;
    }

    if (cosTheta > cosFalloffStart) {
        return 1.0f;
    }

    // Compute falloff inside spotlight cone
    float delta = (cosTheta - cosTotalWidth) / (cosFalloffStart - cosTotalWidth);

    float val = (delta * delta) * (delta * delta);

    return val;
}

Color3f SpotLight::L(const Intersection &isect, const Vector3f &w) const {
    return emittedLight;
}

Color3f SpotLight::powerVal() {
    Color3f lit = emittedLight * 2.0f * (float)M_PI * (1.0f - 0.5f * (cosFalloffStart + cosTotalWidth));
    return lit;
}

