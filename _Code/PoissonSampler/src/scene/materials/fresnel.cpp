#include "fresnel.h"

Color3f FresnelDielectric::Evaluate(float cosThetaI) const
{
    //TODO
    //return Color3f(0.f);

    cosThetaI = glm::clamp(cosThetaI, -1.0f, 1.0f);

    // check if need to switch indices of refraction <- should be done already in Sample_f
    float ei = this->etaI;
    float et = this->etaT;
    if (cosThetaI <= 0.0f) {
        std::swap(ei, et);
        cosThetaI = std::abs(cosThetaI);
    }

    float index = ei/et;

    // computing cosThetaT using snell's law
    // snell's law :
    //      describes the relationship between the angles of incidence and refraction, when
    //      referring to light or other waves passing through a boundary between two different
    //      isotropic media, such as water, glass, or air.
    float sinThetaI = std::sqrt(std::max(0.0f, 1.0f - cosThetaI * cosThetaI));
    float sinThetaT = std::sqrt(index * sinThetaI);
    float cosThetaT = std::sqrt(std::max(0.0f, 1.0f - sinThetaT * sinThetaT));

    // handle the case of total internal reflection
    if (sinThetaT >= 1.0f) {
        return 1.0f * Color3f(1.0f);
    }

    // following notes from class:
    //      etaT: n (index of refraction) of substance going into
    //      etaI: n (index of refraction) of substance leaving
    //      like if going from air to glass or something along those lines
    float r_parallel = ((et * cosThetaI) - (ei * cosThetaT))
                            / ((et * cosThetaI) + (ei * cosThetaT));
    float r_perpendicular = ((ei * cosThetaI) - (et * cosThetaT))
                            / ((ei * cosThetaI) + (et * cosThetaT));

    // Fr = (0.5) * (r|| ^2 + r⊥^2)
    float fr = 0.5f * (r_parallel * r_parallel + r_perpendicular * r_perpendicular);
    return fr * Color3f(1.0f);
}
