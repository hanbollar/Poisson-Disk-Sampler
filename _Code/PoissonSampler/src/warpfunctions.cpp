#define _USE_MATH_DEFINES
#include "warpfunctions.h"
#include <math.h>

Point3f WarpFunctions::squareToDiskUniform(const Point2f &sample)
{
    // TODO - DONE
    // throw std::runtime_error("You haven't yet implemented uniform disk warping!");
    
    // given x,y cartesian coords
    // turn into polar properly r = √(ξ1), theta = 2ρi(ξ2)
    // sample is made of ξ1 and ξ2
    float r = sqrt(sample[0]);
    float theta = 2*Pi*(sample[1]);
    
    float finX = r*cos((double)theta);
    float finY = r*sin((double)theta);
    
    // returning cartesian
    return glm::vec3(finX, finY, 0.0f);
}

Point3f WarpFunctions::squareToDiskConcentric(const Point2f &sample)
{
    //TODO - DONE
    // throw std::runtime_error("You haven't yet implemented concentric disk warping!");
    
    // sample is made of x and y locs on the square
    
//    float r;
//    float phi;
    
//    float a = 2.0f*sample[0] - 1.0f;
//    float b = 2.0f*sample[1] - 1.0f;

//    if (a > -0.001 && a < 0.001) {
//        a = 0.001;
//    }
//    if (b > -0.001 && b < 0.001) {
//        b = 0.001;
//    }
    
//    if (a > -b) {                  // region 1 or 2
//        if (a > b) {              // region 1, also |a| > |b|
//            r = a;
//            phi = (Pi/4.0f) * (b/a);
//        } else {                    // region 2, also |b| > |a|
//            r = b;
//            phi = (Pi/4.0f) * (2.0f - (a/b));
//        }
//    } else {                        // region 3 or 4
//        if (a < b) {                // region 3, also |a| >= |b|, a != 0
//            r = -a;
//            phi = (Pi/4) * (4 + (b/a));
//        } else {
//            // region 4, |b| >= |a|, but a==0 and b==0 could occur.
//            r = -b;
//            if (b != 0) {
//                phi = (Pi/4) * (6 - (a/b));
//            } else {
//                phi = 0;
//            }
//        }
//    }

//    if (isnan(phi)) {
//        std::cout<<"PHI IS NAN"<<std::endl;
//    }
    
//    float u = r * cos(phi);
//    float v = r * sin(phi);

//    if (isnan(u)) {
//        std::cout<<"U. IS NAN"<<std::endl;
//    }
//    if (isnan(v)) {
//        std::cout<<"V. IS NAN"<<std::endl;
//    }
    
//    // returning cartesian
//    return glm::vec3(u, v, 0.0f);

    float phi, r, u, v;
    float a = 2*sample.x-1;
    float b = 2*sample.y-1;
    if (a > -b) {
        if (a > b) {
            r=a;
            phi = (Pi/4.f ) * (b/a);
        }
        else { // region 2, also |b| > |a|
            r = b;
            phi = (Pi/4.f) * (2.f - (a/b));
        }
    } else  {// region 3 or 4
        if (a < b) {// region 3, also |a| >= |b|, a != 0
            r = -a;
            phi = (Pi/4.f) * (4.f + (b/a));
        } else { // region 4, |b| >= |a|, but a==0 and b==0 could occur.
            r = -b;
            if (b != 0) {
                phi = (Pi/4.f) * (6.f - (a/b));
            } else {
                phi = 0;
            }
        }
    }
    u = r * std::cos(phi);
    v = r * std::sin(phi);

    return Point3f(u, v, 0.f);
}

float WarpFunctions::squareToDiskPDF(const Point3f &sample)
{
    //TODO - DONE
    return 1.0f/(Pi);
}

