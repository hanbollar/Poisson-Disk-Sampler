#include "photonIntegrator.h"

Color3f PhotonIntegrator::Li(const Ray &ray, const Scene &scene, std::shared_ptr<Sampler> sampler, int depth,
                            Color3f compoundColor) const
{
    Intersection isx_withScene = Intersection();
    Ray usingRay = ray;
    bool hitScene = scene.Intersect(usingRay, &isx_withScene);
    if (!hitScene) {
        return Color3f(0.0f);
    }

    Ray testingRay = usingRay;

    bool stillLooking = true;
    /* CAUSTIC CHECK */
//    while (hitScene && isx_withScene.objectHit->material != nullptr && stillLooking) {
////        bool prodBSDF = isx_withScene.objectHit->ProduceBSDF(&isx_withScene) ;
////        if ( prodBSDF && !(isx_withScene.bsdf->BxDFsMatchingFlags(BSDF_SPECULAR) > 0) ) {
////            stillLooking = false;
////        } else {
////            if (!prodBSDF) {
////                return Color3f(0.0f);
////            }
////            Vector3f wiW;
////            BxDFType sampledType;
////            isx_withScene.bsdf.get()->Sample_f(-testingRay.direction, &wiW, sampler->Get2D(), new Float(), BSDF_ALL, &sampledType);

////            testingRay = isx_withScene.SpawnRay(wiW);
////            Intersection inter = Intersection();
////            hitScene = (scene.Intersect(testingRay, &inter));
////            if (hitScene) { isx_withScene = inter; }
////        }

//        bool prodBSDF = isx_withScene.objectHit->ProduceBSDF(&isx_withScene) ;
//        if ( prodBSDF && !(isx_withScene.bsdf->BxDFsMatchingFlags(BSDF_DIFFUSE) > 0) ) {
//            Vector3f wiW;
//            BxDFType sampledType;
//            isx_withScene.bsdf.get()->Sample_f(-testingRay.direction, &wiW, sampler->Get2D(), new Float(), BSDF_ALL, &sampledType);

//            testingRay = isx_withScene.SpawnRay(wiW);
//            hitScene = scene.Intersect(testingRay, &isx_withScene);
//        } else {
//            if (!prodBSDF) {
//                return Color3f(0.0f);
//            }

//            stillLooking = false;
//        }
//    }

//    if (!hitScene) {
//        return Color3f(0.0f);
//    }

//    usingRay = testingRay;

    // dir light at intersection
    Color3f c_dir = dir_Int->Li(usingRay, scene, sampler, depth, compoundColor);

    // send out radiance rays to scene and sample accum at those intersections
    Color3f c_MapCol = Color3f(0.0f);
    int numFeelerRays = 25;
    int numHit = 0;
    for (int i = 0; i < numFeelerRays; i++) {
        Intersection isx_check = Intersection();

        Vector3f dir = isx_withScene.objectHit->shape.get()->transform.T3() * WarpFunctions::squareToHemisphereCosine(sampler->Get2D());

        if (scene.Intersect(isx_withScene.SpawnRay(dir), &isx_check)) {
            Color3f tempCol = p_Map->accumAllPhotonsWithinRadius(isx_check.point);
            c_MapCol += tempCol;
            numHit += 1;
        }
    }

    // to weight all the diff color inputs
    float pdf = WarpFunctions::squareToHemisphereCosinePDF(isx_withScene.point);
//    if (pdf != 0) {
//        c_MapCol /= pdf;
    if (numHit > 0) {
        c_MapCol /= (float)numHit;

        return c_dir + c_MapCol;
    }

    return /*Color3f(0.0f); //*/c_dir;
}
