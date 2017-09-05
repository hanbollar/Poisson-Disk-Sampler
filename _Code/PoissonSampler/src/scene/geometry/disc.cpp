#include "disc.h"

#define 	M_PI   3.14159265358979323846	/* pi */

float Disc::Area() const
{
    //TODO - DONE

    return M_PI * transform.getScale().x * transform.getScale().y;
}

bool Disc::Intersect(const Ray &ray, Intersection *isect) const
{
    //Transform the ray
    Ray r_loc = ray.GetTransformedCopy(transform.invT());

    //Ray-plane intersection
    float t = glm::dot(glm::vec3(0,0,1), (glm::vec3(0.5f, 0.5f, 0) - r_loc.origin)) / glm::dot(glm::vec3(0,0,1), r_loc.direction);
    Point3f P = Point3f(t * r_loc.direction + r_loc.origin);
    //Check that P is within the bounds of the disc (not bothering to take the sqrt of the dist b/c we know the radius)
    float dist2 = (P.x * P.x + P.y * P.y);
    if(t > 0 && dist2 <= 1.f)
    {
        InitializeIntersection(isect, t, P);
        return true;
    }
    return false;
}

void Disc::ComputeTBN(const Point3f &P, Normal3f *nor, Vector3f *tan, Vector3f *bit) const
{
    *nor = glm::normalize(transform.invTransT() * Vector3f(0.0f, 0.0f, 1.0f));

    *tan = glm::normalize(transform.T3() * Vector3f(1.0f, 0.0f, 0.0f));
    *bit = glm::normalize(transform.T3() * Vector3f(0.0f, 1.0f, 0.0f));
}


Point2f Disc::GetUVCoordinates(const Point3f &point) const
{
    return glm::vec2((point.x + 1)/2.f, (point.y + 1)/2.f);
}

Intersection Disc::Sample(const Point2f &xi, Float *pdf) const {
    Intersection i = Intersection();
    i.point = Vector3f(transform.T() * glm::vec4(WarpFunctions::squareToDiskConcentric(xi), 1.0f));//Vector3f(xi.x - 0.5f, xi.y - 0.5f, 0.0f); //0.5f offset for xi bet [0,1] and sqPln [-.5, .5]
    i.normalGeometric = Vector3f(glm::normalize(transform.invTransT() * Vector3f(0.0f, 0.0f, 1.0f)));
    *pdf = 1.0f/(this->Area()); // <-- implemented one level higher : in Shape::Sample

    return i;
}

Bounds3f Disc::WorldBound() const {
    // make bounds with min and max of current obj in world space and call Bound3f's apply to make proper
    // bounding box of this region

    // start with min and max in obj space then transform those into world space for proper values
    // note: obj space center is at origin

    // BECAUSE DISC Z == 0
    Point3f min = Point3f(-0.5f, -0.5f, 0.0f);
    Point3f max = Point3f(0.5f, 0.5f, 0.0f);

    // to world space
    Bounds3f b = Bounds3f(min, max);
    b.Apply(transform);

    return b;
}