Point3f WarpFunctions::squareToSphereUniform(const Point2f &sample)
{
    // TODO - DONE
    // throw std::runtime_error("You haven't yet implemented uniform sphere warping!");
    
    // we start with:
    // x = sin(theta)cos(phi)
    // y = sin(theta)sin(phi)
    // z = 1 − 2cos(theta)
    // then we set  Let’s take theta = 2ρξ2 and phi = cos-1(ξ1)
    // so we get the following:
    //      x = cos(2ρξ2)√(1 − z^2)
    //      y = sin(2ρξ2)√(1 − z^2)
    //      z = 1 − 2ξ1
    
    float z = 1.0f - 2.0f * sample[0];
    float y = sin(2 * Pi * sample[1]) * sqrt(1.0f - pow(z, 2.0f));
    float x = cos(2 * Pi * sample[1]) * sqrt(1.0f - pow(z, 2.0f));
    
    return glm::vec3(x, y, z);
}

float WarpFunctions::squareToSphereUniformPDF(const Point3f &sample)
{
    //TODO - DONE
    
    return 1.0f/(4.0f * Pi);
}

Point3f WarpFunctions::squareToSphereCapUniform(const glm::vec2 &sample, float thetaMin)
{
    // TODO - DONE
    // throw std::runtime_error("You haven't yet implemented sphere cap warping!");
    
    // following the way it was done in sphere
    
    float z = 1.0f - 2.0f*sample[0]*(180.0f - thetaMin)/(180.0f);
    float y = sin(2 * Pi * sample[1]) * sqrt(1.0f - pow(z, 2.0f));
    float x = cos(2 * Pi * sample[1]) * sqrt(1.0f - pow(z, 2.0f));
    
    return glm::vec3(x, y, z);
}

float WarpFunctions::squareToSphereCapUniformPDF(const Point3f &sample, float thetaMin)
{
    //TODO - DONE
    
    // SurfaceArea = 2pi*r*h
    // h = 1 - cos(180-thetaMin)
    
    float theta = Pi/180*(180.0f - thetaMin);
    float h = 1.0f - cos(theta);
    float sa = 2.0f * Pi * 1.0f * h;
    
    return 1.0f/sa;
}

Point3f WarpFunctions::squareToHemisphereUniform(const Point2f &sample)
{
    // TODO - DONE
    // throw std::runtime_error("You haven't yet implemented uniform hemisphere warping!");
    
    // we start with:
    // x = sin(theta)cos(phi)
    // y = sin(theta)sin(phi)
    // z = cos(theta)
    // then we set  Let’s take theta = 2ρξ2 and phi = cos-1(ξ1)
    // so we get the following:
    //      x = cos(2ρξ2)√(1 − z^2)
    //      y = sin(2ρξ2)√(1 − z^2)
    //      z = ξ1
    
    float z = sample[0];
    float y = sin(2 * Pi * sample[1]) * sqrt(1.0f - pow(z, 2.0f));
    float x = cos(2 * Pi * sample[1]) * sqrt(1.0f - pow(z, 2.0f));
    
    return glm::vec3(x, y, z);
}

float WarpFunctions::squareToHemisphereUniformPDF(const Point3f &sample)
{
    //TODO - DONE
    // using height from cap idea but cap extends for full hemisphere
    float sa = Pi * 1.0f * 2.0f;
    
    return 1.0f/sa;
}

Point3f WarpFunctions::squareToHemisphereCosine(const Point2f &sample)
{
    //TODO - DONE
    // throw std::runtime_error("You haven't yet implemented cosine-weighted hemisphere warping!");
    
    // using disc concentric for x and y
//    glm::vec3 concentric = squareToDiskConcentric(sample);
//    float x = concentric[0];
//    float y = concentric[1];
//    // using malley's method for z: z = √(1 − x^2 - y^2)
//    float xsqr = powf(x, 2.0f);
//    float ysqr = powf(y, 2.0f);
//    float z = sqrt(std::max(0.f, 1.0f - xsqr - ysqr));
//    if(isnan(z) || xsqr + ysqr > 1.0f)
//    {
//        std::cout << "";
//    }
    
//    return glm::vec3(x, y, z);

    glm::vec3 d = squareToDiskConcentric(sample);
    float z = std::sqrt(std::max(0.f, 1-d.x*d.x - d.y*d.y));

    return Point3f(d.x, d.y, z);
}

float WarpFunctions::squareToHemisphereCosinePDF(const Point3f &sample)
{
    //TODO - DONE
    float theta = sample[2]; // the z value of given sample
    return cos(acos(theta))/(Pi);
}
