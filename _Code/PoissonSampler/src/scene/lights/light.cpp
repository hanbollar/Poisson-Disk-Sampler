#include "light.h"

Color3f Light::Le(const Ray &r) const
{
    return Color3f(0.f);
}

Color3f Light::Sample_Li(const Intersection &ref, const Point2f &xi,
                                     Vector3f *wi, Float *pdf) const {
    // TODO!!!
    return Color3f(1.0f);
}

