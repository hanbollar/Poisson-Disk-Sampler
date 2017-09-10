#include <scene/geometry/mesh.h>
#include <la.h>
#include <tinyobj/tiny_obj_loader.h>
#include <iostream>

Bounds3f Triangle::WorldBound() const
{
    // start with min and max in world space for triangle
    Bounds3f result = Union(Bounds3f(points[0], points[1]), points[2]);
    return result;
}

float Triangle::Area() const
{
    return glm::length(glm::cross(points[0] - points[1], points[2] - points[1])) * 0.5f;
}

Triangle::Triangle(const glm::vec3 &p1, const glm::vec3 &p2, const glm::vec3 &p3):
    Triangle(p1, p2, p3, glm::vec3(1), glm::vec3(1), glm::vec3(1), glm::vec2(0), glm::vec2(0), glm::vec2(0))
{
    for(int i = 0; i < 3; i++)
    {
        normals[i] = planeNormal;
    }
}


Triangle::Triangle(const glm::vec3 &p1, const glm::vec3 &p2, const glm::vec3 &p3, const glm::vec3 &n1, const glm::vec3 &n2, const glm::vec3 &n3):
    Triangle(p1, p2, p3, n1, n2, n3, glm::vec2(0), glm::vec2(0), glm::vec2(0))
{}


Triangle::Triangle(const glm::vec3 &p1, const glm::vec3 &p2, const glm::vec3 &p3, const glm::vec3 &n1, const glm::vec3 &n2, const glm::vec3 &n3, const glm::vec2 &t1, const glm::vec2 &t2, const glm::vec2 &t3)
{
    planeNormal = glm::normalize(glm::cross(p2 - p1, p3 - p2));
    points[0] = p1;
    points[1] = p2;
    points[2] = p3;
    normals[0] = n1;
    normals[1] = n2;
    normals[2] = n3;
    uvs[0] = t1;
    uvs[1] = t2;
    uvs[2] = t3;
}

void Triangle::create() {
    // actually handled in mesh's create?
}

float TriArea(const glm::vec3 &p1, const glm::vec3 &p2, const glm::vec3 &p3)
{
    return glm::length(glm::cross(p1 - p2, p3 - p2)) * 0.5f;
}

//Returns the interpolation of the triangle's three normals based on the point inside the triangle that is given.
Normal3f Triangle::GetNormal(const Point3f &P) const
{
    float A = TriArea(points[0], points[1], points[2]);
    float A0 = TriArea(points[1], points[2], P);
    float A1 = TriArea(points[0], points[2], P);
    float A2 = TriArea(points[0], points[1], P);
    return glm::normalize(normals[0] * A0/A + normals[1] * A1/A + normals[2] * A2/A);
}


//The ray in this function is not transformed because it was *already* transformed in Mesh::GetIntersection
bool Triangle::Intersect(const Ray& r, Intersection* isect) const
{
    //1. Ray-plane intersection
    float t =  glm::dot(planeNormal, (points[0] - r.origin)) / glm::dot(planeNormal, r.direction);
    if(t < 0) return false;

    glm::vec3 P = r.origin + t * r.direction;
    //2. Barycentric test
    float S = 0.5f * glm::length(glm::cross(points[0] - points[1], points[0] - points[2]));
    float s1 = 0.5f * glm::length(glm::cross(P - points[1], P - points[2]))/S;
    float s2 = 0.5f * glm::length(glm::cross(P - points[2], P - points[0]))/S;
    float s3 = 0.5f * glm::length(glm::cross(P - points[0], P - points[1]))/S;
    float sum = s1 + s2 + s3;

    if(s1 >= 0 && s1 <= 1 && s2 >= 0 && s2 <= 1 && s3 >= 0 && s3 <= 1 && fequal(sum, 1.0f)){
        isect->t = t;
        InitializeIntersection(isect, t, Point3f(P));
        return true;
    }
    return false;
}

void Triangle::InitializeIntersection(Intersection *isect, float t, Point3f pLocal) const
{
    isect->point = pLocal;
    isect->uv = GetUVCoordinates(pLocal);
    ComputeTriangleTBN(pLocal, &(isect->normalGeometric), &(isect->tangent), &(isect->bitangent), isect->uv);
    isect->t = t;
}

