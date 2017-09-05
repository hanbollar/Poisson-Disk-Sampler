#include "squareplane.h"

float SquarePlane::Area() const
{
    //TODO - DONE
    // incorporating scale:
    float area = transform.getScale().x * transform.getScale().y;
    return area;
}

bool SquarePlane::Intersect(const Ray &ray, Intersection *isect) const
{
    //Transform the ray
    Ray r_loc = ray.GetTransformedCopy(transform.invT());

    //Ray-plane intersection
    float t = glm::dot(glm::vec3(0,0,1), (glm::vec3(0.5f, 0.5f, 0) - r_loc.origin)) / glm::dot(glm::vec3(0,0,1), r_loc.direction);
    Point3f P = Point3f(t * r_loc.direction + r_loc.origin);
    //Check that P is within the bounds of the square
    if(t > 0 && P.x >= -0.5f && P.x <= 0.5f && P.y >= -0.5f && P.y <= 0.5f)
    {
        InitializeIntersection(isect, t, P);
        return true;
    }
    return false;
}

void SquarePlane::ComputeTBN(const Point3f &P, Normal3f *nor, Vector3f *tan, Vector3f *bit) const
{
    *nor = glm::normalize(transform.invTransT() * Vector3f(0.0f, 0.0f, 1.0f));

    *tan = glm::normalize(transform.T3() * Vector3f(1.0f, 0.0f, 0.0f));
    *bit = glm::normalize(transform.T3() * Vector3f(0.0f, 1.0f, 0.0f));
}


Point2f SquarePlane::GetUVCoordinates(const Point3f &point) const
{
    return Point2f(point.x + 0.5f, point.y + 0.5f);
}

Intersection SquarePlane::Sample(const Point2f &xi, Float *pdf) const {
    Intersection i = Intersection();

    Point3f p = Point3f(xi.x - 0.5f, xi.y - 0.5f, 0.0f);
    Point3f worldSpace_p = Point3f(transform.T() * glm::vec4(p, 1.0f));

    i.point = worldSpace_p;
    i.normalGeometric = glm::normalize(transform.invTransT() * Normal3f(0.0f, 0.0f, 1.0f));
    *pdf = 1.0f / Area();

    return i;
}

Bounds3f SquarePlane::WorldBound() const {
    // make bounds with min and max of current obj in world space and call Bound3f's apply to make proper
    // bounding box of this region

    // start with min and max in obj space then transform those into world space for proper values
    // note: obj space center is at origin

    // BECAUSE SQUARE PLANE: Z == 0

    Point3f min = Point3f(-0.5f, -0.5f, 0.0f);
    Point3f max = Point3f(0.5f, 0.5f, 0.0f);

    // to world space
    Bounds3f b = Bounds3f(min, max);
    b.Apply(transform);

    return b;
}

