#include "fulllightingintegrator.h"

Color3f FullLightingIntegrator::Li(const Ray &ray, const Scene &scene, std::shared_ptr<Sampler> sampler, int depth,
                                   Color3f compoundColor) const
{
    Intersection isx = Intersection();

    Color3f accumRayColor = Color3f(0.0f); // used for final return value
    Color3f accumThroughputColor = Color3f(1.0f); //used in russian roulette termination

    /////////
    Vector3f woW = -1.0f*glm::normalize(ray.direction); // isxToCamera;

    /////////
    Vector3f wiW1;
    float pdf1 = 0.0f;
    BxDFType type1 = BSDF_ALL;
    BxDFType sampledType1;

    ////////
    Vector3f wiW2_dir;
    Vector3f wiW2_bsdf;
    float pdf2_dir = 0.0f;
    float pdf2_bsdf = 0.0f;
    BxDFType type2 = BSDF_ALL;
    BxDFType sampledType2;

    int numLights = scene.lights.size();
    if (numLights == 0) {
        // no lights, no illumination
        return Color3f(0.0f);
    }
    ///////

    if (! (scene.Intersect(ray, &isx))) {
        // never intersected in the first place
        return Color3f(0.0f);
    }
    Color3f le = isx.Le(woW);
    if (depth == 0 || isx.objectHit->material == nullptr) {
        return le;
    }

    bool specularBounce = false;
    bool first = true;
    while (depth > 0) {

        // if bsdf does not exist at this intersection then do next bounce
        if (!isx.objectHit->ProduceBSDF(&isx)) {
            // end
            break;
        }

        // if spot / point light then do proper accounting for it
        if (isx.objectHit != nullptr && dynamic_cast<IgnoreGeoLight*>(isx.objectHit->shape.get()) != NULL) {
            auto light = isx.objectHit;

            bool inter = true;

            while(isx.objectHit == light && inter) {
                isx.point += ray.direction * 0.5f;
                Ray passThrough = isx.SpawnRay(ray.direction);
                inter = scene.Intersect(passThrough, &isx);
            }
        }

        // create BSDF to be used in naive & for later pdf values
        BSDF* usingB_initial = isx.bsdf.get();


        // checking if include Le
        // include if ray directly from camera [first] or perfectly specular surface
        if (specularBounce || first) {
            accumRayColor += accumThroughputColor * le;
        }

        /** ******************* **/
        /** ******************* **/
        /** DIRECT LIGHTING - 2 **/
        /** ******************* **/
        /** ******************* **/

        Color3f L_d = Color3f(0.0f);

        int index = -1;
        if (! specularBounce)  {

            /***************************************************/
            /*** calculating the dir part of direct lighting ***/
            /***************************************************/

            Color3f col2_dir = Color3f(0.0f);

            //choosing random light
            index = sampler->Get1D() * numLights;
            Color3f li2_dir = scene.lights[index]->Sample_Li(isx, sampler->Get2D(), &wiW2_dir, &pdf2_dir);
            // dont need to divide pdf by num lights bc multiplying color by num lights later on so
            //      getting an uneccessary squared factor when only need one of them in the calculation

            // checking for occlusion
            Intersection isx_dirWithLight = Intersection();
            bool rayHitsObj2_dir = scene.Intersect(isx.SpawnRay(wiW2_dir), &isx_dirWithLight);
            // if the object we hit exists and is the light we randomly chose then visibility is 100%
            Color3f visibility = Color3f(0.0f);
            if (rayHitsObj2_dir && isx_dirWithLight.objectHit->areaLight == scene.lights[index]) {
                visibility = Color3f(1.0f);
            }
            col2_dir = usingB_initial->f(woW, wiW2_dir, type2);

            float absDot2_dir = fabs(glm::dot(wiW2_dir, isx.normalGeometric));
            float pdf2_dir_swap = usingB_initial->Pdf(woW, wiW2_dir);
            float weight2_dir = PowerHeuristic(1, pdf2_dir, 1, pdf2_dir_swap);

            Color3f dir_samp_dir = Color3f(0.0f);
            if (pdf2_dir != 0.0f) {
                dir_samp_dir = ((li2_dir * col2_dir * visibility * absDot2_dir * weight2_dir) / pdf2_dir);
            }

            L_d += accumThroughputColor * dir_samp_dir;

            /****************************************************/
            /*** calculating the bsdf part of direct lighting ***/
            /****************************************************/

            Color3f col2_bsdf = Color3f(0.0f);
            Color3f li2_bsdf = Color3f(0.0f);

            // so can create ray to send
            col2_bsdf = usingB_initial->Sample_f(woW, &wiW2_bsdf, sampler->Get2D(), &pdf2_bsdf, type2, &sampledType2);

            // sending ray
            Intersection isx_dirWithBSDF = Intersection();
            bool rayHitsObj2_bsdf = scene.Intersect(isx.SpawnRay(wiW2_bsdf), &isx_dirWithBSDF);

            // if intersection with same light then include bsdf part of dir
            if (rayHitsObj2_bsdf && (isx_dirWithBSDF.objectHit->areaLight == scene.lights[index])) {
                li2_bsdf = isx_dirWithBSDF.objectHit->GetAreaLight()->L(isx_dirWithBSDF, wiW2_bsdf);
            } //otherwise dir_samp_bsdf will be 0.

            // calcs for combining into L_d
            float absDot2_bsdf = fabs(glm::dot(wiW2_bsdf, isx.normalGeometric));
            float pdf2_bsdf_swap = scene.lights[index]->Pdf_Li(isx, wiW2_bsdf);
            float weight2_bsdf = PowerHeuristic(1, pdf2_bsdf, 1, pdf2_bsdf_swap);

            Color3f dir_samp_bsdf = Color3f(0.0f);
            if (pdf2_bsdf != 0.0f) {
                dir_samp_bsdf = (li2_bsdf * col2_bsdf * absDot2_bsdf * weight2_bsdf) / pdf2_bsdf;
            } else {
                break;
            }
            L_d +=  dir_samp_bsdf * accumThroughputColor;

        } // end direct lighting if not specular check

        // final calc of combined
        L_d *= numLights;

        /** ************************ **/
        /** end: DIRECT LIGHTING - 2 **/
        /** ************************ **/

         //for testing
//         accumRayColor = L_d;
//         return accumRayColor;

        accumRayColor +=  L_d;

        /** ******************************************* **/
        /** ******************************************* **/
        /** BOUNCE AND GLOBAL ILLUMINATION LIGHTING - 1 **/
        /** ******************************************* **/
        /** ******************************************* **/

        // calc color for this iteration
        Vector2f randomNums = sampler->Get2D();
        Color3f col1 = usingB_initial->Sample_f(woW, &wiW1, randomNums, &pdf1, type1, &sampledType1);

        Intersection isx_BSDFGlobal = Intersection();
        bool hitObj = scene.Intersect(isx.SpawnRay(wiW1), &isx_BSDFGlobal);

        // no intersection OR hit light used in direct OR col1 is black OR pdf at intersection for global is 0
        if (!hitObj || (index != -1 && isx_BSDFGlobal.objectHit->areaLight == scene.lights[index]) || IsBlack(col1) || pdf1 == 0.0f) {
            break;
        }

        // update specularity
        // include Le if ray directly from camera [first] or perfectly specular surface
        specularBounce = (sampledType1 & BSDF_SPECULAR != 0);

        float absDot1 = fabs(glm::dot(wiW1, isx.normalGeometric));

        // update throughput
        accumThroughputColor *= col1 * absDot1 / pdf1;

        if(std::isnan(accumThroughputColor.r) || std::isnan(accumThroughputColor.g) || std::isnan(accumThroughputColor.b) ||
           std::isnan(accumRayColor.r) || std::isnan(accumRayColor.g) || std::isnan(accumRayColor.b))
        {
            std::cout << "";
        }

        /** ************************************************ **/
        /** end: BOUNCE AND GLOBAL ILLUMINATION LIGHTING - 1 **/
        /** ************************************************ **/

        /** **************** **/
        /** RUSSIAN ROULETTE **/
        /** **************** **/
        // check for russian roulette termination to break/exit the loop
        if (recursionLimit - depth >= 3) {
            float q = std::max(0.05f, (1.0f - std::max(accumThroughputColor.r,
                                                       std::max(accumThroughputColor.g,
                                                                accumThroughputColor.b) )
                                       ));
            if (sampler->Get1D() < q) {
                break;
            }
            accumThroughputColor /= (1.0f - q);
        }

        /** *************************** **/
        /** UPDATING RAYS FOR NEXT ITER **/
        /** *************************** **/

        // update for next looping
        woW = -wiW1;
        isx = isx_BSDFGlobal;
        le = isx.Le(woW);

        // iterate
        depth -= 1;

        first = false;
    }


    return accumRayColor;
}

float BalanceHeuristic(int nf, Float fPdf, int ng, Float gPdf)
{
    if (fPdf == 0.0f && gPdf == 0.0f) {
        return 0.0f; //bc would return nan
    }

    return nf*fPdf / (nf*fPdf + ng*gPdf);
}

float PowerHeuristic(int nf, Float fPdf, int ng, Float gPdf)
{
    if (fPdf == 0.0f && gPdf == 0.0f) {
        return 0.0f; //bc would return nan
    }

    //current power = 2
    float f = nf * fPdf;
    float g = ng * gPdf;
    return f*f / (f*f + g*g);
}

