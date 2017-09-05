#include "lenscamera.h"

LensCamera::LensCamera() : LensCamera(new Sampler(100, 0))
{}

LensCamera::LensCamera(Sampler* samp) : LensCamera(400, 400, -1.0f, 1.0f, samp) {
    look = Vector3f(0,0,-1);
    up = Vector3f(0,1,0);
    right = Vector3f(1,0,0);
}

LensCamera::LensCamera(float lensr, float focald, Sampler* samp)
    :  Camera(400, 400, Vector3f(0,0,10), Vector3f(0,0,0), Vector3f(0,1,0)),
      lensRadius(lensr), focalDistance(focald), sample(samp)
{ }

LensCamera::LensCamera(unsigned int w, unsigned int h, float lensr, float focald, Sampler* samp)
    : lensRadius(lensr), focalDistance(focald), Camera(w, h, Vector3f(0,0,10), Vector3f(0,0,0), Vector3f(0,1,0)), sample(samp)
{ }

LensCamera::LensCamera(unsigned int w, unsigned int h, const Vector3f &e, const Vector3f &r, const Vector3f &worldUp,
                       float lensr, float focald, Sampler* samp)
    : Camera(w, h, e, r, worldUp), lensRadius(lensr), focalDistance(focald), sample(samp)
{ }

LensCamera::LensCamera(const  LensCamera &c, Sampler* samp)
    : Camera(c), lensRadius(1.0f), focalDistance(1.0f), sample(samp)
{ }

void LensCamera::CopyAttributes(const LensCamera &c) {
    fovy = c.fovy;
    near_clip = c.near_clip;
    far_clip = c.far_clip;
    eye = c.eye;
    ref = c.ref;
    look = c.look;
    up = c.up;
    right = c.right;
    width = c.width;
    height = c.height;
    aspect = c.aspect;
    V = c.V;
    H = c.H;

    lensRadius = c.lensRadius;
    focalDistance = c.focalDistance;
}

Ray LensCamera::Raycast(Point2f &pt) const
{
    return Raycast(pt.x, pt.y);
}

Ray LensCamera::Raycast(float x, float y) const
{
    float ndc_x = (2.f*x/width - 1);
    float ndc_y = (1 - 2.f*y/height);
    return RaycastNDC(ndc_x, ndc_y);
}

Ray LensCamera::RaycastNDC(float ndc_x, float ndc_y) const
{
    //Modify ray for depth of field⟩ ≡ if (lensRadius > 0) {
    //Sample point on lens 374⟩
    //Compute point on plane of focus 375⟩ ⟨Update ray for effect of lens

    glm::vec3 P = ref + ndc_x*H + ndc_y*V;
    Ray result(eye, glm::normalize(P - eye));

    Vector2f samp = sample->Get2D();
    // modify ray for depth of field
    if (lensRadius > 0) {
        //Sample point on lens
        Point3f pLens = lensRadius * WarpFunctions::squareToDiskConcentric(samp);

        //Compute point on plane of focus
        Point3f dir = result.direction;
        Float ft = focalDistance / dir.z;
        Point3f pFocus = dir * (ft);

        //Update ray for effect of lens
        result.origin = Point3f(pLens.x, pLens.y, 0.0f);
        result.direction = glm::normalize(pFocus - result.origin);
    }

    return result;
}
