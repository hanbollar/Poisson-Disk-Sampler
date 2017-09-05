#include "shape.h"
#include <QDateTime>

pcg32 Shape::colorRNG = pcg32(QDateTime::currentMSecsSinceEpoch());


void Shape::InitializeIntersection(Intersection *isect, float t, Point3f pLocal) const
{
    isect->point = Point3f(transform.T() * glm::vec4(pLocal, 1));
    ComputeTBN(pLocal, &(isect->normalGeometric), &(isect->tangent), &(isect->bitangent));
    isect->uv = GetUVCoordinates(pLocal);
    isect->t = t;
}

Intersection Shape::Sample(const Intersection &ref, const Point2f &xi, float *pdf) const
{
    //This function must invoke the two-input Sample, to be implemented by subclasses, and generate a
    //      ωi from the resulting Intersection.
    Intersection found = Sample(xi, pdf);

    // convert pdf obtained from pdf with respect to SA to pdf with respect to solid angle at ref point of intersection
    // - note if ever needing to divide by zero just set pdf to 0
    // ^^ r^2 / (cos(theta) * area of light shape)
    Vector3f wi = found.point - ref.point;
    float rSqrd = glm::length2(wi);
    wi = glm::normalize(wi);
    float cosTheta = glm::abs(glm::dot(found.normalGeometric, -wi));

    //std::cout<<"cosTheta: " << cosTheta<<std::endl;
    if (cosTheta == 0) {
        // to prevent dividing by zero
        *pdf = 0.0f;
    } else {
        *pdf *= rSqrd / (cosTheta);
    }

    return found;
}