void Triangle::ComputeTBN(const Point3f &P, Normal3f *nor, Vector3f *tan, Vector3f *bit) const
{
    //Triangle uses ComputeTriangleTBN instead of this function.

    ComputeTriangleTBN(P, nor, tan, bit, this->GetUVCoordinates(P));
}

void Triangle::ComputeTriangleTBN(const Point3f &P, Normal3f *nor, Vector3f *tan, Vector3f *bit, const Point2f &uv) const
{
    *nor = GetNormal(P);
    CoordinateSystem( *nor, tan, bit);

    // NORMAL

//    *nor = GetNormal(P);

//    // TANGENTS FOR TRIANGLE: computing based on change in uvs in direction of other points on the triangle
//    Point3f p0 = points[0];
//    Point3f p1 = points[1];
//    Point3f p2 = points[2];
//    Point2f uv0 = uvs[0];
//    Point2f uv1 = uvs[1];
//    Point2f uv2 = uvs[2];
//    // POINT 0
//    // tangent for triangle point 0
//    Vector3f delP1 = p1 - p0;
//    Vector3f delP2 = p2 - p0;
//    Vector2f delUV1 = uv1 - uv0;
//    Vector2f delUV2 = uv2 - uv0;
//    Vector3f t0 = glm::normalize( (delUV2.y * delP1 - delUV1.y * delP2) / (delUV2.y * delUV1.x - delUV1.y * delUV2.x) );
//    // bitangent for triangle point 0
//    Vector3f b0 = glm::normalize( (delP2 - delUV2.x*t0) / delUV2.y );
//    // POINT 1
//    // tangent for triangle point 1
//    delP1 = p0 - p1;
//    delP2 = p2 - p1;
//    delUV1 = uv0 - uv1;
//    delUV2 = uv2 - uv1;
//    Vector3f t1 =  glm::normalize( (delUV2.y * delP1 - delUV1.y * delP2) / (delUV2.y * delUV1.x - delUV1.y * delUV2.x) );
//    // bitangent for triangle point 1
//    Vector3f b1 = glm::normalize( (delP2 - delUV2.x*t0) / delUV2.y );
//    // POINT 2
//    // tangent for triangle point 2
//    delP1 = p0 - p2;
//    delP2 = p1 - p2;
//    delUV1 = uv0 - uv2;
//    delUV2 = uv1 - uv2;
//    Vector3f t2 = glm::normalize( (delUV2.y * delP1 - delUV1.y * delP2) / (delUV2.y * delUV1.x - delUV1.y * delUV2.x) );
//    // bitangent for triangle point 2
//    Vector3f b2 = glm::normalize( (delP2 - delUV2.x*t0) / delUV2.y );

//    // TANGENTS AND BITANGENTS FOR INPUTED POINT IN THE TRIANGLE

//    // calculating values for U and for V for intersection on triangle

//    // start with point equation in terms of u and v to find the point where A, B, C are p0, p1, p2
//    // P = A + U * (C - A) + V * (B - A)
//    // note: v0 = C - A, v1 = B - A, v2 = P - A
//    // so we get that v2 = U * v0 + V * v1
//    // and from that we can use dot products and get the following system of equations
//    //  v2 dot v0 == U(v0 dot v0) + V(v1 dot v0)
//    //  v2 dot v1 == U(v0 dot v1) + V(v1 dot v1)
//    // solve for U and V and you get the following two equations:
//    // U = ((v1 dot v1)(v2 dot v0) - (v1 dot v0)(v2 dot v1)) / ((v0 dot v0)(v1 dot v1) - (v0 dot v1)(v1 dot v0))
//    // v = ((v0 dot v0)(v2 dot v1) - (v0 dot v1)(v2 dot v0)) / ((v0 dot v0)(v1 dot v1) - (v0 dot v1)(v1 dot v0))

//    // Set up
//    Vector3f v0 = p2 - p0;
//    Vector3f v1 = p1 - p0;
//    Vector3f v2 = P - p0;
//    // Compute dot products
//    float dot00 = glm::dot(v0, v0);
//    float dot01 = glm::dot(v0, v1);
//    float dot02 = glm::dot(v0, v2);
//    float dot11 = glm::dot(v1, v1);
//    float dot12 = glm::dot(v1, v2);
//    // Compute barycentric coordinates
//    float invD = 1.0f / (dot00 * dot11 - dot01 * dot01);
//    float U = (dot11 * dot02 - dot01 * dot12) * invD;
//    float V = (dot00 * dot12 - dot01 * dot02) * invD;

//    // using U and V to compute tangent and bitangent vectors at this point of intersection
//    *tan = t0*(1 - U - V) + t1*U + t2*V;
//    *bit = b0*(1 - U - V) + b1*U + b2*V;

    if (std::isnan(tan->x) || std::isnan(tan->y) || std::isnan(tan->z)) {
        throw;
    }
    if (std::isnan(bit->x) || std::isnan(bit->y) || std::isnan(bit->z)) {
        throw;
    }
    if (std::isnan(nor->x) || std::isnan(nor->y) || std::isnan(nor->z)) {
        throw;
    }
}


