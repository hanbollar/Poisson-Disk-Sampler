#include "bounds.h"

bool Bounds3f::Intersect(const Ray &r, float* t) const
{
    if (min.x > max.x) return false;

    // do intersection based on cube's intersection function
    Ray r_loc = r;

    float t_n = -INFINITY;
    float t_f = INFINITY;
    for(int i = 0; i < 3; i++) {
        //Ray parallel to slab check
        if(r_loc.direction[i] == 0){
            // if parrallel then the ray origin must be within for the following values
            //      so if not then false
            if(r_loc.origin[i] < min[i] || r_loc.origin[i] > max[i]){
                return false;
            }
        }

        //If not parallel, do slab intersect check
        float t0 = (min[i] - r_loc.origin[i])/r_loc.direction[i];
        float t1 = (max[i] - r_loc.origin[i])/r_loc.direction[i];

        // set tnear and tfar to proper vals
        if(t0 > t1){
            float temp = t1;
            t1 = t0;
            t0 = temp;
        }
        if(t0 > t_n){
            t_n = t0;
        }
        if(t1 < t_f){
            t_f = t1;
        }
    }

    if(t_n <= t_f)
    {
        // assign t to proper value
//        *t = t_n > 0 ? t_n : t_f;
        bool within =  (r_loc.origin.x <= max.x && r_loc.origin.x >= min.x
                        && r_loc.origin.y <= max.y && r_loc.origin.y >= min.y
                        && r_loc.origin.z <= max.z && r_loc.origin.y >= min.z);

        if ((t_n < 0 && within) || t_n > 0) {
            *t = t_n;
        } else {
            *t = t_f;
        }

        // if t is neg - this is only valid isect iff the ray's origin is within the bb
        bool returnVal = ((*t < 0 && within) || *t > 0);
        return returnVal;
    }
    //If t_near was greater than t_far, we did not hit the cube
    return false;
}

Bounds3f Bounds3f::Apply(const Transform &tr)
{
    glm::mat4 mat = tr.T();

    // all the points
    glm::vec3 p0 = glm::vec3(mat * glm::vec4(Point3f(min.x, min.y, min.z), 1.0f));
    glm::vec3 p1 = glm::vec3(mat * glm::vec4(Point3f(min.x, min.y, max.z), 1.0f));
    glm::vec3 p2 = glm::vec3(mat * glm::vec4(Point3f(min.x, max.y, max.z), 1.0f));
    glm::vec3 p3 = glm::vec3(mat * glm::vec4(Point3f(max.x, max.y, max.z), 1.0f));
    glm::vec3 p4 = glm::vec3(mat * glm::vec4(Point3f(min.x, max.y, min.z), 1.0f));
    glm::vec3 p5 = glm::vec3(mat * glm::vec4(Point3f(max.x, max.y, min.z), 1.0f));
    glm::vec3 p6 = glm::vec3(mat * glm::vec4(Point3f(max.x, min.y, min.z), 1.0f));
    glm::vec3 p7 = glm::vec3(mat * glm::vec4(Point3f(max.x, min.y, max.z), 1.0f));

    // transform given bb by tMatrix
    Bounds3f result = Union(Union(Union(Union(Union(Union(Bounds3f(p0, p1), p2), p3), p4), p5), p6), p7);

    // set to our min and max values
    min = result.min;
    max = result.max;

    return result;
}

float Bounds3f::SurfaceArea() const
{
    if (min.x > max.x) return 0.0f;

    float x = fabs(max.x - min.x);
    float y = fabs(max.y - min.y);
    float z = fabs(max.z - min.z);
    float SA = 2.0f*(x*y + x*z + y*z);
    return SA;
}

Bounds3f Union(const Bounds3f& b1, const Bounds3f& b2)
{
    if (b1.min.x > b1.max.x) { return b2; }
    if (b2.min.x > b2.max.x) { return b1; }
    return Bounds3f(Point3f(std::min(b1.min.x, b2.min.x),
                            std::min(b1.min.y, b2.min.y),
                            std::min(b1.min.z, b2.min.z)),
                    Point3f(std::max(b1.max.x, b2.max.x),
                            std::max(b1.max.y, b2.max.y),
                            std::max(b1.max.z, b2.max.z)));
}

Bounds3f Union(const Bounds3f& b1, const Point3f& p)
{
    if (b1.min.x > b1.max.x) { return Bounds3f(p); }
    return Bounds3f(Point3f(std::min(b1.min.x, p.x),
                            std::min(b1.min.y, p.y),
                            std::min(b1.min.z, p.z)),
                    Point3f(std::max(b1.max.x, p.x),
                            std::max(b1.max.y, p.y),
                            std::max(b1.max.z, p.z)));
}

Bounds3f Union(const Bounds3f& b1, const glm::vec4& p)
{
    // the valid check for b1 not included here
    return Union(b1, Point3f(p));
}

//added in 700 proj with fanfu//
Bounds3f* Union(Bounds3f* b1, Point3f p) {
    if (b1->min.x > b1->max.x) { return new Bounds3f(p); }
    Point3f minP = Point3f(std::min(b1->min.x, p.x),
                          std::min(b1->min.y, p.y),
                          std::min(b1->min.z, p.z));
    Point3f maxP = Point3f(std::max(b1->max.x, p.x),
                           std::max(b1->max.y, p.y),
                           std::max(b1->max.z, p.z));
    return new Bounds3f(minP, maxP);
}
