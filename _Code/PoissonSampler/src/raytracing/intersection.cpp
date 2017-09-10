#include <raytracing/intersection.h>

Intersection::Intersection():
    point(Point3f(0)),
    normalGeometric(Normal3f(0)),
    uv(Point2f(0)),
    t(INFINITY),
    tangent(0.f), bitangent(0.f)
{}

Ray Intersection::SpawnRay(const Vector3f &d) const
{
    Vector3f originOffset = normalGeometric * RayEpsilon;
    // Make sure to flip the direction of the offset so it's in
    // the same general direction as the ray direction
    originOffset = (glm::dot(d, normalGeometric) > 0) ? originOffset : -originOffset;
    Point3f o(this->point + originOffset);
    return Ray(o, d);
}