Intersection Triangle::Sample(const Point2f &xi, Float *pdf) const
{
    //TODO for extra credit
    return Intersection();
}


Point2f Triangle::GetUVCoordinates(const Point3f &point) const
{
    float A = TriArea(points[0], points[1], points[2]);
    float A0 = TriArea(points[1], points[2], point);
    float A1 = TriArea(points[0], points[2], point);
    float A2 = TriArea(points[0], points[1], point);
    return uvs[0] * A0/A + uvs[1] * A1/A + uvs[2] * A2/A;
}

void Mesh::LoadOBJ(const QStringRef &filename, const QStringRef &local_path, const Transform &transform)
{
    QString filepath = local_path.toString(); filepath.append(filename);
    std::vector<tinyobj::shape_t> shapes; std::vector<tinyobj::material_t> materials;
    std::string errors = tinyobj::LoadObj(shapes, materials, filepath.toStdString().c_str());
    std::cout << errors << std::endl;
    if(errors.size() == 0)
    {
        //Read the information from the vector of shape_ts
        for(unsigned int i = 0; i < shapes.size(); i++)
        {
            std::vector<float> &positions = shapes[i].mesh.positions;
            std::vector<float> &normals = shapes[i].mesh.normals;
            std::vector<float> &uvs = shapes[i].mesh.texcoords;
            std::vector<unsigned int> &indices = shapes[i].mesh.indices;
            for(unsigned int j = 0; j < indices.size(); j += 3)
            {
                glm::vec3 p1 = glm::vec3(transform.T() * glm::vec4(positions[indices[j]*3], positions[indices[j]*3+1], positions[indices[j]*3+2], 1));
                glm::vec3 p2 = glm::vec3(transform.T() * glm::vec4(positions[indices[j+1]*3], positions[indices[j+1]*3+1], positions[indices[j+1]*3+2], 1));
                glm::vec3 p3 = glm::vec3(transform.T() * glm::vec4(positions[indices[j+2]*3], positions[indices[j+2]*3+1], positions[indices[j+2]*3+2], 1));

                auto t = std::make_shared<Triangle>(p1, p2, p3);
                if(normals.size() > 0)
                {
                    glm::vec3 n1 = transform.invTransT() * glm::vec3(normals[indices[j]*3], normals[indices[j]*3+1], normals[indices[j]*3+2]);
                    glm::vec3 n2 = transform.invTransT() * glm::vec3(normals[indices[j+1]*3], normals[indices[j+1]*3+1], normals[indices[j+1]*3+2]);
                    glm::vec3 n3 = transform.invTransT() * glm::vec3(normals[indices[j+2]*3], normals[indices[j+2]*3+1], normals[indices[j+2]*3+2]);
                    t->normals[0] = n1;
                    t->normals[1] = n2;
                    t->normals[2] = n3;
                }
                if(uvs.size() > 0)
                {
                    glm::vec2 t1(uvs[indices[j]*2], uvs[indices[j]*2+1]);
                    glm::vec2 t2(uvs[indices[j+1]*2], uvs[indices[j+1]*2+1]);
                    glm::vec2 t3(uvs[indices[j+2]*2], uvs[indices[j+2]*2+1]);
                    t->uvs[0] = t1;
                    t->uvs[1] = t2;
                    t->uvs[2] = t3;
                }
                this->faces.append(t);
            }
        }
        std::cout << "" << std::endl;
        //TODO: .mtl file loading
    }
    else
    {
        //An error loading the OBJ occurred!
        std::cout << errors << std::endl;
    }
}
