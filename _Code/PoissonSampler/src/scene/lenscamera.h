#ifndef LENSCAMERA_H
#define LENSCAMERA_H

#include "camera.h"
#include "samplers/sampler.h"
#include "warpfunctions.h"

class LensCamera : public Camera
{
public:
    LensCamera();
    LensCamera(Sampler* samp); // initializes lensRadius and focalDistance to 1.0f
    LensCamera(float lensr, float focald, Sampler* samp);

    LensCamera(unsigned int w, unsigned int h, float lensr, float focald, Sampler* samp);
    LensCamera(unsigned int w, unsigned int h, const Vector3f &e, const Vector3f &r, const Vector3f &worldUp,
                           float lensr, float focald, Sampler* samp);

    LensCamera(const LensCamera &c, Sampler* samp);

    void CopyAttributes(const LensCamera &c);

    Ray Raycast(Point2f &pt) const;
    Ray Raycast(float x, float y) const;
    Ray RaycastNDC(float ndc_x, float ndc_y) const;

private:
    Float lensRadius, focalDistance;
    Sampler* sample;
};

#endif // LENSCAMERA_H
