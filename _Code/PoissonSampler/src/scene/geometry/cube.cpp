#include "cube.h"
#include <iostream>

float Cube::Area() const
{
    //TODO
    return 0;
}

// Returns +/- [0, 2]
int GetFaceIndex(const Point3f& P)
{
    int idx = 0;
    float val = -1;
    for(int i = 0; i < 3; i++){
        if(glm::abs(P[i]) > val){
            idx = i * glm::sign(P[i]);
            val = glm::abs(P[i]);
        }
    }
    return idx;
}

Normal3f GetCubeNormal(const Point3f& P)
{
    int idx = glm::abs(GetFaceIndex(Point3f(P)));
    Normal3f N(0,0,0);
    N[idx] = glm::sign(P[idx]);
    return N;
}


bool Cube::Intersect(const Ray& r, Intersection* isect) const
{
    //Transform the ray
    Ray r_loc = r.GetTransformedCopy(transform.invT());

    float t_n = -1000000;
    float t_f = 1000000;
    for(int i = 0; i < 3; i++){
        //Ray parallel to slab check
        if(r_loc.direction[i] == 0){
            if(r_loc.origin[i] < -0.5f || r_loc.origin[i] > 0.5f){
                return false;
            }
        }
        //If not parallel, do slab intersect check
        float t0 = (-0.5f - r_loc.origin[i])/r_loc.direction[i];
        float t1 = (0.5f - r_loc.origin[i])/r_loc.direction[i];
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
    if(t_n < t_f)
    {
        float t = t_n > 0 ? t_n : t_f;
        if(t < 0)
            return false;
        //Lastly, transform the point found in object space by T
        glm::vec4 P = glm::vec4(r_loc.origin + t*r_loc.direction, 1);
        InitializeIntersection(isect, t, Point3f(P));
        return true;
    }
    else{//If t_near was greater than t_far, we did not hit the cube
        return false;
    }
}


void Cube::ComputeTBN(const Point3f& P, Normal3f* nor, Vector3f* tan, Vector3f* bit) const
{
    *nor = glm::normalize(transform.invTransT() * GetCubeNormal(P));
    //Computing tangent and bitangent vectors based on the particular face

    Vector3f x_axis = Vector3f(1.0f, 0.0f, 0.0f);
    Vector3f y_axis = Vector3f(0.0f, 1.0f, 0.0f);
    Vector3f z_axis = Vector3f(0.0f, 0.0f, 1.0f);

    Vector3f x = glm::normalize(transform.T3() * x_axis);
    Vector3f y = glm::normalize(transform.T3() * y_axis);
    Vector3f z = glm::normalize(transform.T3() * z_axis);

    Vector3f x_axis_test = Vector3f( glm::normalize(this->transform.invTransT() * x_axis) );
    Vector3f y_axis_test = Vector3f( glm::normalize(this->transform.invTransT() * y_axis) );
    Vector3f z_axis_test = Vector3f( glm::normalize(this->transform.invTransT() * z_axis) );

    if (*nor == z_axis_test) {
        *tan = -x_axis;
        *bit = -y_axis;
    } else if (*nor == -z_axis_test) {
        *tan = x_axis;
        *bit = -y_axis;
    } else if (*nor == y_axis_test) {
        *tan = x_axis;
        *bit = -z_axis;
    } else if (*nor == -y_axis_test) {
        *tan = x_axis;
        *bit = z_axis;
    } else if (*nor == x_axis_test) {
        *tan = -z_axis;
        *bit = -y_axis;
    } else if (*nor == -x_axis_test) {
        *tan = z_axis;
        *bit = -y_axis;
    } else {
        *tan = Vector3f(-INFINITY, -INFINITY, -INFINITY);
        *bit = Vector3f(-INFINITY, -INFINITY, -INFINITY);
    }
}

glm::vec2 Cube::GetUVCoordinates(const glm::vec3 &point) const
{
    glm::vec3 abs = glm::min(glm::abs(point), 0.5f);
    glm::vec2 UV;//Always offset lower-left corner
    if(abs.x > abs.y && abs.x > abs.z)
    {
        UV = glm::vec2(point.z + 0.5f, point.y + 0.5f)/3.0f;
        //Left face
        if(point.x < 0)
        {
            UV += glm::vec2(0, 0.333f);
        }
        else
        {
            UV += glm::vec2(0, 0.667f);
        }
    }
    else if(abs.y > abs.x && abs.y > abs.z)
    {
        UV = glm::vec2(point.x + 0.5f, point.z + 0.5f)/3.0f;
        //Left face
        if(point.y < 0)
        {
            UV += glm::vec2(0.333f, 0.333f);
        }
        else
        {
            UV += glm::vec2(0.333f, 0.667f);
        }
    }
    else
    {
        UV = glm::vec2(point.x + 0.5f, point.y + 0.5f)/3.0f;
        //Left face
        if(point.z < 0)
        {
            UV += glm::vec2(0.667f, 0.333f);
        }
        else
        {
            UV += glm::vec2(0.667f, 0.667f);
        }
    }
    return UV;
}

Intersection Cube::Sample(const Point2f &xi, Float *pdf) const {
    // TODO
    return Intersection();
}

Bounds3f Cube::WorldBound() const {
    // make bounds with min and max of current obj in world space and call Bound3f's apply to make proper
    // bounding box of this region

    // start with min and max in obj space then transform those into world space for proper values
    // note: obj space center is at origin
    Point3f min = Point3f(-0.5f, -0.5f, -5.0f);
    Point3f max = Point3f(0.5f, 0.5f, 5.0f);

    // to world space
    Bounds3f b = Bounds3f(min, max);
    b.Apply(transform);

    return b;
}
